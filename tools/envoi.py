"""Sends the game and the converted mod to a 3DS running ftpd. Can be rerun: skips files already there.
usage: set FNF3DS_HOST=<3DS IP> (and FNF3DS_PORT, FNF3DS_MOD), then py -3 tools/envoi.py"""
import ftplib
import os
import time
from pathlib import Path

HOST, PORT = os.environ.get("FNF3DS_HOST", "192.168.1.2"), int(os.environ.get("FNF3DS_PORT", "5000"))
FNF = Path(__file__).resolve().parent.parent
SD = FNF / "sd" / "3ds" / "fnf3ds" / os.environ.get("FNF3DS_MOD", "cyber")


def connect():
    f = ftplib.FTP()
    f.connect(HOST, PORT, timeout=60)
    f.login()
    return f


def main():
    files = [
        (FNF / "fnf3ds.cia", "/cia/fnf3ds.cia"),
        (FNF / "fnf3ds.3dsx", "/3ds/fnf3ds.3dsx"),
    ]
    rest = [p for p in SD.rglob("*") if p.is_file()]
    # tout sauf l'audio d'abord, puis l'audio chanson par chanson
    rest.sort(key=lambda p: (p.suffix == ".adp", p.as_posix()))
    for p in rest:
        files.append((p, "/3ds/fnf3ds/" + SD.name + "/" + p.relative_to(SD).as_posix()))
    total = sum(p.stat().st_size for p, _ in files)
    done = 0
    t0 = time.time()
    f = connect()
    made = set()
    for i, (local, remote) in enumerate(files):
        d = os.path.dirname(remote)
        if d not in made:
            cur = ""
            for part in d.strip("/").split("/"):
                cur += "/" + part
                try:
                    f.mkd(cur)
                except ftplib.error_perm:
                    pass
            made.add(d)
        size = local.stat().st_size
        for attempt in range(6):
            try:
                same = False
                always = remote.endswith((".cia", ".3dsx")) or "/ui/" in remote or "/tex/ui_" in remote                     or "/tex/menu_" in remote or remote.endswith("songs.lst")
                if not always:
                    try:
                        same = f.size(remote) == size
                    except Exception:
                        same = False
                if not same:
                    with open(local, "rb") as fh:
                        f.storbinary(f"STOR {remote}", fh, blocksize=64 * 1024)
                break
            except Exception as e:
                print(f"  erreur {remote} ({e}), nouvel essai", flush=True)
                time.sleep(3)
                try:
                    f.close()
                except Exception:
                    pass
                f = connect()
        done += size
        if i < 3 or i % 25 == 0 or i == len(files) - 1:
            el = time.time() - t0
            print(f"{done / 1048576:7.0f}/{total / 1048576:.0f} Mo ({100 * done / total:4.1f} %) "
                  f"{done / max(el, 1) / 1048576:.2f} Mo/s  {remote}", flush=True)
    f.quit()
    print("ENVOI FINI", flush=True)


if __name__ == "__main__":
    main()
