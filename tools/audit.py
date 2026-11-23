"""Audit des scripts du mod : ce que le convertisseur 3DS applique, et tout ce qu'il ignore, chanson par chanson.

usage : py -3 tools/audit.py "<jeu>/bin" [sortie.md] [chanson ...]

Pour chaque chanson, Codename Engine exécute dans PlayState :
  - le script du décor (data/stages/<décor>.hx)
  - les scripts communs à toutes les chansons (data/charts/*.hx)
  - les scripts de la chanson (songs/<chanson>/scripts/*.hx), plus cutscene.hx / end-cutscene.hx
L'audit lit chaque fonction : create/postCreate sont rejoués comme le fait convert.py (instruction appliquée ou
non) ; les autres fonctions (beatHit, stepHit, update, fonctions appelées par les événements "HScript Call"...)
s'exécutent pendant la chanson et ne sont jamais converties : elles sont résumées par catégorie.
"""
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import convert as cv  # noqa: E402

# catégories, dans l'ordre de priorité (la première qui correspond gagne)
CATS = [
    ("vidéo/cinématique", r"Video|Cutscene|startCutscene"),
    ("shader", r"Shader|\.shader\b|addShader|setFilters|\.filters\b"),
    ("HUD", r"camHUD|camNewHUD|iconP[12]|healthBar|scoreTxt|\bstrumLines\b|playerStrums|cpuStrums|\bnotes\.|"
            r"\.strumLine|FlxText|timeBar|comboGroup|ratingGroup|\bhud\b"),
    ("personnage", r"\b(boyfriend|dad|gf|girlfriend)\b|new Character\b|\bchar(acter)?s?\b\s*\.|\.characters\b"),
    ("caméra", r"camGame|FlxG\.camera|defaultCamZoom|camFollow|camZoom|curCameraTarget|cameraOffset|camZooming|"
               r"\.zoom\b|\.shake\(|\.flash\(|\.fade\("),
    ("sprite/décor", r"new (FlxSprite|FunkinSprite|FlxBackdrop|FlxShape\w*|FlxTrail\w*|FlxSpriteGroup)|makeGraphic|"
                     r"loadGraphic|getSparrowAtlas|\badd\(|\binsert\(|\bremove\(|\.animation\.|\.alpha\b|\.visible\b|"
                     r"\.scale\b|\.color\b|\.angle\b|\.velocity\b|\.scrollFactor\b|\.setPosition\(|\.[xy] ?[-+*/]?="),
    ("tween/timer", r"FlxTween|FlxTimer|tween\("),
    ("audio", r"FlxG\.sound|playSound|vocals|\binst\b|Paths\.sound|Paths\.music"),
]
IGNORE = r"^(import |trace\(|var \w+(\s*:\s*[\w<>]+)?$|public var \w+(\s*:\s*[\w<>]+)?$|return\b|break$|continue$)"


def category(st):
    for name, rx in CATS:
        if re.search(rx, st):
            return name
    return "autre"


def functions(src):
    """Fonctions de premier niveau : nom -> corps."""
    out = {}
    for m in re.finditer(r"(?m)^\s*(?:override\s+)?(?:public\s+)?function\s+(\w+)\s*\([^)]*\)[^{]*\{", src):
        depth, i = 1, m.end()
        while i < len(src) and depth:
            depth += {"{": 1, "}": -1}.get(src[i], 0)
            i += 1
        out.setdefault(m.group(1), src[m.end():i - 1])
    return out


def all_statements(body):
    """Toutes les instructions d'un bloc, quelles que soient les conditions/boucles (en-têtes de contrôle retirés)."""
    out, i, n, cur = [], 0, len(body), ""
    while i < n:
        m = re.match(r"(if|for|while|switch)\s*\(", body[i:]) if (i == 0 or not (body[i - 1].isalnum() or body[i - 1] == "_")) else None
        if m:
            i = cv._balanced(body, i + m.end() - 1, "(", ")")
            continue
        m = re.match(r"(else|do)\b", body[i:]) if (i == 0 or not body[i - 1].isalnum()) else None
        if m and not cur.strip():
            i += m.end()
            continue
        m = re.match(r"(case\s+[^:]+|default)\s*:", body[i:]) if not cur.strip() else None
        if m:
            i += m.end()
            continue
        ch = body[i]
        if ch in "'\"":
            j = body.find(ch, i + 1)
            j = n if j < 0 else j
            cur += body[i:j + 1]
            i = j + 1
            continue
        if ch == "(" or ch == "[":
            j = cv._balanced(body, i, ch, ")" if ch == "(" else "]")
            cur += body[i:j]
            i = j
            continue
        if ch in ";{}":
            if cur.strip():
                out.append(re.sub(r"\s+", " ", cur).strip())
            cur = ""
        else:
            cur += ch
        i += 1
    if cur.strip():
        out.append(re.sub(r"\s+", " ", cur).strip())
    return out


