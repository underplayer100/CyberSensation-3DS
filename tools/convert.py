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


def parse_indices(txt):
    """'[0,1,2]', '0..5', '0,2..4' -> liste d'entiers (comme CoolUtil.parseNumberRange)."""
    nums = []
    for part in (txt or "").strip("[] ").split(","):
        part = part.strip()
        if ".." in part:
            lo, hi = part.split("..")
            nums += list(range(int(float(lo)), int(float(hi)) + 1))
        elif part:
            nums.append(int(float(part)))
    return nums


def select_frames(frames, prefix, indices):
    """addByPrefix ou addByIndices (indices = position dans la liste triée du préfixe, comme Flixel)."""
    pref = prefix_frames(frames, prefix)
    nums = parse_indices(indices)
    if not nums:
        return [i for _, i in pref]
    return [pref[n][1] for n in nums if 0 <= n < len(pref)]


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


# ---------------------------------------------------------------- atlas Adobe Animate (FlxAnimate)

class AnimateAtlas:
    """Aplatit un atlas Animate (Animation.json + spritemapN) en images, comme le dessinerait FlxAnimate :
    addBySymbol joue le symbole à la position du sprite (matrice identité), éléments composés récursivement."""

    def __init__(self, folder):
        folder = Path(folder)
        self.data = json.loads((folder / "Animation.json").read_text(encoding="utf-8-sig"))
        self.symbols = {s["SN"]: s["TL"] for s in self.data.get("SD", {}).get("S", [])}
        self.parts = {}
        for sm in sorted(folder.glob("spritemap*.json")):
            atlas = json.loads(sm.read_text(encoding="utf-8-sig"))
            img = load_png(folder / atlas["meta"]["image"])
            for s in atlas["ATLAS"]["SPRITES"]:
                s = s["SPRITE"]
                if s["rotated"]:
                    im = img.crop((s["x"], s["y"], s["x"] + s["h"], s["y"] + s["w"])).rotate(90, expand=True)
                else:
                    im = img.crop((s["x"], s["y"], s["x"] + s["w"], s["y"] + s["h"]))
                self.parts[s["name"]] = im
        self.fps = float(self.data.get("MD", {}).get("FRT", 24))

    @staticmethod
    def matrix(e):
        m = e.get("M3D", [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1])
        if isinstance(m, dict):
            m = [m.get(k, 0) for k in ("m00", "m01", "m02", "m03", "m10", "m11", "m12", "m13",
                                        "m20", "m21", "m22", "m23", "m30", "m31", "m32", "m33")]
        return (m[0], m[1], m[4], m[5], m[12], m[13])   # a, b, c, d, tx, ty

    @staticmethod
    def mul(p, c):  # p puis c : point -> c -> p
        a, b, cc, d, tx, ty = p
        a2, b2, c2, d2, tx2, ty2 = c
        return (a * a2 + cc * b2, b * a2 + d * b2, a * c2 + cc * d2, b * c2 + d * d2,
                a * tx2 + cc * ty2 + tx, b * tx2 + d * ty2 + ty)

    @staticmethod
    def length(tl):
        return max((f["I"] + f["DU"] for l in tl["L"] for f in l["FR"]), default=1)

    def collect(self, tl, frame, m, alpha, out, depth=0):
        """Liste (image, matrice, alpha) dans l'ordre de dessin pour l'image `frame` d'une timeline."""
        if depth > 32:
            return
        for layer in reversed(tl["L"]):  # le premier calque est au-dessus
            for fr in layer["FR"]:
                if fr["I"] <= frame < fr["I"] + fr["DU"]:
                    for e in fr["E"]:
                        if "ASI" in e:
                            im = self.parts.get(e["ASI"]["N"])
                            if im is not None:
                                out.append((im, self.mul(m, self.matrix(e["ASI"])), alpha))
                        elif "SI" in e:
                            si = e["SI"]
                            child = self.symbols.get(si.get("SN"))
                            if child is None:
                                continue
                            n = self.length(child)
                            local = frame - fr["I"]
                            ff = int(si.get("FF", 0))
                            lp = si.get("LP", "LP")
                            if si.get("ST") in ("G", "graphic"):
                                f2 = ff if lp == "SF" else (min(ff + local, n - 1) if lp == "PO" else (ff + local) % n)
                            else:  # movieclip : joue sa propre timeline en boucle
                                f2 = local % n
                            a2 = alpha
                            c = si.get("C") or {}
                            if c.get("M") in ("CA", "Alpha"):
                                a2 *= float(c.get("AM", 1))
                            elif "AM" in c:
                                a2 *= float(c["AM"])
                            self.collect(child, f2, self.mul(m, self.matrix(si)), a2, out, depth + 1)
                    break

    def render(self, tl, frame):
        """Image RGBA recadrée + position de son coin haut-gauche dans l'espace du symbole."""
        items = []
        self.collect(tl, frame, (1, 0, 0, 1, 0, 0), 1.0, items)
        if not items:
            return None
        xs, ys = [], []
        for im, (a, b, c, d, tx, ty), _ in items:
            for px, py in ((0, 0), (im.width, 0), (0, im.height), (im.width, im.height)):
                xs.append(a * px + c * py + tx)
                ys.append(b * px + d * py + ty)
        x0, y0 = int(min(xs)) - 1, int(min(ys)) - 1
        w, h = int(max(xs)) + 2 - x0, int(max(ys)) + 2 - y0
        canvas = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        for im, (a, b, c, d, tx, ty), al in items:
            det = a * d - b * c
            if abs(det) < 1e-9:
                continue
            # inverse : coordonnée canevas -> coordonnée de la pièce
            ia, ib, ic, id_ = d / det, -c / det, -b / det, a / det
            ox, oy = tx - x0, ty - y0
            coeffs = (ia, ib, -(ia * ox + ib * oy), ic, id_, -(ic * ox + id_ * oy))
            layer = im.transform((w, h), Image.AFFINE, coeffs, resample=Image.BILINEAR)
            if al < 1:
                layer.putalpha(layer.getchannel("A").point(lambda v: int(v * al)))
            canvas.alpha_composite(layer)
        bb = canvas.getbbox()
        if not bb:
            return None
        return canvas.crop(bb), x0 + bb[0], y0 + bb[1]

    def timeline_for(self, name):
        """Symbole du même nom, sinon étiquette de la timeline principale -> (timeline, début, fin)."""
        tl = self.symbols.get(name)
        if tl is None:
            for k, v in self.symbols.items():
                if k.strip() == name.strip():
                    tl = v
                    break
        if tl is not None:
            return tl, 0, self.length(tl)
        main = self.data["AN"]["TL"]
        for layer in main["L"]:
            for fr in layer["FR"]:
                if fr.get("N") == name:
                    return main, fr["I"], fr["I"] + fr["DU"]
        return None


def convert_animate_character(name, root, folder, scale):
    """Personnage en atlas Animate -> cadres ordinaires (frameOffset/frameWidth communs à tout l'atlas)."""
    print("  personnage (atlas Animate)", name)
    atlas = AnimateAtlas(folder)
    baked = []   # (nom anim, fps, loop, x, y, [(image, gauche, haut)])
    cache = {}
    for an in root.iter("anim"):
        p = an.attrib
        found = atlas.timeline_for(p.get("anim", ""))
        if not found:
            continue
        tl, start, end = found
        nums = parse_indices(p.get("indices"))
        frames_idx = [start + n for n in nums if start + n < end] if nums else list(range(start, end))
        imgs = []
        for f in frames_idx:
            key = (id(tl), f)
            if key not in cache:
                cache[key] = atlas.render(tl, f)
            if cache[key]:
                imgs.append(cache[key])
        if imgs:
            baked.append((p["name"], float(p.get("fps", atlas.fps)), p.get("loop", "false") == "true",
                          float(p.get("x", 0)), float(p.get("y", 0)), imgs))
    if not baked:
        return None
    # boîte commune : coin haut-gauche de tous les cadres = origine des décalages
    minx = min(x for b in baked for _, x, _ in b[5])
    miny = min(y for b in baked for _, _, y in b[5])
    maxx = max(x + im.width for b in baked for im, x, _ in b[5])
    maxy = max(y + im.height for b in baked for im, _, y in b[5])
    ss = SpriteSet("chr_" + re.sub(r"\W", "_", name), S * scale)
    for an_name, fps, loop, ox, oy, imgs in baked:
        idx = []
        for im, x, y in imgs:
            f = {"x": 0, "y": 0, "w": im.width, "h": im.height, "fx": int(minx - x), "fy": int(miny - y),
                 "fw": int(maxx - minx), "fh": int(maxy - miny), "rot": False}
            idx.append(ss.add_frame(im, f))
        # le symbole est dessiné à la position du sprite : le coin de la boîte est donc à (minx, miny)
        ss.add_anim(an_name, fps, loop, (ox - minx) * scale, (oy - miny) * scale, idx)
    return ss, (maxx - minx) / 2, (maxy - miny) / 2


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
    animate = find_asset(f"images/characters/{sprite}/Animation.json")
    if (png is None or sx is None) and animate is None:
        print("  image introuvable pour", name)
        done[name] = None
        return None
    if SKIP_DONE and (OUT / "chars" / f"{name}.spr").exists():
        done[name] = {"icon": a.get("icon", name), "notesprite": a.get("notesprite", "default")}
        return done[name]
    if png is None or sx is None:
        res = convert_animate_character(name, root, animate.parent, scale)
        if res is None:
            print("  atlas Animate vide pour", name)
            done[name] = None
            return None
        ss, ow, oh = res
        frames = [{"fw": ow * 2, "fh": oh * 2}]
    else:
        print("  personnage", name)
        src = load_png(png)
        frames = parse_sparrow(sx)
        ss = SpriteSet("chr_" + re.sub(r"\W", "_", name), S * scale)
        add_character_anims(ss, root, src, frames, scale)
    finish_character(ss, name, a, scale, frames)
    done[name] = ss.meta
    return ss.meta


