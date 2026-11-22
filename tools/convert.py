"""Convertit un mod Codename Engine (Friday Night Funkin') pour le moteur FNF 3DS.

Sortie (dossier sd/3ds/fnf3ds/<mod>/) :
  songs.lst                  liste des chansons (dossier|nom affiché|icône|couleur)
  songs/<id>/chart.sng       partition binaire
  songs/<id>/inst.adp        musique (DSP ADPCM, lue en continu depuis la SD)
  songs/<id>/voices.adp      voix
  chars/<nom>.spr            personnage (cadres + animations + métadonnées)
  stages/<nom>.stg           décor
  ui/*.spr                   flèches, barre de vie, icônes, notes de jugement
  tex/*.t3x                  textures (pages 3DS, ETC1A4 ou RGBA)

Usage : py -3 convert.py <dossier bin du jeu> <dossier de sortie> [chanson ...]
"""
import json
import os
import re
import struct
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

from PIL import Image

S = 400.0 / 1280.0          # échelle monde FNF (1280x720) -> écran haut 3DS (400x240)
HUD_NOTE_SCALE = 0.7        # taille des flèches dans FNF
PAGE = 1024                 # taille max d'une texture 3DS
PAD = 2
TEX3DS = r"C:\devkitPro\tools\bin\tex3ds.exe"
FFMPEG = "ffmpeg"
HERE = Path(__file__).resolve().parent
ADPCM_ENC = HERE / "adpcm_enc.exe"

SKIP_DONE = False  # --vite : ne refait pas les personnages/décors déjà convertis
GAME = None   # dossier bin
MOD = None    # dossier du mod
OUT = None


def find_asset(rel):
    """Cherche d'abord dans le mod, puis dans les assets de base."""
    for base in (MOD, GAME / "assets"):
        p = base / rel
        if p.exists():
            return p
    return None


def fstr(s, n=32):
    b = s.encode("utf-8")[: n - 1]
    return b + b"\0" * (n - len(b))


# ---------------------------------------------------------------- sparrow

def parse_sparrow(xml_path):
    txt = Path(xml_path).read_text(encoding="utf-8-sig", errors="replace")
    txt = re.sub(r"<\?xml[^>]*\?>", "", txt)
    root = ET.fromstring(txt)
    frames = []
    for st in root.iter("SubTexture"):
        a = st.attrib
        x, y, w, h = (int(float(a[k])) for k in ("x", "y", "width", "height"))
        frames.append({
            "name": a["name"], "x": x, "y": y, "w": w, "h": h,
            "fx": int(float(a.get("frameX", 0))), "fy": int(float(a.get("frameY", 0))),
            "fw": int(float(a.get("frameWidth", w))), "fh": int(float(a.get("frameHeight", h))),
            "rot": a.get("rotated", "false") == "true",
        })
    return frames


def prefix_frames(frames, prefix):
    """Comme Flixel addByPrefix : cadres dont le nom commence par le préfixe, triés par numéro."""
    out = []
    for i, f in enumerate(frames):
        if f["name"].startswith(prefix):
            m = re.search(r"(\d+)\D*$", f["name"][len(prefix):] or "")
            num = int(m.group(1)) if m else 0
            out.append((num, i))
    out.sort()
    return out


