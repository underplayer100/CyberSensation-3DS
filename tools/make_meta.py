"""Builds the HOME menu icon and banner (with sound) from the mod's own art.
usage: py -3 tools/make_meta.py "<game>/bin"     (needs bannertool in PATH or BANNERTOOL env var)"""
import os
import subprocess
import sys
from pathlib import Path

from PIL import Image

game = Path(sys.argv[1])
mod = [p for p in (game / "mods").iterdir() if p.is_dir()][0]
meta = Path(__file__).resolve().parent.parent / "meta"
meta.mkdir(exist_ok=True)
bt = os.environ.get("BANNERTOOL", "bannertool")

bg = Image.open(mod / "images/menus/TAeYaiMenuBG.jpg").convert("RGBA")
logo = Image.open(mod / "images/menus/titlescreen/logo.png").convert("RGBA")
r = max(256 / bg.width, 128 / bg.height)
banner = bg.resize((int(bg.width * r) + 1, int(bg.height * r) + 1)).crop((0, 0, 256, 128))
lr = min(240 / logo.width, 120 / logo.height)
logo = logo.resize((int(logo.width * lr), int(logo.height * lr)))
banner.alpha_composite(logo, ((256 - logo.width) // 2, (128 - logo.height) // 2))
banner.save(meta / "banner.png")

icon = Image.open(mod / "images/icons/nonsense.png").convert("RGBA")
icon = icon.crop((0, 0, icon.width // 2, icon.height)).resize((48, 48), Image.LANCZOS)
square = bg.resize((48, 48))
square.alpha_composite(icon)
square.save(meta / "icon.png")

subprocess.run(["ffmpeg", "-v", "error", "-y", "-t", "2.8", "-i", str(mod / "music/freakyMenu.ogg"), "-ac", "2",
                "-ar", "32000", "-af", "afade=t=out:st=2.3:d=0.5", str(meta / "banner.wav")], check=True)
subprocess.run([bt, "makebanner", "-i", str(meta / "banner.png"), "-a", str(meta / "banner.wav"),
                "-o", str(meta / "banner.bnr")], check=True)
subprocess.run([bt, "makesmdh", "-s", "Cyber Sensation", "-l", "Cyber Sensation: Malware Breakout (Friday Night Funkin')",
                "-p", "CyberSensation-3DS", "-i", str(meta / "icon.png"), "-o", str(meta / "icon.smdh")], check=True)
print("meta ok")