def add_character_anims(ss, root, src, frames, scale):
    for an in root.iter("anim"):
        p = an.attrib
        if "anim" not in p:
            continue
        sel = select_frames(frames, p["anim"], p.get("indices"))
        if not sel:
            continue
        idx = [ss.add_frame(src, frames[i]) for i in sel]
        # frameOffset de Flixel : appliqué avant l'échelle, donc multiplié par scale
        ss.add_anim(p["name"], float(p.get("fps", 24)), p.get("loop", "false") == "true",
                    float(p.get("x", 0)) * scale, float(p.get("y", 0)) * scale, idx)


def finish_character(ss, name, a, scale, frames):
    ss.meta = {
        "isPlayer": 1 if a.get("isPlayer", "false") == "true" else 0,
        "flipX": 1 if a.get("flipX", "false") == "true" else 0,
        "x": float(a.get("x", 0)), "y": float(a.get("y", 0)),
        "camx": float(a.get("camx", 0)), "camy": float(a.get("camy", 0)),
        "holdTime": float(a.get("holdTime", 4)), "scale": scale,
        "icon": a.get("icon", name),
        "gfDance": 1 if any(n[0] == "danceLeft" for n in ss.anims) else 0,
        "notesprite": a.get("notesprite", "default"),
        "interval": float(a.get("interval", 0)),
        "color": a.get("color", ""),
        # origine Flixel = centre du premier cadre de l'atlas (taille source, non mise à l'échelle)
        "ow": frames[0]["fw"] / 2 if frames else 0, "oh": frames[0]["fh"] / 2 if frames else 0,
        "offs": 1,  # décalages d'animation déjà multipliés par scale
    }
    ss.write(OUT / "chars" / f"{name}.spr")


# ---------------------------------------------------------------- décors

def build_stage_sprite(ssname, src, frames, anims, scale):
    """Sprite de décor -> SpriteSet. anims : [(nom, préfixe, indices, fps, boucle, x, y)].
    Renvoie (ss, origine x, origine y, réduction appliquée pour tenir dans une page 3DS)."""
    ss = SpriteSet(ssname, S * scale)
    fit = 1.0
    ow, oh = src.width / 2, src.height / 2
    if frames:
        ow, oh = frames[0]["fw"] / 2, frames[0]["fh"] / 2
        for an_name, prefix, indices, fps, loop, x, y in anims:
            sel = select_frames(frames, prefix, indices)
            if sel:
                idx = [ss.add_frame(src, frames[i]) for i in sel[:64]]
                ss.add_anim(an_name, fps, loop, x * scale, y * scale, idx)
        if not ss.anims:
            # Codename : sans <anim>, une animation "idle" avec tous les cadres
            ss.add_anim("idle", 24, True, 0, 0, [ss.add_frame(src, f) for f in frames[:64]])
    else:
        # image fixe : réduite davantage si elle dépasse une page 3DS
        w, h = src.width * S * scale, src.height * S * scale
        if w > PAGE - PAD or h > PAGE - PAD:
            fit = min((PAGE - PAD) / w, (PAGE - PAD) / h)
            ss.scale *= fit
        ss.add_anim("idle", 24, True, 0, 0, [ss.add_image(src)])
    return ss, ow, oh, fit


def safe_num(expr):
    expr = (expr or "").strip()
    if not expr or not re.fullmatch(r"[-+*/(). \d]+", expr):
        return None
    try:
        return float(eval(expr, {"__builtins__": {}}))
    except Exception:
        return None


class ScriptSprite:
    """État Flixel minimal d'un sprite créé par script (position, échelle, hitbox, défilement...)."""

    def __init__(self, x, y):
        self.x, self.y = x, y
        self.path = None       # image (sans extension), relative à images/
        self.atlas = False
        self.anims = []
        self.play = None
        self.sx = self.sy = 1.0
        self.fw = self.fh = 0.0
        self.width = self.height = 0.0
        self.offx = self.offy = 0.0
        self.scx = self.scy = 1.0
        self.alpha = 1.0
        self.flip = False
        self.visible = True
        self.hud = False
        self.tile = 0          # FlxBackdrop : 1 répété en X, 2 en Y, 3 les deux
        self.rect = None       # makeGraphic : couleur 0xAARRGGBB (rectangle uni, sans image)
        self.shader = None     # nom du shader (CustomShader / FunkinShader) appliqué au sprite
        self.layer = 0         # 1 : caméra ajoutée par le script, dessinée au-dessus du jeu sans défilement
        self.cam = None        # nom de cette caméra
        self.spx = self.spy = 0.0
        self.vx = self.vy = 0.0

    def load(self, path, atlas):
        self.path, self.atlas = path, atlas
        png = find_asset(f"images/{path}.png")
        if png is None:
            self.path = None
            return
        sx = find_asset(f"images/{path}.xml") if atlas else None
        if sx:
            fr = parse_sparrow(sx)
            self.fw, self.fh = (fr[0]["fw"], fr[0]["fh"]) if fr else (0, 0)
        else:
            self.atlas = False
            with Image.open(png) as im:
                self.fw, self.fh = im.size
        self.width, self.height = self.fw, self.fh


CHARNAME = {"boyfriend": "boyfriend", "bf": "boyfriend", "dad": "dad", "gf": "girlfriend", "girlfriend": "girlfriend"}
EXPR = r"([^,()]+(?:\([^()]*\)[^,()]*)*)"


