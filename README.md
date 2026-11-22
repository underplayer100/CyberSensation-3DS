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
- **Characters and stages** from the mod:
  - Sparrow atlases with animation offsets, scaled around the sprite centre like Flixel, parallax scroll
  - Adobe Animate texture atlases (girlfriend, nondead) flattened to frames by the converter
  - simple stage and song scripts read by the converter: character positions, flips, camera offsets, fixed cameras,
    zoom, sprites created in `create()`/`postCreate()`, conditions on the song name
  - alternate sing animations, "No Animation" and "GF Sing" notes
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
Pick **one** of these:

- **Easiest:** download `CyberSensation-3DS-full.cia` and install it with FBI. The whole game is inside, nothing else to copy.
- **SD card version:** download `CyberSensation-3DS-SD.zip` and unzip it at the root of your SD card
  (it creates `3ds/fnf3ds/cyber/`). Then install `CyberSensation-3DS.cia` with FBI, or put `CyberSensation-3DS.3dsx`
  in `/3ds/` and start it from the Homebrew Launcher.

Your console needs `sdmc:/3ds/dspfirm.cdc` for sound (dump it with DSP1).
If both exist, the SD card data is used before the data inside the full `.cia`.

The **source code** in this repository contains no game or mod assets. The release packs contain the mod's art and
music converted for the 3DS, credited below. If you are part of the mod team and want them removed, open an issue
and they will be taken down.

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
powershell -File tools/build_cia.ps1 -Full                  # .cia with the converted mod inside
powershell -File tools/package_release.ps1                  # all release files in release/
```
`tools/test_all.ps1` plays every song in the Azahar emulator and keeps a screenshot of each one in `test_all/`.

## Controls
| | |
|---|---|
| D-Pad / ABXY / Circle Pad | arrows (left, down, up, right), by default |
| Options > Touches | choose the buttons of each arrow (A, B, X, Y, L, R, ZL, ZR, D-Pad, Circle Pad, C-Stick, SELECT) |
| START | pause |
| L / R | jump 5 songs in Freeplay |

## Not supported yet
GLSL shaders, video cutscenes, song mechanics and mid-song effects that only exist as HScript,
extra strumlines beyond the first three (their notes are played, their characters are not shown).

## Known issues / to fix
Found by playing every song in the Azahar emulator (`tools/test_all.ps1`) and comparing with the mod's scripts.
Not checked side by side with the PC game yet: please report what looks different.

**Character placement**
- *That's Me* (kemo): the player is very large and cut off on the right.
- *Sweet Love*, *Glitcher*, *Context EX*, *Highlights EX*, *Errorians*: the player is partly off screen on the right.
- *Bobalolazing* (minecraft): Taeyai is huge in the middle and Bob is tiny in the corner. The chart's gf (`none`) does not exist.
- *New Hope*, *Hoax* (dust), *Underground*, *Karma* (tans), *Data Diva* (monika): the characters look small at the bottom
  of the screen. Their camera offsets come from scripts (`cameraOffset.y -= 700`) and may need checking.
- *No More Sense (I'm Dead)*: nondead is drawn near the top of the screen.
- Characters added by scripts are missing: reflections/shadows (`redad`, `rebf`, `regf` in garage, tans, dust, shadow),
  `shadum` in *Me*, and so on.
- Songs with more than 3 strumlines (*Cyberfunk Attack*, *Errorians*, *Our Hope*, *Fun Gang*, *Me*, *No More Sense*):
  only the first three characters are shown. The others' notes are played, but nobody sings them.
- Positions that scripts change during the song (`stepHit`, `beatHit`, tweens, floating characters) are not applied.

**Stages and backgrounds**
- *Liquated*: the `camellia` stage does not exist in the mod files, so the background is black.
- *No More Sense (I'm Dead)*: the `none` stage has no XML, so the background is black.
- *Me*: a big black rectangle covers part of the background. The extra characters (Minus, Shadow) are not shown.
- *Soft Machine*, *New Day*, *Yawarakai*: a black strip shows on the left side of the background.
- *Open System* (room): the animated hologram panels are missing (they use a shader).
- Not supported: GLSL shaders (colour grading, glitch, bloom, rays), video cutscenes, solid colour boxes created by
  scripts, sprites that scripts create or swap during the song.

**Health bar icons**
- These icons do not exist in the mod, so the default `face` icon is shown, like on PC: `bfex`, `cj`, `ruby`,
  `perseverance-sans`, `shini`, `sticky`, `taeyaiminus`.

**Notes and gameplay**
- *Liquated*: the opponent's arrows use a white note skin. Check whether this matches the PC version.
- Special notes (hurt, bullet, and so on) are played as normal notes.
- The ~2000 `HScript Call` chart events (lyrics, flashes, camera tricks) are ignored.

**Not tested yet**
- The full `.cia` (data in the romfs) has not been tested on a real console.

## Credits
- **Friday Night Funkin'** by ninjamuffin99, PhantomArcade, evilsk8r and Kawai Sprite
- **Codename Engine** by the Codename Crew
- **Cyber Sensation: Malware Breakout** by its mod team; all of the mod's characters, art and music belong to them.
  Play the original on PC and support its creators.
- 3DS engine and converter: this repository