class SpriteSet:
    """Cadres + animations, empaquetés dans des pages de texture 3DS."""

    def __init__(self, name, scale):
        self.name = name
        self.scale = scale
        self.images = []      # (PIL image, offx, offy, fullw, fullh) déjà mis à l'échelle
        self.cache = {}
        self.keep = []
        self.anims = []       # (nom, fps, loop, offx, offy, [indices cadre])
        self.meta = {}

    def add_frame(self, src, f):
        self.keep.append(src)  # garde l'image en vie : sinon id() peut être réutilisé par une autre image
        key = (id(src), f["x"], f["y"], f["w"], f["h"], f["rot"], f["fx"], f["fy"])
        if key in self.cache:
            return self.cache[key]
        if f["rot"]:
            im = src.crop((f["x"], f["y"], f["x"] + f["h"], f["y"] + f["w"])).rotate(90, expand=True)
        else:
            im = src.crop((f["x"], f["y"], f["x"] + f["w"], f["y"] + f["h"]))
        s = self.scale
        nw, nh = max(1, round(im.width * s)), max(1, round(im.height * s))
        if (nw, nh) != im.size:
            im = im.resize((nw, nh), Image.LANCZOS)
        idx = len(self.images)
        self.images.append((im, -f["fx"] * s, -f["fy"] * s, f["fw"] * s, f["fh"] * s))
        self.cache[key] = idx
        return idx

    def add_image(self, src):
        f = {"x": 0, "y": 0, "w": src.width, "h": src.height, "fx": 0, "fy": 0,
             "fw": src.width, "fh": src.height, "rot": False}
        return self.add_frame(src, f)

    def add_anim(self, name, fps, loop, ox, oy, idx):
        self.anims.append((name, fps, loop, ox, oy, idx))

    # -- empaquetage
    def pack(self, fmt):
        order = sorted(range(len(self.images)), key=lambda i: -self.images[i][0].height)
        pages = []  # [ [ (i, x, y) ], shelves ]
        placed = {}
        for i in order:
            im = self.images[i][0]
            w, h = im.width + PAD, im.height + PAD
            if w > PAGE or h > PAGE:
                raise RuntimeError(f"{self.name}: cadre trop grand {im.size}")
            ok = False
            for pi, pg in enumerate(pages):
                for sh in pg["shelves"]:
                    if sh["h"] >= h and sh["x"] + w <= PAGE:
                        placed[i] = (pi, sh["x"], sh["y"])
                        sh["x"] += w
                        ok = True
                        break
                if ok:
                    break
                if pg["y"] + h <= PAGE:
                    sh = {"y": pg["y"], "h": h, "x": w}
                    pg["shelves"].append(sh)
                    pg["y"] += h
                    placed[i] = (pi, 0, sh["y"])
                    ok = True
                    break
            if not ok:
                pages.append({"shelves": [{"y": 0, "h": h, "x": w}], "y": h})
                placed[i] = (len(pages) - 1, 0, 0)
        files = []
        (OUT / "tex").mkdir(parents=True, exist_ok=True)
        for pi, pg in enumerate(pages):
            used_w = max(sh["x"] for sh in pg["shelves"])
            pw = 8
            while pw < used_w:
                pw *= 2
            ph = 8
            while ph < pg["y"]:
                ph *= 2
            canvas = Image.new("RGBA", (pw, ph), (0, 0, 0, 0))
            for i, (p, x, y) in placed.items():
                if p == pi:
                    canvas.paste(self.images[i][0], (x, y))
            base = f"{self.name}_{pi}"
            png = OUT / "tex" / (base + ".png")
            canvas.save(png)
            t3x = OUT / "tex" / (base + ".t3x")
            subprocess.run([TEX3DS, "-f", fmt, "-z", "auto", "-o", str(t3x), str(png)], check=True,
                           stdout=subprocess.DEVNULL)
            png.unlink()
            files.append((base + ".t3x", pw, ph))
        self.placed = placed
        self.page_files = files

    def write(self, path, fmt="etc1a4"):
        self.pack(fmt)
        b = bytearray(b"FSPR")
        b += struct.pack("<IIII", 1, len(self.page_files), len(self.images), len(self.anims))
        for fname, pw, ph in self.page_files:
            b += fstr(fname, 48) + struct.pack("<HH", pw, ph)
        for i, (im, ox, oy, fw, fh) in enumerate(self.images):
            p, x, y = self.placed[i]
            b += struct.pack("<BxHHHHffff", p, x, y, im.width, im.height, ox, oy, fw, fh)
        for name, fps, loop, ox, oy, idx in self.anims:
            b += fstr(name) + struct.pack("<fBxxxffH", fps, 1 if loop else 0, ox, oy, len(idx))
            b += struct.pack(f"<{len(idx)}H", *idx)
            if len(idx) % 2:
                b += b"\0\0"
        keys = sorted(self.meta)
        b += struct.pack("<I", len(keys))
        for k in keys:
            v = self.meta[k]
            if isinstance(v, str):
                b += fstr(k, 16) + b"s\0\0\0" + fstr(v)
            else:
                b += fstr(k, 16) + b"f\0\0\0" + struct.pack("<f", float(v)) + b"\0" * 28
        Path(path).parent.mkdir(parents=True, exist_ok=True)
        Path(path).write_bytes(b)