def sprite_statement(o, rest):
    """Applique une instruction 'nom.<rest>' à un ScriptSprite."""
    E = EXPR
    if (g := re.match(r"loadGraphic\(Paths\.image\(['\"](.+?)['\"]\)", rest)):
        o.load(g.group(1), False)
    elif (g := re.match(r"frames = Paths\.getSparrowAtlas\(['\"](.+?)['\"]\)", rest)):
        o.load(g.group(1), True)
    elif (g := re.match(r"animation\.addByPrefix\(['\"](.+?)['\"] ?, ?['\"](.+?)['\"](?: ?, ?" + NUM + r")?(?: ?, ?(true|false))?", rest)):
        o.anims.append((g.group(1), g.group(2), None, float(g.group(3) or 24), g.group(4) != "false", 0, 0))
    elif (g := re.match(r"animation\.addByIndices\(['\"](.+?)['\"] ?, ?['\"](.+?)['\"] ?, ?\[(.*?)\][^,]*,[^,]*(?:, ?" + NUM + r")?(?: ?, ?(true|false))?", rest)):
        o.anims.append((g.group(1), g.group(2), g.group(3), float(g.group(4) or 24), g.group(5) == "true", 0, 0))
    elif (g := re.match(r"animation\.play\(['\"](.+?)['\"]", rest)):
        o.play = g.group(1)
    elif (g := re.fullmatch(r"scale\.set\(" + E + r"(?:, ?" + E + r")?\)", rest)):
        a, b = safe_num(g.group(1)), safe_num(g.group(2)) if g.group(2) else None
        if a is not None:
            o.sx, o.sy = a, b if b is not None else a
    elif (g := re.fullmatch(r"scale\.([xy]) ?= ?" + E, rest)) and safe_num(g.group(2)) is not None:
        setattr(o, "s" + g.group(1), safe_num(g.group(2)))
    elif (g := re.fullmatch(r"setGraphicSize\(" + E + r"(?:, ?" + E + r")?\)", rest)):
        w, h = safe_num(g.group(1)), (safe_num(g.group(2)) if g.group(2) else 0) or 0
        if w is not None and o.fw and o.fh:
            o.sx = w / o.fw if w > 0 else h / o.fh
            o.sy = h / o.fh if h > 0 else o.sx
    elif rest.startswith("updateHitbox("):
        o.width, o.height = o.fw * abs(o.sx), o.fh * abs(o.sy)
        o.offx, o.offy = -0.5 * (o.width - o.fw), -0.5 * (o.height - o.fh)
    elif (g := re.fullmatch(r"screenCenter\((.*)\)", rest)):
        ax = g.group(1).replace("FlxAxes.", "").strip()
        if ax in ("", "XY", "X"):
            o.x = (1280 - o.width) / 2
        if ax in ("", "XY", "Y"):
            o.y = (720 - o.height) / 2
    elif (g := re.fullmatch(r"scrollFactor\.set\(" + E + r"(?:, ?" + E + r")?\)", rest)):
        a, b = safe_num(g.group(1)), safe_num(g.group(2)) if g.group(2) else None
        if a is not None:
            o.scx, o.scy = a, b if b is not None else a
    elif (g := re.fullmatch(r"scrollFactor\.([xy]) ?= ?" + E, rest)) and safe_num(g.group(2)) is not None:
        setattr(o, "sc" + g.group(1), safe_num(g.group(2)))
    elif (g := re.fullmatch(r"setPosition\(" + E + r", ?" + E + r"\)", rest)):
        a, b = safe_num(g.group(1)), safe_num(g.group(2))
        if a is not None and b is not None:
            o.x, o.y = a, b
    elif (g := re.fullmatch(r"([xy]) ?([-+]?=) ?" + E, rest)) and safe_num(g.group(3)) is not None:
        v = safe_num(g.group(3))
        cur = getattr(o, g.group(1))
        setattr(o, g.group(1), v if g.group(2) == "=" else cur + v if g.group(2) == "+=" else cur - v)
    elif (g := re.fullmatch(r"alpha ?= ?" + E, rest)) and safe_num(g.group(1)) is not None:
        o.alpha = safe_num(g.group(1))
    elif rest.replace(" ", "") == "flipX=true":
        o.flip = True
    elif rest.replace(" ", "") == "visible=false":
        o.visible = False
    elif rest.replace(" ", "") == "visible=true":
        o.visible = True
    elif (g := re.fullmatch(r"velocity\.set\(" + E + r"(?:, ?" + E + r")?\)", rest)):
        a, b = safe_num(g.group(1)), safe_num(g.group(2)) if g.group(2) else None
        if a is not None:
            o.vx, o.vy = a, b if b is not None else a
    elif (g := re.fullmatch(r"velocity\.([xy]) ?= ?" + E, rest)) and safe_num(g.group(2)) is not None:
        setattr(o, "v" + g.group(1), safe_num(g.group(2)))
    elif (g := re.fullmatch(r"makeGraphic\(" + E + r", ?" + E + r"(?:, ?" + E + r")?\)", rest)):
        w, h = safe_num(g.group(1)), safe_num(g.group(2))
        col = (g.group(3) or "0xFFFFFFFF").strip()
        named = {"FlxColor.BLACK": 0xFF000000, "FlxColor.WHITE": 0xFFFFFFFF, "FlxColor.RED": 0xFFFF0000}
        c = named.get(col, int(col, 16) if re.fullmatch(r"0x[0-9a-fA-F]+", col) else None)
        if w and h and c is not None:
            if c <= 0xFFFFFF:
                c |= 0xFF000000
            o.rect, o.path, o.atlas = c, None, False
            o.fw, o.fh = o.width, o.height = w, h
    elif re.match(r"blend ?=", rest):
        o.hud = True  # mode de mélange (masque, addition...) : écarté plutôt que dessiné en opaque
    elif rest.startswith("cameras") or rest.startswith("camera ="):
        if re.search(r"camHUD|camNewHUD|camOther|camGameOver", rest):
            o.hud = True
        elif "camGame" not in rest:
            o.layer = 1
            cm = re.search(r"\[\s*(\w+)", rest)
            o.cam = cm.group(1) if cm else None


def run_stage_script(src, objects, order, env=None):
    """Exécute les créations de sprites simples de create/postCreate.
    objects : nom -> ScriptSprite ou clé de sprite XML ; order : liste ('char', tag) / ('spr', clé).
    env : valeurs connues remplacées dans les expressions (ex. "dad.x" -> 50)."""
    src = strip_comments(src)
    idxvars = {}
    shaders = {}
    env = env or {}
    envrx = re.compile(r"(?<![\w.])(?:" + "|".join(re.escape(k) for k in sorted(env, key=len, reverse=True)) +
                       r")(?![\w.])") if env else None

    def key_of(name):
        if name in CHARNAME:
            return ("char", CHARNAME[name])
        if name in objects:
            return ("spr", name)
        return None

    def put(k, before):
        if k in order:
            order.remove(k)
        if before in order:
            order.insert(order.index(before), k)
        else:
            order.append(k)

    for fn in ("create", "postCreate"):
        for st in executed(function_body(src, fn), {}):
            st = re.sub(r"\s+", " ", st).strip()
            st = re.sub(r"^var ", "", st)
            if envrx:
                st = envrx.sub(lambda m: repr(env[m.group(0)]), st)
            m = re.match(r"(\w+)(?: ?: ?\w+)? = new (?:CustomShader|FunkinShader)\((?:Assets\.getText\(Paths\.fragShader\()?['\"](.+?)['\"]", st)
            if m:
                shaders[m.group(1)] = m.group(2)
                continue
            m = re.fullmatch(r"(\w+)\.shader ?= ?(\w+)", st)
            if m and isinstance(objects.get(m.group(1)), ScriptSprite):
                objects[m.group(1)].shader = shaders.get(m.group(2))
                continue
            m = re.fullmatch(r"(\w+)(?: ?: ?\w+)? = members\.indexOf\((\w+)\)", st)
            if m:
                idxvars[m.group(1)] = key_of(m.group(2))
                continue
            # FlxBackdrop(graphic, axes, spacingX, spacingY) : image répétée sur tout l'écran
            m = re.match(r"(\w+)(?: ?: ?\w+)? = new FlxBackdrop\(Paths\.image\(['\"](.+?)['\"]\)(.*)\)$", st)
            if m:
                o = ScriptSprite(0, 0)
                o.load(m.group(2), False)
                rest = [v.strip() for v in m.group(3).split(",") if v.strip()]
                ax = rest[0].replace("FlxAxes.", "") if rest else "XY"
                o.tile = {"X": 1, "Y": 2, "XY": 3}.get(ax, 3)
                if len(rest) > 1 and safe_num(rest[1]) is not None:
                    o.spx = safe_num(rest[1])
                if len(rest) > 2 and safe_num(rest[2]) is not None:
                    o.spy = safe_num(rest[2])
                objects[m.group(1)] = o
                continue
            m = re.match(r"(\w+)(?: ?: ?\w+)? = new (?:FunkinSprite|FlxSprite)\((.*?)\)(.*)$", st)
            if m:
                args = [safe_num(v) for v in m.group(2).split(",")] if m.group(2).strip() else []
                o = ScriptSprite(args[0] if args and args[0] is not None else 0,
                                 args[1] if len(args) > 1 and args[1] is not None else 0)
                objects[m.group(1)] = o
                # appels chaînés : new FlxSprite(...).loadGraphic(...)
                for call in re.findall(r"\.(\w+\(.*?\)\)?)(?=\.|$)", m.group(3)):
                    sprite_statement(o, call)
                continue
            m = re.match(r"(\w+)\.(.*)$", st)
            if m and isinstance(objects.get(m.group(1)), ScriptSprite):
                sprite_statement(objects[m.group(1)], m.group(2))
                continue
            m = re.fullmatch(r"add\((\w+)\)", st)
            if m and isinstance(objects.get(m.group(1)), ScriptSprite):
                put(("spr", m.group(1)), None)
                continue
            m = re.fullmatch(r"insert\((.+?), ?(\w+)\)", st)
            if m and key_of(m.group(2)):
                where = m.group(1).strip()
                g = re.fullmatch(r"members\.indexOf\((\w+)\)", where)
                before = key_of(g.group(1)) if g else idxvars.get(where)
                put(key_of(m.group(2)), before)
                continue
            m = re.fullmatch(r"remove\((\w+)\)", st)
            if m and m.group(1) in CHARNAME:
                k = ("char", CHARNAME[m.group(1)])
                if k in order:
                    order.remove(k)


def item(name, x, y, scx=1.0, scy=1.0, sc=1.0, alpha=1.0, flip=0, **kw):
    """Élément de décor (format FSTG v3)."""
    d = {"name": name, "x": x, "y": y, "scx": scx, "scy": scy, "sc": sc, "alpha": alpha, "flip": flip,
         "tile": (0, 0, 0, 0, 0), "kind": 0, "color": 0, "w": 0.0, "h": 0.0, "layer": 0, "before": 255, "anchor": -1.0}
    d.update(kw)
    return d


CHAR_CODE = {"boyfriend": "bf", "dad": "dad", "girlfriend": "gf"}
BEFORE_CODE = {"boyfriend": 0, "dad": 1, "girlfriend": 2}


def pack_item(r, char=None):
    """98 octets : genre (0 sprite, 1 perso, 2 rectangle), nom, position, défilement, échelle, alpha, miroir,
    répétition (FlxBackdrop), couleur et taille (rectangle), calque, perso devant lequel insérer, ancre de temps."""
    if char is not None:
        return struct.pack("<B", 1) + fstr(CHAR_CODE[char]) + bytes(98 - 33)
    b = struct.pack("<B", r["kind"]) + fstr(r["name"])
    b += struct.pack("<ffffffB", r["x"], r["y"], r["scx"], r["scy"], r["sc"], r["alpha"], r["flip"])
    b += struct.pack("<Bxxxffff", *r["tile"])
    b += struct.pack("<IffBBxxf", r["color"] & 0xFFFFFFFF, r["w"], r["h"], r["layer"], r["before"], r["anchor"])
    assert len(b) == 98
    return b