def handled_char_fix(st, ctx):
    """Instruction comprise par script_fixes (persos, zoom, caméra fixe) ?"""
    ph = cv.script_fixes("function create() {" + st + ";}", ctx)
    return bool(ph.get("create"))


def handled_stage_sprite(st, known_vars):
    """Instruction comprise par run_stage_script (sprites créés par le script du décor) ?"""
    s = re.sub(r"^var ", "", st)
    if re.match(r"(\w+)(?: ?: ?\w+)? = members\.indexOf\(", s):
        return True
    if re.match(r"(\w+)(?: ?: ?\w+)? = new (FunkinSprite|FlxSprite|FlxBackdrop)\(", s):
        known_vars.add(re.match(r"(\w+)", s).group(1))
        return True
    m = re.match(r"(\w+)\.(.*)$", s)
    if m and m.group(1) in known_vars:
        o = cv.ScriptSprite(0, 0)
        o.fw = o.fh = 100.0
        before = dict(o.__dict__)
        cv.sprite_statement(o, m.group(2))
        return o.__dict__ != before
    m = re.fullmatch(r"(add|insert|remove)\((?:[^,]*, ?)?(\w+)\)", s)
    if m:
        return m.group(2) in known_vars or m.group(2) in cv.CHARNAME
    return False


def audit_script(path, is_stage, ctx, hscript_calls):
    src = cv.strip_comments(path.read_text(encoding="utf-8-sig", errors="replace"))
    fns = functions(src)
    res = {"applied": [], "ignored": [], "skipped": [], "runtime": {}}
    known = set()
    for fn in ("create", "postCreate"):
        body = fns.get(fn, "")
        done = [re.sub(r"\s+", " ", s).strip() for s in cv.executed(body, ctx)]
        for st in done:
            if re.match(IGNORE, st):
                continue
            ok = handled_char_fix(st, ctx) or (is_stage and handled_stage_sprite(st, known))
            (res["applied"] if ok else res["ignored"]).append((fn, category(st), st))
        done_set = set(done)
        for st in all_statements(body):
            if st not in done_set and not re.match(IGNORE, st):
                res["skipped"].append((fn, category(st), st))
    for fn, body in fns.items():
        if fn in ("create", "postCreate"):
            continue
        sts = [s for s in all_statements(body) if not re.match(IGNORE, s)]
        cats = Counter(category(s) for s in sts)
        cats.pop("autre", None)
        if cats:
            res["runtime"][fn] = (cats, hscript_calls.get(fn, 0), sts)
    return res


def song_scripts(mod, folder, stage):
    files = []
    st = mod / "data" / "stages" / f"{stage}.hx"
    if st.exists():
        files.append((st, True))
    files += [(p, False) for p in sorted((mod / "data" / "charts").glob("*.hx"))]
    sd = mod / "songs" / folder
    files += [(p, False) for p in sorted((sd / "scripts").glob("*.hx"))] if (sd / "scripts").is_dir() else []
    files += [(p, False) for p in (sd / "cutscene.hx", sd / "end-cutscene.hx") if p.exists()]
    return files