def load_png(path):
    im = Image.open(path)
    return im.convert("RGBA")


# ---------------------------------------------------------------- personnages

def convert_character(name, done):
    if name in done:
        return done[name]
    xml = find_asset(f"data/characters/{name}.xml")
    if xml is None:
        print("  personnage introuvable:", name)
        done[name] = None
        return None
    root = ET.fromstring(re.sub(r"<!DOCTYPE[^>]*>", "", xml.read_text(encoding="utf-8-sig")))
    a = root.attrib
    sprite = a.get("sprite", name)
    scale = float(a.get("scale", 1))
    png = find_asset(f"images/characters/{sprite}.png")
    sx = find_asset(f"images/characters/{sprite}.xml")
    if png is None or sx is None:
        print("  image introuvable pour", name)
        done[name] = None
        return None
    if SKIP_DONE and (OUT / "chars" / f"{name}.spr").exists():
        done[name] = {"icon": a.get("icon", name)}
        return done[name]
    print("  personnage", name)
    src = load_png(png)
    frames = parse_sparrow(sx)
    ss = SpriteSet("chr_" + re.sub(r"\W", "_", name), S * scale)
    for an in root.iter("anim"):
        p = an.attrib
        pref = prefix_frames(frames, p["anim"])
        if not pref:
            continue
        ind = (p.get("indices") or "").strip("[] ")
        if ind:
            bynum = {n: i for n, i in pref}
            nums = []
            for part in ind.split(","):
                part = part.strip()
                if ".." in part:
                    lo, hi = part.split("..")
                    nums += list(range(int(lo), int(hi) + 1))
                elif part:
                    nums.append(int(part))
            sel = [bynum[n] for n in nums if n in bynum]
        else:
            sel = [i for _, i in pref]
        if not sel:
            continue
        idx = [ss.add_frame(src, frames[i]) for i in sel]
        ss.add_anim(p["name"], float(p.get("fps", 24)), p.get("loop", "false") == "true",
                    float(p.get("x", 0)), float(p.get("y", 0)), idx)
    ss.meta = {
        "isPlayer": 1 if a.get("isPlayer", "false") == "true" else 0,
        "flipX": 1 if a.get("flipX", "false") == "true" else 0,
        "x": float(a.get("x", 0)), "y": float(a.get("y", 0)),
        "camx": float(a.get("camx", 0)), "camy": float(a.get("camy", 0)),
        "holdTime": float(a.get("holdTime", 4)), "scale": scale,
        "icon": a.get("icon", name),
        "gfDance": 1 if any(n[0] == "danceLeft" for n in ss.anims) else 0,
    }
    ss.write(OUT / "chars" / f"{name}.spr")
    done[name] = ss.meta
    return ss.meta


# ---------------------------------------------------------------- décors