def build_script_items(objects, order, prefix, first, keep=(), windows=None, shader_cams=()):
    """Sprites créés par un script -> éléments de décor. keep : variables gardées même invisibles au départ
    (un événement les fait apparaître) ; windows : variable -> (début, fin) en ms pour les shaders pré-calculés."""
    import shaders as shd
    records = {}
    for var, o in objects.items():
        if not isinstance(o, ScriptSprite) or ("spr", var) not in order:
            continue
        hidden = not o.visible or o.alpha <= 0.01
        rendered = o.shader in shd.RENDERERS and windows and var in windows
        shaded_cam = o.layer and o.cam in shader_cams and not rendered
        if o.hud or shaded_cam or (hidden and var not in keep) or (o.path is None and o.rect is None):
            order.remove(("spr", var))
            continue
        nm = f"{prefix}{first + len(records)}"
        sx, sy = abs(o.sx) or 1.0, abs(o.sy) or abs(o.sx) or 1.0
        alpha = min(max(o.alpha, 0.0), 1.0)
        hid = 0 if o.visible else 2  # bit 1 de l'octet miroir : caché au départ (visible = false)
        if rendered:
            # shader pré-calculé : images plein écran jouées à la position de la chanson
            t0, t1 = windows[var]
            ss = SpriteSet(nm, 1.0)
            fps, w, h = 10, 192, 108
            n = int((t1 - t0) / 1000 * fps) + 1
            if SKIP_DONE and (OUT / "stages" / f"{nm}.spr").exists():
                records[var] = item(nm, 0, 0, 0, 0, 400.0 / w, alpha, 0, layer=1, anchor=float(t0))
                continue
            print(f"    shader {o.shader} pré-calculé : {n} images {w}x{h}")
            ss.add_anim("idle", fps, False, 0, 0, [ss.add_image(shd.RENDERERS[o.shader](t0 / 1000 + 2.0 + k / fps, w, h))
                                                   for k in range(n)])
            ss.write(OUT / "stages" / f"{nm}.spr", "etc1")
            records[var] = item(nm, 0, 0, 0, 0, 400.0 / w, alpha, 0, layer=1, anchor=float(t0))
            continue
        if o.rect is not None:
            # rectangle uni : taille finale, mise à l'échelle autour du centre comme Flixel
            w, h = o.fw * sx, o.fh * sy
            px = o.x - o.offx + o.fw / 2 * (1 - sx)
            py = o.y - o.offy + o.fh / 2 * (1 - sy)
            records[var] = item("rect_" + nm, px, py, o.scx, o.scy, 1.0, alpha * ((o.rect >> 24) / 255), hid,
                                kind=2, color=o.rect, w=w, h=h, layer=o.layer)
            print(f"    rectangle de script {var} #{o.rect:08X}")
            continue
        png = find_asset(f"images/{o.path}.png")
        sxml = find_asset(f"images/{o.path}.xml") if o.atlas else None
        frames = parse_sparrow(sxml) if sxml else None
        anims = sorted(o.anims, key=lambda t: t[0] != o.play) if o.play else list(o.anims)
        s = sx
        try:
            img = load_png(png)
            if frames is None and abs(sy - s) > 1e-3:
                # échelle non uniforme : l'image est étirée d'avance, le SpriteSet garde une échelle unique
                img = img.resize((img.width, max(1, round(img.height * sy / s))), Image.LANCZOS)
            ss, ow, oh, fit = build_stage_sprite(nm, img, frames, anims, s)
            if frames is None:
                oh = o.fh / 2
            ss.write(OUT / "stages" / f"{ss.name}.spr")
        except Exception as ex:
            print(f"    sprite de script {var} ignoré ({ex})")
            order.remove(("spr", var))
            continue
        # écran = x - offset + origin + scale * (pixel - origin)
        px = o.x - o.offx + ow * (1 - s)
        py = o.y - o.offy + oh * (1 - (abs(o.sy) or s))
        records[var] = item(ss.name, px, py, o.scx, o.scy, 1.0 / fit, alpha, (1 if o.flip else 0) | hid,
                            tile=(o.tile, o.spx, o.spy, o.vx, o.vy), layer=o.layer)
        print(f"    sprite de script {var} ({o.path})")
    return records


def convert_stage(name, done):
    if name in done:
        return
    done[name] = True
    if SKIP_DONE and (OUT / "stages" / f"{name}.stg").exists():
        return
    xml = find_asset(f"data/stages/{name}.xml")
    hx = find_asset(f"data/stages/{name}.hx")
    if xml is None and hx is None:
        print("  décor introuvable:", name)
        return
    print("  décor", name)
    if xml is not None:
        root = ET.fromstring(re.sub(r"<!DOCTYPE[^>]*>", "", xml.read_text(encoding="utf-8-sig")))
    else:
        root = ET.fromstring("<stage/>")
    folder = root.attrib.get("folder", "")
    if folder and not folder.endswith("/"):
        folder += "/"
    zoom = float(root.attrib.get("zoom", 1.05))
    records = {}  # clé -> (nom spr, x, y, scx, scy, 1/fit, alpha, flip)
    chars = {"boyfriend": (770, 100, 1, 1), "dad": (100, 100, 1, 1), "girlfriend": (400, 130, 0.95, 0.95)}
    order = []    # ('char', nom) ou ('spr', clé)
    objects = {}  # nom de sprite (attribut name du XML ou variable du script) -> ScriptSprite / clé
    sname = re.sub(r"\W", "_", name)
    for el in root:
        tag = el.tag
        a = el.attrib
        tag = {"bf": "boyfriend", "player": "boyfriend", "gf": "girlfriend", "opponent": "dad"}.get(tag, tag)
        if tag in chars:
            base = chars[tag]
            sc = a.get("scroll")
            chars[tag] = (float(a.get("x", base[0])), float(a.get("y", base[1])),
                          float(sc if sc else a.get("scrollx", base[2])), float(sc if sc else a.get("scrolly", base[3])))
            order.append(("char", tag))
            continue
        if tag not in ("sprite", "spr", "sparrow"):
            continue
        spr = a.get("sprite")
        if not spr:
            continue
        png = find_asset(f"images/{folder}{spr}.png")
        if png is None:
            continue
        sxml = find_asset(f"images/{folder}{spr}.xml")
        scale = float(a.get("scale", 1))
        anims = [(c.get("name", "idle"), c.get("anim", ""), c.get("indices"), float(c.get("fps", 24)),
                  c.get("loop", "true") != "false", float(c.get("x", 0)), float(c.get("y", 0)))
                 for c in el if c.tag == "anim"]
        ss, ow, oh, fit = build_stage_sprite(f"stg_{sname}_{len(records)}", load_png(png),
                                             parse_sparrow(sxml) if sxml else None, anims, scale)
        ss.write(OUT / "stages" / f"{ss.name}.spr")
        sc = a.get("scroll")
        scx = float(sc if sc else a.get("scrollx", 1))
        scy = float(sc if sc else a.get("scrolly", 1))
        # Flixel met à l'échelle autour du centre (origin) ; updateHitbox="true" annule ce décalage
        px, py = float(a.get("x", 0)), float(a.get("y", 0))
        if a.get("updateHitbox") != "true":
            px += ow * (1 - scale)
            py += oh * (1 - scale)
        key = a.get("name") or f"#{len(records)}"
        records[key] = item(ss.name, px, py, scx, scy, 1.0 / fit, float(a.get("alpha", 1)),
                            1 if a.get("flipX") == "true" else 0)
        objects[key] = key
        order.append(("spr", key))
    if hx is not None:
        src = hx.read_text(encoding="utf-8-sig", errors="replace")
        run_stage_script(src, objects, order)
        cams = set(re.findall(r"(\w+)\.(?:addShader|setFilters)\(", src))
        records.update(build_script_items(objects, order, f"stg_{sname}_", len(records),
                                          shown_by_events(step_events(src)), None, cams))
    order = [k for k in order if k[0] == "char" or k[1] in records]
    b = bytearray(b"FSTG") + struct.pack("<If", 3, zoom)
    for k in ("boyfriend", "dad", "girlfriend"):
        b += struct.pack("<ffff", *chars[k])
    b += struct.pack("<I", len(order))
    for kind, v in order:
        b += pack_item(records[v] if kind == "spr" else None, v if kind == "char" else None)
    # variable du script -> nom de l'élément, pour les événements des chansons (fix.txt "ev ...")
    (OUT / "stages").mkdir(parents=True, exist_ok=True)
    (OUT / "stages" / f"{name}.vars").write_text(
        "".join(f"{k} {records[k]['name']}\n" for kind, k in order if kind == "spr"), encoding="utf-8")
    (OUT / "stages" / f"{name}.stg").write_bytes(b)


# ---------------------------------------------------------------- scripts HScript (cas simples)

WHO = {"boyfriend": 0, "bf": 0, "dad": 1, "gf": 2, "girlfriend": 2}
NUM = r"(-?\d+(?:\.\d+)?)"


def strip_comments(src):
    src = re.sub(r"/\*.*?\*/", "", src, flags=re.S)
    return re.sub(r"//[^\n]*", "", src)


