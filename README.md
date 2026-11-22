# Cyber Sensation 3DS

**Cyber Sensation: Malware Breakout**, the Friday Night Funkin' mod, running natively on the **Nintendo 3DS**, including the Old 3DS.

This is not an emulator or a port of the PC executable. It is a small FNF engine written in C (citro2d/citro3d), plus a
PC converter that reads the mod's **Codename Engine** files (charts, characters, stages, menus, music) and turns them into
3DS-friendly data.

## Screenshots
| Title screen | Main menu |
|---|---|
| ![Title](docs/screenshots/title.png) | ![Main menu](docs/screenshots/main_menu.png) |
| **Story Mode** | **Freeplay** |
| ![Story Mode](docs/screenshots/story_mode.png) | ![Freeplay](docs/screenshots/freeplay.png) |
| **Gameplay** | **Gameplay** |
| ![Gameplay](docs/screenshots/gameplay_1.png) | ![Gameplay](docs/screenshots/gameplay_2.png) |

*Top screen of the 3DS (400x240), shown at 2x.*

## Features
- **Gameplay**:
  - real charts (Codename Engine and legacy FNF formats), holds, BPM changes, camera events, zoom on beats
  - ratings (sick/good/bad/shit), combo, score, accuracy, health bar with icons, game over, pause menu
- **Characters and stages** from the mod (Sparrow atlases with animation offsets, parallax scroll)
- **Audio**:
  - music and voices streamed from the SD card as DSP ADPCM, decoded by the 3DS sound chip, so it uses almost no CPU
  - voices muted on a miss
- **Menus recreated from the mod's scripts**:
  - title screen (beat-synced lasers)
  - main menu (random art, animated bar and spinning square)
  - Story Mode (5 chapters, main and side stories)
  - Freeplay (song carousel, covers, difficulties, music preview, personal best, animated wave background)
  - Options (downscroll, ghost tapping)
- Faithful to Codename Engine: character placement and camera, health bar and bouncing icons, Score/Misses/Accuracy with rank, rating popups and combo numbers, note splashes, per-character note skins, countdown and miss sounds, strum intro
- Runs at **60 fps** on an Old 3DS. Installable as `.cia` (HOME menu icon and banner with sound) or `.3dsx`.

## Install (release)
1. Download `CyberSensation-3DS.cia` (or `.3dsx`) from the Releases page.
2. Convert **your own copy** of the mod with the converter (see below). Copy the resulting `sd/3ds/fnf3ds/` folder to
   the root of your SD card.
3. Install the `.cia` with FBI, or start the `.3dsx` from the Homebrew Launcher.
4. Your console needs `sdmc:/3ds/dspfirm.cdc` for sound (dump it with DSP1).

**This repository and its releases contain no game or mod assets.** You need the mod files on your PC.

## Converting the mod
Requirements: Python 3 with `pillow` and `numpy`, `ffmpeg` in PATH, devkitPro (`tex3ds`, `mkbcfnt`).

```
gcc -O2 -o tools/adpcm_enc.exe tools/adpcm_enc.c
py -3 tools/convert.py "<path to the game>/bin" sd/3ds/fnf3ds/cyber
```
The `--vite` flag skips characters and stages that are already converted.
`tools/envoi.py` can send everything to a 3DS running ftpd (`set FNF3DS_HOST=<3DS IP>`).

## Building
devkitPro with devkitARM, libctru, citro2d and citro3d:
```
make
py -3 tools/make_meta.py "<path to the game>/bin"            # icon + banner, needs bannertool
powershell -File tools/build_cia.ps1                        # needs makerom
```

## Controls
| | |
|---|---|
| D-Pad / ABXY / Circle Pad | arrows (left, down, up, right) |
| START | pause |
| L / R | jump 5 songs in Freeplay |

## Not supported yet
GLSL shaders, video cutscenes, song mechanics that only exist as HScript, Adobe Animate texture atlases
(e.g. girlfriend).

## Credits
- **Friday Night Funkin'** by ninjamuffin99, PhantomArcade, evilsk8r and Kawai Sprite
- **Codename Engine** by the Codename Crew
- **Cyber Sensation: Malware Breakout** by its mod team; all characters, art and music belong to them
- 3DS engine and converter: this repository