def convert_stage(name, done):
    if name in done:
        return
    done[name] = True
    if SKIP_DONE and (OUT / "stages" / f"{name}.stg").exists():
        return
    xml = find_asset(f"data/stages/{name}.xml")
    if xml is None:
        print("  décor introuvable:", name)
        return
    print("  décor", name)
    root = ET.fromstring(re.sub(r"<!DOCTYPE[^>]*>", "", xml.read_text(encoding="utf-8-sig")))
    folder = root.attrib.get("folder", "")
    if folder and not folder.endswith("/"):
        folder += "/"
    zoom = float(root.attrib.get("zoom", 1.0))
    sprites = []
    chars = {"boyfriend": (770, 100, 1, 1), "dad": (100, 100, 1, 1), "girlfriend": (400, 130, 0.95, 0.95)}
    order = []  # éléments dans l'ordre : ('spr', i) ou ('char', nom)
    for el in root:
        tag = el.tag
        a = el.attrib
        if tag in ("boyfriend", "bf", "player"):
            tag = "boyfriend"
        if tag in ("girlfriend", "gf"):
            tag = "girlfriend"
        if tag in ("dad", "opponent"):
            tag = "dad"
        if tag in chars:
            base = chars[tag]
            sc = a.get("scroll")
            chars[tag] = (float(a.get("x", base[0])), float(a.get("y", base[1])),
                          float(sc if sc else a.get("scrollx", base[2])), float(sc if sc else a.get("scrolly", base[3])))
            order.append(("char", tag))
            continue
        if tag != "sprite":
            continue
        spr = a.get("sprite")
        if not spr:
            continue
        png = find_asset(f"images/{folder}{spr}.png")
        if png is None:
            continue
        sxml = find_asset(f"images/{folder}{spr}.xml")
        scale = float(a.get("scale", 1))
        src = load_png(png)
        sid = len(sprites)
        ss = SpriteSet(f"stg_{re.sub(r'\W', '_', name)}_{sid}", S * scale)
        fit = 1.0
        if sxml is not None:
            frames = parse_sparrow(sxml)
            anims = [c.attrib for c in el if c.tag == "anim"]
            if anims:
                for an in anims:
                    pref = prefix_frames(frames, an.get("anim", ""))
                    if pref:
                        idx = [ss.add_frame(src, frames[i]) for _, i in pref]
                        ss.add_anim(an.get("name", "idle"), float(an.get("fps", 24)),
                                    an.get("loop", "true") != "false", float(an.get("x", 0)),
                                    float(an.get("y", 0)), idx)
            if not ss.anims and frames:
                idx = [ss.add_frame(src, f) for f in frames[:1]]
                ss.add_anim("idle", 24, True, 0, 0, idx)
        else:
            # image fixe : réduite davantage si elle dépasse une page 3DS
            w, h = src.width * S * scale, src.height * S * scale
            if w > PAGE - PAD or h > PAGE - PAD:
                fit = min((PAGE - PAD) / w, (PAGE - PAD) / h)
                ss.scale *= fit
            ss.add_anim("idle", 24, True, 0, 0, [ss.add_image(src)])
        ss.write(OUT / "stages" / f"{ss.name}.spr")
        sc = a.get("scroll")
        scx = float(sc if sc else a.get("scrollx", 1))
        scy = float(sc if sc else a.get("scrolly", 1))
        sprites.append((ss.name, float(a.get("x", 0)), float(a.get("y", 0)), scx, scy,
                        1.0 / fit, float(a.get("alpha", 1)), 1 if a.get("flipX") == "true" else 0))
        order.append(("spr", sid))
    b = bytearray(b"FSTG") + struct.pack("<If", 1, zoom)
    for k in ("boyfriend", "dad", "girlfriend"):
        b += struct.pack("<ffff", *chars[k])
    b += struct.pack("<I", len(order))
    for kind, v in order:
        if kind == "char":
            b += struct.pack("<B", 1) + fstr({"boyfriend": "bf", "dad": "dad", "girlfriend": "gf"}[v]) + \
                struct.pack("<ffffffB", 0, 0, 0, 0, 0, 0, 0)
        else:
            n, x, y, scx, scy, sc, al, fl = sprites[v]
            b += struct.pack("<B", 0) + fstr(n) + struct.pack("<ffffffB", x, y, scx, scy, sc, al, fl)
    (OUT / "stages").mkdir(parents=True, exist_ok=True)
    (OUT / "stages" / f"{name}.stg").write_bytes(b)