def function_body(src, name):
    m = re.search(r"function\s+" + name + r"\s*\([^)]*\)[^{]*\{", src)
    if not m:
        return ""
    depth, i = 1, m.end()
    while i < len(src) and depth:
        depth += {"{": 1, "}": -1}.get(src[i], 0)
        i += 1
    return src[m.end():i - 1]


def _balanced(src, i, open_ch, close_ch):
    """src[i] == open_ch -> indice juste après la fermeture correspondante."""
    depth = 0
    while i < len(src):
        if src[i] == open_ch:
            depth += 1
        elif src[i] == close_ch:
            depth -= 1
            if depth == 0:
                return i + 1
        elif src[i] in "'\"":
            q = src[i]
            i += 1
            while i < len(src) and src[i] != q:
                i += 1
        i += 1
    return len(src)


def eval_condition(cond, ctx):
    """Conditions simples sur le nom de la chanson / des persos ; None si on ne sait pas évaluer."""
    c = cond
    for k, v in ctx.items():
        c = c.replace(k, repr(v))
    c = c.replace("||", " or ").replace("&&", " and ").replace("|", " or ")
    c = re.sub(r"!(?!=)", " not ", c)
    bare = re.sub(r"'[^']*'|\"[^\"]*\"", "''", c)
    if any(w not in ("or", "and", "not", "True", "False") for w in re.findall(r"[A-Za-z_]\w*", bare)):
        return None
    try:
        return bool(eval(c, {"__builtins__": {}}))
    except Exception:
        return None


def expand_chain(st):
    """Affectation en chaîne de propriétés -> une instruction par cible."""
    parts = [x.strip() for x in st.split("=")]
    if len(parts) > 2 and all(re.fullmatch(r"[\w.]+", x) for x in parts[:-1]) and "==" not in st:
        return [f"{x} = {parts[-1]}" for x in parts[:-1]]
    return [st]


def executed(body, ctx):
    """Instructions exécutées d'un bloc : suit les if/else dont la condition est connue (ctx),
    ignore boucles, switch, fonctions locales et conditions inconnues."""
    out, i, n = [], 0, len(body)
    while i < n:
        while i < n and body[i] in " \t\r\n;":
            i += 1
        if i >= n:
            break
        m = re.match(r"if\s*\(", body[i:])
        if m:
            p = i + m.end() - 1
            end = _balanced(body, p, "(", ")")
            ok = eval_condition(body[p + 1:end - 1], ctx)
            j = end
            while j < n and body[j] in " \t\r\n":
                j += 1
            if j < n and body[j] == "{":
                bend = _balanced(body, j, "{", "}")
                block = body[j + 1:bend - 1]
            else:
                bend = body.find(";", j)
                bend = n if bend < 0 else bend + 1
                block = body[j:bend]
            if ok:
                out += executed(block, ctx)
            i = bend
            em = re.match(r"\s*else\b", body[i:])
            if em:
                k = i + em.end()
                while k < n and body[k] in " \t\r\n":
                    k += 1
                if re.match(r"if\s*\(", body[k:]):
                    # else if : toute la suite de la chaîne n'est prise que si la condition est fausse
                    if ok is False:
                        out += executed(body[k:], ctx)
                        return out
                    # condition vraie ou inconnue : saute le reste de la chaîne
                    body, n, i = body[k:], len(body) - k, 0
                    pm = re.match(r"if\s*\(", body)
                    j = _balanced(body, pm.end() - 1, "(", ")")
                    while j < n and body[j] in " \t\r\n":
                        j += 1
                    i = _balanced(body, j, "{", "}") if j < n and body[j] == "{" else body.find(";", j) + 1
                    while True:
                        em = re.match(r"\s*else\b\s*", body[i:])
                        if not em:
                            break
                        k = i + em.end()
                        if re.match(r"if\s*\(", body[k:]):
                            pm = re.match(r"if\s*\(", body[k:])
                            j = _balanced(body, k + pm.end() - 1, "(", ")")
                            while j < n and body[j] in " \t\r\n":
                                j += 1
                            i = _balanced(body, j, "{", "}") if j < n and body[j] == "{" else body.find(";", j) + 1
                        else:
                            i = _balanced(body, k, "{", "}") if body[k:k + 1] == "{" else body.find(";", k) + 1
                            break
                elif k < n and body[k] == "{":
                    bend = _balanced(body, k, "{", "}")
                    if ok is False:
                        out += executed(body[k + 1:bend - 1], ctx)
                    i = bend
                else:
                    bend = body.find(";", k)
                    bend = n if bend < 0 else bend + 1
                    if ok is False:
                        out += executed(body[k:bend], ctx)
                    i = bend
            continue
        # instruction ordinaire ; un bloc { } (for, switch, fonction locale...) est sauté en entier
        j, depth = i, 0
        while j < n:
            ch = body[j]
            if ch in "'\"":
                q = ch
                j += 1
                while j < n and body[j] != q:
                    j += 1
            elif ch in "({[":
                depth += 1
            elif ch in ")}]":
                depth -= 1
                if depth == 0 and ch == "}" and not re.match(r"\s*[.,)]", body[j + 1:]):
                    j += 1
                    break
            elif ch == ";" and depth == 0:
                break
            j += 1
        st = body[i:j].strip()
        # accolades acceptées seulement dans un appel (FlxTween.tween(x, {alpha: 1}, ...)), pas pour un bloc
        braces_ok = "{" not in st or re.match(r"[\w.]+\(", st) and st.index("(") < st.index("{")
        if st and braces_ok and not re.match(r"(for|while|switch|else|function|return|if)\b", st):
            out += expand_chain(st)
        i = j + 1
    return out


def top_level(body):
    """Instructions qui ne sont dans aucun bloc (if, for, switch, fonction locale...)."""
    out, depth, cur = [], 0, ""
    for ch in body:
        if ch == "{":
            depth += 1
            cur = ""
        elif ch == "}":
            depth -= 1
            cur = ""
        elif ch == ";":
            if depth == 0:
                out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    # une instruction précédée de "if (...)" sans accolades n'est pas toujours exécutée
    return [s for s in out if not re.match(r"(if|else|for|while)\b", s)]


def script_fixes(src, ctx=None):
    """Repositionnements, zoom et caméra fixes faits par un script de décor ou de chanson."""
    src = strip_comments(src)
    phases = {}
    for fn in ("create", "postCreate"):
        res = phases.setdefault(fn, [])
        for st in executed(function_body(src, fn), ctx or {}):
            st = re.sub(r"\s+", " ", st)
            m = re.fullmatch(r"(\w+)\.flipX ?= ?(true|false)", st)
            if m and m.group(1) in WHO:
                res.append(f"flip {WHO[m.group(1)]} {1 if m.group(2) == 'true' else 0}")
                continue
            m = re.fullmatch(r"(\w+)\.setPosition\( ?" + NUM + r" ?, ?" + NUM + r" ?\)", st)
            if m and m.group(1) in WHO:
                res.append(f"pos {WHO[m.group(1)]} {m.group(2)} {m.group(3)}")
                continue
            m = re.fullmatch(r"(\w+)\.(cameraOffset\.)?([xy]) ?([-+]?=) ?" + NUM, st)
            if m and m.group(1) in WHO:
                kind = ("cam" if m.group(2) else "") + m.group(3)
                op = {"=": "set", "+=": "add", "-=": "sub"}[m.group(4)]
                res.append(f"{op} {kind} {WHO[m.group(1)]} {m.group(5)}")
                continue
            m = re.fullmatch(r"(\w+)\.scrollFactor\.set\( ?" + NUM + r"(?: ?, ?" + NUM + r")? ?\)", st)
            if m and m.group(1) in WHO:
                res.append(f"scroll {WHO[m.group(1)]} {m.group(2)} {m.group(3) or m.group(2)}")
                continue
            ops = alpha_assign(st)
            if ops:
                for who, v in ops:
                    if who in WHO:
                        res.append(f"alpha {WHO[who]} {v}")
                continue
            # color = FlxColor.fromHSL(hue, saturation, L, a) sur un perso blanc : gris L, l'image est multipliée par L
            m = re.fullmatch(r"(\w+)\.color ?= ?FlxColor\.fromHSL\(.*?, ?" + NUM + r" ?(?:, ?" + NUM + r" ?)?\)", st)
            if m and m.group(1) in WHO:
                res.append(f"tint {WHO[m.group(1)]} {m.group(2)}")
                continue
            m = re.fullmatch(r"(\w+)\.visible ?= ?false", st)
            if m and m.group(1) in WHO:
                res.append(f"hide {WHO[m.group(1)]}")
                continue
            m = re.fullmatch(r"defaultCamZoom ?= ?" + NUM, st)
            if m:
                res.append(f"zoom {m.group(1)}")
    # zoom fixé au compte à rebours (souvent dans le onComplete d'un tween) : il gagne sur create
    res = phases.setdefault("countdown", [])
    for fn in ("onCountdown", "onStartCountdown", "onSongStart"):
        z = re.findall(r"defaultCamZoom\s*=\s*" + NUM, function_body(src, fn))
        if z:
            res.append(f"zoom {z[-1]}")
    # caméra fixe par cible : switch (curCameraTarget) { case 0: camFollow.setPosition(x, y); ... }
    for fn in ("update", "postUpdate"):
        body = function_body(src, fn)
        sw = re.search(r"switch\s*\(\s*curCameraTarget\s*\)\s*\{", body)
        if not sw:
            continue
        for m in re.finditer(r"case\s+(\d+)\s*:\s*camFollow\.setPosition\(\s*" + NUM + r"\s*,\s*" + NUM + r"\s*\)",
                             body[sw.end():]):
            res.append(f"camfix {m.group(1)} {m.group(2)} {m.group(3)}")
        break
    return phases