def main():
    game = Path(sys.argv[1])
    out = Path(sys.argv[2]) if len(sys.argv) > 2 else Path(__file__).parent.parent / "docs" / "audit.md"
    wanted = sys.argv[3:]
    cv.GAME = game
    mod = cv.MOD = [p for p in (game / "mods").iterdir() if p.is_dir()][0]
    lst = Path(__file__).parent.parent / "sd" / "3ds" / "fnf3ds" / "cyber" / "songs.lst"
    songs = []
    for l in lst.read_text(encoding="utf-8").splitlines():
        p = l.split("|")
        if len(p) > 6 and (not wanted or p[0] in wanted):
            songs.append((p[0], p[4], p[5].lower(), p[1]))

    common = {}  # résultat des scripts communs (data/charts) : identiques pour toutes les chansons
    lines = ["# Audit des scripts du mod", "",
             "Généré par `tools/audit.py`. Pour chaque chanson : les scripts que Codename Engine exécute, ce que le "
             "convertisseur 3DS applique, et ce qu'il ignore.", "",
             "- **appliqué** : instruction de `create`/`postCreate` reprise par `convert.py`",
             "- **ignoré** : instruction de `create`/`postCreate` exécutée sur PC mais pas convertie",
             "- **non suivi** : instruction de `create`/`postCreate` dans un bloc que le convertisseur ne suit pas "
             "(condition inconnue, boucle, switch)",
             "- **pendant la chanson** : fonctions appelées en cours de partie (beatHit, stepHit, update, événements "
             "`HScript Call`...), jamais converties", ""]
    summary = []
    details = []
    totals = Counter()
    for sid, folder, diff, display in songs:
        sd = mod / "songs" / folder
        cp = sd / "charts" / f"{diff}.json"
        if not cp.exists():
            continue
        d = json.loads(cp.read_text(encoding="utf-8-sig"))
        if "strumLines" not in d and isinstance(d.get("song"), dict):
            d = cv.legacy_to_cne(d["song"], {})
        stage = d.get("stage", "stage")
        names = [(s.get("characters") or [""])[0] for s in d.get("strumLines", [])[:3]] + ["", "", ""]
        meta = json.loads((sd / "meta.json").read_text(encoding="utf-8-sig")) if (sd / "meta.json").exists() else {}
        ctx = {"PlayState.SONG.meta.displayName": meta.get("displayName", folder),
               "PlayState.SONG.meta.name": meta.get("name", folder),
               "SONG.meta.displayName": meta.get("displayName", folder), "SONG.meta.name": meta.get("name", folder),
               "PlayState.difficulty": diff, "dad.curCharacter": names[0], "boyfriend.curCharacter": names[1],
               "gf.curCharacter": names[2]}
        calls = Counter()
        for e in d.get("events", []):
            if e.get("name") == "HScript Call" and e.get("params"):
                calls[str(e["params"][0])] += 1
        extra = len(d.get("strumLines", [])) - 3
        cnt = Counter()
        block = [f"## {display} (`{sid}`)", "",
                 f"Décor `{stage}`, persos {', '.join(n for n in names[:3] if n) or '-'}"
                 + (f", **{extra} ligne(s) de notes en plus** (persos non affichés)" if extra > 0 else "")
                 + (f", {sum(calls.values())} événements HScript Call" if calls else ""), ""]
        for path, is_stage in song_scripts(mod, folder, stage):
            rel = path.relative_to(mod).as_posix()
            if rel.startswith("data/charts/"):
                if rel not in common:
                    common[rel] = audit_script(path, False, ctx, Counter())
                continue
            r = audit_script(path, is_stage, ctx, calls)
            cnt["appliqué"] += len(r["applied"])
            cnt["ignoré"] += len(r["ignored"])
            cnt["non suivi"] += len(r["skipped"])
            cnt["pendant"] += len(r["runtime"])
            block.append(f"### `{rel}`")
            for title, key in (("Appliqué", "applied"), ("Ignoré", "ignored"), ("Non suivi", "skipped")):
                items = r[key]
                if not items:
                    continue
                block.append(f"**{title}** ({len(items)})")
                for fn, cat, st in items:
                    block.append(f"- [{cat}] `{st[:140]}`" + (f" *({fn})*" if fn != "create" else ""))
                block.append("")
            if r["runtime"]:
                block.append("**Pendant la chanson**")
                for fn, (cats, ncall, _) in r["runtime"].items():
                    c = ", ".join(f"{k} {v}" for k, v in cats.most_common())
                    block.append(f"- `{fn}`" + (f" (appelée {ncall} fois par le chart)" if ncall else "") + f" : {c}")
                block.append("")
        totals.update(cnt)
        summary.append(f"| {display} | `{stage}` | {cnt['appliqué']} | {cnt['ignoré']} | {cnt['non suivi']} | "
                       f"{cnt['pendant']} | {sum(calls.values())} | {max(extra, 0)} |")
        details += block

    lines += ["## Résumé", "",
              "| Chanson | Décor | Appliqué | Ignoré | Non suivi | Fonctions pendant la chanson | HScript Call | "
              "Lignes en plus |", "|---|---|---|---|---|---|---|---|"] + summary + [""]
    lines += ["## Scripts communs à toutes les chansons (`data/charts/`)", ""]
    for rel, r in common.items():
        lines.append(f"### `{rel}`")
        for title, key in (("Ignoré", "ignored"), ("Non suivi", "skipped")):
            if r[key]:
                lines.append(f"**{title}** : " + "; ".join(f"[{c}] `{s[:80]}`" for _, c, s in r[key][:12]))
        for fn, (cats, _, _) in r["runtime"].items():
            lines.append(f"- `{fn}` : " + ", ".join(f"{k} {v}" for k, v in cats.most_common()))
        lines.append("")
    lines += details
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(len(summary), "chansons ->", out, dict(totals))


if __name__ == "__main__":
    main()