# ---------------------------------------------------------------- audio

def convert_audio(src, dst):
    if dst.exists() and dst.stat().st_mtime > src.stat().st_mtime:
        return
    raw = dst.with_suffix(".raw")
    subprocess.run([FFMPEG, "-v", "error", "-y", "-i", str(src), "-ac", "2", "-ar", "32000",
                    "-f", "s16le", str(raw)], check=True)
    subprocess.run([str(ADPCM_ENC), str(raw), str(dst), "32000", "2"], check=True)
    raw.unlink()


# ---------------------------------------------------------------- partitions

def legacy_to_cne(song, meta):
    """Ancien format FNF (sections, mustHitSection) -> structure Codename Engine."""
    bpm = float(song.get("bpm", meta.get("bpm", 100)))
    dad, bf = {"type": 0, "notes": [], "characters": [song.get("player2", "dad")]},         {"type": 1, "notes": [], "characters": [song.get("player1", "bf")]}
    gf = {"type": 2, "notes": [], "characters": [song.get("gfVersion") or song.get("player3") or "gf"]}
    events = []
    t = 0.0
    last_cam = None
    for sec in song.get("notes", []):
        if sec.get("changeBPM") and sec.get("bpm"):
            bpm = float(sec["bpm"])
            events.append({"name": "BPM Change", "time": t, "params": [bpm]})
        must = bool(sec.get("mustHitSection", True))
        cam = 2 if sec.get("gfSection") and False else (1 if must else 0)
        if cam != last_cam:
            events.append({"name": "Camera Movement", "time": t, "params": [cam]})
            last_cam = cam
        for n in sec.get("sectionNotes", []):
            if len(n) < 2 or not isinstance(n[1], (int, float)) or n[1] < 0 or n[1] > 7:
                continue
            d = int(n[1])
            player = (d < 4) == must
            note = {"time": float(n[0]), "id": d % 4, "sLen": float(n[2]) if len(n) > 2 and isinstance(n[2], (int, float)) else 0, "type": 0}
            (bf if player else dad)["notes"].append(note)
        steps = sec.get("lengthInSteps") or (sec.get("sectionBeats", 4) * 4)
        t += float(steps) * 60000.0 / bpm / 4.0
    return {"strumLines": [dad, bf, gf], "events": events, "stage": song.get("stage", "stage"),
            "scrollSpeed": float(song.get("speed", 1)), "bpm": float(song.get("bpm", 100))}