def alpha_assign(st):
    """'a.alpha = b.alpha = 0.5' -> [('a', 0.5), ('b', 0.5)]"""
    parts = [x.strip() for x in st.split("=")]
    if len(parts) < 2 or not all(re.fullmatch(r"\w+\.alpha", x) for x in parts[:-1]):
        return None
    v = safe_num(parts[-1])
    return None if v is None else [(x[:-6], v) for x in parts[:-1]]


FLASH_COLORS = {"WHITE": 0xFFFFFFFF, "BLACK": 0xFF000000, "RED": 0xFFFF0000}


def timeline_ops(st):
    """Instruction d'un stepHit -> opérations ('alpha', var, valeur, durée s) / ('tint', var, L) / ('flash', couleur, durée)."""
    st = re.sub(r"\s+", " ", st).strip()
    ops = alpha_assign(st)
    if ops:
        return [("alpha", v, x, 0.0) for v, x in ops]
    m = re.fullmatch(r"(dad|boyfriend|bf|gf|girlfriend) ?= ?(\w+)", st)
    if m:  # PlayState.set_dad : la ligne de notes est chantée par ce perso
        return [("assign", m.group(1), m.group(2))]
    m = re.match(r"FlxTween\.tween\((\w+), ?\{([^}]*)\}, ?([\d.]+)", st)
    if m:
        out = []
        props = dict(re.findall(r"\b(\w+) ?: ?([^,}]+)", m.group(2)))
        if "alpha" in props and safe_num(props["alpha"]) is not None:
            out.append(("alpha", m.group(1), safe_num(props["alpha"]), float(m.group(3))))
        x, y = safe_num(props.get("x")), safe_num(props.get("y"))
        if x is not None or y is not None:
            out.append(("move", m.group(1), x, y, float(m.group(3))))
        return out
    m = re.fullmatch(r"(\w+)\.setPosition\( ?" + NUM + r" ?, ?" + NUM + r" ?\)", st)
    if m:
        return [("move", m.group(1), float(m.group(2)), float(m.group(3)), 0.0)]
    m = re.fullmatch(r"(\w+)\.([xy]) ?= ?" + NUM, st)
    if m:
        v = float(m.group(3))
        return [("move", m.group(1), v if m.group(2) == "x" else None, v if m.group(2) == "y" else None, 0.0)]
    m = re.fullmatch(r"(\w+)\.flipX ?= ?(true|false)", st)
    if m:
        return [("flip", m.group(1), m.group(2) == "true")]
    m = re.fullmatch(r"(\w+)\.visible ?= ?(true|false)", st)
    if m:
        return [("vis", m.group(1), m.group(2) == "true")]
    m = re.fullmatch(r"(\w+)\.color ?= ?FlxColor\.fromHSL\(.*?, ?" + NUM + r" ?(?:, ?" + NUM + r" ?)?\)", st)
    if m:
        return [("tint", m.group(1), float(m.group(2)))]
    # colorTransform.xxxOffset = 255 (silhouette blanche) : teinte > 1 = fondu vers le blanc (2 = blanc pur)
    m = re.fullmatch(r"(\w+)\.colorTransform\.\w+Offset ?= ?(?:\w+\.colorTransform\.\w+Offset ?= ?)*" + NUM, st)
    if m:
        return [("tint", m.group(1), 1.0 + min(max(float(m.group(2)), 0.0), 255.0) / 255.0)]
    m = re.match(r"(?:camGame|FlxG\.camera)\.flash\((?:FlxColor\.(\w+)|(0x[0-9A-Fa-f]+))?(?: ?, ?([\d.]+))?", st)
    if m:
        c = FLASH_COLORS.get(m.group(1) or "", int(m.group(2), 16) if m.group(2) else 0xFFFFFFFF)
        return [("flash", c, float(m.group(3) or 1))]
    return []


def step_events(src):
    """stepHit/beatHit : switch (curStep) { case N: ... } et if (curStep == N) {...} -> [(pas, opération)]."""
    src = strip_comments(src)
    out = []
    body = function_body(src, "onSongStart")
    for st in executed(body, {}):
        for op in timeline_ops(st):
            out.append((0, op))
    # new FlxTimer().start(3, function(tmr) { ... }) : instructions retardées (pas = ("ms", délai))
    for m in re.finditer(r"new FlxTimer\(\)\.start\(\s*" + NUM + r"\s*,\s*function\s*\([^)]*\)\s*\{", body):
        seg = body[m.end():_balanced(body, m.end() - 1, "{", "}") - 1]
        for st in executed(seg, {}):
            for op in timeline_ops(st):
                out.append((("ms", float(m.group(1)) * 1000), op))
    for fn, var, mul in (("stepHit", "curStep", 1), ("beatHit", "curBeat", 4)):
        body = function_body(src, fn)
        if not body:
            continue
        for sw in re.finditer(r"switch\s*\(\s*" + var + r"\s*\)\s*\{", body):
            block = body[sw.end():_balanced(body, sw.end() - 1, "{", "}") - 1]
            labels = list(re.finditer(r"\bcase\s+([\d,\s]+):|\bdefault\s*:", block))
            for i, lb in enumerate(labels):
                if not lb.group(1):
                    continue
                seg = block[lb.end():labels[i + 1].start() if i + 1 < len(labels) else len(block)]
                for st in executed(seg, {}):
                    for op in timeline_ops(st):
                        for n in re.findall(r"\d+", lb.group(1)):
                            out.append((int(n) * mul, op))
        for m in re.finditer(r"if\s*\(\s*" + var + r"\s*==\s*(\d+)\s*\)\s*\{", body):
            seg = body[m.end():_balanced(body, m.end() - 1, "{", "}") - 1]
            for st in executed(seg, {}):
                for op in timeline_ops(st):
                    out.append((int(m.group(1)) * mul, op))
    return out


def step_to_ms(step, bpm, changes):
    """Pas -> ms avec les changements de BPM [(ms, bpm)]."""
    t, beat, cur = 0.0, 0.0, bpm
    target = step / 4.0
    for ms, b in changes:
        nb = beat + (ms - t) * cur / 60000.0
        if nb >= target:
            break
        t, beat, cur = ms, nb, b
    return t + (target - beat) * 60000.0 / cur


def shown_by_events(raw):
    """Variables qu'un événement fait apparaître (alpha > 0 ou visible = true) : gardées même cachées au départ."""
    return {op[1] for _, op in raw if (op[0] == "alpha" and op[2] > 0) or (op[0] == "vis" and op[2])}


