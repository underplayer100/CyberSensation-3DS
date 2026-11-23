"""Captures de référence du jeu PC, pour comparer avec le portage 3DS.

usage :
  py -3 tools/pc_ref.py install "<jeu>/bin" [chanson ...]   ajoute le pilote au global.hx du mod + liste des chansons
  py -3 tools/pc_ref.py remove  "<jeu>/bin"                 remet global.hx comme avant
  py -3 tools/pc_ref.py compare "<jeu>/bin" <dossier 3DS> <sortie>
        planches PC | 3DS pour chaque capture (3DS : shot_pXXXXX.bmp copiés par test_all.ps1 en <id>_pXXXXX.bmp)

  py -3 tools/pc_ref.py run     "<jeu>/bin"                 lance le jeu hors des écrans et attend la fin

Le jeu joue chaque chanson en botplay, enregistre bin/claude_ref/<id>_<ms>.png et se ferme.
"""
import re
import sys
from pathlib import Path

HERE = Path(__file__).parent
BEGIN, END = "// PC_REF :", "// PC_REF FIN"
IMPORTS = ["import openfl.display.BitmapData;", "import openfl.display.PNGEncoderOptions;",
           "import openfl.geom.Matrix;", "import sys.io.File;", "import sys.FileSystem;"]
TAG = " // PC_REF"


def mod_dir(game):
    return [p for p in (Path(game) / "mods").iterdir() if p.is_dir()][0]


def strip(src):
    src = re.sub(re.escape(BEGIN) + r".*?" + re.escape(END) + r"\n?", "", src, flags=re.S)
    return "\n".join(l for l in src.split("\n") if not l.endswith(TAG))


def install(game, wanted):
    g = mod_dir(game) / "data" / "global.hx"
    src = strip(g.read_text(encoding="utf-8"))
    head = "\n".join(i + TAG for i in IMPORTS)
    g.write_text(head + "\n" + src.rstrip("\n") + "\n\n" + (HERE / "pc_ref.hx").read_text(encoding="utf-8"),
                 encoding="utf-8")
    lst = HERE.parent / "sd" / "3ds" / "fnf3ds" / "cyber" / "songs.lst"
    lines = []
    for l in lst.read_text(encoding="utf-8").splitlines():
        p = l.split("|")
        if len(p) > 6 and (not wanted or p[0] in wanted):
            lines.append(f"{p[4]}|{p[5].lower()}|{re.sub(r'[^A-Za-z0-9_-]', '_', p[0])}")
    (Path(game) / "claude_ref.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(len(lines), "chansons ->", Path(game) / "claude_ref.txt")


def run(game, timeout=1800):
    """Lance le jeu réduit, le replace hors des écrans sans l'activer (une fenêtre réduite n'est plus dessinée,
    les captures seraient noires) et attend la fin de la liste (claude_ref/log.txt : FINI) ou la fermeture du jeu."""
    import ctypes
    import subprocess
    import time
    from ctypes import wintypes

    class WP(ctypes.Structure):
        _fields_ = [("len", wintypes.UINT), ("flags", wintypes.UINT), ("show", wintypes.UINT),
                    ("min", wintypes.POINT), ("max", wintypes.POINT), ("normal", wintypes.RECT)]

    exe = next(Path(game).glob("*.exe"))
    si = subprocess.STARTUPINFO()
    si.dwFlags = subprocess.STARTF_USESHOWWINDOW
    si.wShowWindow = 7  # SW_SHOWMINNOACTIVE
    proc = subprocess.Popen([str(exe)], cwd=str(game), startupinfo=si)
    user32 = ctypes.windll.user32
    hwnd = None
    t0 = time.time()
    while time.time() - t0 < 60 and hwnd is None:
        time.sleep(0.5)
        found = []

        @ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
        def cb(h, _):
            pid = wintypes.DWORD()
            user32.GetWindowThreadProcessId(h, ctypes.byref(pid))
            if pid.value == proc.pid and user32.IsWindowVisible(h):
                found.append(h)
            return True
        user32.EnumWindows(cb, 0)
        hwnd = found[0] if found else None
    if hwnd:
        wp = WP()
        wp.len = ctypes.sizeof(WP)
        wp.show = 4  # SW_SHOWNOACTIVATE
        wp.normal = wintypes.RECT(-4000, 0, -4000 + 1280, 720)
        user32.SetWindowPlacement(hwnd, ctypes.byref(wp))
    log = Path(game) / "claude_ref" / "log.txt"
    while proc.poll() is None and time.time() - t0 < timeout:
        time.sleep(2)
        if log.exists() and "FINI" in log.read_text(encoding="utf-8", errors="replace"):
            break
    if proc.poll() is None:
        proc.kill()
    print(log.read_text(encoding="utf-8", errors="replace") if log.exists() else "pas de journal")


def remove(game):
    g = mod_dir(game) / "data" / "global.hx"
    g.write_text(strip(g.read_text(encoding="utf-8")), encoding="utf-8")
    f = Path(game) / "claude_ref.txt"
    if f.exists():
        f.unlink()
    print("global.hx remis en état")


def compare(game, shots3ds, out):
    from PIL import Image, ImageDraw
    out = Path(out)
    out.mkdir(parents=True, exist_ok=True)
    n = 0
    for pc in sorted((Path(game) / "claude_ref").glob("*_*.png")):
        sid, ms = pc.stem.rsplit("_", 1)
        ds = Path(shots3ds) / f"{sid}_p{int(ms):05d}.bmp"
        a = Image.open(pc).convert("RGB")
        # zone de jeu 16:9 au centre de la fenêtre
        w, h = a.size
        gh = min(h, w * 9 // 16)
        gw = gh * 16 // 9
        a = a.crop(((w - gw) // 2, (h - gh) // 2, (w + gw) // 2, (h + gh) // 2)).resize((400, 225), Image.LANCZOS)
        sheet = Image.new("RGB", (812, 260), (30, 30, 30))
        sheet.paste(a, (4, 31))
        if ds.exists():
            sheet.paste(Image.open(ds).convert("RGB"), (408, 16))
        d = ImageDraw.Draw(sheet)
        d.text((4, 2), f"PC  {sid} {ms} ms", fill=(255, 255, 255))
        d.text((408, 2), "3DS" + ("" if ds.exists() else " (pas de capture)"), fill=(255, 255, 255))
        sheet.save(out / f"{sid}_{ms}.png")
        n += 1
    print(n, "planches ->", out)


if __name__ == "__main__":
    cmd, game = sys.argv[1], sys.argv[2]
    if cmd == "install":
        install(game, sys.argv[3:])
    elif cmd == "run":
        run(game)
    elif cmd == "remove":
        remove(game)
    elif cmd == "compare":
        compare(game, sys.argv[3], sys.argv[4])