def convert_song(song_dir, chars_done, stages_done, chart_p):
    diff = chart_p.stem
    sid = song_dir.name if diff == "regular" else f"{song_dir.name}-{diff}"
    meta = {}
    if (song_dir / "meta.json").exists():
        meta = json.loads((song_dir / "meta.json").read_text(encoding="utf-8-sig"))
    d = json.loads(chart_p.read_text(encoding="utf-8-sig"))
    if "strumLines" not in d and isinstance(d.get("song"), dict):
        d = legacy_to_cne(d["song"], meta)
    print("chanson", sid)
    display = meta.get("displayName", song_dir.name) + ("" if diff == "regular" else f" ({diff.upper()})")
    bpm = float(meta.get("bpm", d.get("bpm", 100)))
    stage = d.get("stage", "stage")
    lines = d["strumLines"]

    def line(t):
        for s in lines:
            if s.get("type") == t:
                return s
        return None

    dad, bf, gf = line(0), line(1), line(2)
    names = []
    for s, default in ((dad, "dad"), (bf, "bf"), (gf, "gf")):
        names.append((s.get("characters") or [default])[0] if s else "")
    for n in names:
        if n:
            convert_character(n, chars_done)
    convert_stage(stage, stages_done)

    out = OUT / "songs" / sid
    out.mkdir(parents=True, exist_ok=True)
    events = []
    for e in d.get("events", []):
        n, p = e["name"], e.get("params", [])
        if n == "Camera Movement" and p:
            # paramètre = indice de la ligne de notes visée
            idx = int(p[0])
            t = lines[idx].get("type", 0) if 0 <= idx < len(lines) else 0
            events.append((float(e["time"]), 0, float(t)))
        elif n == "BPM Change" and p:
            events.append((float(e["time"]), 1, float(p[0])))
        elif n == "Alt Animation Toggle" and p:
            events.append((float(e["time"]), 2, 1.0 if p[0] else 0.0))
    events.sort()

    def notes_of(s):
        res = []
        for n in (s or {}).get("notes", []):
            lane = int(n.get("id", 0))
            if 0 <= lane < 4:
                res.append((float(n["time"]), lane, int(n.get("type", 0)), float(n.get("sLen", 0))))
        res.sort()
        return res

    nd, nb = notes_of(dad), notes_of(bf)
    b = bytearray(b"FSNG") + struct.pack("<Iff", 1, bpm, float(d.get("scrollSpeed", 1)))
    b += fstr(stage) + fstr(names[0]) + fstr(names[1]) + fstr(names[2])
    b += fstr(display, 48)
    b += struct.pack("<III", len(nd), len(nb), len(events))
    for t, lane, typ, sl in nd + nb:
        b += struct.pack("<fBBxxf", t, lane, typ, sl)
    for t, k, v in events:
        b += struct.pack("<fBxxxf", t, k, v)
    (out / "chart.sng").write_bytes(b)

    songf = song_dir / "song"
    inst = songf / f"Inst-{diff}.ogg"
    voices = songf / f"Voices-{diff}.ogg"
    if not inst.exists():
        inst = songf / "Inst.ogg"
    if not voices.exists():
        voices = songf / "Voices.ogg"
    if inst.exists():
        convert_audio(inst, out / "inst.adp")
    if voices.exists():
        convert_audio(voices, out / "voices.adp")
    icon = meta.get("icon", "")
    color = meta.get("color", "")
    base = meta.get("displayName", song_dir.name)
    return f"{sid}|{display}|{icon}|{color}|{song_dir.name}|{diff.upper()}|{meta.get('composer', '')}|{cover_of(base)}|{base}"


# ---------------------------------------------------------------- interface

def convert_ui():
    print("interface")
    ui = OUT / "ui"
    # flèches
    xml = find_asset("images/game/notes/default.xml")
    frames = parse_sparrow(xml)
    src = load_png(find_asset("images/game/notes/default.png"))
    ss = SpriteSet("ui_notes", S * HUD_NOTE_SCALE)
    for name in ["arrowLEFT", "arrowDOWN", "arrowUP", "arrowRIGHT",
                 "left press", "down press", "up press", "right press",
                 "left confirm", "down confirm", "up confirm", "right confirm",
                 "purple0", "blue0", "green0", "red0",
                 "purple hold piece", "blue hold piece", "green hold piece", "red hold piece",
                 "pruple end hold", "blue hold end", "green hold end", "red hold end"]:
        pref = prefix_frames(frames, name)
        idx = [ss.add_frame(src, frames[i]) for _, i in pref]
        ss.add_anim(name.rstrip("0"), 24, False, 0, 0, idx)
    ss.write(ui / "notes.spr", "rgba8")
    # barre de vie, jugements, chiffres, compte à rebours
    ss = SpriteSet("ui_misc", S)
    for n in ["healthBar", "score/sick", "score/good", "score/bad", "score/shit", "ready", "set", "go"] + \
             [f"score/num{i}" for i in range(10)]:
        p = find_asset(f"images/game/{n}.png")
        if p is None:
            continue
        im = load_png(p)
        scale = 0.7 if n.startswith("score/") and "num" not in n else (0.5 if "num" in n else 1.0)
        if n in ("ready", "set", "go"):
            scale = 0.8
        if scale != 1.0:
            im = im.resize((max(1, round(im.width * scale)), max(1, round(im.height * scale))), Image.LANCZOS)
        ss.add_anim(n.split("/")[-1], 24, False, 0, 0, [ss.add_image(im)])
    ss.write(ui / "misc.spr", "rgba8")