def song_extras(song_dir, stage, sid, out, bpm, changes, fixes, names=("", "", ""), chars_done=None):
    """Sprites créés par les scripts de la chanson (songs/<id>/extra.stg) et événements de pas (lignes "ev")."""
    # positions des persos (décor converti + repositionnements des scripts), pour "dad.x" etc. dans les scripts
    pos = [[0.0, 0.0] for _ in range(3)]
    stg = OUT / "stages" / f"{stage}.stg"
    if stg.exists():
        b = stg.read_bytes()
        for k in range(3):
            pos[k] = list(struct.unpack_from("<ff", b, 12 + 16 * k))
    for f in fixes:
        p = f.split()
        if p[0] == "pos":
            pos[int(p[1])] = [float(p[2]), float(p[3])]
        elif p[0] in ("set", "add", "sub") and p[1] in ("x", "y"):
            i, v = (0 if p[1] == "x" else 1), float(p[3])
            pos[int(p[2])][i] = v if p[0] == "set" else pos[int(p[2])][i] + (v if p[0] == "add" else -v)
    env = {}
    for who, k in WHO.items():
        env[f"{who}.x"], env[f"{who}.y"] = pos[k]
    stage_vars = {}
    vf = OUT / "stages" / f"{stage}.vars"
    if vf.exists():
        for l in vf.read_text(encoding="utf-8").splitlines():
            if " " in l:
                k, v = l.split(" ", 1)
                stage_vars[k] = v
    files = []
    st = find_asset(f"data/stages/{stage}.hx")
    if st:
        files.append((st, True))
    files += [(f, False) for f in sorted((song_dir / "scripts").glob("*.hx"))] if (song_dir / "scripts").is_dir() else []
    objects = {}
    order = [("char", "girlfriend"), ("char", "dad"), ("char", "boyfriend")]
    raw = []
    extra = {}       # variable -> (indice 3+, nom, x, y, joueur, devant)
    reflections = set()
    char_lines = []
    envrx = re.compile(r"(?<![\w.])(?:" + "|".join(re.escape(k) for k in sorted(env, key=len, reverse=True)) +
                       r")(?![\w.])")
    for f, is_stage in files:
        src = f.read_text(encoding="utf-8-sig", errors="replace")
        raw += step_events(src)
        if not is_stage:
            run_stage_script(src, objects, order, env)
        # new Character(x, y, "nom"[, joueur]) dans create/postCreate, inséré devant un perso
        for fn in ("create", "postCreate"):
            for st in executed(function_body(strip_comments(src), fn), {}):
                st = envrx.sub(lambda m: repr(env[m.group(0)]), re.sub(r"\s+", " ", st).strip())
                m = re.fullmatch(r"(?:var )?(\w+)(?: ?: ?\w+)? = new Character\((.*)\)", st)
                if m and len(extra) < 3:
                    args = [a.strip() for a in m.group(2).split(",")]
                    nm = args[2].strip("'\"") if len(args) > 2 else ""
                    cm = re.fullmatch(r"(\w+)\.curCharacter", args[2]) if len(args) > 2 else None
                    if cm and cm.group(1) in WHO:
                        nm = names[{0: 1, 1: 0, 2: 2}[WHO[cm.group(1)]]]
                    x, y = safe_num(args[0]), safe_num(args[1]) if len(args) > 1 else None
                    if nm and x is not None and y is not None and convert_character(nm, chars_done if chars_done is not None else {}):
                        player = len(args) > 3 and args[3] == "true"
                        extra[m.group(1)] = [3 + len(extra), nm, x, y, player, 255]
                    continue
                m = re.fullmatch(r"insert\(members\.indexOf\((\w+)\), ?(\w+)\)", st)
                if m and m.group(2) in extra and m.group(1) in CHARNAME:
                    extra[m.group(2)][5] = BEFORE_CODE[CHARNAME[m.group(1)]]
                    continue
                m = re.fullmatch(r"(\w+)\.flipY ?= ?true", st)
                if m and m.group(1) in extra:  # reflet (retourné, incliné) : pas encore dessinable, écarté
                    reflections.add(m.group(1))
                    continue
                m = re.fullmatch(r"(\w+)\.cameras ?= ?\[(.*)\]", st)
                if m and m.group(1) in extra and "camGame" not in m.group(2):  # autre caméra : effet à part, écarté
                    reflections.add(m.group(1))
                    continue
                m = re.fullmatch(r"(\w+)\.([xy]) ?([-+])= ?" + NUM, st)
                if m and m.group(1) in extra:  # décalage relatif dans create : appliqué à la position de départ
                    i = 2 if m.group(2) == "x" else 3
                    extra[m.group(1)][i] += float(m.group(4)) * (1 if m.group(3) == "+" else -1)
                    continue
                m = re.match(r"(\w+)\.", st)
                if m and m.group(1) in extra:  # état initial (teinte, alpha, visible...) : pas -1000
                    raw += [(-1000, op) for op in timeline_ops(st)]
    for var in reflections:
        del extra[var]
    for i, var in enumerate(extra):  # indices 3, 4, 5 sans trou
        extra[var][0] = 3 + i
    for var, (k, nm, x, y, player, before) in extra.items():
        char_lines.append(f"char {k} |{nm}| {x} {y} {1 if player else 0} {before}")
    # délais en ms (minuteries) -> pas, au BPM du début ; puis un seul ordre chronologique (tri stable)
    raw = [((e[0][1] * bpm / 15000.0) if isinstance(e[0], tuple) else e[0], e[1]) for e in raw]
    raw.sort(key=lambda e: e[0])
    # sprites qu'un événement fait apparaître, fenêtres d'affichage des shaders pré-calculés
    keep = shown_by_events(raw)
    windows = {}
    for var, o in objects.items():
        if isinstance(o, ScriptSprite) and o.shader:
            evs = [(stp, op) for stp, op in raw if op[0] == "alpha" and op[1] == var]
            on = next((stp for stp, op in evs if op[2] > 0), None)
            if on is None:
                continue
            off = next((step_to_ms(stp, bpm, changes) + op[3] * 1000 for stp, op in evs if stp > on and op[2] <= 0),
                       step_to_ms(on, bpm, changes) + 30000)
            windows[var] = (step_to_ms(on, bpm, changes), off)
    sname = re.sub(r"\W", "_", sid)
    shader_cams = set()
    for f, _ in files:
        shader_cams |= set(re.findall(r"(\w+)\.(?:addShader|setFilters)\(",
                                      f.read_text(encoding="utf-8-sig", errors="replace")))
    records = build_script_items(objects, order, f"sng_{sname}_", 0, keep, windows, shader_cams)
    order = [k for k in order if k[0] == "char" or k[1] in records]
    ex = out / "extra.stg"
    if records:
        b = bytearray(b"FSTX") + struct.pack("<II", 3, len(records))
        for i, (kind, v) in enumerate(order):
            if kind != "spr":
                continue
            nxt = next((c for k2, c in order[i + 1:] if k2 == "char"), None)
            r = dict(records[v])
            r["before"] = BEFORE_CODE[nxt] if nxt and not r["layer"] else 255
            b += pack_item(r)
        ex.write_bytes(b)
    else:
        # fichier vide plutôt que supprimé : la copie vers la SD (FTP, robocopy) n'efface pas les anciens fichiers
        ex.write_bytes(b"FSTX" + struct.pack("<II", 3, 0))
    spr_names = dict(stage_vars)
    spr_names.update({k: r["name"] for k, r in records.items()})
    # qui est "dad" / "boyfriend" à chaque instant : "dad = shadum" change la cible des instructions suivantes
    bind = {"boyfriend": 0, "bf": 0, "dad": 1, "gf": 2, "girlfriend": 2}
    bind.update({v: e[0] for v, e in extra.items()})
    slot_of_name = {"boyfriend": 0, "bf": 0, "dad": 1, "gf": 2, "girlfriend": 2}
    lines = list(char_lines)
    for stp, op in sorted(raw, key=lambda e: e[0]):
        kind = op[0]
        if kind == "flash":
            lines.append(f"ev {stp} flash {op[1]:08X} {op[2]}")
            continue
        if kind == "assign":
            k = bind.get(op[2])
            if k is not None:
                lines.append(f"ev {stp} sing {slot_of_name[op[1]]} {k}")
                bind[op[1]] = k
                if op[1] in ("boyfriend", "bf"):
                    bind["boyfriend"] = bind["bf"] = k
            continue
        tgt = f"@{bind[op[1]]}" if op[1] in bind else spr_names.get(op[1])
        if tgt is None:
            continue
        if kind == "alpha":
            lines.append(f"ev {stp} alpha {tgt} {op[2]} {op[3]}")
        elif kind == "tint" and tgt.startswith("@"):
            lines.append(f"ev {stp} tint {tgt} {op[2]} 0")
        elif kind == "move" and tgt.startswith("@"):
            x = 1e30 if op[2] is None else op[2]
            y = 1e30 if op[3] is None else op[3]
            lines.append(f"ev {stp} move {tgt} {x} {y} {op[4]}")
        elif kind == "flip" and tgt.startswith("@"):
            lines.append(f"ev {stp} flip {tgt} {1 if op[2] else 0}")
        elif kind == "vis":
            lines.append(f"ev {stp} vis {tgt} {1 if op[2] else 0}")
    return lines


def song_fixes(song_dir, stage, ctx):
    """Ordre de Codename : create du décor puis des chansons, postCreate idem (approché par fichier)."""
    files = []
    st = find_asset(f"data/stages/{stage}.hx")
    if st:
        files.append(st)
    files += sorted((song_dir / "scripts").glob("*.hx")) if (song_dir / "scripts").is_dir() else []
    per = [script_fixes(f.read_text(encoding="utf-8-sig", errors="replace"), ctx) for f in files]
    out = []
    for phase in ("create", "postCreate", "countdown"):
        for p in per:
            out += p.get(phase, [])
    return out


# ---------------------------------------------------------------- audio