COVERS = ["mayhem", "sha", "ct", "old", "def", "soft", "tans", "dust", "hope"]
COVER_GROUPS = {3: ["Open System", "Wear A Mask", "Last Hope"], 0: ["You And Me", "Fun Till End", "Light It Up"],
                1: ["Virus", "Malware", "Crowd Control", "Errorians"], 2: ["Cyber Trouble"],
                5: ["Soft Machine", "New Day", "Yawarakai"], 6: ["Underground", "Karma"], 7: ["New Hope", "Hoax"],
                8: ["Our Hope"]}


def cover_of(name):
    for k, v in COVER_GROUPS.items():
        if name in v:
            return k
    return 4


def convert_menus():
    """Écran titre, menu principal, freeplay : images, polices, sons et musique du menu."""
    print("menus")
    ui = OUT / "ui"
    ui.mkdir(parents=True, exist_ok=True)

    def img(rel):
        p = find_asset(f"images/{rel}.png")
        return load_png(p) if p else None

    # écran titre
    ss = SpriteSet("menu_title", S)
    for n in ["fade", "lazers-effect", "lazers-effectless", "logo"]:
        im = img(f"menus/titlescreen/{n}")
        if im:
            ss.add_anim(n, 0, False, 0, 0, [ss.add_image(im)])
    xml = find_asset("images/menus/titlescreen/titleEnter.xml")
    if xml:
        frames = parse_sparrow(xml)
        src = img("menus/titlescreen/titleEnter")
        for name, an in (("idle", "Press Enter to Begin"), ("press", "ENTER PRESSED")):
            idx = [ss.add_frame(src, frames[i]) for _, i in prefix_frames(frames, an)]
            if idx:
                ss.add_anim(name, 24, True, 0, 0, idx)
    ss.write(ui / "title.spr", "rgba8")
    # menu principal + freeplay (interface en RGBA)
    ss = SpriteSet("menu_ui", S)
    for n in ["menus/mainmenu/fade", "menus/freeplay/songbar", "menus/freeplay/overlay"]:
        im = img(n)
        if im:
            ss.add_anim(n.split("/")[-2] + "_" + n.split("/")[-1], 0, False, 0, 0, [ss.add_image(im)])
    for c in COVERS:
        im = img(f"menus/freeplay/cover/{c}")
        if im:
            im = im.resize((max(1, round(im.width * 2 / 3)), max(1, round(im.height * 2 / 3))), Image.LANCZOS)
            ss.add_anim("cover_" + c, 0, False, 0, 0, [ss.add_image(im)])
    ss.write(ui / "menu.spr", "rgba8")
    # grandes illustrations du menu principal (affichées à 1,4x l'écran)
    ss = SpriteSet("menu_art", S)
    for c in ["desk", "grrrr", "shadum"]:
        im = img(f"menus/mainmenu/art/{c}")
        if im:
            im = im.resize((1792, 1008), Image.LANCZOS)
            ss.add_anim(c, 0, False, 0, 0, [ss.add_image(im)])
    ss.write(ui / "art.spr", "etc1a4")
    # mode histoire
    ss = SpriteSet("menu_story", S)
    for name, rel, size in (("pause_fade", "game/pause/fade", None), ("hologram", "game/pause/hologram", (1536, 864)),
                            ("chap1", "menus/story/cover/chap1", (392, 554)), ("chap2", "menus/story/cover/chap2", (392, 554)),
                            ("chap3", "menus/story/cover/chap3", (392, 554)), ("chap4", "menus/story/cover/chap4", (392, 554)),
                            ("chap5", "menus/story/cover/chap5", (392, 554)), ("side_main", "menus/story/side/main", (800, 735)),
                            ("side_side", "menus/story/side/side", (800, 735))):
        im = img(rel)
        if im:
            if size:
                im = im.resize(size, Image.LANCZOS)
            ss.add_anim(name, 0, False, 0, 0, [ss.add_image(im)])
    ss.write(ui / "story.spr", "etc1a4")
    # polices
    wl = ui / "chars.txt"
    wl.write_text(" ".join(str(c) for c in list(range(32, 127)) + list(range(0xC0, 0x100))), encoding="ascii")
    for f, size in (("freeplay", 26), ("vcr", 22)):
        src = find_asset(f"fonts/{f}.ttf")
        if src:
            subprocess.run([str(Path(TEX3DS).parent / "mkbcfnt.exe"), "-s", str(size), "-w", str(wl), "-o",
                            str(ui / f"{f}.bcfnt"), str(src)], check=True, stdout=subprocess.DEVNULL)
    wl.unlink()
    # sons et musique
    for n in ["scroll", "confirm", "cancel", "selected"]:
        p = find_asset(f"sounds/menu/{n}.ogg")
        if p:
            convert_audio(p, ui / f"sfx_{n}.adp")
    p = find_asset("music/freakyMenu.ogg")
    if p:
        convert_audio(p, ui / "menu.adp")


def convert_icons(names):
    ss = SpriteSet("ui_icons", S)
    for n in sorted(set(x for x in names if x)):
        p = find_asset(f"images/icons/{n}.png") or find_asset(f"images/icons/icon-{n}.png")
        if p is None:
            continue
        im = load_png(p)
        half = im.width // 2 if im.width >= im.height * 2 else im.width
        idx = [ss.add_image(im.crop((0, 0, half, im.height)))]
        if half < im.width:
            idx.append(ss.add_image(im.crop((half, 0, half * 2, im.height))))
        ss.add_anim(n, 0, False, 0, 0, idx)
    ss.write(OUT / "ui" / "icons.spr", "rgba8")


def main():
    global GAME, MOD, OUT
    GAME = Path(sys.argv[1])
    mods = [p for p in (GAME / "mods").iterdir() if p.is_dir()]
    MOD = mods[0]
    OUT = Path(sys.argv[2])
    OUT.mkdir(parents=True, exist_ok=True)
    global SKIP_DONE
    wanted = [a for a in sys.argv[3:] if a != "--vite"]
    SKIP_DONE = "--vite" in sys.argv
    chars_done, stages_done = {}, {}
    convert_ui()
    convert_menus()
    order = []
    lst = MOD / "data" / "freeplaySonglist.txt"
    if lst.exists():
        order = [l.strip() for l in lst.read_text(encoding="utf-8-sig").splitlines() if l.strip()]
    for p in sorted((MOD / "songs").iterdir()):
        if p.is_dir() and p.name not in order:
            order.append(p.name)
    entries = []
    for sid in order:
        if wanted and sid not in wanted:
            continue
        sd = MOD / "songs" / sid
        if not sd.is_dir():
            continue
        charts = sorted((sd / "charts").glob("*.json"), key=lambda c: (c.stem != "regular", c.stem))
        for cp in charts:
            try:
                e = convert_song(sd, chars_done, stages_done, cp)
            except Exception as ex:  # une chanson cassée ne bloque pas les autres
                print("  ERREUR", sid, cp.stem, ex)
                e = None
            if e:
                entries.append(e)
    icons = [m["icon"] for m in chars_done.values() if m] + [e.split("|")[2] for e in entries]
    convert_icons(icons)
    (OUT / "songs.lst").write_text("\n".join(entries) + "\n", encoding="utf-8")
    print("FINI", len(entries), "chansons")


if __name__ == "__main__":
    main()