def convert_audio(src, dst, channels=2):
    if dst.exists() and dst.stat().st_mtime > src.stat().st_mtime:
        return
    raw = dst.with_suffix(".raw")
    subprocess.run([FFMPEG, "-v", "error", "-y", "-i", str(src), "-ac", str(channels), "-ar", "32000",
                    "-f", "s16le", str(raw)], check=True)
    subprocess.run([str(ADPCM_ENC), str(raw), str(dst), "32000", str(channels)], check=True)
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

    # PlayState : dad = strumLines[0], boyfriend = strumLines[1], gf = strumLines[2] (par indice, pas par type),
    # chacun à la position de sa ligne ("dad", "boyfriend", "girlfriend"). Les lignes suivantes sont ignorées.
    def slot_of(idx):
        if 0 <= idx < min(3, len(lines)):
            return idx
        t = lines[idx].get("type", 0) if 0 <= idx < len(lines) else 0
        return {0: 0, 1: 1}.get(t, 2)

    slots = [lines[i] if i < len(lines) else None for i in range(3)]
    dad, bf, gf = slots
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
            events.append((float(e["time"]), 0, float(slot_of(int(p[0])))))
        elif n == "BPM Change" and p:
            events.append((float(e["time"]), 1, float(p[0])))
        elif n == "Alt Animation Toggle" and p:
            # params : [ligne de notes, chant alternatif activé, (idle alternatif)] -> type * 2 + activé
            ints = [v for v in p if isinstance(v, (int, float)) and not isinstance(v, bool)]
            bools = [v for v in p if isinstance(v, bool)]
            t = slot_of(int(ints[0]) if ints else 0)
            events.append((float(e["time"]), 2, float(t * 2 + (1 if bools and bools[0] else 0))))
    events.sort()

    # type de note (indice dans noteTypes, 1 = premier) -> 0 normal, 1 alt, 2 sans animation, 3 chantée par gf
    ntypes = d.get("noteTypes", [])
    tcode = {"Alt Animation": 1, "Alt Anim Note": 1, "No Animation": 2, "GF Sing": 3}

    def notes_of(s):
        res = []
        for n in (s or {}).get("notes", []):
            lane = int(n.get("id", 0))
            ti = int(n.get("type", 0))
            code = tcode.get(ntypes[ti - 1], 0) if 0 < ti <= len(ntypes) else 0
            if 0 <= lane < 4:
                res.append((float(n["time"]), lane, code, float(n.get("sLen", 0))))
        res.sort()
        return res

    # toutes les lignes jouées par le joueur (type 1) d'un côté, toutes les lignes CPU de l'autre ;
    # chanteur : 0 perso du côté, 3 emplacement gf, 4 perso de l'autre côté, 2 personne (perso non chargé).
    # Une ligne de type 2 (ADDITIONAL) a ses propres flèches sur PC (souvent cachées ou déplacées par un script) :
    # ses notes font chanter son perso mais ne s'affichent pas sur les flèches de l'adversaire (bit 0x80).
    nd, nb = [], []
    for idx, s in enumerate(lines):
        t = s.get("type", 0)
        side = 1 if t == 1 else 0
        sl = idx if idx < 3 else None
        notes = notes_of(s)
        if t == 2:
            notes = [(a, l, c | 0x80, h) for a, l, c, h in notes]
        if sl == side:
            pass
        elif sl == 2:
            notes = [(a, l, 3 | (c & 0x80), h) for a, l, c, h in notes]
        elif sl is not None:
            notes = [(a, l, 4 | (c & 0x80), h) for a, l, c, h in notes]
        else:
            notes = [(a, l, 2 | (c & 0x80), h) for a, l, c, h in notes]
        (nb if side else nd).extend(notes)

    def dedupe(ns):  # lignes parallèles : une seule note par instant et par flèche
        out, seen = [], set()
        for n in sorted(ns):
            k = (round(n[0]), n[1], n[2] & 0x80)
            if k not in seen:
                seen.add(k)
                out.append(n)
        return out

    nd, nb = dedupe(nd), dedupe(nb)
    b = bytearray(b"FSNG") + struct.pack("<Iff", 1, bpm, float(d.get("scrollSpeed", 1)))
    b += fstr(stage) + fstr(names[0]) + fstr(names[1]) + fstr(names[2])
    b += fstr(display, 48)
    b += struct.pack("<III", len(nd), len(nb), len(events))
    for t, lane, typ, sl in nd + nb:
        b += struct.pack("<fBBxxf", t, lane, typ, sl)
    for t, k, v in events:
        b += struct.pack("<fBxxxf", t, k, v)
    (out / "chart.sng").write_bytes(b)

    fixes = []
    # position de scène de chaque emplacement (indice du jeu : 0 bf, 1 dad, 2 gf)
    pos_idx = {"boyfriend": 0, "bf": 0, "player": 0, "dad": 1, "opponent": 1, "girlfriend": 2, "gf": 2}
    for slot, game_idx in ((0, 1), (1, 0), (2, 2)):
        s = slots[slot]
        if s and s.get("position") and pos_idx.get(s["position"], game_idx) != game_idx:
            fixes.append(f"slot {game_idx} {pos_idx[s['position']]}")
    ctx = {"PlayState.SONG.meta.displayName": meta.get("displayName", song_dir.name),
           "PlayState.SONG.meta.name": meta.get("name", song_dir.name),
           "SONG.meta.displayName": meta.get("displayName", song_dir.name),
           "SONG.meta.name": meta.get("name", song_dir.name),
           "PlayState.difficulty": diff, "dad.curCharacter": names[0], "boyfriend.curCharacter": names[1],
           "gf.curCharacter": names[2]}
    for f in song_fixes(song_dir, stage, ctx):
        p = f.split()
        if p[0] == "camfix":  # indice de ligne de notes -> emplacement (0 adversaire, 1 joueur, 2 gf)
            p[1] = str(slot_of(int(p[1])))
        fixes.append(" ".join(p))
    fixes += song_extras(song_dir, stage, sid, out, bpm, [(t, v) for t, k, v in events if k == 1], fixes,
                         names, chars_done)
    fx = out / "fix.txt"
    if fixes:
        fx.write_text("\n".join(fixes) + "\n", encoding="ascii")
        print("  script:", "; ".join(fixes))
    else:
        fx.write_text("", encoding="ascii")  # vide plutôt que supprimé (voir extra.stg)

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

def convert_noteskin(skin):
    xml = find_asset(f"images/game/notes/{skin}.xml")
    png = find_asset(f"images/game/notes/{skin}.png")
    if xml is None or png is None:
        return
    print("  fleches", skin)
    frames = parse_sparrow(xml)
    src = load_png(png)
    ss = SpriteSet("ui_notes_" + re.sub(r"\W", "_", skin), S * HUD_NOTE_SCALE)
    for name in ["arrowLEFT", "arrowDOWN", "arrowUP", "arrowRIGHT",
                 "left press", "down press", "up press", "right press",
                 "left confirm", "down confirm", "up confirm", "right confirm",
                 "purple0", "blue0", "green0", "red0",
                 "purple hold piece", "blue hold piece", "green hold piece", "red hold piece",
                 "pruple end hold", "blue hold end", "green hold end", "red hold end"]:
        pref = prefix_frames(frames, name)
        idx = [ss.add_frame(src, frames[i]) for _, i in pref]
        ss.add_anim(name.rstrip("0"), 24, False, 0, 0, idx)
    ss.write(OUT / "ui" / ("notes.spr" if skin == "default" else f"notes_{skin}.spr"), "rgba8")


def convert_ui():
    print("interface")
    ui = OUT / "ui"
    ui.mkdir(parents=True, exist_ok=True)
    convert_noteskin("default")
    # éclaboussures (splash) des SICK
    xml = find_asset("images/game/splashes/default.xml")
    if xml:
        frames = parse_sparrow(xml)
        src = load_png(find_asset("images/game/splashes/default.png"))
        ss = SpriteSet("ui_splash", S * HUD_NOTE_SCALE)
        for v in ("1", "2"):
            for c in ("purple", "blue", "green", "red"):
                idx = [ss.add_frame(src, frames[i]) for _, i in prefix_frames(frames, f"note impact {v} {c}")]
                if idx:
                    ss.add_anim(f"{v}{c}", 24, False, 0, 0, idx)
        ss.write(ui / "splash.spr", "rgba8")
    # sons de la partie (mono)
    for n in ["intro3", "intro2", "intro1", "introGo", "missnote1", "missnote2", "missnote3"]:
        p = find_asset(f"sounds/{n}.ogg")
        if p:
            convert_audio(p, ui / f"sfx_{n}.adp", 1)
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
            scale = 0.6
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
            convert_audio(p, ui / f"sfx_{n}.adp", 1)
    p = find_asset("music/freakyMenu.ogg")
    if p:
        convert_audio(p, ui / "menu.adp")


def convert_icons(names):
    # HealthIcon : images carrées (côté = hauteur) : normale, perd, (gagne) ; icône absente -> "face"
    ss = SpriteSet("ui_icons", S)
    for n in sorted(set(x for x in names if x) | {"face"}):
        p = find_asset(f"images/icons/{n}.png") or find_asset(f"images/icons/icon-{n}.png")
        if p is None:
            print("  icône introuvable (remplacée par face):", n)
            continue
        im = load_png(p)
        side = im.height
        count = max(1, im.width // side) if side else 1
        idx = [ss.add_image(im.crop((k * side, 0, (k + 1) * side, side))) for k in range(min(count, 3))]
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
    # Windows ignore la casse : "Karma" (liste) et "karma" (dossier) sont la même chanson
    order = [s for i, s in enumerate(order) if s.lower() not in {o.lower() for o in order[:i]}]
    for p in sorted((MOD / "songs").iterdir()):
        if p.is_dir() and p.name.lower() not in {o.lower() for o in order}:
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
    for skin in sorted(set(m.get("notesprite", "default") for m in chars_done.values() if m)):
        if skin != "default":
            convert_noteskin(skin)
    icons = [m["icon"] for m in chars_done.values() if m] + [e.split("|")[2] for e in entries]
    convert_icons(icons)
    (OUT / "songs.lst").write_text("\n".join(entries) + "\n", encoding="utf-8")
    print("FINI", len(entries), "chansons")


if __name__ == "__main__":
    main()
