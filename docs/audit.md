# Audit des scripts du mod

Généré par `tools/audit.py`. Pour chaque chanson : les scripts que Codename Engine exécute, ce que le convertisseur 3DS applique, et ce qu'il ignore.

- **appliqué** : instruction de `create`/`postCreate` reprise par `convert.py`
- **ignoré** : instruction de `create`/`postCreate` exécutée sur PC mais pas convertie
- **non suivi** : instruction de `create`/`postCreate` dans un bloc que le convertisseur ne suit pas (condition inconnue, boucle, switch)
- **pendant la chanson** : fonctions appelées en cours de partie (beatHit, stepHit, update, événements `HScript Call`...), jamais converties

## Résumé

| Chanson | Décor | Appliqué | Ignoré | Non suivi | Fonctions pendant la chanson | HScript Call | Lignes en plus |
|---|---|---|---|---|---|---|---|
| Open System | `room` | 6 | 40 | 39 | 10 | 0 | 0 |
| Open System (EX) | `roomex` | 7 | 37 | 17 | 8 | 0 | 0 |
| Wear A Mask | `room` | 5 | 80 | 56 | 17 | 0 | 0 |
| Wear A Mask (EX) | `roomex` | 6 | 77 | 34 | 15 | 138 | 0 |
| Last Hope | `lastroom` | 14 | 97 | 38 | 24 | 0 | 0 |
| Last Hope (EX) | `lastroom` | 14 | 97 | 38 | 24 | 288 | 0 |
| Sensical | `nonsense` | 3 | 33 | 16 | 8 | 0 | 0 |
| Sensational | `nonsense` | 1 | 21 | 16 | 6 | 0 | 0 |
| Resensation | `voidsense` | 16 | 61 | 43 | 13 | 0 | 0 |
| GuideLine | `tiny` | 7 | 21 | 18 | 4 | 0 | 0 |
| Common Sense (EX) | `nonsense` | 1 | 0 | 0 | 0 | 0 | 0 |
| Highlights (EX) | `nonsense` | 1 | 0 | 0 | 1 | 0 | 0 |
| Context (EX) | `nonsense` | 1 | 0 | 0 | 1 | 0 | 0 |
| Nonsensical (EX) | `nonsense` | 1 | 81 | 0 | 5 | 0 | 0 |
| You And Me | `garage` | 7 | 60 | 97 | 8 | 0 | 0 |
| Fun Till End | `garage` | 11 | 103 | 96 | 16 | 39 | 0 |
| Light It Up | `garage` | 31 | 127 | 73 | 22 | 127 | 0 |
| Brand New Life | `roomex` | 10 | 50 | 20 | 6 | 0 | 0 |
| Soft Machine | `soft` | 19 | 25 | 0 | 5 | 25 | 0 |
| New Day | `soft` | 19 | 34 | 0 | 3 | 0 | 0 |
| Yawarakai | `soft` | 21 | 82 | 1 | 6 | 64 | 0 |
| Underground | `tans` | 11 | 78 | 13 | 8 | 244 | 0 |
| Karma | `tans` | 15 | 81 | 8 | 7 | 0 | 0 |
| Bobalolazing | `minecraft` | 14 | 74 | 18 | 9 | 0 | 0 |
| Feathered Firewall | `geese` | 3 | 22 | 11 | 6 | 29 | 0 |
| Trolameme | `geese` | 3 | 43 | 11 | 5 | 41 | 0 |
| Julius | `geese` | 5 | 43 | 11 | 5 | 0 | 0 |
| shini | `geese` | 3 | 12 | 11 | 3 | 113 | 0 |
| Fun Gang | `geese` | 4 | 53 | 14 | 7 | 19 | 2 |
| New Hope | `dust` | 24 | 109 | 8 | 9 | 4 | 0 |
| Hoax | `dust` | 24 | 58 | 7 | 2 | 0 | 0 |
| Virus | `shadow` | 8 | 36 | 22 | 13 | 0 | 0 |
| Virus (LEGACY) | `room` | 5 | 25 | 43 | 10 | 0 | 0 |
| Crowd Control | `shadow` | 10 | 43 | 3 | 11 | 79 | 0 |
| malware | `shadow` | 8 | 36 | 3 | 13 | 0 | 0 |
| Errorians | `shadow` | 8 | 118 | 31 | 15 | 427 | 2 |
| Kowase | `kowase` | 28 | 87 | 29 | 14 | 111 | 0 |
| Your Hope | `tiny` | 8 | 70 | 2 | 11 | 110 | 0 |
| our hope | `shadow` | 11 | 227 | 57 | 15 | 18 | 4 |
| No More Sense (IM DEAD) | `none` | 4 | 42 | 3 | 5 | 0 | 1 |
| Data Diva | `monika` | 22 | 12 | 0 | 4 | 23 | 0 |
| That Me! | `kemo` | 8 | 36 | 0 | 6 | 97 | 0 |
| Cyberfunk Attack | `funk` | 21 | 12 | 0 | 3 | 0 | 1 |
| Sweet Love | `shiver` | 15 | 50 | 0 | 7 | 10 | 0 |
| Me | `me` | 12 | 13 | 6 | 7 | 0 | 2 |
| Glitcher | `glitcher` | 0 | 17 | 0 | 2 | 0 | 0 |
| Cyber Trouble | `exe` | 40 | 36 | 5 | 7 | 0 | 0 |
| credits (CREDITS) | `credits` | 7 | 26 | 3 | 3 | 0 | 0 |
| Liquated | `camellia` | 0 | 0 | 0 | 0 | 0 | 0 |

## Scripts communs à toutes les chansons (`data/charts/`)

### `data/charts/botplay.hx`

### `data/charts/camera.hx`
**Ignoré** : [sprite/décor] `add(smoothCamFollow)`; [caméra] `FlxG.camera.target = smoothCamFollow`
- `update` : caméra 3
- `postUpdate` : sprite/décor 3

### `data/charts/gameover.hx`

### `data/charts/notes.hx`
**Ignoré** : [autre] `muteVocalsOnMiss = false`
**Non suivi** : [autre] `s.setGraphicSize(Std.int(160 * 0.77))`; [autre] `s.setGraphicSize(Std.int(160 * 0.66))`; [sprite/décor] `s.y += 10`
- `onNoteCreation` : HUD 1
- `onStrumCreation` : HUD 1, personnage 1
- `onPlayerMiss` : tween/timer 1

### `data/charts/songinfo.hx`
**Ignoré** : [sprite/décor] `songbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)`; [sprite/décor] `songbackground.alpha = 0`; [autre] `songbackground.updateHitbox()`; [HUD] `songbackground.cameras = [camHUD]`; [autre] `songbackground.screenCenter(FlxAxes.X)`; [sprite/décor] `songbackground.scrollFactor.set()`; [sprite/décor] `songinfo = new FlxSprite()`; [sprite/décor] `songinfo.frames = Paths.getSparrowAtlas('game/song_info2')`; [sprite/décor] `songinfo.animation.addByPrefix('idle', 'song info', 24, false)`; [autre] `songinfo.antialiasing = true`; [autre] `songinfo.updateHitbox()`; [sprite/décor] `songinfo.x = -400`
**Non suivi** : [autre] `songtite.text = '柔らかい'`; [sprite/décor] `songtite.y -= 40`; [autre] `songtite.font = Paths.font("JP.ttf")`
- `onCountdown` : tween/timer 3, sprite/décor 1
- `onStartSong` : sprite/décor 1

### `data/charts/ui.hx`
**Ignoré** : [HUD] `camNewHUD = new FlxCamera()`; [HUD] `camNewHUD.bgColor = 0`; [HUD] `FlxG.cameras.remove(camHUD, false)`; [HUD] `FlxG.cameras.add(camNewHUD, false)`; [HUD] `FlxG.cameras.add(camHUD, false)`; [autre] `introSprites = ["game/ui/3", "game/ui/2", "game/ui/1", "game/ui/go"]`; [HUD] `iconP1.visible = false`; [HUD] `iconP2.visible = false`; [HUD] `healthBar.visible = false`; [HUD] `healthBarBG.visible = false`; [sprite/décor] `accuracyTxt.visible = false`; [sprite/décor] `accBarStatus = new FlxSprite()`
**Non suivi** : [HUD] `iconP1.visible = iconP2.visible = healthBar.visible = healthBarBG.visible = accu`; [sprite/décor] `accBarStatus.y += 7`; [sprite/décor] `accBarText.shadowOffset.x = -1`; [autre] `charname = 'talia'`; [sprite/décor] `chars1.loadGraphic(Paths.image('game/ui/char/' + charname + '-normal'))`; [sprite/décor] `chars2.loadGraphic(Paths.image('game/ui/char/' + charname + '-half'))`; [sprite/décor] `chars3.loadGraphic(Paths.image('game/ui/char/' + charname + '-low'))`; [sprite/décor] `chars1.scale.set(0.1, 0.1)`; [sprite/décor] `chars1.setPosition(-845,-272.562)`; [sprite/décor] `chars2.scale.set(0.1, 0.1)`; [sprite/décor] `chars2.setPosition(-845,-272.562)`; [sprite/décor] `chars3.scale.set(0.1, 0.1)`
- `onPostCountdown` : sprite/décor 3, HUD 1, tween/timer 1
- `onCountdown` : tween/timer 17, HUD 1
- `onNoteHit` : sprite/décor 2
- `onPlayerMiss` : sprite/décor 8
- `onPlayerHit` : sprite/décor 23, tween/timer 10
- `postUpdate` : sprite/décor 14, HUD 7, shader 2, audio 1

## Open System (`open system`)

Décor `room`, persos taeyai, bf, gf

### `data/stages/room.hx`
**Appliqué** (5)
- [autre] `var i = members.indexOf(roombg2)`
- [personnage] `boyfriend.setPosition(930,535)` *(postCreate)*
- [personnage] `dad.setPosition(-50,525)` *(postCreate)*
- [personnage] `dad.alpha = 0.4` *(postCreate)*
- [personnage] `boyfriend.alpha = 0.4` *(postCreate)*

**Ignoré** (14)
- [autre] `var positions = [ [true, 644, 282, 382, 272, [16, 2], [373, 29], [7, 263], [365, 242], 0], [true, 652, 532, 367, 273, [2, 10], [354, 1], [5,`
- [autre] `var easterEgg = FlxG.random.bool(1/4.2)`
- [autre] `var baseImages = [for(f in Paths.getFolderContent('images/stages/room/frames')) if (Path.extension(f).toLowerCase() == "png") Path.withoutEx`
- [autre] `var images = []`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(720,410)` *(postCreate)*
- [shader] `colorShader = new FunkinShader(Assets.getText(Paths.fragShader("colorgrade")))` *(postCreate)*
- [shader] `colorShader.topr = 3.1` *(postCreate)*
- [shader] `colorShader.topg = 1.1` *(postCreate)*
- [shader] `colorShader.topb = 1.1` *(postCreate)*
- [shader] `colorShader.botr = 1.1` *(postCreate)*
- [shader] `colorShader.botg = 1.1` *(postCreate)*
- [shader] `colorShader.botb = 2.1` *(postCreate)*
- [shader] `camGame.addShader(colorShader)` *(postCreate)*

**Non suivi** (23)
- [sprite/décor] `var spr = new FunkinSprite(roombg2.x, roombg2.y)`
- [autre] `images = [for(i in baseImages) i]`
- [autre] `var curImg = images[FlxG.random.int(0, images.length-1)]`
- [sprite/décor] `images.remove(curImg)`
- [autre] `spr.loadSprite(easterEgg ? Paths.image('stages/room/putin') : Paths.image('stages/room/frames/' + curImg))`
- [sprite/décor] `spr.animation.add("idle", [for(i in 0...spr.frames.frames.length) i], 24, true)`
- [sprite/décor] `spr.animation.play("idle")`
- [sprite/décor] `spr.scrollFactor = roombg2.scrollFactor`
- [autre] `spr.antialiasing = true`
- [sprite/décor] `spr.alpha = 0.8`
- [sprite/décor] `spr.x += pos[1]`
- [sprite/décor] `spr.y += pos[2]`
- [autre] `spr.setGraphicSize(pos[3], pos[4])`
- [autre] `spr.updateHitbox()`
- [shader] `spr.shader = new CustomShader("room-shader")`
- [shader] `spr.shader.topLeft = pos[5]`
- [shader] `spr.shader.topRight = pos[6]`
- [shader] `spr.shader.bottomLeft = pos[7]`
- [shader] `spr.shader.bottomRight = pos[8]`
- [shader] `spr.shader.hologramSize = [pos[3], pos[4]]`
- [sprite/décor] `spr.angle = pos[9]`
- [sprite/décor] `insert(i, spr)`
- [personnage] `dad.alpha = boyfriend.alpha = 0.4` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : personnage 2, caméra 1
- `postUpdate` : caméra 3

### `songs/open system/scripts/camflash.hx`
**Appliqué** (1)
- [caméra] `defaultCamZoom = 0.85` *(postCreate)*

**Ignoré** (11)
- [autre] `cameraMovementEnabled=false` *(postCreate)*
- [shader] `colorTransformShader = new CustomShader("colorTransform")` *(postCreate)*
- [shader] `colorTransformShader.redMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.greenMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.blueMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.alphaMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.redOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.greenOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.blueOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.alphaOffset = 0` *(postCreate)*
- [shader] `camGame.addShader(colorTransformShader)` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : shader 6, HUD 1
- `beatHit` : caméra 8, HUD 2

### `songs/open system/scripts/events.hx`
**Pendant la chanson**
- `stepHit` : caméra 4, HUD 2
- `beatHit` : caméra 5, HUD 5

### `songs/open system/scripts/lol.hx`
**Ignoré** (1)
- [vidéo/cinématique] `playCutscenes = true`

### `songs/open system/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

## Open System (EX) (`open system-ex`)

Décor `roomex`, persos taeyaiex, bfex, gf

### `data/stages/roomex.hx`
**Appliqué** (6)
- [autre] `var i = members.indexOf(roombg)`
- [sprite/décor] `var spr = new FunkinSprite(roombg.x, roombg.y)`
- [sprite/décor] `spr.animation.play("idle")`
- [autre] `spr.updateHitbox()`
- [autre] `spr.screenCenter(FlxAxes.XY)`
- [sprite/décor] `insert(i, spr)`

**Ignoré** (11)
- [autre] `var positions = [ [false, 855, 409, 386, 233], ]`
- [autre] `var easterEgg = FlxG.random.bool(1/4.2)`
- [autre] `var baseImages = [for(f in Paths.getFolderContent('images/stages/room/frames')) if (Path.extension(f).toLowerCase() == "png") Path.withoutEx`
- [autre] `var images = []`
- [autre] `var curImg = images[FlxG.random.int(0, images.length-1)]`
- [sprite/décor] `images.remove(curImg)`
- [autre] `spr.loadSprite(easterEgg ? Paths.image('stages/room/putin') : Paths.image('stages/room/frames/' + curImg))`
- [sprite/décor] `spr.animation.add("idle", [for(i in 0...spr.frames.frames.length) i], 24, true)`
- [sprite/décor] `spr.scrollFactor = roombg.scrollFactor`
- [autre] `spr.antialiasing = true`
- [autre] `spr.setGraphicSize(FlxG.width *2,FlxG.height * 2)`

**Non suivi** (1)
- [autre] `images = [for(i in baseImages) i]`

### `songs/open system/scripts/camflash.hx`
**Appliqué** (1)
- [caméra] `defaultCamZoom = 0.85` *(postCreate)*

**Ignoré** (11)
- [autre] `cameraMovementEnabled=false` *(postCreate)*
- [shader] `colorTransformShader = new CustomShader("colorTransform")` *(postCreate)*
- [shader] `colorTransformShader.redMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.greenMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.blueMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.alphaMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.redOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.greenOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.blueOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.alphaOffset = 0` *(postCreate)*
- [shader] `camGame.addShader(colorTransformShader)` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : shader 6, HUD 1
- `beatHit` : caméra 8, HUD 2

### `songs/open system/scripts/events.hx`
**Pendant la chanson**
- `stepHit` : caméra 4, HUD 2
- `beatHit` : caméra 5, HUD 5

### `songs/open system/scripts/lol.hx`
**Ignoré** (1)
- [vidéo/cinématique] `playCutscenes = true`

### `songs/open system/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

## Wear A Mask (`wear a mask`)

Décor `room`, persos taeyai, bf, gf

### `data/stages/room.hx`
**Appliqué** (5)
- [autre] `var i = members.indexOf(roombg2)`
- [personnage] `boyfriend.setPosition(930,535)` *(postCreate)*
- [personnage] `dad.setPosition(-50,525)` *(postCreate)*
- [personnage] `dad.alpha = 0.4` *(postCreate)*
- [personnage] `boyfriend.alpha = 0.4` *(postCreate)*

**Ignoré** (14)
- [autre] `var positions = [ [true, 644, 282, 382, 272, [16, 2], [373, 29], [7, 263], [365, 242], 0], [true, 652, 532, 367, 273, [2, 10], [354, 1], [5,`
- [autre] `var easterEgg = FlxG.random.bool(1/4.2)`
- [autre] `var baseImages = [for(f in Paths.getFolderContent('images/stages/room/frames')) if (Path.extension(f).toLowerCase() == "png") Path.withoutEx`
- [autre] `var images = []`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(720,410)` *(postCreate)*
- [shader] `colorShader = new FunkinShader(Assets.getText(Paths.fragShader("colorgrade")))` *(postCreate)*
- [shader] `colorShader.topr = 3.1` *(postCreate)*
- [shader] `colorShader.topg = 1.1` *(postCreate)*
- [shader] `colorShader.topb = 1.1` *(postCreate)*
- [shader] `colorShader.botr = 1.1` *(postCreate)*
- [shader] `colorShader.botg = 1.1` *(postCreate)*
- [shader] `colorShader.botb = 2.1` *(postCreate)*
- [shader] `camGame.addShader(colorShader)` *(postCreate)*

**Non suivi** (23)
- [sprite/décor] `var spr = new FunkinSprite(roombg2.x, roombg2.y)`
- [autre] `images = [for(i in baseImages) i]`
- [autre] `var curImg = images[FlxG.random.int(0, images.length-1)]`
- [sprite/décor] `images.remove(curImg)`
- [autre] `spr.loadSprite(easterEgg ? Paths.image('stages/room/putin') : Paths.image('stages/room/frames/' + curImg))`
- [sprite/décor] `spr.animation.add("idle", [for(i in 0...spr.frames.frames.length) i], 24, true)`
- [sprite/décor] `spr.animation.play("idle")`
- [sprite/décor] `spr.scrollFactor = roombg2.scrollFactor`
- [autre] `spr.antialiasing = true`
- [sprite/décor] `spr.alpha = 0.8`
- [sprite/décor] `spr.x += pos[1]`
- [sprite/décor] `spr.y += pos[2]`
- [autre] `spr.setGraphicSize(pos[3], pos[4])`
- [autre] `spr.updateHitbox()`
- [shader] `spr.shader = new CustomShader("room-shader")`
- [shader] `spr.shader.topLeft = pos[5]`
- [shader] `spr.shader.topRight = pos[6]`
- [shader] `spr.shader.bottomLeft = pos[7]`
- [shader] `spr.shader.bottomRight = pos[8]`
- [shader] `spr.shader.hologramSize = [pos[3], pos[4]]`
- [sprite/décor] `spr.angle = pos[9]`
- [sprite/décor] `insert(i, spr)`
- [personnage] `dad.alpha = boyfriend.alpha = 0.4` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : personnage 2, caméra 1
- `postUpdate` : caméra 3

### `songs/wear a mask/scripts/cock.hx`
**Ignoré** (18)
- [personnage] `var index = members.indexOf(stage.characterPoses["boyfriend"].group)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [sprite/décor] `overlayfx = new FlxSprite(0,0).loadGraphic(Paths.image('fx'))` *(postCreate)*
- [sprite/décor] `overlayfx.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `overlayfx.alpha = 0` *(postCreate)*
- [autre] `overlayfx.cameras = [camfx]` *(postCreate)*
- [autre] `overlayfx.setGraphicSize(FlxG.width * 1.2, FlxG.height * 1.2)` *(postCreate)*
- [autre] `overlayfx.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(overlayfx)` *(postCreate)*

**Pendant la chanson**
- `flash` : sprite/décor 1, tween/timer 1
- `stepHit` : caméra 7, tween/timer 5, HUD 2
- `beatHit` : caméra 7, HUD 4

### `songs/wear a mask/scripts/ex.hx`
**Ignoré** (14)
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(-100,-300)` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 0` *(postCreate)*
- [caméra] `camFollow.setPosition(-100, -900)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*

**Pendant la chanson**
- `bump` : caméra 1, HUD 1
- `onCountdown` : sprite/décor 2, caméra 1
- `stepHit` : caméra 51, tween/timer 17, HUD 10
- `beatHit` : caméra 2, HUD 2

### `songs/wear a mask/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

### `songs/wear a mask/end-cutscene.hx`
**Ignoré** (20)
- [autre] `FlxG.save.data.lasthope = true`
- [autre] `FlxG.save.flush()`
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`
- [autre] `var taskList = new Process("tasklist", [])`
- [autre] `var hereyouare = taskList.stdout.readAll().toString().toLowerCase()`
- [autre] `var checkProgram:Array<String> = ['obs64.exe', 'obs32.exe', 'streamlabs obs.exe', 'streamlabs obs32.exe']`
- [autre] `taskList.close()`

**Non suivi** (17)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `streamer = true`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2
- `update` : sprite/décor 2
- `setSubtitle` : sprite/décor 2

## Wear A Mask (EX) (`wear a mask-ex`)

Décor `roomex`, persos taeyaiex, bfex, 138 événements HScript Call

### `data/stages/roomex.hx`
**Appliqué** (6)
- [autre] `var i = members.indexOf(roombg)`
- [sprite/décor] `var spr = new FunkinSprite(roombg.x, roombg.y)`
- [sprite/décor] `spr.animation.play("idle")`
- [autre] `spr.updateHitbox()`
- [autre] `spr.screenCenter(FlxAxes.XY)`
- [sprite/décor] `insert(i, spr)`

**Ignoré** (11)
- [autre] `var positions = [ [false, 855, 409, 386, 233], ]`
- [autre] `var easterEgg = FlxG.random.bool(1/4.2)`
- [autre] `var baseImages = [for(f in Paths.getFolderContent('images/stages/room/frames')) if (Path.extension(f).toLowerCase() == "png") Path.withoutEx`
- [autre] `var images = []`
- [autre] `var curImg = images[FlxG.random.int(0, images.length-1)]`
- [sprite/décor] `images.remove(curImg)`
- [autre] `spr.loadSprite(easterEgg ? Paths.image('stages/room/putin') : Paths.image('stages/room/frames/' + curImg))`
- [sprite/décor] `spr.animation.add("idle", [for(i in 0...spr.frames.frames.length) i], 24, true)`
- [sprite/décor] `spr.scrollFactor = roombg.scrollFactor`
- [autre] `spr.antialiasing = true`
- [autre] `spr.setGraphicSize(FlxG.width *2,FlxG.height * 2)`

**Non suivi** (1)
- [autre] `images = [for(i in baseImages) i]`

### `songs/wear a mask/scripts/cock.hx`
**Ignoré** (18)
- [personnage] `var index = members.indexOf(stage.characterPoses["boyfriend"].group)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [sprite/décor] `overlayfx = new FlxSprite(0,0).loadGraphic(Paths.image('fx'))` *(postCreate)*
- [sprite/décor] `overlayfx.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `overlayfx.alpha = 0` *(postCreate)*
- [autre] `overlayfx.cameras = [camfx]` *(postCreate)*
- [autre] `overlayfx.setGraphicSize(FlxG.width * 1.2, FlxG.height * 1.2)` *(postCreate)*
- [autre] `overlayfx.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(overlayfx)` *(postCreate)*

**Pendant la chanson**
- `flash` : sprite/décor 1, tween/timer 1
- `stepHit` : caméra 7, tween/timer 5, HUD 2
- `beatHit` : caméra 7, HUD 4

### `songs/wear a mask/scripts/ex.hx`
**Ignoré** (14)
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(-100,-300)` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 0` *(postCreate)*
- [caméra] `camFollow.setPosition(-100, -900)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*

**Pendant la chanson**
- `bump` (appelée 138 fois par le chart) : caméra 1, HUD 1
- `onCountdown` : sprite/décor 2, caméra 1
- `stepHit` : caméra 51, tween/timer 17, HUD 10
- `beatHit` : caméra 2, HUD 2

### `songs/wear a mask/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

### `songs/wear a mask/end-cutscene.hx`
**Ignoré** (20)
- [autre] `FlxG.save.data.lasthope = true`
- [autre] `FlxG.save.flush()`
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`
- [autre] `var taskList = new Process("tasklist", [])`
- [autre] `var hereyouare = taskList.stdout.readAll().toString().toLowerCase()`
- [autre] `var checkProgram:Array<String> = ['obs64.exe', 'obs32.exe', 'streamlabs obs.exe', 'streamlabs obs32.exe']`
- [autre] `taskList.close()`

**Non suivi** (17)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `streamer = true`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2
- `update` : sprite/décor 2
- `setSubtitle` : sprite/décor 2

## Last Hope (`last hope`)

Décor `lastroom`, persos taeyai-evil, bf, gf

### `data/stages/lastroom.hx`
**Appliqué** (11)
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `lastroombg = new FlxSprite(-600, -200).loadGraphic(Paths.image('stages/lastroom/BG'))` *(postCreate)*
- [sprite/décor] `lastroombg.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `insert(index,lastroombg)` *(postCreate)*
- [sprite/décor] `beam = new FlxSprite(-600, -200).loadGraphic(Paths.image('stages/lastroom/beam'))` *(postCreate)*
- [sprite/décor] `beam.scrollFactor.set(0.9, 0.9)` *(postCreate)*
- [sprite/décor] `beam.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `beam.alpha = 0` *(postCreate)*
- [sprite/décor] `add(beam)` *(postCreate)*
- [personnage] `boyfriend.setPosition(1120,370)` *(postCreate)*
- [personnage] `dad.setPosition(-100,100)` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : personnage 2, sprite/décor 1
- `onCameraMove` : personnage 1
- `update` : sprite/décor 2

### `songs/last hope/scripts/ex.hx`
**Ignoré** (25)
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(eventbackground)` *(postCreate)*
- [shader] `chromaticGame = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromaticGame.chromaticAmount = 0.002` *(postCreate)*
- [shader] `camGame.addShader(chromaticGame)` *(postCreate)*
- [shader] `glitch = new CustomShader('screenglitch')` *(postCreate)*
- [autre] `glitch.enabled = false` *(postCreate)*
- [shader] `camGame.addShader(glitch)` *(postCreate)*
- [shader] `hudglitch = new CustomShader('hudglitch')` *(postCreate)*
- [autre] `hudglitch.enabled = false` *(postCreate)*
- [shader] `camHUD.addShader(hudglitch)` *(postCreate)*
- [shader] `camNewHUD.addShader(hudglitch)` *(postCreate)*
- [autre] `coolText = new FunkinText(0,0,0,'I CAN', 46, true)` *(postCreate)*
- [autre] `coolText.screenCenter()` *(postCreate)*
- [autre] `coolText.antialiasing = true` *(postCreate)*
- [sprite/décor] `coolText.alpha = 0` *(postCreate)*
- [autre] `coolText.borderSize = 3` *(postCreate)*
- [autre] `coolText.font = Paths.font("freeplay.ttf")` *(postCreate)*
- [HUD] `coolText.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `add(coolText)` *(postCreate)*

**Pendant la chanson**
- `textup` : HUD 2, sprite/décor 1, caméra 1, tween/timer 1
- `bump` : HUD 2, caméra 1
- `doglitch` : caméra 2
- `beatHit` : HUD 14, caméra 7
- `stepHit` : HUD 5, sprite/décor 4
- `notebump` : HUD 56

### `songs/last hope/scripts/nice tricks, camera!.hx`
**Appliqué** (3)
- [personnage] `boyfriend.cameraOffset.y -= 50` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.x += 100` *(postCreate)*

**Ignoré** (42)
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [caméra] `camGame.rotateSprite = false` *(postCreate)*
- [personnage] `var camPoss:FlxPoint = new FlxPoint(boyfriend.getGraphicMidpoint().x, boyfriend.getGraphicMidpoint().y)` *(postCreate)*
- [personnage] `var camPoss2:FlxPoint = new FlxPoint(dad.getGraphicMidpoint().x, dad.getGraphicMidpoint().y)` *(postCreate)*
- [caméra] `add(newcamFollow)` *(postCreate)*
- [caméra] `add(newcamFollow2)` *(postCreate)*
- [personnage] `createCamera(_halfWidth, 0xFFFFCCCC, boyfriend)` *(postCreate)*
- [personnage] `newcamFollow.setPosition(dad.getGraphicMidpoint().x,dad.getGraphicMidpoint().y + 100)` *(postCreate)*
- [personnage] `newcamFollow2.setPosition(boyfriend.getGraphicMidpoint().x,boyfriend.getGraphicMidpoint().y + 300)` *(postCreate)*
- [autre] `camsplit1.width = FlxG.width / 2` *(postCreate)*
- [autre] `camsplit1.height = FlxG.height` *(postCreate)*
- [sprite/décor] `camsplit1.alpha = 1` *(postCreate)*
- [caméra] `camsplit1.zoom = 0.7` *(postCreate)*
- [sprite/décor] `camsplit1.x -= FlxG.width` *(postCreate)*
- [caméra] `camsplit1.follow(newcamFollow)` *(postCreate)*
- [caméra] `camsplit1.focusOn(newcamFollow.getPosition())` *(postCreate)*
- [autre] `camsplit2.width = FlxG.width / 2` *(postCreate)*
- [autre] `camsplit2.height = FlxG.height` *(postCreate)*
- [sprite/décor] `camsplit2.x += FlxG.width` *(postCreate)*
- [caméra] `camsplit2.follow(newcamFollow2)` *(postCreate)*
- [caméra] `camsplit2.focusOn(newcamFollow2.getPosition())` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 10` *(postCreate)*
- [shader] `radialBlur = new CustomShader('radialBlur')` *(postCreate)*
- [autre] `radialBlur.cx = 0` *(postCreate)*
- [autre] `radialBlur.cy = 0` *(postCreate)*
- [autre] `radialBlur.blurWidth = 0` *(postCreate)*
- [autre] `radialBlur.chromaticAmount = 0` *(postCreate)*
- [shader] `camGame.addShader(radialBlur)` *(postCreate)*
- [shader] `glitch = new CustomShader('screenglitch')` *(postCreate)*
- [autre] `glitch.enabled = false` *(postCreate)*
- [shader] `camGame.addShader(glitch)` *(postCreate)*
- [shader] `hudglitch = new CustomShader('hudglitch')` *(postCreate)*
- [autre] `hudglitch.enabled = false` *(postCreate)*
- [shader] `camHUD.addShader(hudglitch)` *(postCreate)*
- [shader] `camNewHUD.addShader(hudglitch)` *(postCreate)*

**Non suivi** (6)
- [caméra] `newcamFollow = newprevCamFollow` *(postCreate)*
- [autre] `newprevCamFollow = null` *(postCreate)*
- [caméra] `newcamFollow = new FlxObject(0, 0, 2, 2)` *(postCreate)*
- [caméra] `newcamFollow.setPosition(camPoss.x, camPoss.y)` *(postCreate)*
- [caméra] `newcamFollow2 = new FlxObject(0, 0, 2, 2)` *(postCreate)*
- [caméra] `newcamFollow2.setPosition(camPoss2.x, camPoss2.y)` *(postCreate)*

**Pendant la chanson**
- `beatHit` : HUD 20, caméra 10, tween/timer 2
- `onCameraMove` : caméra 4
- `stepHit` : caméra 19, HUD 15, tween/timer 9
- `part` : caméra 2, HUD 2, personnage 1
- `notemove` : HUD 16, tween/timer 4
- `camflip` : HUD 4, caméra 3
- `createCamera` : HUD 4, caméra 2

### `songs/last hope/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

### `songs/last hope/end-cutscene.hx`
**Ignoré** (16)
- [autre] `FlxG.save.data.lasthope = false`
- [autre] `FlxG.save.flush()`
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2
- `update` : sprite/décor 2
- `setSubtitle` : sprite/décor 2

## Last Hope (EX) (`last hope-ex`)

Décor `lastroom`, persos taeyaiex-fly, bfex, 288 événements HScript Call

### `data/stages/lastroom.hx`
**Appliqué** (11)
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `lastroombg = new FlxSprite(-600, -200).loadGraphic(Paths.image('stages/lastroom/BG'))` *(postCreate)*
- [sprite/décor] `lastroombg.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `insert(index,lastroombg)` *(postCreate)*
- [sprite/décor] `beam = new FlxSprite(-600, -200).loadGraphic(Paths.image('stages/lastroom/beam'))` *(postCreate)*
- [sprite/décor] `beam.scrollFactor.set(0.9, 0.9)` *(postCreate)*
- [sprite/décor] `beam.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `beam.alpha = 0` *(postCreate)*
- [sprite/décor] `add(beam)` *(postCreate)*
- [personnage] `boyfriend.setPosition(1120,370)` *(postCreate)*
- [personnage] `dad.setPosition(-100,100)` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : personnage 2, sprite/décor 1
- `onCameraMove` : personnage 1
- `update` : sprite/décor 2

### `songs/last hope/scripts/ex.hx`
**Ignoré** (25)
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(eventbackground)` *(postCreate)*
- [shader] `chromaticGame = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromaticGame.chromaticAmount = 0.002` *(postCreate)*
- [shader] `camGame.addShader(chromaticGame)` *(postCreate)*
- [shader] `glitch = new CustomShader('screenglitch')` *(postCreate)*
- [autre] `glitch.enabled = false` *(postCreate)*
- [shader] `camGame.addShader(glitch)` *(postCreate)*
- [shader] `hudglitch = new CustomShader('hudglitch')` *(postCreate)*
- [autre] `hudglitch.enabled = false` *(postCreate)*
- [shader] `camHUD.addShader(hudglitch)` *(postCreate)*
- [shader] `camNewHUD.addShader(hudglitch)` *(postCreate)*
- [autre] `coolText = new FunkinText(0,0,0,'I CAN', 46, true)` *(postCreate)*
- [autre] `coolText.screenCenter()` *(postCreate)*
- [autre] `coolText.antialiasing = true` *(postCreate)*
- [sprite/décor] `coolText.alpha = 0` *(postCreate)*
- [autre] `coolText.borderSize = 3` *(postCreate)*
- [autre] `coolText.font = Paths.font("freeplay.ttf")` *(postCreate)*
- [HUD] `coolText.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `add(coolText)` *(postCreate)*

**Pendant la chanson**
- `textup` (appelée 6 fois par le chart) : HUD 2, sprite/décor 1, caméra 1, tween/timer 1
- `bump` (appelée 159 fois par le chart) : HUD 2, caméra 1
- `doglitch` (appelée 5 fois par le chart) : caméra 2
- `beatHit` : HUD 14, caméra 7
- `stepHit` : HUD 5, sprite/décor 4
- `notebump` (appelée 40 fois par le chart) : HUD 56

### `songs/last hope/scripts/nice tricks, camera!.hx`
**Appliqué** (3)
- [personnage] `boyfriend.cameraOffset.y -= 50` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.x += 100` *(postCreate)*

**Ignoré** (42)
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [caméra] `camGame.rotateSprite = false` *(postCreate)*
- [personnage] `var camPoss:FlxPoint = new FlxPoint(boyfriend.getGraphicMidpoint().x, boyfriend.getGraphicMidpoint().y)` *(postCreate)*
- [personnage] `var camPoss2:FlxPoint = new FlxPoint(dad.getGraphicMidpoint().x, dad.getGraphicMidpoint().y)` *(postCreate)*
- [caméra] `add(newcamFollow)` *(postCreate)*
- [caméra] `add(newcamFollow2)` *(postCreate)*
- [personnage] `createCamera(_halfWidth, 0xFFFFCCCC, boyfriend)` *(postCreate)*
- [personnage] `newcamFollow.setPosition(dad.getGraphicMidpoint().x,dad.getGraphicMidpoint().y + 100)` *(postCreate)*
- [personnage] `newcamFollow2.setPosition(boyfriend.getGraphicMidpoint().x,boyfriend.getGraphicMidpoint().y + 300)` *(postCreate)*
- [autre] `camsplit1.width = FlxG.width / 2` *(postCreate)*
- [autre] `camsplit1.height = FlxG.height` *(postCreate)*
- [sprite/décor] `camsplit1.alpha = 1` *(postCreate)*
- [caméra] `camsplit1.zoom = 0.7` *(postCreate)*
- [sprite/décor] `camsplit1.x -= FlxG.width` *(postCreate)*
- [caméra] `camsplit1.follow(newcamFollow)` *(postCreate)*
- [caméra] `camsplit1.focusOn(newcamFollow.getPosition())` *(postCreate)*
- [autre] `camsplit2.width = FlxG.width / 2` *(postCreate)*
- [autre] `camsplit2.height = FlxG.height` *(postCreate)*
- [sprite/décor] `camsplit2.x += FlxG.width` *(postCreate)*
- [caméra] `camsplit2.follow(newcamFollow2)` *(postCreate)*
- [caméra] `camsplit2.focusOn(newcamFollow2.getPosition())` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 10` *(postCreate)*
- [shader] `radialBlur = new CustomShader('radialBlur')` *(postCreate)*
- [autre] `radialBlur.cx = 0` *(postCreate)*
- [autre] `radialBlur.cy = 0` *(postCreate)*
- [autre] `radialBlur.blurWidth = 0` *(postCreate)*
- [autre] `radialBlur.chromaticAmount = 0` *(postCreate)*
- [shader] `camGame.addShader(radialBlur)` *(postCreate)*
- [shader] `glitch = new CustomShader('screenglitch')` *(postCreate)*
- [autre] `glitch.enabled = false` *(postCreate)*
- [shader] `camGame.addShader(glitch)` *(postCreate)*
- [shader] `hudglitch = new CustomShader('hudglitch')` *(postCreate)*
- [autre] `hudglitch.enabled = false` *(postCreate)*
- [shader] `camHUD.addShader(hudglitch)` *(postCreate)*
- [shader] `camNewHUD.addShader(hudglitch)` *(postCreate)*

**Non suivi** (6)
- [caméra] `newcamFollow = newprevCamFollow` *(postCreate)*
- [autre] `newprevCamFollow = null` *(postCreate)*
- [caméra] `newcamFollow = new FlxObject(0, 0, 2, 2)` *(postCreate)*
- [caméra] `newcamFollow.setPosition(camPoss.x, camPoss.y)` *(postCreate)*
- [caméra] `newcamFollow2 = new FlxObject(0, 0, 2, 2)` *(postCreate)*
- [caméra] `newcamFollow2.setPosition(camPoss2.x, camPoss2.y)` *(postCreate)*

**Pendant la chanson**
- `beatHit` : HUD 20, caméra 10, tween/timer 2
- `onCameraMove` : caméra 4
- `stepHit` : caméra 19, HUD 15, tween/timer 9
- `part` : caméra 2, HUD 2, personnage 1
- `notemove` : HUD 16, tween/timer 4
- `camflip` : HUD 4, caméra 3
- `createCamera` : HUD 4, caméra 2

### `songs/last hope/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

### `songs/last hope/end-cutscene.hx`
**Ignoré** (16)
- [autre] `FlxG.save.data.lasthope = false`
- [autre] `FlxG.save.flush()`
- [autre] `jsonString = Paths.file('videos/CH1/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch1/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2
- `update` : sprite/décor 2
- `setSubtitle` : sprite/décor 2

## Sensical (`sensical`)

Décor `nonsense`, persos nonsense, taeyai

### `data/stages/nonsense.hx`
**Appliqué** (1)
- [personnage] `gf.visible = false` *(postCreate)*

### `songs/sensical/scripts/events.hx`
**Appliqué** (2)
- [personnage] `dad.cameraOffset.y -= 100` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y += 60` *(postCreate)*

**Ignoré** (19)
- [autre] `cameraMovementEnabled=false` *(postCreate)*
- [shader] `colorTransformShader = new CustomShader("colorTransform")` *(postCreate)*
- [shader] `colorTransformShader.redMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.greenMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.blueMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.alphaMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.redOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.greenOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.blueOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.alphaOffset = 0` *(postCreate)*
- [shader] `camGame.addShader(colorTransformShader)` *(postCreate)*
- [autre] `camLock = true` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 0` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `black = new FlxSprite(-800, -800)` *(postCreate)*
- [sprite/décor] `black.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFF000000)` *(postCreate)*
- [sprite/décor] `black.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `black.alpha = 0` *(postCreate)*
- [sprite/décor] `insert(index, black)` *(postCreate)*

**Pendant la chanson**
- `onCameraMove` : personnage 5, caméra 2
- `postUpdate` : shader 9
- `beatHit` : caméra 1, HUD 1
- `stepHit` : caméra 18, HUD 5, tween/timer 1, sprite/décor 1

### `songs/sensical/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH2/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch2/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

## Sensational (`sensational`)

Décor `nonsense`, persos sense, taeyai, gf

### `data/stages/nonsense.hx`
**Appliqué** (1)
- [personnage] `gf.visible = false` *(postCreate)*

### `songs/sensational/scripts/event.hx`
**Ignoré** (7)
- [sprite/décor] `no = new FlxSprite(0,0)` *(postCreate)*
- [sprite/décor] `no.frames = Paths.getSparrowAtlas('stages/nonsense/nope')` *(postCreate)*
- [sprite/décor] `no.animation.addByPrefix('no', 'no', 24, false)` *(postCreate)*
- [autre] `no.screenCenter()` *(postCreate)*
- [HUD] `no.cameras=[camHUD]` *(postCreate)*
- [sprite/décor] `no.visible=false` *(postCreate)*
- [sprite/décor] `add(no)` *(postCreate)*

**Pendant la chanson**
- `bubble` : sprite/décor 4
- `beatHit` : personnage 1

### `songs/sensational/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH2/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch2/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

## Resensation (`resensation`)

Décor `voidsense`, persos sense, taeyainoholo, bf

### `data/stages/voidsense.hx`
**Appliqué** (16)
- [autre] `var index2 = members.indexOf(debrisback)` *(postCreate)*
- [autre] `var index = members.indexOf(debris)` *(postCreate)*
- [sprite/décor] `bf.scrollFactor.set(0.8, 0.8)` *(postCreate)*
- [sprite/décor] `insert(index, bf)` *(postCreate)*
- [personnage] `gf.visible = false` *(postCreate)*
- [autre] `var index = members.indexOf(bg2)` *(postCreate)*
- [sprite/décor] `red = new FlxSprite(-800, -800)` *(postCreate)*
- [sprite/décor] `red.scrollFactor.set(0.1, 0.1)` *(postCreate)*
- [sprite/décor] `red.alpha = 0.5` *(postCreate)*
- [autre] `red.blend = 0` *(postCreate)*
- [sprite/décor] `insert(index,red)` *(postCreate)*
- [sprite/décor] `forhead = new FlxSprite(-800, -800)` *(postCreate)*
- [sprite/décor] `forhead.scrollFactor.set(0.8, 0.8)` *(postCreate)*
- [sprite/décor] `forhead.alpha = 0.1` *(postCreate)*
- [autre] `forhead.blend = 0` *(postCreate)*
- [sprite/décor] `add(forhead)` *(postCreate)*

**Ignoré** (31)
- [caméra] `camGame.zoom = 0.4` *(postCreate)*
- [autre] `debrisback.flipX = true` *(postCreate)*
- [autre] `bg2.blend = 0` *(postCreate)*
- [sprite/décor] `bg2.alpha = 0.5` *(postCreate)*
- [autre] `bg2.flipX = true` *(postCreate)*
- [autre] `bg3.blend = 0` *(postCreate)*
- [sprite/décor] `bg3.alpha = 0.25` *(postCreate)*
- [autre] `bg3.flipY = true` *(postCreate)*
- [autre] `objectGroup = new FlxTypedGroup()` *(postCreate)*
- [sprite/décor] `insert(index2, objectGroup)` *(postCreate)*
- [personnage] `flyed = dad` *(postCreate)*
- [personnage] `bf = new Character(1340, debrisback.y - 550, 'bf', true)` *(postCreate)*
- [autre] `bfMid = bf.getGraphicMidpoint()` *(postCreate)*
- [sprite/décor] `debris2.y += 100` *(postCreate)*
- [sprite/décor] `debris.y -= 150` *(postCreate)*
- [autre] `flystyle = 'hover'` *(postCreate)*
- [caméra] `camGame.scroll.set(453,310)` *(postCreate)*
- [HUD] `strumLines.members[2].characters = [bf]` *(postCreate)*
- [shader] `spaceShader = new FunkinShader(Assets.getText(Paths.fragShader("space")))` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `glowShader = new FunkinShader(Assets.getText(Paths.fragShader("edgeglow")))` *(postCreate)*
- [sprite/décor] `red.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFdb0000)` *(postCreate)*
- [sprite/décor] `forhead.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFdb0000)` *(postCreate)*
- [shader] `red.shader = spaceShader` *(postCreate)*
- [shader] `forhead.shader = spaceShader` *(postCreate)*
- [shader] `bg.shader = glowShader` *(postCreate)*
- [shader] `bg2.shader = glowShader` *(postCreate)*
- [shader] `bg3.shader = glowShader` *(postCreate)*
- [shader] `bg.shader = bloomShader` *(postCreate)*
- [shader] `bg2.shader = bloomShader` *(postCreate)*
- [shader] `bg3.shader = bloomShader` *(postCreate)*

**Non suivi** (11)
- [sprite/décor] `obj = new FunkinSprite(-300,-100)` *(postCreate)*
- [sprite/décor] `obj.scrollFactor.set(0.45, 0.45)` *(postCreate)*
- [autre] `obj.loadSprite(Paths.image("stages/nonsense/break/object"))` *(postCreate)*
- [autre] `obj.playAnim(objectName[i], false)` *(postCreate)*
- [sprite/décor] `obj.scale.set(0.8,0.8)` *(postCreate)*
- [sprite/décor] `obj.setPosition(objPos[i][0], objPos[i][1])` *(postCreate)*
- [autre] `obj.ID = i` *(postCreate)*
- [sprite/décor] `objectGroup.add(obj)` *(postCreate)*
- [caméra] `s.cameras = [camGame]` *(postCreate)*
- [sprite/décor] `s.scrollFactor.set(0.8, 0.8)` *(postCreate)*
- [sprite/décor] `s.alpha = 0.4` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : caméra 1
- `update` : sprite/décor 9, shader 4
- `stepHit` : personnage 2
- `postUpdate` : personnage 5, sprite/décor 4
- `onPostNoteCreation` : sprite/décor 1

### `songs/resensation/scripts/event.hx`
### `songs/resensation/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH2/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch2/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

### `songs/resensation/end-cutscene.hx`
**Ignoré** (16)
- [autre] `FlxG.save.data.lasthope = false`
- [autre] `FlxG.save.flush()`
- [autre] `jsonString = Paths.file('videos/CH2/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch2/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2
- `update` : sprite/décor 2
- `setSubtitle` : sprite/décor 2

## GuideLine (`guideline`)

Décor `tiny`, persos tiny, bf, gf

### `data/stages/tiny.hx`
**Appliqué** (7)
- [personnage] `var index = members.indexOf(boyfriend)`
- [sprite/décor] `white = new FlxSprite(-800, -800)`
- [sprite/décor] `white.scrollFactor.set(0, 0)`
- [autre] `white.blend = 1`
- [sprite/décor] `insert(index, white)`
- [personnage] `boyfriend.y += 70` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*

**Ignoré** (11)
- [sprite/décor] `white.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFffffff)`
- [personnage] `canvas1 = new FlxShapeCircle(boyfriend.x + 120,boyfriend.y + 630,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas1)` *(postCreate)*
- [sprite/décor] `canvas1.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas1.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas1.alpha = 0.3` *(postCreate)*
- [personnage] `canvas2 = new FlxShapeCircle(dad.x + 140,dad.y + 580,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas2)` *(postCreate)*
- [sprite/décor] `canvas2.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas2.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas2.alpha = 0.3` *(postCreate)*

### `songs/guideline/scripts/lyrics.hx`
**Ignoré** (10)
- [autre] `lyrics = new FunkinText(525, 547, 0, "YOUR MOM", 24, true)` *(postCreate)*
- [sprite/décor] `lyrics.alpha = 0` *(postCreate)*
- [HUD] `lyrics.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `background = new FlxSprite((FlxG.width / 2) - 1, lyrics.y - 5).makeGraphic(2, Std.int(lyrics.height + 10), 0xFF000000)` *(postCreate)*
- [sprite/décor] `background.alpha = 0` *(postCreate)*
- [autre] `background.updateHitbox()` *(postCreate)*
- [HUD] `background.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `background.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(background)` *(postCreate)*
- [sprite/décor] `add(lyrics)` *(postCreate)*

**Pendant la chanson**
- `popinLyrics` : sprite/décor 3, tween/timer 2
- `popoutLyrics` : tween/timer 2
- `stepHit` : sprite/décor 1

### `songs/guideline/cutscene.hx`
**Non suivi** (18)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37/2, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!) Basically! After BF jump into portal he appear in white void space and he have no idea where the`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [vidéo/cinématique] `PlayState.instance.inCutscene = false`
- [autre] `PlayState.instance.startCountdown()`

**Pendant la chanson**
- `update` : sprite/décor 3, vidéo/cinématique 1

## Common Sense (EX) (`common sense-ex`)

Décor `nonsense`, persos nonsense-ex, taeyaiex, gf

### `data/stages/nonsense.hx`
**Appliqué** (1)
- [personnage] `gf.visible = false` *(postCreate)*

## Highlights (EX) (`highlights-ex`)

Décor `nonsense`, persos nonsense-ex, taeyaiex, gf

### `data/stages/nonsense.hx`
**Appliqué** (1)
- [personnage] `gf.visible = false` *(postCreate)*

### `songs/highlights/scripts/event.hx`
**Pendant la chanson**
- `beatHit` : caméra 8, HUD 1

## Context (EX) (`context-ex`)

Décor `nonsense`, persos nonsense-ex, taeyaiex, gf

### `data/stages/nonsense.hx`
**Appliqué** (1)
- [personnage] `gf.visible = false` *(postCreate)*

### `songs/context/scripts/event.hx`
**Pendant la chanson**
- `beatHit` : caméra 13, HUD 1

## Nonsensical (EX) (`nonsensical-ex`)

Décor `nonsense`, persos nonsense-ex, taeyaiex, gf

### `data/stages/nonsense.hx`
**Appliqué** (1)
- [personnage] `gf.visible = false` *(postCreate)*

### `songs/nonsensical/scripts/events.hx`
**Ignoré** (81)
- [autre] `cameraMovementEnabled=false` *(postCreate)*
- [shader] `colorTransformShader = new CustomShader("colorTransform")` *(postCreate)*
- [shader] `colorTransformShader.redMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.greenMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.blueMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.alphaMultiplier = 1` *(postCreate)*
- [shader] `colorTransformShader.redOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.greenOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.blueOffset = 0` *(postCreate)*
- [shader] `colorTransformShader.alphaOffset = 0` *(postCreate)*
- [shader] `camGame.addShader(colorTransformShader)` *(postCreate)*
- [shader] `screen = new CustomShader("split-screen")` *(postCreate)*
- [autre] `screen.multi = 1` *(postCreate)*
- [shader] `camGame.addShader(screen)` *(postCreate)*
- [shader] `loops = new CustomShader("loop")` *(postCreate)*
- [shader] `distortion = new CustomShader("distortion")` *(postCreate)*
- [shader] `spin = new CustomShader("spinbg")` *(postCreate)*
- [shader] `beatbg = new CustomShader("beatbg")` *(postCreate)*
- [shader] `disbg = new CustomShader("dis")` *(postCreate)*
- [autre] `cameffect = new FlxCamera()` *(postCreate)*
- [autre] `cameffect.bgColor = 0` *(postCreate)*
- [autre] `camChar = new FlxCamera()` *(postCreate)*
- [autre] `camChar.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camChar, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(cameffect, false)` *(postCreate)*
- [sprite/décor] `image = new FlxSprite(0, 0)` *(postCreate)*
- [autre] `image.antialiasing = true` *(postCreate)*
- [sprite/décor] `image.loadGraphic(Paths.image('game/pause/hologram'))` *(postCreate)*
- [autre] `image.blend = 9` *(postCreate)*
- [shader] `image.shader = spin` *(postCreate)*
- [autre] `image.updateHitbox()` *(postCreate)*
- [autre] `image.screenCenter(FlxAxes.XY)` *(postCreate)*
- [autre] `image.cameras = [cameffect]` *(postCreate)*
- [sprite/décor] `image.visible =false` *(postCreate)*
- [sprite/décor] `add(image)` *(postCreate)*
- [sprite/décor] `image2 = new FlxSprite(-2225, -1215)` *(postCreate)*
- [autre] `image2.antialiasing = true` *(postCreate)*
- [sprite/décor] `image2.loadGraphic(Paths.image('game/pause/hologram'))` *(postCreate)*
- [shader] `image2.shader = beatbg` *(postCreate)*
- [autre] `image2.setGraphicSize(FlxG.width * 5, FlxG.height * 5)` *(postCreate)*
- [autre] `image2.updateHitbox()` *(postCreate)*
- [caméra] `image2.cameras = [camGame]` *(postCreate)*
- [sprite/décor] `image2.visible = false` *(postCreate)*
- [personnage] `insert(members.indexOf(boyfriend), image2)` *(postCreate)*
- [sprite/décor] `image3 = new FlxSprite(0, 0)` *(postCreate)*
- [autre] `image3.antialiasing = true` *(postCreate)*
- [sprite/décor] `image3.loadGraphic(Paths.image('noise'))` *(postCreate)*
- [autre] `image3.blend = 9` *(postCreate)*
- [shader] `image3.shader = disbg` *(postCreate)*
- [autre] `image3.setGraphicSize(FlxG.width, FlxG.height)` *(postCreate)*
- [autre] `image3.updateHitbox()` *(postCreate)*
- [autre] `image3.screenCenter(FlxAxes.XY)` *(postCreate)*
- [autre] `image3.cameras = [cameffect]` *(postCreate)*
- [sprite/décor] `image3.alpha = 0.1` *(postCreate)*
- [sprite/décor] `image3.visible =false` *(postCreate)*
- [sprite/décor] `add(image3)` *(postCreate)*
- [personnage] `non = new Character(100,100,dad.curCharacter,false)` *(postCreate)*
- [HUD] `stage.applyCharStuff(non,cpuStrums.data.position,0)` *(postCreate)*
- [autre] `non.cameras = [camChar]` *(postCreate)*
- [sprite/décor] `non.y += FlxG.height` *(postCreate)*
- [sprite/décor] `add(non)` *(postCreate)*
- [personnage] `tae = new Character(100,100,boyfriend.curCharacter,boyfriend.isPlayer)` *(postCreate)*
- [HUD] `stage.applyCharStuff(tae,playerStrums.data.position,1)` *(postCreate)*
- [sprite/décor] `tae.x -= 300` *(postCreate)*
- [sprite/décor] `tae.y += FlxG.height` *(postCreate)*
- [autre] `tae.cameras = [camChar]` *(postCreate)*
- [sprite/décor] `add(tae)` *(postCreate)*
- [shader] `iLikeToMoveItMoveIt = new CustomShader('i like to move it move it', "130")` *(postCreate)*
- [shader] `camGame.addShader(iLikeToMoveItMoveIt)` *(postCreate)*
- [sprite/décor] `scream = new FlxSprite(0, 0)` *(postCreate)*
- [autre] `scream.antialiasing = true` *(postCreate)*
- [sprite/décor] `scream.loadGraphic(Paths.image('scream'))` *(postCreate)*
- [autre] `scream.blend = 8` *(postCreate)*
- [autre] `scream.updateHitbox()` *(postCreate)*
- [autre] `scream.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `scream.x += 90` *(postCreate)*
- [HUD] `scream.cameras = [camNewHUD]` *(postCreate)*
- [sprite/décor] `scream.alpha = 0` *(postCreate)*
- [sprite/décor] `add(scream)` *(postCreate)*

**Pendant la chanson**
- `screamit` : sprite/décor 2, tween/timer 1
- `postUpdate` : sprite/décor 15, shader 6, HUD 1
- `onCameraMove` : caméra 10, personnage 5
- `stepHit` : caméra 5, HUD 3
- `beatHit` : caméra 36, HUD 28, tween/timer 20, shader 10, sprite/décor 9, personnage 6

## You And Me (`you and me`)

Décor `garage`, persos cm-cj, cm-tae, gf

### `data/stages/garage.hx`
**Appliqué** (7)
- [personnage] `boyfriend.y = 130` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y += 100` *(postCreate)*
- [personnage] `dad.y += 40` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `rayTracker = new FlxSprite(1430,145)` *(postCreate)*
- [sprite/décor] `rayTracker.scale.set(0.75,0.75)` *(postCreate)*
- [autre] `rayTracker.cameras = [camNoRays]` *(postCreate)*

**Ignoré** (46)
- [personnage] `camGame.scroll.set(dad.getMidpoint().x + 200,dad.getMidpoint().y)`
- [autre] `light.blend = 0` *(postCreate)*
- [personnage] `camGame.scroll.set(dad.getMidpoint().x + 200,dad.getMidpoint().y)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, dad.curCharacter, false)` *(postCreate)*
- [autre] `redad.flipY = true` *(postCreate)*
- [sprite/décor] `redad.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `redad.alpha = 0.4` *(postCreate)*
- [personnage] `redad.cameraOffset = dad.cameraOffset` *(postCreate)*
- [caméra] `redad.cameraOffset.x += 130` *(postCreate)*
- [caméra] `redad.cameraOffset.y -= 300` *(postCreate)*
- [personnage] `rebf = new Character(dad.x, dad.y, 'cm-tae', true)` *(postCreate)*
- [autre] `rebf.flipY = true` *(postCreate)*
- [sprite/décor] `rebf.x = 590` *(postCreate)*
- [sprite/décor] `rebf.y = 756` *(postCreate)*
- [sprite/décor] `rebf.skew.x = -210` *(postCreate)*
- [sprite/décor] `rebf.skew.y = -175` *(postCreate)*
- [sprite/décor] `rebf.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `rebf.alpha = 0.4` *(postCreate)*
- [personnage] `rebf.cameraOffset = boyfriend.cameraOffset` *(postCreate)*
- [caméra] `rebf.cameraOffset.y -= 300` *(postCreate)*
- [sprite/décor] `insert(index, rebf)` *(postCreate)*
- [sprite/décor] `insert(index, redad)` *(postCreate)*
- [autre] `rayTracker.active = false` *(postCreate)*
- [autre] `camRays = new FlxCamera()` *(postCreate)*
- [autre] `camRays.bgColor = 0` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godrays-morning")))` *(postCreate)*
- [shader] `camRays.addShader(rayShader)` *(postCreate)*
- [autre] `camNoRays = new FlxCamera()` *(postCreate)*
- [autre] `camNoRays.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.remove(camGame, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camGame, true)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camRays, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camNoRays, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [personnage] `dad.cameras = [camRays]` *(postCreate)*
- [personnage] `boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = [camRays]` *(postCreate)*
- [autre] `redad.cameras = [camRays]` *(postCreate)*
- [HUD] `cpuStrums.characters = [redad,dad]` *(postCreate)*
- [HUD] `playerStrums.characters = [rebf,boyfriend]` *(postCreate)*

**Non suivi** (81)
- [autre] `i++` *(postCreate)*
- [shader] `c.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `c.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `c.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `c.shader.flipped = c.flipX` *(postCreate)*
- [personnage] `dad.y -= 40` *(postCreate)*
- [sprite/décor] `redad.x = -114` *(postCreate)*
- [sprite/décor] `redad.y = 770` *(postCreate)*
- [sprite/décor] `redad.skew.x = -211` *(postCreate)*
- [sprite/décor] `redad.skew.y = 0` *(postCreate)*
- [sprite/décor] `redad.x = -99` *(postCreate)*
- [sprite/décor] `redad.y = 730` *(postCreate)*
- [sprite/décor] `redad.skew.x = -211` *(postCreate)*
- [sprite/décor] `redad.skew.y = -20` *(postCreate)*
- [personnage] `dad.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = [camRays]` *(postCreate)*
- [sprite/décor] `redad.visible = rebf.visible = false` *(postCreate)*
- [personnage] `dad.cameraOffset.y += 200` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 99` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].cameraOffset.x += 300` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].cameraOffset.y += 200` *(postCreate)*
- [HUD] `strumLines.members[0].characters[0].setPosition(140 ,125 )` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].setPosition(-205 ,225 )` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].cameraOffset.x += 300` *(postCreate)*
- [HUD] `strumLines.members[6].characters[0].cameraOffset.x = 550` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].setPosition(-105 ,90 )` *(postCreate)*
- [HUD] `strumLines.members[5].characters[0].setPosition(1020 ,95 )` *(postCreate)*
- [HUD] `strumLines.members[6].characters[0].setPosition(205,145)` *(postCreate)*
- [autre] `var index = members.indexOf(sofa)` *(postCreate)*
- [sprite/décor] `remove(bg)` *(postCreate)*
- [sprite/décor] `bg = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/evening'))` *(postCreate)*
- [sprite/décor] `bg.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `bg.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,bg)` *(postCreate)*
- [sprite/décor] `fade = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/fade'))` *(postCreate)*
- [sprite/décor] `fade.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `fade.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,fade)` *(postCreate)*
- [personnage] `var index = members.indexOf(gf)` *(postCreate)*
- [sprite/décor] `remove(sofa)` *(postCreate)*
- [sprite/décor] `sofa = new FlxSprite(-1050,-525).loadGraphic(Paths.image('stages/garage/sofa-evening'))` *(postCreate)*
- [sprite/décor] `sofa.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `sofa.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,sofa)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `remove(light)` *(postCreate)*
- [sprite/décor] `light = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/light-evening'))` *(postCreate)*
- [sprite/décor] `light.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,light)` *(postCreate)*
- [personnage] `dad.x -= 90` *(postCreate)*
- [personnage] `dad.y += 60` *(postCreate)*
- [personnage] `gf.y -= 40` *(postCreate)*
- [personnage] `gf.x -= 120` *(postCreate)*
- [personnage] `boyfriend.y += 50` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, 'cm-cj', false)` *(postCreate)*
- [sprite/décor] `redad.y = 775` *(postCreate)*
- [sprite/décor] `redad.x = -204` *(postCreate)*
- [sprite/décor] `redad.skew.x = -211` *(postCreate)*
- [sprite/décor] `redad.skew.y = -20` *(postCreate)*
- [personnage] `regf = new Character(gf.x, gf.y, 'cm-ruby', false)` *(postCreate)*
- [autre] `regf.flipY = true` *(postCreate)*
- [sprite/décor] `regf.x = -144` *(postCreate)*
- [sprite/décor] `regf.y = 774` *(postCreate)*
- [sprite/décor] `regf.skew.x = -46` *(postCreate)*
- [sprite/décor] `regf.skew.y = 0` *(postCreate)*
- [sprite/décor] `regf.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `regf.alpha = 0.4` *(postCreate)*
- [personnage] `regf.cameraOffset = gf.cameraOffset` *(postCreate)*
- [caméra] `regf.cameraOffset.y -= 300` *(postCreate)*
- [sprite/décor] `rebf.x = 575` *(postCreate)*
- [sprite/décor] `rebf.y = 811` *(postCreate)*
- [personnage] `var index = members.indexOf(gf)` *(postCreate)*
- [sprite/décor] `insert(index, regf)` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godrays")))` *(postCreate)*
- [personnage] `dad.cameras = gf.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = regf.cameras = [camRays]` *(postCreate)*
- [personnage] `rubyMid = gf.getGraphicMidpoint()` *(postCreate)*
- [autre] `s.cameras = [camRays]` *(postCreate)*
- [sprite/décor] `s.scrollFactor.set(0.8, 0.8)` *(postCreate)*
- [sprite/décor] `s.alpha = 0.4` *(postCreate)*
- [HUD] `strumLines.members[2].characters = [regf,gf]` *(postCreate)*

**Pendant la chanson**
- `onPostCountdown` : HUD 1
- `update` : sprite/décor 3, shader 1
- `postUpdate` : sprite/décor 44, caméra 9, shader 1
- `onPostNoteCreation` : sprite/décor 1

### `songs/you and me/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH3/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch3/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

## Fun Till End (`fun till end`)

Décor `garage`, persos cm-ruby, cm-tae, 39 événements HScript Call

### `data/stages/garage.hx`
**Appliqué** (8)
- [personnage] `boyfriend.y = 130` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y += 100` *(postCreate)*
- [personnage] `dad.y += 40` *(postCreate)*
- [personnage] `dad.y -= 40` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `rayTracker = new FlxSprite(1430,145)` *(postCreate)*
- [sprite/décor] `rayTracker.scale.set(0.75,0.75)` *(postCreate)*
- [autre] `rayTracker.cameras = [camNoRays]` *(postCreate)*

**Ignoré** (46)
- [personnage] `camGame.scroll.set(dad.getMidpoint().x + 200,dad.getMidpoint().y)`
- [autre] `light.blend = 0` *(postCreate)*
- [personnage] `camGame.scroll.set(dad.getMidpoint().x + 200,dad.getMidpoint().y)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, dad.curCharacter, false)` *(postCreate)*
- [autre] `redad.flipY = true` *(postCreate)*
- [sprite/décor] `redad.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `redad.alpha = 0.4` *(postCreate)*
- [personnage] `redad.cameraOffset = dad.cameraOffset` *(postCreate)*
- [caméra] `redad.cameraOffset.x += 130` *(postCreate)*
- [caméra] `redad.cameraOffset.y -= 300` *(postCreate)*
- [personnage] `rebf = new Character(dad.x, dad.y, 'cm-tae', true)` *(postCreate)*
- [autre] `rebf.flipY = true` *(postCreate)*
- [sprite/décor] `rebf.x = 590` *(postCreate)*
- [sprite/décor] `rebf.y = 756` *(postCreate)*
- [sprite/décor] `rebf.skew.x = -210` *(postCreate)*
- [sprite/décor] `rebf.skew.y = -175` *(postCreate)*
- [sprite/décor] `rebf.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `rebf.alpha = 0.4` *(postCreate)*
- [personnage] `rebf.cameraOffset = boyfriend.cameraOffset` *(postCreate)*
- [caméra] `rebf.cameraOffset.y -= 300` *(postCreate)*
- [sprite/décor] `insert(index, rebf)` *(postCreate)*
- [sprite/décor] `insert(index, redad)` *(postCreate)*
- [autre] `rayTracker.active = false` *(postCreate)*
- [autre] `camRays = new FlxCamera()` *(postCreate)*
- [autre] `camRays.bgColor = 0` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godrays-morning")))` *(postCreate)*
- [shader] `camRays.addShader(rayShader)` *(postCreate)*
- [autre] `camNoRays = new FlxCamera()` *(postCreate)*
- [autre] `camNoRays.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.remove(camGame, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camGame, true)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camRays, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camNoRays, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [personnage] `dad.cameras = [camRays]` *(postCreate)*
- [personnage] `boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = [camRays]` *(postCreate)*
- [autre] `redad.cameras = [camRays]` *(postCreate)*
- [HUD] `cpuStrums.characters = [redad,dad]` *(postCreate)*
- [HUD] `playerStrums.characters = [rebf,boyfriend]` *(postCreate)*

**Non suivi** (80)
- [autre] `i++` *(postCreate)*
- [shader] `c.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `c.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `c.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `c.shader.flipped = c.flipX` *(postCreate)*
- [sprite/décor] `redad.x = -114` *(postCreate)*
- [sprite/décor] `redad.y = 770` *(postCreate)*
- [sprite/décor] `redad.skew.x = -211` *(postCreate)*
- [sprite/décor] `redad.skew.y = 0` *(postCreate)*
- [sprite/décor] `redad.x = -99` *(postCreate)*
- [sprite/décor] `redad.y = 730` *(postCreate)*
- [sprite/décor] `redad.skew.x = -211` *(postCreate)*
- [sprite/décor] `redad.skew.y = -20` *(postCreate)*
- [personnage] `dad.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = [camRays]` *(postCreate)*
- [sprite/décor] `redad.visible = rebf.visible = false` *(postCreate)*
- [personnage] `dad.cameraOffset.y += 200` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 99` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].cameraOffset.x += 300` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].cameraOffset.y += 200` *(postCreate)*
- [HUD] `strumLines.members[0].characters[0].setPosition(140 ,125 )` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].setPosition(-205 ,225 )` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].cameraOffset.x += 300` *(postCreate)*
- [HUD] `strumLines.members[6].characters[0].cameraOffset.x = 550` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].setPosition(-105 ,90 )` *(postCreate)*
- [HUD] `strumLines.members[5].characters[0].setPosition(1020 ,95 )` *(postCreate)*
- [HUD] `strumLines.members[6].characters[0].setPosition(205,145)` *(postCreate)*
- [autre] `var index = members.indexOf(sofa)` *(postCreate)*
- [sprite/décor] `remove(bg)` *(postCreate)*
- [sprite/décor] `bg = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/evening'))` *(postCreate)*
- [sprite/décor] `bg.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `bg.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,bg)` *(postCreate)*
- [sprite/décor] `fade = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/fade'))` *(postCreate)*
- [sprite/décor] `fade.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `fade.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,fade)` *(postCreate)*
- [personnage] `var index = members.indexOf(gf)` *(postCreate)*
- [sprite/décor] `remove(sofa)` *(postCreate)*
- [sprite/décor] `sofa = new FlxSprite(-1050,-525).loadGraphic(Paths.image('stages/garage/sofa-evening'))` *(postCreate)*
- [sprite/décor] `sofa.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `sofa.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,sofa)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `remove(light)` *(postCreate)*
- [sprite/décor] `light = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/light-evening'))` *(postCreate)*
- [sprite/décor] `light.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,light)` *(postCreate)*
- [personnage] `dad.x -= 90` *(postCreate)*
- [personnage] `dad.y += 60` *(postCreate)*
- [personnage] `gf.y -= 40` *(postCreate)*
- [personnage] `gf.x -= 120` *(postCreate)*
- [personnage] `boyfriend.y += 50` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, 'cm-cj', false)` *(postCreate)*
- [sprite/décor] `redad.y = 775` *(postCreate)*
- [sprite/décor] `redad.x = -204` *(postCreate)*
- [sprite/décor] `redad.skew.x = -211` *(postCreate)*
- [sprite/décor] `redad.skew.y = -20` *(postCreate)*
- [personnage] `regf = new Character(gf.x, gf.y, 'cm-ruby', false)` *(postCreate)*
- [autre] `regf.flipY = true` *(postCreate)*
- [sprite/décor] `regf.x = -144` *(postCreate)*
- [sprite/décor] `regf.y = 774` *(postCreate)*
- [sprite/décor] `regf.skew.x = -46` *(postCreate)*
- [sprite/décor] `regf.skew.y = 0` *(postCreate)*
- [sprite/décor] `regf.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `regf.alpha = 0.4` *(postCreate)*
- [personnage] `regf.cameraOffset = gf.cameraOffset` *(postCreate)*
- [caméra] `regf.cameraOffset.y -= 300` *(postCreate)*
- [sprite/décor] `rebf.x = 575` *(postCreate)*
- [sprite/décor] `rebf.y = 811` *(postCreate)*
- [personnage] `var index = members.indexOf(gf)` *(postCreate)*
- [sprite/décor] `insert(index, regf)` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godrays")))` *(postCreate)*
- [personnage] `dad.cameras = gf.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = regf.cameras = [camRays]` *(postCreate)*
- [personnage] `rubyMid = gf.getGraphicMidpoint()` *(postCreate)*
- [autre] `s.cameras = [camRays]` *(postCreate)*
- [sprite/décor] `s.scrollFactor.set(0.8, 0.8)` *(postCreate)*
- [sprite/décor] `s.alpha = 0.4` *(postCreate)*
- [HUD] `strumLines.members[2].characters = [regf,gf]` *(postCreate)*

**Pendant la chanson**
- `onPostCountdown` : HUD 1
- `update` : sprite/décor 3, shader 1
- `postUpdate` : sprite/décor 44, caméra 9, shader 1
- `onPostNoteCreation` : sprite/décor 1

### `songs/fun till end/scripts/events.hx`
**Appliqué** (3)
- [personnage] `boyfriend.alpha = 0` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 100` *(postCreate)*

**Ignoré** (43)
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 1` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*
- [caméra] `camGame.zoom = 2` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [personnage] `eventbackground2 = new FlxSprite(dad.x,dad.y).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [autre] `eventbackground2.cameras = [camRays]` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [autre] `txtFX = new FunkinText(0, FlxG.height * 0.875, 0, "HEY!", 48)` *(postCreate)*
- [autre] `txtFX.alignment = 'CENTER'` *(postCreate)*
- [autre] `txtFX.screenCenter(FlxAxes.Y)` *(postCreate)*
- [sprite/décor] `txtFX.color = 0xFFde2a8a` *(postCreate)*
- [autre] `txtFX.cameras = [camfx]` *(postCreate)*
- [sprite/décor] `txtFX.x -= FlxG.width` *(postCreate)*
- [sprite/décor] `add(txtFX)` *(postCreate)*
- [sprite/décor] `overlayfx = new FlxSprite(0,0).loadGraphic(Paths.image('fx'))` *(postCreate)*
- [sprite/décor] `overlayfx.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `overlayfx.alpha = 0` *(postCreate)*
- [autre] `overlayfx.cameras = [camfx]` *(postCreate)*
- [autre] `overlayfx.setGraphicSize(FlxG.width * 1.2, FlxG.height * 1.2)` *(postCreate)*
- [autre] `overlayfx.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(overlayfx)` *(postCreate)*
- [sprite/décor] `bleh = new FlxSprite(0,0).loadGraphic(Paths.image('bleh'))` *(postCreate)*
- [sprite/décor] `bleh.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `bleh.alpha = 0` *(postCreate)*
- [autre] `bleh.cameras = [camfx]` *(postCreate)*
- [autre] `bleh.setGraphicSize(FlxG.width, FlxG.height)` *(postCreate)*
- [autre] `bleh.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(bleh)` *(postCreate)*

**Pendant la chanson**
- `onSongStart` : tween/timer 2, caméra 1
- `hey` : sprite/décor 4, tween/timer 2
- `onCountdown` : sprite/décor 2
- `bump` (appelée 38 fois par le chart) : caméra 1, HUD 1
- `blehh` (appelée 1 fois par le chart) : sprite/décor 1, tween/timer 1
- `flash` : sprite/décor 9, tween/timer 1
- `beatHit` : caméra 5, HUD 5
- `stepHit` : caméra 16, personnage 7, tween/timer 5, HUD 4

### `songs/fun till end/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH3/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch3/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

## Light It Up (`light it up`)

Décor `garage`, persos cm-cj, cm-tae, cm-ruby, 127 événements HScript Call

### `data/stages/garage.hx`
**Appliqué** (31)
- [personnage] `boyfriend.y = 130` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y += 100` *(postCreate)*
- [personnage] `dad.y += 40` *(postCreate)*
- [autre] `var index = members.indexOf(sofa)` *(postCreate)*
- [sprite/décor] `bg = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/evening'))` *(postCreate)*
- [sprite/décor] `bg.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `bg.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,bg)` *(postCreate)*
- [sprite/décor] `fade = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/fade'))` *(postCreate)*
- [sprite/décor] `fade.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `fade.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,fade)` *(postCreate)*
- [personnage] `var index = members.indexOf(gf)` *(postCreate)*
- [sprite/décor] `sofa = new FlxSprite(-1050,-525).loadGraphic(Paths.image('stages/garage/sofa-evening'))` *(postCreate)*
- [sprite/décor] `sofa.scrollFactor.set(0.95,0.95)` *(postCreate)*
- [sprite/décor] `sofa.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,sofa)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `light = new FlxSprite(-1050,-425).loadGraphic(Paths.image('stages/garage/light-evening'))` *(postCreate)*
- [sprite/décor] `light.scale.set(0.75,0.75)` *(postCreate)*
- [sprite/décor] `insert(index,light)` *(postCreate)*
- [personnage] `dad.x -= 90` *(postCreate)*
- [personnage] `dad.y += 60` *(postCreate)*
- [personnage] `gf.y -= 40` *(postCreate)*
- [personnage] `gf.x -= 120` *(postCreate)*
- [personnage] `boyfriend.y += 50` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [personnage] `var index = members.indexOf(gf)` *(postCreate)*
- [sprite/décor] `rayTracker = new FlxSprite(1430,145)` *(postCreate)*
- [sprite/décor] `rayTracker.scale.set(0.75,0.75)` *(postCreate)*
- [autre] `rayTracker.cameras = [camNoRays]` *(postCreate)*

**Ignoré** (68)
- [personnage] `camGame.scroll.set(dad.getMidpoint().x + 200,dad.getMidpoint().y)`
- [autre] `light.blend = 0` *(postCreate)*
- [personnage] `camGame.scroll.set(dad.getMidpoint().x + 200,dad.getMidpoint().y)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [sprite/décor] `remove(bg)` *(postCreate)*
- [sprite/décor] `remove(sofa)` *(postCreate)*
- [sprite/décor] `remove(light)` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, 'cm-cj', false)` *(postCreate)*
- [autre] `redad.flipY = true` *(postCreate)*
- [sprite/décor] `redad.y = 775` *(postCreate)*
- [sprite/décor] `redad.x = -204` *(postCreate)*
- [sprite/décor] `redad.skew.x = -211` *(postCreate)*
- [sprite/décor] `redad.skew.y = -20` *(postCreate)*
- [sprite/décor] `redad.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `redad.alpha = 0.4` *(postCreate)*
- [personnage] `redad.cameraOffset = dad.cameraOffset` *(postCreate)*
- [caméra] `redad.cameraOffset.x += 130` *(postCreate)*
- [caméra] `redad.cameraOffset.y -= 300` *(postCreate)*
- [personnage] `regf = new Character(gf.x, gf.y, 'cm-ruby', false)` *(postCreate)*
- [autre] `regf.flipY = true` *(postCreate)*
- [sprite/décor] `regf.x = -144` *(postCreate)*
- [sprite/décor] `regf.y = 774` *(postCreate)*
- [sprite/décor] `regf.skew.x = -46` *(postCreate)*
- [sprite/décor] `regf.skew.y = 0` *(postCreate)*
- [sprite/décor] `regf.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `regf.alpha = 0.4` *(postCreate)*
- [personnage] `regf.cameraOffset = gf.cameraOffset` *(postCreate)*
- [caméra] `regf.cameraOffset.y -= 300` *(postCreate)*
- [personnage] `rebf = new Character(dad.x, dad.y, 'cm-tae', true)` *(postCreate)*
- [autre] `rebf.flipY = true` *(postCreate)*
- [sprite/décor] `rebf.x = 575` *(postCreate)*
- [sprite/décor] `rebf.y = 811` *(postCreate)*
- [sprite/décor] `rebf.skew.x = -210` *(postCreate)*
- [sprite/décor] `rebf.skew.y = -175` *(postCreate)*
- [sprite/décor] `rebf.color = FlxColor.BLACK` *(postCreate)*
- [sprite/décor] `rebf.alpha = 0.4` *(postCreate)*
- [personnage] `rebf.cameraOffset = boyfriend.cameraOffset` *(postCreate)*
- [caméra] `rebf.cameraOffset.y -= 300` *(postCreate)*
- [sprite/décor] `insert(index, rebf)` *(postCreate)*
- [sprite/décor] `insert(index, redad)` *(postCreate)*
- [sprite/décor] `insert(index, regf)` *(postCreate)*
- [autre] `rayTracker.active = false` *(postCreate)*
- [autre] `camRays = new FlxCamera()` *(postCreate)*
- [autre] `camRays.bgColor = 0` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godrays")))` *(postCreate)*
- [shader] `camRays.addShader(rayShader)` *(postCreate)*
- [autre] `camNoRays = new FlxCamera()` *(postCreate)*
- [autre] `camNoRays.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.remove(camGame, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camGame, true)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camRays, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camNoRays, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [personnage] `dad.cameras = [camRays]` *(postCreate)*
- [personnage] `gf.cameras = [camRays]` *(postCreate)*
- [personnage] `boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = [camRays]` *(postCreate)*
- [autre] `redad.cameras = [camRays]` *(postCreate)*
- [autre] `regf.cameras = [camRays]` *(postCreate)*
- [personnage] `rubyMid = gf.getGraphicMidpoint()` *(postCreate)*
- [HUD] `strumLines.members[2].characters = [regf,gf]` *(postCreate)*
- [HUD] `cpuStrums.characters = [redad,dad]` *(postCreate)*
- [HUD] `playerStrums.characters = [rebf,boyfriend]` *(postCreate)*

**Non suivi** (34)
- [autre] `i++` *(postCreate)*
- [shader] `c.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `c.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `c.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `c.shader.flipped = c.flipX` *(postCreate)*
- [personnage] `dad.y -= 40` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, dad.curCharacter, false)` *(postCreate)*
- [sprite/décor] `redad.x = -114` *(postCreate)*
- [sprite/décor] `redad.y = 770` *(postCreate)*
- [sprite/décor] `redad.skew.y = 0` *(postCreate)*
- [sprite/décor] `redad.x = -99` *(postCreate)*
- [sprite/décor] `redad.y = 730` *(postCreate)*
- [sprite/décor] `rebf.x = 590` *(postCreate)*
- [sprite/décor] `rebf.y = 756` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godrays-morning")))` *(postCreate)*
- [personnage] `dad.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = [camRays]` *(postCreate)*
- [sprite/décor] `redad.visible = rebf.visible = false` *(postCreate)*
- [personnage] `dad.cameraOffset.y += 200` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 99` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].cameraOffset.x += 300` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].cameraOffset.y += 200` *(postCreate)*
- [HUD] `strumLines.members[0].characters[0].setPosition(140 ,125 )` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].setPosition(-205 ,225 )` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].cameraOffset.x += 300` *(postCreate)*
- [HUD] `strumLines.members[6].characters[0].cameraOffset.x = 550` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].setPosition(-105 ,90 )` *(postCreate)*
- [HUD] `strumLines.members[5].characters[0].setPosition(1020 ,95 )` *(postCreate)*
- [HUD] `strumLines.members[6].characters[0].setPosition(205,145)` *(postCreate)*
- [personnage] `dad.cameras = gf.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = regf.cameras = [camRays]` *(postCreate)*
- [autre] `s.cameras = [camRays]` *(postCreate)*
- [sprite/décor] `s.scrollFactor.set(0.8, 0.8)` *(postCreate)*
- [sprite/décor] `s.alpha = 0.4` *(postCreate)*

**Pendant la chanson**
- `onPostCountdown` : HUD 1
- `update` : sprite/décor 3, shader 1
- `postUpdate` : sprite/décor 44, caméra 9, shader 1
- `onPostNoteCreation` : sprite/décor 1

### `songs/light it up/scripts/events.hx`
**Ignoré** (31)
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 1` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*
- [caméra] `camGame.zoom = 2` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [personnage] `eventbackground2 = new FlxSprite(dad.x,dad.y).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.Y)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [autre] `eventbackground2.cameras = [camNoRays]` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [autre] `comGrp = new FlxTypedGroup()` *(postCreate)*
- [sprite/décor] `add(comGrp)` *(postCreate)*
- [sprite/décor] `overlayfx = new FlxSprite(0,0).loadGraphic(Paths.image('fx'))` *(postCreate)*
- [sprite/décor] `overlayfx.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `overlayfx.alpha = 0` *(postCreate)*
- [autre] `overlayfx.cameras = [camfx]` *(postCreate)*
- [autre] `overlayfx.setGraphicSize(FlxG.width * 1.2, FlxG.height * 1.2)` *(postCreate)*
- [autre] `overlayfx.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(overlayfx)` *(postCreate)*

**Non suivi** (8)
- [autre] `txtFX = new FunkinText(0, FlxG.random.int(0,700), 0, 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA` *(postCreate)*
- [autre] `txtFX.alignment = 'CENTER'` *(postCreate)*
- [autre] `txtFX.cameras = [camNoRays]` *(postCreate)*
- [sprite/décor] `txtFX.x += FlxG.width + 700` *(postCreate)*
- [sprite/décor] `comGrp.add(txtFX)` *(postCreate)*
- [autre] `txtFX.kill()` *(postCreate)*
- [autre] `txtFX.destroy()` *(postCreate)*
- [sprite/décor] `comGrp.remove(txtFX)` *(postCreate)*

**Pendant la chanson**
- `onSongStart` : tween/timer 2, caméra 1
- `onCountdown` : sprite/décor 2
- `spawmCom` : sprite/décor 2
- `postUpdate` : sprite/décor 1
- `bump` (appelée 56 fois par le chart) : caméra 1, HUD 1
- `libump` (appelée 70 fois par le chart) : caméra 1, HUD 1
- `flash` : sprite/décor 9, tween/timer 1
- `camZooms` : caméra 1
- `stepHit` : caméra 29, HUD 11, tween/timer 3
- `beatHit` : caméra 4, HUD 4

### `songs/light it up/cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH3/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch3/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (16)
- [autre] `usesub = true`
- [autre] `dublang = '-th'`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2, vidéo/cinématique 1
- `update` : sprite/décor 2, vidéo/cinématique 1
- `setSubtitle` : sprite/décor 2

### `songs/light it up/end-cutscene.hx`
**Ignoré** (14)
- [autre] `jsonString = Paths.file('videos/CH3/' + videoname + sublang + '.json')`
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `video.bitmap.onEndReached.add(videokill)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `playvideo(videoname)`
- [audio] `cutva = FlxG.sound.load(Paths.sound('cutscene/ch3/' + videoname + dublang), 1)`
- [sprite/décor] `bgsub = new FlxSprite(0, FlxG.height * 0.85).makeGraphic(1, 1, 0xFF000000)`
- [sprite/décor] `bgsub.alpha = 0.5`
- [sprite/décor] `bgsub.visible = false`

**Non suivi** (15)
- [autre] `usesub = true`
- [autre] `sublang = '-th'`
- [autre] `subtitle = new FunkinText(0, FlxG.height * 0.875, 0, "", 32)`
- [autre] `subtitle.alignment = 'CENTER'`
- [autre] `subtitle.font = Paths.font('thai-font.ttf')`
- [autre] `subtitle.screenCenter(FlxAxes.X)`
- [sprite/décor] `subtitle.visible = false`
- [sprite/décor] `bgsub.scale.set(subtitle.width + 8, subtitle.height + 8)`
- [autre] `bgsub.updateHitbox()`
- [sprite/décor] `bgsub.setPosition(subtitle.x - 4, subtitle.y - 4)`
- [autre] `subtitle.cameras = [videoCamera]`
- [autre] `bgsub.cameras = [videoCamera]`
- [sprite/décor] `add(bgsub)`
- [sprite/décor] `add(subtitle)`
- [autre] `parseSubtitle(jsonString)`

**Pendant la chanson**
- `playvideo` : tween/timer 1
- `videokill` : sprite/décor 2
- `update` : sprite/décor 2
- `setSubtitle` : sprite/décor 2

## Brand New Life (`brand new life`)

Décor `roomex`, persos taeyaiminus, bf

### `data/stages/roomex.hx`
**Appliqué** (6)
- [autre] `var i = members.indexOf(roombg)`
- [sprite/décor] `var spr = new FunkinSprite(roombg.x, roombg.y)`
- [sprite/décor] `spr.animation.play("idle")`
- [autre] `spr.updateHitbox()`
- [autre] `spr.screenCenter(FlxAxes.XY)`
- [sprite/décor] `insert(i, spr)`

**Ignoré** (11)
- [autre] `var positions = [ [false, 855, 409, 386, 233], ]`
- [autre] `var easterEgg = FlxG.random.bool(1/4.2)`
- [autre] `var baseImages = [for(f in Paths.getFolderContent('images/stages/room/frames')) if (Path.extension(f).toLowerCase() == "png") Path.withoutEx`
- [autre] `var images = []`
- [autre] `var curImg = images[FlxG.random.int(0, images.length-1)]`
- [sprite/décor] `images.remove(curImg)`
- [autre] `spr.loadSprite(easterEgg ? Paths.image('stages/room/putin') : Paths.image('stages/room/frames/' + curImg))`
- [sprite/décor] `spr.animation.add("idle", [for(i in 0...spr.frames.frames.length) i], 24, true)`
- [sprite/décor] `spr.scrollFactor = roombg.scrollFactor`
- [autre] `spr.antialiasing = true`
- [autre] `spr.setGraphicSize(FlxG.width *2,FlxG.height * 2)`

**Non suivi** (1)
- [autre] `images = [for(i in baseImages) i]`

### `songs/brand new life/scripts/events.hx`
**Appliqué** (4)
- [personnage] `dad.cameraOffset.x -= 50` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 50` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.x -= 100` *(postCreate)*
- [personnage] `boyfriend.y -= 50` *(postCreate)*

**Ignoré** (39)
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [sprite/décor] `eventwhite = new FlxSprite(0,0).makeGraphic(1280,720, 0xFFFFFFFF)` *(postCreate)*
- [sprite/décor] `eventwhite.alpha = 0` *(postCreate)*
- [autre] `eventwhite.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventwhite.scale.set(10,10)` *(postCreate)*
- [autre] `eventwhite.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventwhite.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(eventwhite)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [HUD] `camHUD.alpha = 0` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `vignette=new FlxSprite().loadGraphic(Paths.image('stages/room/vignette'))` *(postCreate)*
- [autre] `vignette.screenCenter()` *(postCreate)*
- [autre] `vignette.cameras=[camfx]` *(postCreate)*
- [sprite/décor] `vignette.alpha=0.7` *(postCreate)*
- [sprite/décor] `vignette.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(vignette)` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*
- [caméra] `camGame.zoom = 1.3` *(postCreate)*

**Non suivi** (1)
- [HUD] `camNewHUD.alpha = camHUD.alpha = 0` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 2
- `bump` : caméra 1, HUD 1
- `onSongStart` : tween/timer 1
- `beatHit` : caméra 5, HUD 5
- `stepHit` : caméra 19, tween/timer 5, HUD 3, sprite/décor 1

### `songs/brand new life/cutscene.hx`
**Non suivi** (18)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37/2, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!) Basically! BF try to get his way home! He almost give up but Tiny Keep cheering him up so BF dec`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [vidéo/cinématique] `PlayState.instance.inCutscene = false`
- [autre] `PlayState.instance.startCountdown()`

**Pendant la chanson**
- `update` : sprite/décor 3, vidéo/cinématique 1

## Soft Machine (`soft machine`)

Décor `soft`, persos soft, bf, 25 événements HScript Call

### `data/stages/soft.hx`
**Appliqué** (19)
- [personnage] `var index = members.indexOf(boyfriend)`
- [sprite/décor] `back = new FunkinSprite(-280, -310).loadGraphic(Paths.image('stages/soft/back'))`
- [sprite/décor] `back.scale.set(0.7,0.7)`
- [autre] `back.screenCenter(FlxAxes.XY)`
- [sprite/décor] `back.scrollFactor.set(0.8,1)`
- [sprite/décor] `back.y += 600`
- [sprite/décor] `front = new FunkinSprite(-280, -310).loadGraphic(Paths.image('stages/soft/front'))`
- [sprite/décor] `front.scale.set(0.7,0.7)`
- [autre] `front.screenCenter(FlxAxes.XY)`
- [sprite/décor] `front.y += 600`
- [sprite/décor] `light = new FunkinSprite(-1262 , -354.5 ).loadGraphic(Paths.image('stages/soft/light'))`
- [sprite/décor] `light.scale.set(0.7,0.7)`
- [autre] `light.screenCenter(FlxAxes.XY)`
- [sprite/décor] `light.y += 600`
- [sprite/décor] `add(light)`
- [sprite/décor] `insert(index, front)`
- [sprite/décor] `insert(index, back)`
- [personnage] `dad.setPosition(-65,1050)`
- [personnage] `boyfriend.setPosition(980,1025)`

**Ignoré** (25)
- [personnage] `camGame.scroll.set(dad.getMidpoint().x,dad.getMidpoint().y)`
- [personnage] `camGame.scroll.set(dad.getMidpoint().x,dad.getMidpoint().y - 300)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [shader] `colorShader = new FunkinShader(Assets.getText(Paths.fragShader("colorgrade")))` *(postCreate)*
- [shader] `colorShader.topr = 3.1` *(postCreate)*
- [shader] `colorShader.topg = 1.1` *(postCreate)*
- [shader] `colorShader.topb = 1.1` *(postCreate)*
- [shader] `colorShader.botr = 3.1` *(postCreate)*
- [shader] `colorShader.botg = 1.1` *(postCreate)*
- [shader] `colorShader.botb = 1.1` *(postCreate)*
- [shader] `camGame.addShader(colorShader)` *(postCreate)*
- [shader] `chromaticGame = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromaticGame.chromaticAmount = 0.002` *(postCreate)*
- [shader] `camGame.addShader(chromaticGame)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : personnage 13, shader 1
- `stepHit` : sprite/décor 2

### `songs/soft machine/scripts/events.hx`
**Pendant la chanson**
- `bump` (appelée 25 fois par le chart) : caméra 1, HUD 1
- `stepHit` : HUD 5, caméra 3
- `beatHit` : caméra 5, HUD 5

## New Day (`new day`)

Décor `soft`, persos soft, bf

### `data/stages/soft.hx`
**Appliqué** (19)
- [personnage] `var index = members.indexOf(boyfriend)`
- [sprite/décor] `back = new FunkinSprite(-280, -310).loadGraphic(Paths.image('stages/soft/back'))`
- [sprite/décor] `back.scale.set(0.7,0.7)`
- [autre] `back.screenCenter(FlxAxes.XY)`
- [sprite/décor] `back.scrollFactor.set(0.8,1)`
- [sprite/décor] `back.y += 600`
- [sprite/décor] `front = new FunkinSprite(-280, -310).loadGraphic(Paths.image('stages/soft/front'))`
- [sprite/décor] `front.scale.set(0.7,0.7)`
- [autre] `front.screenCenter(FlxAxes.XY)`
- [sprite/décor] `front.y += 600`
- [sprite/décor] `light = new FunkinSprite(-1262 , -354.5 ).loadGraphic(Paths.image('stages/soft/light'))`
- [sprite/décor] `light.scale.set(0.7,0.7)`
- [autre] `light.screenCenter(FlxAxes.XY)`
- [sprite/décor] `light.y += 600`
- [sprite/décor] `add(light)`
- [sprite/décor] `insert(index, front)`
- [sprite/décor] `insert(index, back)`
- [personnage] `dad.setPosition(-65,1050)`
- [personnage] `boyfriend.setPosition(980,1025)`

**Ignoré** (25)
- [personnage] `camGame.scroll.set(dad.getMidpoint().x,dad.getMidpoint().y)`
- [personnage] `camGame.scroll.set(dad.getMidpoint().x,dad.getMidpoint().y - 300)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [shader] `colorShader = new FunkinShader(Assets.getText(Paths.fragShader("colorgrade")))` *(postCreate)*
- [shader] `colorShader.topr = 3.1` *(postCreate)*
- [shader] `colorShader.topg = 1.1` *(postCreate)*
- [shader] `colorShader.topb = 1.1` *(postCreate)*
- [shader] `colorShader.botr = 3.1` *(postCreate)*
- [shader] `colorShader.botg = 1.1` *(postCreate)*
- [shader] `colorShader.botb = 1.1` *(postCreate)*
- [shader] `camGame.addShader(colorShader)` *(postCreate)*
- [shader] `chromaticGame = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromaticGame.chromaticAmount = 0.002` *(postCreate)*
- [shader] `camGame.addShader(chromaticGame)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : personnage 13, shader 1
- `stepHit` : sprite/décor 2

### `songs/new day/scripts/events.hx`
**Ignoré** (9)
- [caméra] `camZooming = false` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*

**Pendant la chanson**
- `stepHit` : caméra 20, HUD 3, tween/timer 1, sprite/décor 1

## Yawarakai (`Yawarakai`)

Décor `soft`, persos soft, bf, 64 événements HScript Call

### `data/stages/soft.hx`
**Appliqué** (19)
- [personnage] `var index = members.indexOf(boyfriend)`
- [sprite/décor] `back = new FunkinSprite(-280, -310).loadGraphic(Paths.image('stages/soft/back'))`
- [sprite/décor] `back.scale.set(0.7,0.7)`
- [autre] `back.screenCenter(FlxAxes.XY)`
- [sprite/décor] `back.scrollFactor.set(0.8,1)`
- [sprite/décor] `back.y += 600`
- [sprite/décor] `front = new FunkinSprite(-280, -310).loadGraphic(Paths.image('stages/soft/front'))`
- [sprite/décor] `front.scale.set(0.7,0.7)`
- [autre] `front.screenCenter(FlxAxes.XY)`
- [sprite/décor] `front.y += 600`
- [sprite/décor] `light = new FunkinSprite(-1262 , -354.5 ).loadGraphic(Paths.image('stages/soft/light'))`
- [sprite/décor] `light.scale.set(0.7,0.7)`
- [autre] `light.screenCenter(FlxAxes.XY)`
- [sprite/décor] `light.y += 600`
- [sprite/décor] `add(light)`
- [sprite/décor] `insert(index, front)`
- [sprite/décor] `insert(index, back)`
- [personnage] `dad.setPosition(-65,1050)`
- [personnage] `boyfriend.setPosition(980,1025)`

**Ignoré** (25)
- [personnage] `camGame.scroll.set(dad.getMidpoint().x,dad.getMidpoint().y)`
- [personnage] `camGame.scroll.set(dad.getMidpoint().x,dad.getMidpoint().y - 300)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [shader] `colorShader = new FunkinShader(Assets.getText(Paths.fragShader("colorgrade")))` *(postCreate)*
- [shader] `colorShader.topr = 3.1` *(postCreate)*
- [shader] `colorShader.topg = 1.1` *(postCreate)*
- [shader] `colorShader.topb = 1.1` *(postCreate)*
- [shader] `colorShader.botr = 3.1` *(postCreate)*
- [shader] `colorShader.botg = 1.1` *(postCreate)*
- [shader] `colorShader.botb = 1.1` *(postCreate)*
- [shader] `camGame.addShader(colorShader)` *(postCreate)*
- [shader] `chromaticGame = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromaticGame.chromaticAmount = 0.002` *(postCreate)*
- [shader] `camGame.addShader(chromaticGame)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : personnage 13, shader 1
- `stepHit` : sprite/décor 2

### `songs/Yawarakai/scripts/events.hx`
**Appliqué** (2)
- [personnage] `dad.alpha = 0` *(postCreate)*
- [personnage] `boyfriend.alpha = 0` *(postCreate)*

**Ignoré** (57)
- [caméra] `camZooming = false` *(postCreate)*
- [autre] `camLock = true` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 1` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `eventwhite = new FlxSprite(0,0).makeGraphic(1280,720, 0xFFFFFFFF)` *(postCreate)*
- [sprite/décor] `eventwhite.alpha = 0` *(postCreate)*
- [autre] `eventwhite.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventwhite.scale.set(4,4)` *(postCreate)*
- [autre] `eventwhite.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventwhite.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventwhite)` *(postCreate)*
- [personnage] `eventbackground2 = new FlxSprite(dad.x,dad.y).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [personnage] `eventbackground3 = new FlxSprite(dad.x,dad.y).makeGraphic(1280,720, 0xFFFFFFFF)` *(postCreate)*
- [sprite/décor] `eventbackground3.alpha = 0` *(postCreate)*
- [autre] `eventbackground3.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground3.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground3.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground3.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground3)` *(postCreate)*
- [autre] `cameffect = new FlxCamera()` *(postCreate)*
- [autre] `cameffect.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(cameffect, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [shader] `spin = new CustomShader("coolbg")` *(postCreate)*
- [sprite/décor] `image = new FlxSprite(0, 0).makeGraphic(1280,720, 0xFFFFFFFF)` *(postCreate)*
- [autre] `image.antialiasing = true` *(postCreate)*
- [shader] `image.shader = spin` *(postCreate)*
- [autre] `image.setGraphicSize(FlxG.width, FlxG.height)` *(postCreate)*
- [autre] `image.updateHitbox()` *(postCreate)*
- [autre] `image.screenCenter(FlxAxes.XY)` *(postCreate)*
- [autre] `image.cameras = [cameffect]` *(postCreate)*
- [sprite/décor] `image.alpha = 0` *(postCreate)*
- [sprite/décor] `add(image)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [personnage] `txtFX = new FunkinText(dad.x + 20, dad.y + 200, 0, 'エミコ', 46)` *(postCreate)*
- [autre] `txtFX.font = Paths.font("JP.ttf")` *(postCreate)*
- [autre] `txtFX.alignment = 'CENTER'` *(postCreate)*
- [sprite/décor] `txtFX.alpha = 0` *(postCreate)*
- [sprite/décor] `insert(index, txtFX)` *(postCreate)*
- [caméra] `FlxG.camera.zoom = 1` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 0` *(postCreate)*

**Non suivi** (1)
- [personnage] `dad.alpha = boyfriend.alpha = 0` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 2
- `onCameraMove` : personnage 6, caméra 3
- `bump` (appelée 64 fois par le chart) : caméra 1, HUD 1
- `stepHit` : caméra 53, personnage 22, tween/timer 14, sprite/décor 10, HUD 8

## Underground (`Underground`)

Décor `tans`, persos tans, bf, 244 événements HScript Call

### `data/stages/tans.hx`
**Appliqué** (11)
- [personnage] `dad.flipX = true` *(postCreate)*
- [personnage] `boyfriend.flipX = true` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.x += 470` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 300` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 700` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 700` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `rayTracker = new FlxSprite(600,170)` *(postCreate)*
- [sprite/décor] `rayTracker.scale.set(0.75,0.75)` *(postCreate)*
- [autre] `rayTracker.cameras = [camNoRays]` *(postCreate)*

**Ignoré** (48)
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(253,310)` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, 'tans', false)` *(postCreate)*
- [autre] `redad.flipX = true` *(postCreate)*
- [autre] `redad.flipY = true` *(postCreate)*
- [sprite/décor] `redad.x = 1961` *(postCreate)*
- [sprite/décor] `redad.y = 1475` *(postCreate)*
- [sprite/décor] `redad.skew.x = 9` *(postCreate)*
- [sprite/décor] `redad.color = FlxColor.YELLOW` *(postCreate)*
- [sprite/décor] `redad.alpha = 0.1` *(postCreate)*
- [sprite/décor] `insert(index, redad)` *(postCreate)*
- [personnage] `rebf = new Character(boyfriend.x, boyfriend.y, 'bf', true)` *(postCreate)*
- [autre] `rebf.flipX = true` *(postCreate)*
- [autre] `rebf.flipY = true` *(postCreate)*
- [sprite/décor] `rebf.x = 291` *(postCreate)*
- [sprite/décor] `rebf.y = 1115` *(postCreate)*
- [sprite/décor] `rebf.skew.x = -19` *(postCreate)*
- [sprite/décor] `rebf.skew.y = 5` *(postCreate)*
- [autre] `rebf.blend = 0` *(postCreate)*
- [sprite/décor] `rebf.color = FlxColor.YELLOW` *(postCreate)*
- [sprite/décor] `rebf.alpha = 0.1` *(postCreate)*
- [sprite/décor] `insert(index, rebf)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `rayTracker.active = false` *(postCreate)*
- [autre] `camRays = new FlxCamera()` *(postCreate)*
- [autre] `camRays.bgColor = 0` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godrays-morning")))` *(postCreate)*
- [shader] `camRays.addShader(rayShader)` *(postCreate)*
- [autre] `camNoRays = new FlxCamera()` *(postCreate)*
- [autre] `camNoRays.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.remove(camGame, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camGame, true)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camRays, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camNoRays, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [personnage] `dad.cameras = [camRays]` *(postCreate)*
- [personnage] `boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = [camRays]` *(postCreate)*
- [autre] `redad.cameras = [camRays]` *(postCreate)*
- [HUD] `cpuStrums.characters = [redad,dad]` *(postCreate)*
- [HUD] `playerStrums.characters = [rebf,boyfriend]` *(postCreate)*
- [HUD] `playerStrums.forEach(function(spr:FlxSprite) { spr.x -= 645; })` *(postCreate)*
- [HUD] `cpuStrums.forEach(function(spr:FlxSprite) { spr.x += 645; })` *(postCreate)*

**Non suivi** (13)
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `c.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `c.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `c.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `c.shader.flipped = c.flipX` *(postCreate)*
- [personnage] `dad.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = [camRays]` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camRays.addShader(idkshade)` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : caméra 1
- `postUpdate` : caméra 6, sprite/décor 4, shader 1

### `songs/Underground/scripts/events.hx`
**Ignoré** (30)
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.cameras = [camRays]` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `vignette=new FlxSprite().loadGraphic(Paths.image('stages/room/vignette'))` *(postCreate)*
- [autre] `vignette.screenCenter()` *(postCreate)*
- [autre] `vignette.cameras=[camfx]` *(postCreate)*
- [sprite/décor] `vignette.alpha=0` *(postCreate)*
- [sprite/décor] `vignette.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(vignette)` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 2
- `bump` (appelée 244 fois par le chart) : caméra 1, HUD 1
- `onSongStart` : tween/timer 1
- `serious` : caméra 2, tween/timer 1
- `goback` : caméra 2, tween/timer 1
- `stepHit` : tween/timer 6, HUD 3, caméra 2, sprite/décor 1

## Karma (`Karma`)

Décor `tans`, persos tans, bf

### `data/stages/tans.hx`
**Appliqué** (15)
- [personnage] `dad.flipX = true` *(postCreate)*
- [personnage] `boyfriend.flipX = true` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.x += 470` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 300` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 700` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 700` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `rayTracker = new FlxSprite(600,170)` *(postCreate)*
- [sprite/décor] `rayTracker.scale.set(0.75,0.75)` *(postCreate)*
- [autre] `rayTracker.cameras = [camNoRays]` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*

**Ignoré** (50)
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(253,310)` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, 'tans', false)` *(postCreate)*
- [autre] `redad.flipX = true` *(postCreate)*
- [autre] `redad.flipY = true` *(postCreate)*
- [sprite/décor] `redad.x = 1961` *(postCreate)*
- [sprite/décor] `redad.y = 1475` *(postCreate)*
- [sprite/décor] `redad.skew.x = 9` *(postCreate)*
- [sprite/décor] `redad.color = FlxColor.YELLOW` *(postCreate)*
- [sprite/décor] `redad.alpha = 0.1` *(postCreate)*
- [sprite/décor] `insert(index, redad)` *(postCreate)*
- [personnage] `rebf = new Character(boyfriend.x, boyfriend.y, 'bf', true)` *(postCreate)*
- [autre] `rebf.flipX = true` *(postCreate)*
- [autre] `rebf.flipY = true` *(postCreate)*
- [sprite/décor] `rebf.x = 291` *(postCreate)*
- [sprite/décor] `rebf.y = 1115` *(postCreate)*
- [sprite/décor] `rebf.skew.x = -19` *(postCreate)*
- [sprite/décor] `rebf.skew.y = 5` *(postCreate)*
- [autre] `rebf.blend = 0` *(postCreate)*
- [sprite/décor] `rebf.color = FlxColor.YELLOW` *(postCreate)*
- [sprite/décor] `rebf.alpha = 0.1` *(postCreate)*
- [sprite/décor] `insert(index, rebf)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `rayTracker.active = false` *(postCreate)*
- [autre] `camRays = new FlxCamera()` *(postCreate)*
- [autre] `camRays.bgColor = 0` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godrays-morning")))` *(postCreate)*
- [shader] `camRays.addShader(rayShader)` *(postCreate)*
- [autre] `camNoRays = new FlxCamera()` *(postCreate)*
- [autre] `camNoRays.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.remove(camGame, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camGame, true)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camRays, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camNoRays, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [personnage] `dad.cameras = [camRays]` *(postCreate)*
- [personnage] `boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = [camRays]` *(postCreate)*
- [autre] `redad.cameras = [camRays]` *(postCreate)*
- [HUD] `cpuStrums.characters = [redad,dad]` *(postCreate)*
- [HUD] `playerStrums.characters = [rebf,boyfriend]` *(postCreate)*
- [HUD] `playerStrums.forEach(function(spr:FlxSprite) { spr.x -= 645; })` *(postCreate)*
- [HUD] `cpuStrums.forEach(function(spr:FlxSprite) { spr.x += 645; })` *(postCreate)*
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camRays.addShader(idkshade)` *(postCreate)*

**Non suivi** (7)
- [autre] `i++` *(postCreate)*
- [shader] `c.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `c.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `c.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `c.shader.flipped = c.flipX` *(postCreate)*
- [personnage] `dad.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = [camRays]` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : caméra 1
- `postUpdate` : caméra 6, sprite/décor 4, shader 1

### `songs/Karma/scripts/events.hx`
**Ignoré** (31)
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.cameras = [camRays]` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [HUD] `camHUD.alpha = 0` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `vignette=new FlxSprite().loadGraphic(Paths.image('stages/room/vignette'))` *(postCreate)*
- [autre] `vignette.screenCenter()` *(postCreate)*
- [autre] `vignette.cameras=[camfx]` *(postCreate)*
- [sprite/décor] `vignette.alpha=0` *(postCreate)*
- [sprite/décor] `vignette.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(vignette)` *(postCreate)*

**Non suivi** (1)
- [HUD] `camNewHUD.alpha = camHUD.alpha = 0` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 2
- `bump` : caméra 1, HUD 1
- `onSongStart` : tween/timer 1
- `stepHit` : caméra 25, HUD 6, tween/timer 4, sprite/décor 4
- `beatHit` : caméra 5, HUD 5

## Bobalolazing (`bobalolazing`)

Décor `minecraft`, persos bob, veethree, none

### `data/stages/minecraft.hx`
**Appliqué** (14)
- [personnage] `var index = members.indexOf(dad)`
- [sprite/décor] `bobblock = new FlxSprite(0, 0).loadGraphic(Paths.image("stages/bob/bobblock"))`
- [sprite/décor] `bobblock.frames = Paths.getSparrowAtlas('stages/bob/bobblock')`
- [autre] `bobblock.screenCenter(FlxAxes.Y)`
- [sprite/décor] `craft = new FlxSprite(0, 0).loadGraphic(Paths.image("stages/bob/craft"))`
- [autre] `craft.updateHitbox()`
- [autre] `craft.screenCenter(FlxAxes.XY)`
- [sprite/décor] `craft.scale.set(0.7,0.7)`
- [sprite/décor] `craft.x += 100`
- [sprite/décor] `craft.y += 500`
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.x += 200` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 40` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 330` *(postCreate)*

**Ignoré** (34)
- [personnage] `redad = new Character(dad.x, dad.y, 'bob', false)`
- [autre] `redad.flipY = true`
- [sprite/décor] `redad.y = 715`
- [sprite/décor] `redad.x = -29`
- [sprite/décor] `redad.skew.x = -36`
- [sprite/décor] `redad.color = FlxColor.BLACK`
- [sprite/décor] `redad.alpha = 0.4`
- [personnage] `rebf = new Character(dad.x, dad.y, 'taeyai-v2', true)`
- [autre] `rebf.flipY = true`
- [sprite/décor] `rebf.y = 1060`
- [sprite/décor] `rebf.x = 956`
- [sprite/décor] `rebf.skew.x = 36`
- [sprite/décor] `rebf.color = FlxColor.BLACK`
- [sprite/décor] `rebf.alpha = 0.4`
- [sprite/décor] `insert(index, rebf)`
- [sprite/décor] `insert(index, redad)`
- [sprite/décor] `bobblock.animation.addByPrefix('idle', "bob0", 24,false)`
- [autre] `craft.antialiasing = true`
- [sprite/décor] `bobblock.x = craft.x + 105`
- [sprite/décor] `bobblock.y = craft.y -70`
- [caméra] `camGame.scroll.set(253,310)`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(253,310)` *(postCreate)*
- [caméra] `rebf.cameraOffset.y = 10` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : caméra 1
- `beatHit` : sprite/décor 1
- `stepHit` : caméra 2
- `postUpdate` : sprite/décor 6, caméra 4, HUD 2

### `songs/bobalolazing/scripts/events.hx`
**Ignoré** (40)
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [shader] `colorTransformShader = new FunkinShader(Assets.getText(Paths.fragShader("colorTransform")))`
- [shader] `colorTransformShader.redMultiplier = 1`
- [shader] `colorTransformShader.greenMultiplier = 1`
- [shader] `colorTransformShader.blueMultiplier = 1`
- [shader] `colorTransformShader.alphaMultiplier = 1`
- [shader] `colorTransformShader.redOffset = 0`
- [shader] `colorTransformShader.greenOffset = 0`
- [shader] `colorTransformShader.blueOffset = 0`
- [shader] `colorTransformShader.alphaOffset = 0`
- [shader] `camGame.addShader(colorTransformShader)`
- [autre] `camFX = new FlxCamera()` *(postCreate)*
- [autre] `camFX.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camFX, false)` *(postCreate)*
- [sprite/décor] `red = new FlxSprite(-800, -800)` *(postCreate)*
- [sprite/décor] `red.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFdb0000)` *(postCreate)*
- [sprite/décor] `red.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `red.alpha = 0` *(postCreate)*
- [autre] `red.blend = 0` *(postCreate)*
- [sprite/décor] `add(red)` *(postCreate)*
- [sprite/décor] `fortnite = new FlxSprite(0, 0).loadGraphic(Paths.image('stages/bob/meme/fortnite'))` *(postCreate)*
- [sprite/décor] `fortnite.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `fortnite.alpha = 0` *(postCreate)*
- [autre] `fortnite.cameras = [camFX]` *(postCreate)*
- [sprite/décor] `fortnite.scale.set(0.5,0.5)` *(postCreate)*
- [sprite/décor] `fortnite.frames = Paths.getSparrowAtlas('stages/bob/meme/fortnite')` *(postCreate)*
- [sprite/décor] `fortnite.animation.addByPrefix('idle', "frame_", 24,true)` *(postCreate)*
- [sprite/décor] `fortnite.animation.play('idle')` *(postCreate)*
- [sprite/décor] `add(fortnite)` *(postCreate)*
- [sprite/décor] `jerma = new FlxSprite(0, 0).loadGraphic(Paths.image('stages/bob/meme/jerma'))` *(postCreate)*
- [sprite/décor] `jerma.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `jerma.alpha = 0` *(postCreate)*
- [autre] `jerma.blend = 0` *(postCreate)*
- [HUD] `jerma.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `add(jerma)` *(postCreate)*

**Pendant la chanson**
- `videomidsong` : tween/timer 1
- `postUpdate` : shader 6, HUD 1
- `stepHit` : sprite/décor 9, tween/timer 7, caméra 3, HUD 1
- `beatHit` : HUD 2, caméra 1

### `songs/bobalolazing/cutscene.hx`
**Non suivi** (18)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!)\nBasically! Talia just take a break from work and go do researc-\nI mean have fun by travel to r`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [vidéo/cinématique] `PlayState.instance.inCutscene = false`
- [autre] `PlayState.instance.startCountdown()`

**Pendant la chanson**
- `update` : sprite/décor 3, vidéo/cinématique 1

## Feathered Firewall (`feathered firewall`)

Décor `geese`, persos geese, veethree, 29 événements HScript Call

### `data/stages/geese.hx`
**Appliqué** (3)
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 60` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.7, 0.7)` *(postCreate)*

**Ignoré** (12)
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camGame.addShader(idkshade)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (11)
- [personnage] `dad.y -= 340` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 360` *(postCreate)*
- [personnage] `dad.y = 370` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.y += 80` *(postCreate)*
- [personnage] `dad.x -= 50` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 100` *(postCreate)*

### `songs/feathered firewall/scripts/events.hx`
**Ignoré** (10)
- [caméra] `camGame.zoom = 1.3` *(postCreate)*
- [caméra] `camGame.scroll.set(253,310)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*

**Pendant la chanson**
- `bump` (appelée 29 fois par le chart) : caméra 1, HUD 1
- `onSongStart` : caméra 1
- `camZooms` : caméra 1
- `postUpdate` : HUD 1, caméra 1
- `stepHit` : caméra 22, HUD 9, personnage 4
- `beatHit` : HUD 5, caméra 5

## Trolameme (`trolameme`)

Décor `geese`, persos sticky, veethree, 41 événements HScript Call

### `data/stages/geese.hx`
**Appliqué** (3)
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 60` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.7, 0.7)` *(postCreate)*

**Ignoré** (12)
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camGame.addShader(idkshade)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (11)
- [personnage] `dad.y -= 340` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 360` *(postCreate)*
- [personnage] `dad.y = 370` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.y += 80` *(postCreate)*
- [personnage] `dad.x -= 50` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 100` *(postCreate)*

### `songs/trolameme/scripts/events.hx`
**Ignoré** (31)
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `vignette=new FlxSprite().loadGraphic(Paths.image('stages/room/vignette'))` *(postCreate)*
- [autre] `vignette.screenCenter()` *(postCreate)*
- [autre] `vignette.cameras=[camfx]` *(postCreate)*
- [sprite/décor] `vignette.alpha=0.7` *(postCreate)*
- [sprite/décor] `vignette.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(vignette)` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*
- [caméra] `camGame.zoom = 1.3` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 2
- `bump` (appelée 41 fois par le chart) : caméra 1, HUD 1
- `onSongStart` : tween/timer 1
- `beatHit` : caméra 5, HUD 5
- `stepHit` : caméra 11, HUD 4, tween/timer 3, sprite/décor 1

## Julius (`julius`)

Décor `geese`, persos fever, veethree

### `data/stages/geese.hx`
**Appliqué** (3)
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 60` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.7, 0.7)` *(postCreate)*

**Ignoré** (12)
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camGame.addShader(idkshade)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (11)
- [personnage] `dad.y -= 340` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 360` *(postCreate)*
- [personnage] `dad.y = 370` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.y += 80` *(postCreate)*
- [personnage] `dad.x -= 50` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 100` *(postCreate)*

### `songs/julius/scripts/events.hx`
**Appliqué** (2)
- [personnage] `dad.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.y += 200` *(postCreate)*

**Ignoré** (31)
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `vignette=new FlxSprite().loadGraphic(Paths.image('stages/room/vignette'))` *(postCreate)*
- [autre] `vignette.screenCenter()` *(postCreate)*
- [autre] `vignette.cameras=[camfx]` *(postCreate)*
- [sprite/décor] `vignette.alpha=0.7` *(postCreate)*
- [sprite/décor] `vignette.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(vignette)` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*
- [caméra] `camGame.zoom = 1.3` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 2
- `bump` : caméra 1, HUD 1
- `onSongStart` : tween/timer 1
- `beatHit` : caméra 5, HUD 5
- `stepHit` : caméra 24, HUD 8, tween/timer 4, sprite/décor 2, personnage 1

## shini (`shini`)

Décor `geese`, persos shini, veethree, 113 événements HScript Call

### `data/stages/geese.hx`
**Appliqué** (3)
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 60` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.7, 0.7)` *(postCreate)*

**Ignoré** (12)
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camGame.addShader(idkshade)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (11)
- [personnage] `dad.y -= 340` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 360` *(postCreate)*
- [personnage] `dad.y = 370` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.y += 80` *(postCreate)*
- [personnage] `dad.x -= 50` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 100` *(postCreate)*

### `songs/shini/scripts/events.hx`
**Pendant la chanson**
- `bump` (appelée 113 fois par le chart) : caméra 1, HUD 1
- `beatHit` : caméra 5, HUD 5
- `stepHit` : caméra 18, HUD 7

## Fun Gang (`fun gang`)

Décor `geese`, persos sticky, veethree, shini, **2 ligne(s) de notes en plus** (persos non affichés), 19 événements HScript Call

### `data/stages/geese.hx`
**Appliqué** (3)
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 60` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.7, 0.7)` *(postCreate)*

**Ignoré** (12)
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camGame.addShader(idkshade)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [1, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (11)
- [personnage] `dad.y -= 340` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 360` *(postCreate)*
- [personnage] `dad.y = 370` *(postCreate)*
- [personnage] `dad.x -= 300` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `dad.y += 80` *(postCreate)*
- [personnage] `dad.x -= 50` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 100` *(postCreate)*

### `songs/fun gang/scripts/events.hx`
**Appliqué** (1)
- [caméra] `defaultCamZoom = 0.8` *(postCreate)*

**Ignoré** (41)
- [HUD] `strumLines.members[2].characters[0].visible = false` *(postCreate)*
- [HUD] `strumLines.members[3].characters[0].visible = false` *(postCreate)*
- [HUD] `strumLines.members[4].characters[0].visible = false` *(postCreate)*
- [HUD] `strumLines.members[4].characters[0].y = 370` *(postCreate)*
- [HUD] `strumLines.members[4].characters[0].x -= 300` *(postCreate)*
- [HUD] `strumLines.members[3].characters[0].color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].y += 80` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].x -= 50` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].cameraOffset.y -= 100` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `bleh = new FlxSprite(0,0).loadGraphic(Paths.image('cat_scream'))` *(postCreate)*
- [sprite/décor] `bleh.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `bleh.alpha = 0` *(postCreate)*
- [autre] `bleh.cameras = [camfx]` *(postCreate)*
- [autre] `bleh.setGraphicSize(FlxG.width, FlxG.height)` *(postCreate)*
- [autre] `bleh.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(bleh)` *(postCreate)*
- [sprite/décor] `silcat = new FlxSprite(0,0).loadGraphic(Paths.image('cat/cat'+FlxG.random.int(0,4)))` *(postCreate)*
- [sprite/décor] `silcat.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `silcat.alpha = 0` *(postCreate)*
- [autre] `silcat.cameras = [camfx]` *(postCreate)*
- [autre] `silcat.setGraphicSize(FlxG.width, FlxG.height)` *(postCreate)*
- [autre] `silcat.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(silcat)` *(postCreate)*
- [autre] `camfx2 = new FlxCamera()` *(postCreate)*
- [autre] `camfx2.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx2, false)` *(postCreate)*
- [sprite/décor] `overlayfx = new FlxSprite(0,0).loadGraphic(Paths.image('fx'))` *(postCreate)*
- [sprite/décor] `overlayfx.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `overlayfx.alpha = 0` *(postCreate)*
- [autre] `overlayfx.cameras = [camfx2]` *(postCreate)*
- [autre] `overlayfx.setGraphicSize(FlxG.width * 1.2, FlxG.height * 1.2)` *(postCreate)*
- [autre] `overlayfx.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(overlayfx)` *(postCreate)*

**Non suivi** (3)
- [sprite/décor] `s.visible = false` *(postCreate)*
- [sprite/décor] `s.visible = false` *(postCreate)*
- [sprite/décor] `s.visible = false` *(postCreate)*

**Pendant la chanson**
- `update` : sprite/décor 2
- `goose` (appelée 2 fois par le chart) : sprite/décor 4, tween/timer 1
- `meow` (appelée 3 fois par le chart) : sprite/décor 4, tween/timer 1
- `flash` (appelée 5 fois par le chart) : sprite/décor 9, tween/timer 1
- `stepHit` : HUD 28, caméra 22, sprite/décor 11, tween/timer 2
- `bump` (appelée 9 fois par le chart) : caméra 1, HUD 1
- `beatHit` : caméra 5, HUD 5

## New Hope (`new hope`)

Décor `dust`, persos sans, veethree, 4 événements HScript Call

### `data/stages/dust.hx`
**Appliqué** (24)
- [personnage] `dad.flipX = true` *(postCreate)*
- [personnage] `boyfriend.flipX = true` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.x += 470` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 300` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 700` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 700` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `bg = new FlxSprite(-400).loadGraphic(Paths.image('stages/tans/bg'))` *(postCreate)*
- [sprite/décor] `bg.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `light = new FlxSprite(-400).loadGraphic(Paths.image('stages/tans/light'))` *(postCreate)*
- [sprite/décor] `light.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `front = new FlxSprite(-400).loadGraphic(Paths.image('stages/tans/front'))` *(postCreate)*
- [sprite/décor] `front.scrollFactor.set(0.9,0.9)` *(postCreate)*
- [sprite/décor] `front.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `insert(index,bg)` *(postCreate)*
- [sprite/décor] `insert(index,light)` *(postCreate)*
- [sprite/décor] `add(front)` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `rayTracker = new FlxSprite(600,170)` *(postCreate)*
- [sprite/décor] `rayTracker.scale.set(0.75,0.75)` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*
- [autre] `rayTracker.cameras = [camNoRays]` *(postCreate)*

**Ignoré** (58)
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(253,310)` *(postCreate)*
- [sprite/décor] `bg.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `light.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `bg.color = FlxColor.CYAN` *(postCreate)*
- [sprite/décor] `light.color = FlxColor.CYAN` *(postCreate)*
- [sprite/décor] `front.color = FlxColor.CYAN` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, 'sans', false)` *(postCreate)*
- [autre] `redad.flipX = true` *(postCreate)*
- [autre] `redad.flipY = true` *(postCreate)*
- [sprite/décor] `redad.x = 1951` *(postCreate)*
- [sprite/décor] `redad.y = 1305` *(postCreate)*
- [sprite/décor] `redad.skew.x = 9` *(postCreate)*
- [sprite/décor] `redad.color = FlxColor.BLUE` *(postCreate)*
- [sprite/décor] `redad.alpha = 0.1` *(postCreate)*
- [sprite/décor] `insert(index, redad)` *(postCreate)*
- [personnage] `rebf = new Character(boyfriend.x, boyfriend.y, 'taeyai-v2', true)` *(postCreate)*
- [autre] `rebf.flipX = true` *(postCreate)*
- [autre] `rebf.flipY = true` *(postCreate)*
- [sprite/décor] `rebf.x = 261 + 37` *(postCreate)*
- [sprite/décor] `rebf.y = 1455` *(postCreate)*
- [sprite/décor] `rebf.skew.x = -19` *(postCreate)*
- [sprite/décor] `rebf.skew.y = 5` *(postCreate)*
- [autre] `rebf.blend = 0` *(postCreate)*
- [sprite/décor] `rebf.color = FlxColor.CYAN` *(postCreate)*
- [sprite/décor] `rebf.alpha = 0.1` *(postCreate)*
- [sprite/décor] `insert(index, rebf)` *(postCreate)*
- [sprite/décor] `bg.color = 0xFF212bde` *(postCreate)*
- [sprite/décor] `light.color = 0xFF212bde` *(postCreate)*
- [sprite/décor] `front.color = 0xFF212bde` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `rayTracker.active = false` *(postCreate)*
- [autre] `camRays = new FlxCamera()` *(postCreate)*
- [autre] `camRays.bgColor = 0` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godray-sans")))` *(postCreate)*
- [shader] `camRays.addShader(rayShader)` *(postCreate)*
- [autre] `camNoRays = new FlxCamera()` *(postCreate)*
- [autre] `camNoRays.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.remove(camGame, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camGame, true)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camRays, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camNoRays, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [personnage] `dad.cameras = [camRays]` *(postCreate)*
- [personnage] `boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = [camRays]` *(postCreate)*
- [autre] `redad.cameras = [camRays]` *(postCreate)*
- [HUD] `cpuStrums.characters = [redad,dad]` *(postCreate)*
- [HUD] `playerStrums.characters = [rebf,boyfriend]` *(postCreate)*
- [HUD] `playerStrums.forEach(function(spr:FlxSprite) { spr.x -= 645; })` *(postCreate)*
- [HUD] `cpuStrums.forEach(function(spr:FlxSprite) { spr.x += 645; })` *(postCreate)*
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camRays.addShader(idkshade)` *(postCreate)*

**Non suivi** (7)
- [autre] `i++` *(postCreate)*
- [shader] `c.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `c.shader.color = [1, 255 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `c.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `c.shader.flipped = c.flipX` *(postCreate)*
- [personnage] `dad.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = [camRays]` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : caméra 1
- `postUpdate` : caméra 6, sprite/décor 4, shader 1

### `songs/new hope/scripts/events.hx`
**Ignoré** (51)
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `eventwhite = new FlxSprite(0,0).makeGraphic(1280,720, 0xFFFFFFFF)` *(postCreate)*
- [sprite/décor] `eventwhite.alpha = 0` *(postCreate)*
- [autre] `eventwhite.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventwhite.scale.set(4,4)` *(postCreate)*
- [autre] `eventwhite.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventwhite.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventwhite)` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.cameras = [camRays]` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [personnage] `eventbackground3 = new FlxSprite(dad.x,dad.y).makeGraphic(1280,720, 0xFFFFFFFF)` *(postCreate)*
- [sprite/décor] `eventbackground3.alpha = 0` *(postCreate)*
- [autre] `eventbackground3.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground3.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground3.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground3.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground3)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [HUD] `camHUD.alpha = 0` *(postCreate)*
- [autre] `cameffect = new FlxCamera()` *(postCreate)*
- [autre] `cameffect.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(cameffect, false)` *(postCreate)*
- [sprite/décor] `image = new FlxSprite(0, 0).loadGraphic(Paths.image('stages/tans/heart'))` *(postCreate)*
- [autre] `image.antialiasing = false` *(postCreate)*
- [autre] `image.updateHitbox()` *(postCreate)*
- [autre] `image.screenCenter(FlxAxes.XY)` *(postCreate)*
- [autre] `image.cameras = [cameffect]` *(postCreate)*
- [sprite/décor] `image.alpha = 0` *(postCreate)*
- [sprite/décor] `add(image)` *(postCreate)*
- [shader] `glitch = new CustomShader('screenglitch')` *(postCreate)*
- [autre] `glitch.enabled = false` *(postCreate)*
- [shader] `camGame.addShader(glitch)` *(postCreate)*
- [shader] `camNoRays.addShader(glitch)` *(postCreate)*
- [shader] `camRays.addShader(glitch)` *(postCreate)*
- [shader] `hudglitch = new CustomShader('hudglitch')` *(postCreate)*
- [autre] `hudglitch.enabled = false` *(postCreate)*
- [shader] `camHUD.addShader(hudglitch)` *(postCreate)*
- [shader] `camNewHUD.addShader(hudglitch)` *(postCreate)*

**Non suivi** (1)
- [HUD] `camNewHUD.alpha = camHUD.alpha = 0` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 2
- `bump` : caméra 1, HUD 1
- `onSongStart` : tween/timer 1
- `eheheh` : tween/timer 4, HUD 4, caméra 2
- `beatHit` : sprite/décor 2
- `stepHit` : caméra 13, HUD 7, tween/timer 6, sprite/décor 6, personnage 5
- `Countdown` (appelée 4 fois par le chart) : sprite/décor 6, tween/timer 3

## Hoax (`hoax`)

Décor `dust`, persos sans, veethree, none

### `data/stages/dust.hx`
**Appliqué** (24)
- [personnage] `dad.flipX = true` *(postCreate)*
- [personnage] `boyfriend.flipX = true` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.x += 470` *(postCreate)*
- [personnage] `dad.cameraOffset.x -= 300` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 700` *(postCreate)*
- [personnage] `dad.cameraOffset.y -= 700` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `bg = new FlxSprite(-400).loadGraphic(Paths.image('stages/tans/bg'))` *(postCreate)*
- [sprite/décor] `bg.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `light = new FlxSprite(-400).loadGraphic(Paths.image('stages/tans/light'))` *(postCreate)*
- [sprite/décor] `light.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `front = new FlxSprite(-400).loadGraphic(Paths.image('stages/tans/front'))` *(postCreate)*
- [sprite/décor] `front.scrollFactor.set(0.9,0.9)` *(postCreate)*
- [sprite/décor] `front.scale.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `insert(index,bg)` *(postCreate)*
- [sprite/décor] `insert(index,light)` *(postCreate)*
- [sprite/décor] `add(front)` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `rayTracker = new FlxSprite(600,170)` *(postCreate)*
- [sprite/décor] `rayTracker.scale.set(0.75,0.75)` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*
- [autre] `rayTracker.cameras = [camNoRays]` *(postCreate)*

**Ignoré** (58)
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(253,310)` *(postCreate)*
- [sprite/décor] `bg.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `light.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `bg.color = FlxColor.CYAN` *(postCreate)*
- [sprite/décor] `light.color = FlxColor.CYAN` *(postCreate)*
- [sprite/décor] `front.color = FlxColor.CYAN` *(postCreate)*
- [personnage] `redad = new Character(dad.x, dad.y, 'sans', false)` *(postCreate)*
- [autre] `redad.flipX = true` *(postCreate)*
- [autre] `redad.flipY = true` *(postCreate)*
- [sprite/décor] `redad.x = 1951` *(postCreate)*
- [sprite/décor] `redad.y = 1305` *(postCreate)*
- [sprite/décor] `redad.skew.x = 9` *(postCreate)*
- [sprite/décor] `redad.color = FlxColor.BLUE` *(postCreate)*
- [sprite/décor] `redad.alpha = 0.1` *(postCreate)*
- [sprite/décor] `insert(index, redad)` *(postCreate)*
- [personnage] `rebf = new Character(boyfriend.x, boyfriend.y, 'taeyai-v2', true)` *(postCreate)*
- [autre] `rebf.flipX = true` *(postCreate)*
- [autre] `rebf.flipY = true` *(postCreate)*
- [sprite/décor] `rebf.x = 261 + 37` *(postCreate)*
- [sprite/décor] `rebf.y = 1455` *(postCreate)*
- [sprite/décor] `rebf.skew.x = -19` *(postCreate)*
- [sprite/décor] `rebf.skew.y = 5` *(postCreate)*
- [autre] `rebf.blend = 0` *(postCreate)*
- [sprite/décor] `rebf.color = FlxColor.CYAN` *(postCreate)*
- [sprite/décor] `rebf.alpha = 0.1` *(postCreate)*
- [sprite/décor] `insert(index, rebf)` *(postCreate)*
- [sprite/décor] `bg.color = 0xFF212bde` *(postCreate)*
- [sprite/décor] `light.color = 0xFF212bde` *(postCreate)*
- [sprite/décor] `front.color = 0xFF212bde` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `rayTracker.active = false` *(postCreate)*
- [autre] `camRays = new FlxCamera()` *(postCreate)*
- [autre] `camRays.bgColor = 0` *(postCreate)*
- [shader] `rayShader = new FunkinShader(Assets.getText(Paths.fragShader("godray-sans")))` *(postCreate)*
- [shader] `camRays.addShader(rayShader)` *(postCreate)*
- [autre] `camNoRays = new FlxCamera()` *(postCreate)*
- [autre] `camNoRays.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.remove(camGame, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camGame, true)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camRays, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camNoRays, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camGame.addShader(bloomShader)` *(postCreate)*
- [personnage] `dad.cameras = [camRays]` *(postCreate)*
- [personnage] `boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = [camRays]` *(postCreate)*
- [autre] `redad.cameras = [camRays]` *(postCreate)*
- [HUD] `cpuStrums.characters = [redad,dad]` *(postCreate)*
- [HUD] `playerStrums.characters = [rebf,boyfriend]` *(postCreate)*
- [HUD] `playerStrums.forEach(function(spr:FlxSprite) { spr.x -= 645; })` *(postCreate)*
- [HUD] `cpuStrums.forEach(function(spr:FlxSprite) { spr.x += 645; })` *(postCreate)*
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camRays.addShader(idkshade)` *(postCreate)*

**Non suivi** (7)
- [autre] `i++` *(postCreate)*
- [shader] `c.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `c.shader.color = [1, 255 / 255, 0.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `c.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `c.shader.flipped = c.flipX` *(postCreate)*
- [personnage] `dad.cameras = boyfriend.cameras = [camRays]` *(postCreate)*
- [autre] `rebf.cameras = redad.cameras = [camRays]` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : caméra 1
- `postUpdate` : caméra 6, sprite/décor 4, shader 1

## Virus (`virus`)

Décor `shadow`, persos shadow taeyai, bf, none

### `data/stages/shadow.hx`
**Appliqué** (8)
- [personnage] `var index = members.indexOf(dad)`
- [sprite/décor] `light = new FlxSprite(-1080, -865).loadGraphic(Paths.image('stages/shadow/light'))`
- [sprite/décor] `light.scale.set(1.5,1.5)`
- [sprite/décor] `add(light)`
- [personnage] `dad.x = -840` *(postCreate)*
- [personnage] `dad.y = -210` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*

**Ignoré** (25)
- [personnage] `rebf = new Character(dad.x, dad.y, 'bf', false)`
- [autre] `rebf.flipX = false`
- [autre] `rebf.flipY = true`
- [sprite/décor] `rebf.y = 450`
- [sprite/décor] `rebf.x = 891`
- [sprite/décor] `rebf.skew.x = 36`
- [autre] `rebf.blend = 0`
- [sprite/décor] `rebf.color = FlxColor.RED`
- [sprite/décor] `rebf.alpha = 0.1`
- [sprite/décor] `insert(index, rebf)`
- [sprite/décor] `light.alpha = 1`
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))`
- [shader] `camGame.addShader(idkshade)`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(-300,110)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [3, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (2)
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)`
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*

**Pendant la chanson**
- `onPostNoteCreation` : sprite/décor 1
- `onCountdown` : caméra 1
- `postUpdate` : caméra 3, personnage 2, HUD 1
- `stepHit` : personnage 5, tween/timer 4, sprite/décor 2, caméra 2
- `addGameShader` : shader 2

### `songs/virus/scripts/glitch.hx`
**Ignoré** (8)
- [shader] `iLikeToMoveItMoveIt = new CustomShader('i like to move it move it', "130")` *(postCreate)*
- [shader] `virusShader = new CustomShader('virus', "130")` *(postCreate)*
- [shader] `virusShader.uEnabled = false` *(postCreate)*
- [shader] `virusShader.screenSize = [1280, 720]` *(postCreate)*
- [shader] `virusShader.colorMultiplier = 1` *(postCreate)*
- [shader] `virusShader.colorTransform = true` *(postCreate)*
- [shader] `addGameShader(virusShader)` *(postCreate)*
- [autre] `wasDownscroll = downscroll` *(postCreate)*

**Non suivi** (1)
- [sprite/décor] `sl.onNoteUpdate.add(updateNote)` *(postCreate)*

**Pendant la chanson**
- `update` : shader 2
- `updateNote` : sprite/décor 1
- `stepHit` : shader 3, caméra 2, HUD 1
- `addGameShader` : shader 2

### `songs/virus/scripts/testlol.hx`
**Ignoré** (3)
- [shader] `screenshader = new CustomShader('glitchscreen')` *(postCreate)*
- [autre] `screenshader.uEnabled = false` *(postCreate)*
- [shader] `addGameShader(screenshader)` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : HUD 1
- `addGameShader` : shader 2

### `songs/virus/scripts/timeSkip.hx`
**Non suivi** (1)
- [sprite/décor] `sl.onNoteUpdate.add(updateNote)` *(postCreate)*

**Pendant la chanson**
- `update` : audio 1

### `songs/virus/cutscene.hx`
**Non suivi** (18)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37/2, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!) Basically! BF has been teleported to Dark Void by someone and then he found the someone that loo`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [vidéo/cinématique] `PlayState.instance.inCutscene = false`
- [autre] `PlayState.instance.startCountdown()`

**Pendant la chanson**
- `update` : sprite/décor 3, vidéo/cinématique 1

## Virus (LEGACY) (`virus-legacy`)

Décor `room`, persos shadow taeyai, bf, gf

### `data/stages/room.hx`
**Appliqué** (5)
- [autre] `var i = members.indexOf(roombg2)`
- [personnage] `boyfriend.setPosition(930,535)` *(postCreate)*
- [personnage] `dad.setPosition(-50,525)` *(postCreate)*
- [personnage] `dad.alpha = 0.4` *(postCreate)*
- [personnage] `boyfriend.alpha = 0.4` *(postCreate)*

**Ignoré** (14)
- [autre] `var positions = [ [true, 644, 282, 382, 272, [16, 2], [373, 29], [7, 263], [365, 242], 0], [true, 652, 532, 367, 273, [2, 10], [354, 1], [5,`
- [autre] `var easterEgg = FlxG.random.bool(1/4.2)`
- [autre] `var baseImages = [for(f in Paths.getFolderContent('images/stages/room/frames')) if (Path.extension(f).toLowerCase() == "png") Path.withoutEx`
- [autre] `var images = []`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(720,410)` *(postCreate)*
- [shader] `colorShader = new FunkinShader(Assets.getText(Paths.fragShader("colorgrade")))` *(postCreate)*
- [shader] `colorShader.topr = 3.1` *(postCreate)*
- [shader] `colorShader.topg = 1.1` *(postCreate)*
- [shader] `colorShader.topb = 1.1` *(postCreate)*
- [shader] `colorShader.botr = 1.1` *(postCreate)*
- [shader] `colorShader.botg = 1.1` *(postCreate)*
- [shader] `colorShader.botb = 2.1` *(postCreate)*
- [shader] `camGame.addShader(colorShader)` *(postCreate)*

**Non suivi** (23)
- [sprite/décor] `var spr = new FunkinSprite(roombg2.x, roombg2.y)`
- [autre] `images = [for(i in baseImages) i]`
- [autre] `var curImg = images[FlxG.random.int(0, images.length-1)]`
- [sprite/décor] `images.remove(curImg)`
- [autre] `spr.loadSprite(easterEgg ? Paths.image('stages/room/putin') : Paths.image('stages/room/frames/' + curImg))`
- [sprite/décor] `spr.animation.add("idle", [for(i in 0...spr.frames.frames.length) i], 24, true)`
- [sprite/décor] `spr.animation.play("idle")`
- [sprite/décor] `spr.scrollFactor = roombg2.scrollFactor`
- [autre] `spr.antialiasing = true`
- [sprite/décor] `spr.alpha = 0.8`
- [sprite/décor] `spr.x += pos[1]`
- [sprite/décor] `spr.y += pos[2]`
- [autre] `spr.setGraphicSize(pos[3], pos[4])`
- [autre] `spr.updateHitbox()`
- [shader] `spr.shader = new CustomShader("room-shader")`
- [shader] `spr.shader.topLeft = pos[5]`
- [shader] `spr.shader.topRight = pos[6]`
- [shader] `spr.shader.bottomLeft = pos[7]`
- [shader] `spr.shader.bottomRight = pos[8]`
- [shader] `spr.shader.hologramSize = [pos[3], pos[4]]`
- [sprite/décor] `spr.angle = pos[9]`
- [sprite/décor] `insert(i, spr)`
- [personnage] `dad.alpha = boyfriend.alpha = 0.4` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : personnage 2, caméra 1
- `postUpdate` : caméra 3

### `songs/virus/scripts/glitch.hx`
**Ignoré** (8)
- [shader] `iLikeToMoveItMoveIt = new CustomShader('i like to move it move it', "130")` *(postCreate)*
- [shader] `virusShader = new CustomShader('virus', "130")` *(postCreate)*
- [shader] `virusShader.uEnabled = false` *(postCreate)*
- [shader] `virusShader.screenSize = [1280, 720]` *(postCreate)*
- [shader] `virusShader.colorMultiplier = 1` *(postCreate)*
- [shader] `virusShader.colorTransform = true` *(postCreate)*
- [shader] `addGameShader(virusShader)` *(postCreate)*
- [autre] `wasDownscroll = downscroll` *(postCreate)*

**Non suivi** (1)
- [sprite/décor] `sl.onNoteUpdate.add(updateNote)` *(postCreate)*

**Pendant la chanson**
- `update` : shader 2
- `updateNote` : sprite/décor 1
- `stepHit` : shader 3, caméra 2, HUD 1
- `addGameShader` : shader 2

### `songs/virus/scripts/testlol.hx`
**Ignoré** (3)
- [shader] `screenshader = new CustomShader('glitchscreen')` *(postCreate)*
- [autre] `screenshader.uEnabled = false` *(postCreate)*
- [shader] `addGameShader(screenshader)` *(postCreate)*

**Pendant la chanson**
- `postUpdate` : HUD 1
- `addGameShader` : shader 2

### `songs/virus/scripts/timeSkip.hx`
**Non suivi** (1)
- [sprite/décor] `sl.onNoteUpdate.add(updateNote)` *(postCreate)*

**Pendant la chanson**
- `update` : audio 1

### `songs/virus/cutscene.hx`
**Non suivi** (18)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37/2, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!) Basically! BF has been teleported to Dark Void by someone and then he found the someone that loo`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [vidéo/cinématique] `PlayState.instance.inCutscene = false`
- [autre] `PlayState.instance.startCountdown()`

**Pendant la chanson**
- `update` : sprite/décor 3, vidéo/cinématique 1

## Crowd Control (`crowd control`)

Décor `shadow`, persos shadow taeyai, bf, 79 événements HScript Call

### `data/stages/shadow.hx`
**Appliqué** (8)
- [personnage] `var index = members.indexOf(dad)`
- [sprite/décor] `light = new FlxSprite(-1080, -865).loadGraphic(Paths.image('stages/shadow/light'))`
- [sprite/décor] `light.scale.set(1.5,1.5)`
- [sprite/décor] `add(light)`
- [personnage] `dad.x = -840` *(postCreate)*
- [personnage] `dad.y = -210` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*

**Ignoré** (25)
- [personnage] `rebf = new Character(dad.x, dad.y, 'bf', false)`
- [autre] `rebf.flipX = false`
- [autre] `rebf.flipY = true`
- [sprite/décor] `rebf.y = 450`
- [sprite/décor] `rebf.x = 891`
- [sprite/décor] `rebf.skew.x = 36`
- [autre] `rebf.blend = 0`
- [sprite/décor] `rebf.color = FlxColor.RED`
- [sprite/décor] `rebf.alpha = 0.1`
- [sprite/décor] `insert(index, rebf)`
- [sprite/décor] `light.alpha = 1`
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))`
- [shader] `camGame.addShader(idkshade)`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(-300,110)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [3, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (2)
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)`
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*

**Pendant la chanson**
- `onPostNoteCreation` : sprite/décor 1
- `onCountdown` : caméra 1
- `postUpdate` : caméra 3, personnage 2, HUD 1
- `stepHit` : personnage 5, tween/timer 4, sprite/décor 2, caméra 2
- `addGameShader` : shader 2

### `songs/crowd control/scripts/events.hx`
**Appliqué** (2)
- [personnage] `dad.alpha = 0` *(postCreate)*
- [personnage] `boyfriend.alpha = 0` *(postCreate)*

**Ignoré** (18)
- [caméra] `camGame.zoom = 2` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 1` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [shader] `blur = new CustomShader("radialBlur")` *(postCreate)*
- [autre] `blur.cx = 0.5` *(postCreate)*
- [autre] `blur.cy = 0.5` *(postCreate)*
- [autre] `blur.blurWidth = 0.0` *(postCreate)*
- [shader] `camGame.addShader(blur)` *(postCreate)*
- [shader] `camHUD.addShader(blur)` *(postCreate)*
- [shader] `camNewHUD.addShader(blur)` *(postCreate)*

**Non suivi** (1)
- [personnage] `dad.alpha = boyfriend.alpha = 0` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 2
- `hardbump` (appelée 17 fois par le chart) : caméra 1, HUD 1
- `bump` (appelée 62 fois par le chart) : caméra 1, HUD 1
- `onSongStart` : personnage 2, tween/timer 1, caméra 1
- `stepHit` : caméra 48, HUD 6, tween/timer 2
- `beatHit` : caméra 4, HUD 4

## malware (`malware`)

Décor `shadow`, persos shadow taeyai, bf

### `data/stages/shadow.hx`
**Appliqué** (8)
- [personnage] `var index = members.indexOf(dad)`
- [sprite/décor] `light = new FlxSprite(-1080, -865).loadGraphic(Paths.image('stages/shadow/light'))`
- [sprite/décor] `light.scale.set(1.5,1.5)`
- [sprite/décor] `add(light)`
- [personnage] `dad.x = -840` *(postCreate)*
- [personnage] `dad.y = -210` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*

**Ignoré** (25)
- [personnage] `rebf = new Character(dad.x, dad.y, 'bf', false)`
- [autre] `rebf.flipX = false`
- [autre] `rebf.flipY = true`
- [sprite/décor] `rebf.y = 450`
- [sprite/décor] `rebf.x = 891`
- [sprite/décor] `rebf.skew.x = 36`
- [autre] `rebf.blend = 0`
- [sprite/décor] `rebf.color = FlxColor.RED`
- [sprite/décor] `rebf.alpha = 0.1`
- [sprite/décor] `insert(index, rebf)`
- [sprite/décor] `light.alpha = 1`
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))`
- [shader] `camGame.addShader(idkshade)`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(-300,110)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [3, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (2)
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)`
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*

**Pendant la chanson**
- `onPostNoteCreation` : sprite/décor 1
- `onCountdown` : caméra 1
- `postUpdate` : caméra 3, personnage 2, HUD 1
- `stepHit` : personnage 5, tween/timer 4, sprite/décor 2, caméra 2
- `addGameShader` : shader 2

### `songs/malware/scripts/events.hx`
**Ignoré** (11)
- [shader] `colorTransformShader = new FunkinShader(Assets.getText(Paths.fragShader("colorTransform")))`
- [shader] `colorTransformShader.redMultiplier = 1`
- [shader] `colorTransformShader.greenMultiplier = 1`
- [shader] `colorTransformShader.blueMultiplier = 1`
- [shader] `colorTransformShader.alphaMultiplier = 1`
- [shader] `colorTransformShader.redOffset = 0`
- [shader] `colorTransformShader.greenOffset = 0`
- [shader] `colorTransformShader.blueOffset = 0`
- [shader] `colorTransformShader.alphaOffset = 0`
- [shader] `FlxG.game.setFilters([new ShaderFilter(colorTransformShader)])`
- [shader] `FlxG.game._filters.push(new ShaderFilter(colorTransformShader))`

**Non suivi** (1)
- [sprite/décor] `sl.onNoteUpdate.add(updateNote)` *(postCreate)*

**Pendant la chanson**
- `update` : shader 2
- `postUpdate` : shader 1
- `stepHit` : caméra 13, HUD 10, shader 3, tween/timer 2, sprite/décor 1
- `onPlayerHit` : HUD 1
- `dotrails` : personnage 4, sprite/décor 3, tween/timer 1
- `beatHit` : caméra 5, HUD 4
- `particle` : sprite/décor 7
- `floating` : sprite/décor 1

## Errorians (`errorians`)

Décor `shadow`, persos shadow taeyai, taescare, futo, **2 ligne(s) de notes en plus** (persos non affichés), 427 événements HScript Call

### `data/stages/shadow.hx`
**Appliqué** (8)
- [personnage] `var index = members.indexOf(dad)`
- [sprite/décor] `light = new FlxSprite(-1080, -865).loadGraphic(Paths.image('stages/shadow/light'))`
- [sprite/décor] `light.scale.set(1.5,1.5)`
- [sprite/décor] `add(light)`
- [personnage] `dad.x = -840` *(postCreate)*
- [personnage] `dad.y = -210` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*

**Ignoré** (25)
- [personnage] `rebf = new Character(dad.x, dad.y, 'bf', false)`
- [autre] `rebf.flipX = false`
- [autre] `rebf.flipY = true`
- [sprite/décor] `rebf.y = 450`
- [sprite/décor] `rebf.x = 891`
- [sprite/décor] `rebf.skew.x = 36`
- [autre] `rebf.blend = 0`
- [sprite/décor] `rebf.color = FlxColor.RED`
- [sprite/décor] `rebf.alpha = 0.1`
- [sprite/décor] `insert(index, rebf)`
- [sprite/décor] `light.alpha = 1`
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))`
- [shader] `camGame.addShader(idkshade)`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(-300,110)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [3, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (2)
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)`
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*

**Pendant la chanson**
- `onPostNoteCreation` : sprite/décor 1
- `onCountdown` : caméra 1
- `postUpdate` : caméra 3, personnage 2, HUD 1
- `stepHit` : personnage 5, tween/timer 4, sprite/décor 2, caméra 2
- `addGameShader` : shader 2

### `songs/errorians/scripts/events.hx`
**Ignoré** (93)
- [HUD] `futo = strumLines.members[2].characters[0]` *(postCreate)*
- [HUD] `conner = strumLines.members[3].characters[0]` *(postCreate)*
- [HUD] `daisy = strumLines.members[4].characters[0]` *(postCreate)*
- [HUD] `shaStrum = strumLines.members[0]` *(postCreate)*
- [HUD] `futoStrum = strumLines.members[2]` *(postCreate)*
- [HUD] `connerStrum = strumLines.members[3]` *(postCreate)*
- [HUD] `daisyStrum = strumLines.members[4]` *(postCreate)*
- [personnage] `taeyai = boyfriend` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `conner.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `conner.shader.color = [3, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `conner.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `conner.shader.flipped = false` *(postCreate)*
- [shader] `daisy.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `daisy.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `daisy.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `daisy.shader.flipped = true` *(postCreate)*
- [shader] `futo.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `futo.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `futo.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `futo.shader.flipped = false` *(postCreate)*
- [sprite/décor] `conner.color = FlxColor.fromHSL(conner.color.hue, conner.color.saturation, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `daisy.color = FlxColor.fromHSL(daisy.color.hue, daisy.color.saturation, 0.7, 0.7)` *(postCreate)*
- [sprite/décor] `futo.color = FlxColor.fromHSL(futo.color.hue, futo.color.saturation, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `daisy.y = -300` *(postCreate)*
- [sprite/décor] `conner.setPosition(-570,105)` *(postCreate)*
- [sprite/décor] `futo.setPosition(-1280,-100)` *(postCreate)*
- [sprite/décor] `futo.visible = false` *(postCreate)*
- [sprite/décor] `conner.visible = false` *(postCreate)*
- [sprite/décor] `daisy.visible = false` *(postCreate)*
- [sprite/décor] `taeyai.y += 100` *(postCreate)*
- [sprite/décor] `add(fixedCamFollow)` *(postCreate)*
- [caméra] `camGame.alpha = 0` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 0` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [shader] `blur = new CustomShader("radialBlur")` *(postCreate)*
- [autre] `blur.cx = 0.5` *(postCreate)*
- [autre] `blur.cy = 0.5` *(postCreate)*
- [autre] `blur.blurWidth = 0.0` *(postCreate)*
- [shader] `chromaticGame = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromaticGame.chromaticAmount = 0.003` *(postCreate)*
- [shader] `chromatic = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromatic.chromaticAmount = 0.001` *(postCreate)*
- [shader] `camGame.addShader(blur)` *(postCreate)*
- [shader] `camHUD.addShader(blur)` *(postCreate)*
- [shader] `camNewHUD.addShader(blur)` *(postCreate)*
- [shader] `camGame.addShader(chromaticGame)` *(postCreate)*
- [shader] `camHUD.addShader(chromatic)` *(postCreate)*
- [shader] `camNewHUD.addShader(chromatic)` *(postCreate)*
- [HUD] `var index = members.indexOf(strumLines.members[0].characters[0])` *(postCreate)*
- [sprite/décor] `clown1 = new FlxSprite(-1470 ,-895 ).loadGraphic(Paths.image('stages/shadow/clown2'))` *(postCreate)*
- [sprite/décor] `insert(index, clown1)` *(postCreate)*
- [sprite/décor] `clownf = new FlxSprite(-1400 ,-1055 ).loadGraphic(Paths.image("stages/shadow/flame"))` *(postCreate)*
- [sprite/décor] `clownf.frames = Paths.getSparrowAtlas('stages/shadow/flame')` *(postCreate)*
- [autre] `clownf.antialiasing = true` *(postCreate)*
- [sprite/décor] `clownf.scrollFactor.set(0.7,0.7)` *(postCreate)*
- [sprite/décor] `clownf.animation.addByPrefix('idle', "f", 24,true)` *(postCreate)*
- [sprite/décor] `clownf.animation.play('idle',true)` *(postCreate)*
- [sprite/décor] `insert(index, clownf)` *(postCreate)*
- [sprite/décor] `clown0 = new FlxSprite(-1310 ,-1010).loadGraphic(Paths.image('stages/shadow/clown1'))` *(postCreate)*
- [sprite/décor] `insert(index, clown0)` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [autre] `coolText = new FunkinText(0,0,0,'CHAOS', 64, true)` *(postCreate)*
- [autre] `coolText.antialiasing = true` *(postCreate)*
- [autre] `coolText.font = Paths.font("seriouss.ttf")` *(postCreate)*
- [autre] `coolText.cameras = [camfx]` *(postCreate)*
- [sprite/décor] `coolText.color = 0xFFf5675b` *(postCreate)*
- [autre] `coolText.screenCenter()` *(postCreate)*
- [sprite/décor] `add(coolText)` *(postCreate)*
- [sprite/décor] `scary = new FlxSprite(-1310 ,-1010).loadGraphic(Paths.image('stages/shadow/scary'))` *(postCreate)*
- [autre] `scary.cameras = [camfx]` *(postCreate)*
- [sprite/décor] `scary.alpha = 0` *(postCreate)*
- [autre] `scary.screenCenter()` *(postCreate)*
- [sprite/décor] `add(scary)` *(postCreate)*
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)` *(postCreate)*
- [autre] `video.cameras = [camfx]` *(postCreate)*
- [sprite/décor] `add(video)` *(postCreate)*
- [shader] `glitch = new CustomShader('screenglitch')` *(postCreate)*
- [autre] `glitch.enabled = true` *(postCreate)*
- [shader] `camfx.addShader(glitch)` *(postCreate)*
- [sprite/décor] `clown1.visible = false` *(postCreate)*
- [sprite/décor] `clownf.visible = false` *(postCreate)*
- [sprite/décor] `clown0.visible = false` *(postCreate)*
- [sprite/décor] `coolText.visible = false` *(postCreate)*

**Non suivi** (11)
- [sprite/décor] `futo.visible = conner.visible = daisy.visible = false` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `s.color = FlxColor.fromHSL(3, 0.3, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `s.color = FlxColor.fromHSL(298, 0.3, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*
- [HUD] `camGame.alpha = camNewHUD.alpha = 0` *(postCreate)*
- [sprite/décor] `clown1.visible = clownf.visible = clown0.visible = coolText.visible = false` *(postCreate)*
- [autre] `FlxG.save.data.bfmode = false` *(postCreate)*

**Pendant la chanson**
- `videomidsong` : tween/timer 1
- `onCountdown` : sprite/décor 2
- `onPostCountdown` : caméra 2, sprite/décor 1, tween/timer 1
- `onPostNoteCreation` : sprite/décor 3
- `postUpdate` : sprite/décor 2
- `bump` (appelée 275 fois par le chart) : caméra 1, HUD 1
- `hardbump` (appelée 152 fois par le chart) : caméra 1, HUD 1
- `beatHit` : caméra 2, HUD 2
- `stepHit` : caméra 80, sprite/décor 59, tween/timer 15, HUD 12, personnage 4

### `songs/errorians/cutscene.hx`
**Non suivi** (18)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37/2, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!) Basically! Talia Detected something between the World Space and then after she finding where it’`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [vidéo/cinématique] `PlayState.instance.inCutscene = false`
- [autre] `PlayState.instance.startCountdown()`

**Pendant la chanson**
- `update` : sprite/décor 3, vidéo/cinématique 1

## Kowase (`kowase`)

Décor `kowase`, persos gffake, sadbf, picoillusion, 111 événements HScript Call

### `data/stages/kowase.hx`
**Appliqué** (28)
- [HUD] `var index = members.indexOf(strumLines.members[2].characters[0])`
- [sprite/décor] `white = new FlxSprite(-800, -800)`
- [sprite/décor] `white.scrollFactor.set(0, 0)`
- [autre] `white.blend = 1`
- [sprite/décor] `insert(index, white)`
- [personnage] `boyfriend.y += 70` *(postCreate)*
- [personnage] `boyfriend.flipX = false` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [autre] `var index = members.indexOf(pico)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [sprite/décor] `crazyBG = new FlxSprite()` *(postCreate)*
- [sprite/décor] `crazyBG.loadGraphic(Paths.image("tex2"))` *(postCreate)*
- [autre] `crazyBG.cameras=[camfx]` *(postCreate)*
- [autre] `crazyBG.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `crazyBG.alpha = 0.1` *(postCreate)*
- [sprite/décor] `crazyBG.visible = false` *(postCreate)*
- [sprite/décor] `add(crazyBG)` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.7, 0.7)` *(postCreate)*

**Ignoré** (77)
- [sprite/décor] `white.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFffffff)`
- [HUD] `gffake = strumLines.members[0].characters[0]` *(postCreate)*
- [HUD] `pico = strumLines.members[2].characters[0]` *(postCreate)*
- [HUD] `gfStrum = strumLines.members[0]` *(postCreate)*
- [HUD] `bfStrum = strumLines.members[1]` *(postCreate)*
- [HUD] `picoStrum = strumLines.members[2]` *(postCreate)*
- [sprite/décor] `pico.y += 70` *(postCreate)*
- [personnage] `boyfriend.isPlayer = true` *(postCreate)*
- [sprite/décor] `pico.setPosition(-80,515)` *(postCreate)*
- [sprite/décor] `gffake.setPosition(-240,665)` *(postCreate)*
- [sprite/décor] `gffake.visible = false` *(postCreate)*
- [sprite/décor] `pico.visible = false` *(postCreate)*
- [personnage] `canvas1 = new FlxShapeCircle(boyfriend.x+20,boyfriend.y + 630,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas1)` *(postCreate)*
- [sprite/décor] `canvas1.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas1.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas1.alpha = 0.3` *(postCreate)*
- [sprite/décor] `canvas2 = new FlxShapeCircle(-210,1275,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas2)` *(postCreate)*
- [sprite/décor] `canvas2.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas2.scale.y = 0.4` *(postCreate)*
- [sprite/décor] `canvas2.alpha = 0.3` *(postCreate)*
- [sprite/décor] `canvas3 = new FlxShapeCircle(75,1160,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas3)` *(postCreate)*
- [sprite/décor] `canvas3.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas3.scrollFactor = pico.scrollFactor` *(postCreate)*
- [sprite/décor] `canvas3.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas3.alpha = 0.3` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [caméra] `eventbackground2.cameras = [camGame]` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `vignette=new FlxSprite().loadGraphic(Paths.image('stages/room/vignette'))` *(postCreate)*
- [autre] `vignette.screenCenter()` *(postCreate)*
- [autre] `vignette.cameras=[camfx]` *(postCreate)*
- [sprite/décor] `vignette.alpha=1` *(postCreate)*
- [sprite/décor] `vignette.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(vignette)` *(postCreate)*
- [autre] `crazyBG.antialiasing = true` *(postCreate)*
- [autre] `crazyBG.setGraphicSize(FlxG.width,FlxG.height)` *(postCreate)*
- [sprite/décor] `crazyBG.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `imgGrps = new FlxSpriteGroup(0,0)` *(postCreate)*
- [autre] `imgGrps.cameras = [camfx]` *(postCreate)*
- [sprite/décor] `add(imgGrps)` *(postCreate)*
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `chromaticGame = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromaticGame.chromaticAmount = 0.002` *(postCreate)*
- [shader] `illusion = new CustomShader("illusion")` *(postCreate)*
- [shader] `drunk = new CustomShader("itfake")` *(postCreate)*
- [shader] `crazyShad = new CustomShader("crazy")` *(postCreate)*
- [shader] `camGame.addShader(chromaticGame)` *(postCreate)*
- [shader] `crazyBG.shader = crazyShad` *(postCreate)*
- [shader] `camGame.addShader(idkshade)` *(postCreate)*
- [shader] `blur = new CustomShader("radialBlur")` *(postCreate)*
- [autre] `blur.cx = 0.5` *(postCreate)*
- [autre] `blur.cy = 0.5` *(postCreate)*
- [autre] `blur.blurWidth = 0.0` *(postCreate)*
- [shader] `camGame.addShader(blur)` *(postCreate)*
- [shader] `camHUD.addShader(blur)` *(postCreate)*
- [shader] `camNewHUD.addShader(blur)` *(postCreate)*
- [shader] `virusShader = new CustomShader('virus', "130")` *(postCreate)*
- [shader] `virusShader.uEnabled = false` *(postCreate)*
- [shader] `virusShader.screenSize = [1280, 720]` *(postCreate)*
- [shader] `virusShader.colorMultiplier = 1` *(postCreate)*
- [shader] `virusShader.colorTransform = true` *(postCreate)*
- [shader] `camGame.addShader(virusShader)` *(postCreate)*
- [shader] `camHUD.addShader(virusShader)` *(postCreate)*
- [shader] `camNewHUD.addShader(virusShader)` *(postCreate)*
- [caméra] `camZooming = false` *(postCreate)*

**Non suivi** (11)
- [sprite/décor] `gffake.visible = pico.visible = false` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `illusionPic = new FlxSprite(0,0)` *(postCreate)*
- [sprite/décor] `illusionPic.loadGraphic((Paths.image('illusion/'+i)))` *(postCreate)*
- [sprite/décor] `illusionPic.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `illusionPic.alpha = 0` *(postCreate)*
- [autre] `illusionPic.cameras = [camfx]` *(postCreate)*
- [autre] `illusionPic.setGraphicSize(FlxG.width, FlxG.height)` *(postCreate)*
- [autre] `illusionPic.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `imgGrps.add(illusionPic)` *(postCreate)*

**Pendant la chanson**
- `jumpscare` : sprite/décor 2, tween/timer 1
- `onPostCountdown` : sprite/décor 1
- `onCountdown` : sprite/décor 1, tween/timer 1
- `update` : sprite/décor 1, shader 1
- `bump` (appelée 111 fois par le chart) : caméra 1, HUD 1
- `hardBump` : caméra 1, HUD 1
- `onSongStart` : caméra 2, tween/timer 1, HUD 1
- `beatHit` : caméra 3, HUD 3
- `stepHit` : caméra 46, tween/timer 25, HUD 23, shader 14, sprite/décor 11, personnage 3
- `notebump` : HUD 84

### `songs/kowase/scripts/lyrics.hx`
**Ignoré** (10)
- [autre] `lyrics = new FunkinText(525, 547, 0, "YOUR MOM", 24, true)` *(postCreate)*
- [sprite/décor] `lyrics.alpha = 0` *(postCreate)*
- [HUD] `lyrics.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `background = new FlxSprite((FlxG.width / 2) - 1, lyrics.y - 5).makeGraphic(2, Std.int(lyrics.height + 10), 0xFF000000)` *(postCreate)*
- [sprite/décor] `background.alpha = 0` *(postCreate)*
- [autre] `background.updateHitbox()` *(postCreate)*
- [HUD] `background.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `background.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(background)` *(postCreate)*
- [sprite/décor] `add(lyrics)` *(postCreate)*

**Pendant la chanson**
- `popinLyrics` : sprite/décor 3, tween/timer 2
- `popoutLyrics` : tween/timer 2
- `stepHit` : sprite/décor 1

### `songs/kowase/cutscene.hx`
**Non suivi** (18)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37/2, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!) Basically! BF dying And now stuck in his own mind and give up everything! \n\n[Press Enter To Co`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [vidéo/cinématique] `PlayState.instance.inCutscene = false`
- [autre] `PlayState.instance.startCountdown()`

**Pendant la chanson**
- `update` : sprite/décor 3, vidéo/cinématique 1

## Your Hope (`your hope`)

Décor `tiny`, persos tiny, bf, 110 événements HScript Call

### `data/stages/tiny.hx`
**Appliqué** (7)
- [personnage] `var index = members.indexOf(boyfriend)`
- [sprite/décor] `white = new FlxSprite(-800, -800)`
- [sprite/décor] `white.scrollFactor.set(0, 0)`
- [autre] `white.blend = 1`
- [sprite/décor] `insert(index, white)`
- [personnage] `boyfriend.y += 70` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*

**Ignoré** (11)
- [sprite/décor] `white.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFffffff)`
- [personnage] `canvas1 = new FlxShapeCircle(boyfriend.x + 120,boyfriend.y + 630,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas1)` *(postCreate)*
- [sprite/décor] `canvas1.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas1.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas1.alpha = 0.3` *(postCreate)*
- [personnage] `canvas2 = new FlxShapeCircle(dad.x + 140,dad.y + 580,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas2)` *(postCreate)*
- [sprite/décor] `canvas2.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas2.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas2.alpha = 0.3` *(postCreate)*

### `songs/your hope/scripts/chart.hx`
**Appliqué** (1)
- [personnage] `dad.visible = false` *(postCreate)*

**Ignoré** (59)
- [sprite/décor] `bg = new FlxSprite(-FlxG.width, -FlxG.height).makeGraphic(1, 1, 0xFF000000)` *(postCreate)*
- [sprite/décor] `bg.scale.set(FlxG.width * 4, FlxG.height * 4)` *(postCreate)*
- [sprite/décor] `bg.scrollFactor.set()` *(postCreate)*
- [HUD] `bg.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `add(bg)` *(postCreate)*
- [sprite/décor] `vignette=new FlxSprite().loadGraphic(Paths.image('stages/room/vignette'))` *(postCreate)*
- [autre] `vignette.screenCenter()` *(postCreate)*
- [HUD] `vignette.cameras=[camHUD]` *(postCreate)*
- [sprite/décor] `vignette.alpha=0` *(postCreate)*
- [sprite/décor] `vignette.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(vignette)` *(postCreate)*
- [caméra] `FlxG.camera.zoom = 0.65` *(postCreate)*
- [shader] `shockwave = new FunkinShader(Assets.getText(Paths.fragShader("shockwave")))` *(postCreate)*
- [autre] `shockwave.enabled = true` *(postCreate)*
- [shader] `camGame.addShader(shockwave)` *(postCreate)*
- [shader] `fishZoom = new FunkinShader(Assets.getText(Paths.fragShader("fish")))` *(postCreate)*
- [shader] `camGame.addShader(fishZoom)` *(postCreate)*
- [shader] `camHUD.addShader(fishZoom)` *(postCreate)*
- [shader] `camNewHUD.addShader(fishZoom)` *(postCreate)*
- [autre] `fishZoom.distortionScale = 0` *(postCreate)*
- [caméra] `camFollow.setPosition(-100, -900)` *(postCreate)*
- [caméra] `camGame.scroll.set(-100,-900)` *(postCreate)*
- [shader] `glitch = new CustomShader('screenglitch')` *(postCreate)*
- [autre] `glitch.enabled = false` *(postCreate)*
- [shader] `camGame.addShader(glitch)` *(postCreate)*
- [shader] `camHUD.addShader(glitch)` *(postCreate)*
- [shader] `camNewHUD.addShader(glitch)` *(postCreate)*
- [autre] `cameffect = new FlxCamera()` *(postCreate)*
- [autre] `cameffect.bgColor = 0` *(postCreate)*
- [sprite/décor] `eventwhite = new FlxSprite(0,0).makeGraphic(1280,720, 0xFFFFFFFF)` *(postCreate)*
- [sprite/décor] `eventwhite.alpha = 0` *(postCreate)*
- [autre] `eventwhite.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventwhite.scale.set(4,4)` *(postCreate)*
- [autre] `eventwhite.screenCenter()` *(postCreate)*
- [sprite/décor] `eventwhite.scrollFactor.set()` *(postCreate)*
- [autre] `eventwhite.cameras = [cameffect]` *(postCreate)*
- [sprite/décor] `add(eventwhite)` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(cameffect, false)` *(postCreate)*
- [sprite/décor] `bleh = new FlxSprite(0,0).loadGraphic(Paths.image('flash/'+FlxG.random.int(0,8)))` *(postCreate)*
- [sprite/décor] `bleh.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `bleh.alpha = 0` *(postCreate)*
- [autre] `bleh.cameras = [camfx]` *(postCreate)*
- [autre] `bleh.setGraphicSize(FlxG.width, FlxG.height)` *(postCreate)*
- [autre] `bleh.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(bleh)` *(postCreate)*
- [autre] `coolText = new FunkinText(0,0,0,'', 26, true)` *(postCreate)*
- [autre] `coolText.screenCenter()` *(postCreate)*
- [autre] `coolText.antialiasing = true` *(postCreate)*
- [autre] `coolText.borderSize = 3` *(postCreate)*
- [autre] `coolText.font = Paths.font("freeplay.ttf")` *(postCreate)*
- [autre] `coolText.cameras = [camfx]` *(postCreate)*
- [sprite/décor] `add(coolText)` *(postCreate)*

**Non suivi** (2)
- [sprite/décor] `bleh.loadGraphic((Paths.image('flash/'+i)))` *(postCreate)*
- [autre] `FlxG.save.data.bfmode = true` *(postCreate)*

**Pendant la chanson**
- `flashback` (appelée 9 fois par le chart) : sprite/décor 2, tween/timer 1
- `textup` (appelée 16 fois par le chart) : sprite/décor 1, tween/timer 1
- `onSongStart` : tween/timer 1, caméra 1, sprite/décor 1
- `update` : caméra 2
- `onPostGenerateStrums` : sprite/décor 1
- `hitGlitch` (appelée 13 fois par le chart) : tween/timer 1
- `hardKick` (appelée 50 fois par le chart) : tween/timer 1
- `beatHit` : caméra 6, HUD 6, tween/timer 2
- `forceZoom` (appelée 22 fois par le chart) : caméra 3
- `stepHit` : caméra 11, HUD 3, tween/timer 2
- `tinyPopup` : sprite/décor 8, caméra 2, tween/timer 2, personnage 1

## our hope (`our hope`)

Décor `shadow`, persos shadow taeyai, bf, taescare, **4 ligne(s) de notes en plus** (persos non affichés), 18 événements HScript Call

### `data/stages/shadow.hx`
**Appliqué** (8)
- [personnage] `var index = members.indexOf(dad)`
- [sprite/décor] `light = new FlxSprite(-1080, -865).loadGraphic(Paths.image('stages/shadow/light'))`
- [sprite/décor] `light.scale.set(1.5,1.5)`
- [sprite/décor] `add(light)`
- [personnage] `dad.x = -840` *(postCreate)*
- [personnage] `dad.y = -210` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.4, 0.4)` *(postCreate)*

**Ignoré** (25)
- [personnage] `rebf = new Character(dad.x, dad.y, 'bf', false)`
- [autre] `rebf.flipX = false`
- [autre] `rebf.flipY = true`
- [sprite/décor] `rebf.y = 450`
- [sprite/décor] `rebf.x = 891`
- [sprite/décor] `rebf.skew.x = 36`
- [autre] `rebf.blend = 0`
- [sprite/décor] `rebf.color = FlxColor.RED`
- [sprite/décor] `rebf.alpha = 0.1`
- [sprite/décor] `insert(index, rebf)`
- [sprite/décor] `light.alpha = 1`
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))`
- [shader] `camGame.addShader(idkshade)`
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(-300,110)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [3, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Non suivi** (2)
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)`
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*

**Pendant la chanson**
- `onPostNoteCreation` : sprite/décor 1
- `onCountdown` : caméra 1
- `postUpdate` : caméra 3, personnage 2, HUD 1
- `stepHit` : personnage 5, tween/timer 4, sprite/décor 2, caméra 2
- `addGameShader` : shader 2

### `songs/our hope/scripts/events.hx`
**Appliqué** (3)
- [sprite/décor] `bf.color = FlxColor.fromHSL(bf.color.hue, bf.color.saturation, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `bf.setPosition(540,-90)` *(postCreate)*
- [caméra] `defaultCamZoom = 0.6` *(postCreate)*

**Ignoré** (202)
- [HUD] `bf = strumLines.members[1].characters[0]` *(postCreate)*
- [HUD] `taeyai = strumLines.members[2].characters[0]` *(postCreate)*
- [HUD] `conner = strumLines.members[3].characters[0]` *(postCreate)*
- [HUD] `daisy = strumLines.members[4].characters[0]` *(postCreate)*
- [HUD] `futo = strumLines.members[5].characters[0]` *(postCreate)*
- [HUD] `pico = strumLines.members[6].characters[0]` *(postCreate)*
- [HUD] `shaStrum = strumLines.members[0]` *(postCreate)*
- [HUD] `bfStrum = strumLines.members[1]` *(postCreate)*
- [HUD] `taeStrum = strumLines.members[2]` *(postCreate)*
- [HUD] `connerStrum = strumLines.members[3]` *(postCreate)*
- [HUD] `daisyStrum = strumLines.members[4]` *(postCreate)*
- [HUD] `futoStrum = strumLines.members[5]` *(postCreate)*
- [HUD] `picoStrum = strumLines.members[6]` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `conner.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `conner.shader.color = [3, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `conner.shader.shadowLength = 35 * ((3 - i) / 2)` *(postCreate)*
- [shader] `conner.shader.flipped = false` *(postCreate)*
- [shader] `daisy.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `daisy.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `daisy.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `daisy.shader.flipped = true` *(postCreate)*
- [shader] `futo.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `futo.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `futo.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `futo.shader.flipped = false` *(postCreate)*
- [shader] `bf.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `bf.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `bf.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `bf.shader.flipped = true` *(postCreate)*
- [shader] `taeyai.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `taeyai.shader.color = [2, 249 / 255, 0.5, 0.33 * ((3 - i) / 3)]` *(postCreate)*
- [shader] `taeyai.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `taeyai.shader.flipped = true` *(postCreate)*
- [sprite/décor] `conner.color = FlxColor.fromHSL(conner.color.hue, conner.color.saturation, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `daisy.color = FlxColor.fromHSL(daisy.color.hue, daisy.color.saturation, 0.7, 0.7)` *(postCreate)*
- [sprite/décor] `futo.color = FlxColor.fromHSL(futo.color.hue, futo.color.saturation, 0.4, 0.4)` *(postCreate)*
- [personnage] `taeyai.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `daisy.x = -1705` *(postCreate)*
- [sprite/décor] `daisy.y = -300` *(postCreate)*
- [sprite/décor] `conner.setPosition(-570,105)` *(postCreate)*
- [sprite/décor] `futo.setPosition(-1280,-100)` *(postCreate)*
- [sprite/décor] `futo.visible = false` *(postCreate)*
- [sprite/décor] `conner.visible = false` *(postCreate)*
- [sprite/décor] `daisy.visible = false` *(postCreate)*
- [sprite/décor] `taeyai.y += 100` *(postCreate)*
- [sprite/décor] `add(fixedCamFollow)` *(postCreate)*
- [caméra] `camGame.alpha = 0` *(postCreate)*
- [HUD] `camNewHUD.alpha = 0` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 0` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*
- [shader] `blur = new CustomShader("radialBlur")` *(postCreate)*
- [autre] `blur.cx = 0.5` *(postCreate)*
- [autre] `blur.cy = 0.5` *(postCreate)*
- [autre] `blur.blurWidth = 0.0` *(postCreate)*
- [shader] `chromaticGame = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromaticGame.chromaticAmount = 0.003` *(postCreate)*
- [shader] `chromatic = new CustomShader("stupid")` *(postCreate)*
- [autre] `chromatic.chromaticAmount = 0.001` *(postCreate)*
- [shader] `camGame.addShader(blur)` *(postCreate)*
- [shader] `camHUD.addShader(blur)` *(postCreate)*
- [shader] `camNewHUD.addShader(blur)` *(postCreate)*
- [shader] `camGame.addShader(chromaticGame)` *(postCreate)*
- [shader] `camHUD.addShader(chromatic)` *(postCreate)*
- [shader] `camNewHUD.addShader(chromatic)` *(postCreate)*
- [HUD] `var index = members.indexOf(strumLines.members[0].characters[0])` *(postCreate)*
- [sprite/décor] `door = new FlxSprite(-1470 ,-895 ).loadGraphic(Paths.image('stages/shadow/door'))` *(postCreate)*
- [sprite/décor] `door.scale.set(1.5,1.5)` *(postCreate)*
- [sprite/décor] `insert(index, door)` *(postCreate)*
- [sprite/décor] `portal = new FlxSprite(-1140 ,-1200 ).loadGraphic(Paths.image("stages/shadow/portal"))` *(postCreate)*
- [sprite/décor] `portal.frames = Paths.getSparrowAtlas('stages/shadow/portal')` *(postCreate)*
- [autre] `portal.antialiasing = true` *(postCreate)*
- [autre] `portal.blend = 0` *(postCreate)*
- [sprite/décor] `portal.color = FlxColor.fromHSL(portal.color.hue, portal.color.saturation, 0.7, 0.7)` *(postCreate)*
- [sprite/décor] `portal.scrollFactor.set(0.7,0.7)` *(postCreate)*
- [sprite/décor] `portal.animation.addByPrefix('idle', "frame_0", 24,true)` *(postCreate)*
- [sprite/décor] `portal.animation.play('idle',true)` *(postCreate)*
- [sprite/décor] `portal.visible = false` *(postCreate)*
- [sprite/décor] `insert(index, portal)` *(postCreate)*
- [sprite/décor] `clown1 = new FlxSprite(-1470 ,-895 ).loadGraphic(Paths.image('stages/shadow/clown2'))` *(postCreate)*
- [sprite/décor] `insert(index, clown1)` *(postCreate)*
- [sprite/décor] `clownf = new FlxSprite(-1400 ,-1055 ).loadGraphic(Paths.image("stages/shadow/flame"))` *(postCreate)*
- [sprite/décor] `clownf.frames = Paths.getSparrowAtlas('stages/shadow/flame')` *(postCreate)*
- [autre] `clownf.antialiasing = true` *(postCreate)*
- [sprite/décor] `clownf.scrollFactor.set(0.7,0.7)` *(postCreate)*
- [sprite/décor] `clownf.animation.addByPrefix('idle', "f", 24,true)` *(postCreate)*
- [sprite/décor] `clownf.animation.play('idle',true)` *(postCreate)*
- [sprite/décor] `insert(index, clownf)` *(postCreate)*
- [sprite/décor] `clown0 = new FlxSprite(-1310 ,-1010).loadGraphic(Paths.image('stages/shadow/clown1'))` *(postCreate)*
- [sprite/décor] `insert(index, clown0)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `fire = new FlxSprite(-1510 ,-300 ).loadGraphic(Paths.image("stages/shadow/fire"))` *(postCreate)*
- [sprite/décor] `fire.frames = Paths.getSparrowAtlas('stages/shadow/fire')` *(postCreate)*
- [autre] `fire.antialiasing = true` *(postCreate)*
- [sprite/décor] `fire.scrollFactor.set(0.7,0.7)` *(postCreate)*
- [sprite/décor] `fire.animation.addByPrefix('idle', "FIREEE instance 1", 24,true)` *(postCreate)*
- [sprite/décor] `fire.animation.play('idle',true)` *(postCreate)*
- [sprite/décor] `insert(index, fire)` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [sprite/décor] `fire.visible = false` *(postCreate)*
- [sprite/décor] `eventbackground.visible = false` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [autre] `camChar = new FlxCamera()` *(postCreate)*
- [autre] `camChar.bgColor = 0` *(postCreate)*
- [caméra] `camChar.zoom = 1.4` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camChar, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [personnage] `daiclone = new Character(-640 ,180 ,'daisy-wrath',false)` *(postCreate)*
- [autre] `daiclone.flipX = false` *(postCreate)*
- [autre] `daiclone.cameras = [camChar]` *(postCreate)*
- [sprite/décor] `add(daiclone)` *(postCreate)*
- [personnage] `taeclone = new Character(645 ,155 ,'taescare',taeyai.isPlayer)` *(postCreate)*
- [autre] `taeclone.cameras = [camChar]` *(postCreate)*
- [sprite/décor] `add(taeclone)` *(postCreate)*
- [personnage] `bfclone = new Character(670 ,-60 ,boyfriend.curCharacter,boyfriend.isPlayer)` *(postCreate)*
- [autre] `bfclone.cameras = [camChar]` *(postCreate)*
- [sprite/décor] `add(bfclone)` *(postCreate)*
- [sprite/décor] `daiclone.visible = false` *(postCreate)*
- [sprite/décor] `taeclone.visible = false` *(postCreate)*
- [sprite/décor] `bfclone.visible = false` *(postCreate)*
- [sprite/décor] `camChar.visible = false` *(postCreate)*
- [sprite/décor] `daiclone.alpha = 0` *(postCreate)*
- [sprite/décor] `taeclone.alpha = 0` *(postCreate)*
- [sprite/décor] `bfclone.alpha = 0` *(postCreate)*
- [sprite/décor] `camChar.alpha = 0` *(postCreate)*
- [autre] `coolText = new FunkinText(0,0,0,'CHAOS', 64, true)` *(postCreate)*
- [autre] `coolText.antialiasing = true` *(postCreate)*
- [autre] `coolText.font = Paths.font("seriouss.ttf")` *(postCreate)*
- [autre] `coolText.cameras = [camfx]` *(postCreate)*
- [sprite/décor] `coolText.color = 0xFFf5675b` *(postCreate)*
- [autre] `coolText.screenCenter()` *(postCreate)*
- [sprite/décor] `add(coolText)` *(postCreate)*
- [sprite/décor] `scary = new FlxSprite( -218.5 , -13).loadGraphic(Paths.image('stages/shadow/scary2'))` *(postCreate)*
- [autre] `scary.cameras = [camfx]` *(postCreate)*
- [sprite/décor] `scary.alpha = 0` *(postCreate)*
- [sprite/décor] `scary.scale.set(1.5,1.5)` *(postCreate)*
- [sprite/décor] `add(scary)` *(postCreate)*
- [sprite/décor] `white = new FlxSprite(-800, -800)` *(postCreate)*
- [sprite/décor] `white.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFffffff)` *(postCreate)*
- [sprite/décor] `white.scrollFactor.set(0, 0)` *(postCreate)*
- [autre] `white.blend = 1` *(postCreate)*
- [personnage] `gffake = new FlxSprite(dad.x ,dad.y).loadGraphic(Paths.image("characters/gffake"))` *(postCreate)*
- [sprite/décor] `gffake.frames = Paths.getSparrowAtlas('characters/gffake')` *(postCreate)*
- [autre] `gffake.antialiasing = true` *(postCreate)*
- [personnage] `gffake.animation.addByPrefix('idle', "gf idle0", 24,false)` *(postCreate)*
- [sprite/décor] `gffake.animation.play('idle',true)` *(postCreate)*
- [sprite/décor] `add(gffake)` *(postCreate)*
- [sprite/décor] `insert(index, white)` *(postCreate)*
- [sprite/décor] `pico.setPosition(-795 ,-410 )` *(postCreate)*
- [sprite/décor] `gffake.setPosition(-935 ,-25 )` *(postCreate)*
- [sprite/décor] `gffake.color = FlxColor.fromHSL(gffake.color.hue, gffake.color.saturation, 0.6, 0.6)` *(postCreate)*
- [sprite/décor] `pico.color = FlxColor.fromHSL(pico.color.hue, pico.color.saturation, 0.6, 0.6)` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `canvas1 = new FlxShapeCircle(665 ,305 ,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas1)` *(postCreate)*
- [sprite/décor] `canvas1.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas1.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas1.alpha = 0.7` *(postCreate)*
- [autre] `var index = members.indexOf(pico)` *(postCreate)*
- [sprite/décor] `canvas2 = new FlxShapeCircle(-885 ,350 ,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas2)` *(postCreate)*
- [sprite/décor] `canvas2.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas2.scale.y = 0.4` *(postCreate)*
- [sprite/décor] `canvas2.alpha = 0.7` *(postCreate)*
- [sprite/décor] `canvas3 = new FlxShapeCircle(-655 ,230 ,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas3)` *(postCreate)*
- [sprite/décor] `canvas3.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas3.scrollFactor = pico.scrollFactor` *(postCreate)*
- [sprite/décor] `canvas3.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas3.alpha = 0.7` *(postCreate)*
- [sprite/décor] `pico.visible = false` *(postCreate)*
- [sprite/décor] `gffake.visible = false` *(postCreate)*
- [sprite/décor] `canvas1.visible = false` *(postCreate)*
- [sprite/décor] `canvas2.visible = false` *(postCreate)*
- [sprite/décor] `canvas3.visible = false` *(postCreate)*
- [sprite/décor] `white.visible = false` *(postCreate)*
- [shader] `glitch = new CustomShader('screenglitch')` *(postCreate)*
- [autre] `glitch.enabled = true` *(postCreate)*
- [shader] `portal.shader = glitch` *(postCreate)*
- [shader] `camfx.addShader(glitch)` *(postCreate)*
- [shader] `drunk = new CustomShader("itfake")` *(postCreate)*
- [shader] `camChar.addShader(drunk)` *(postCreate)*
- [shader] `bloomShader = new FunkinShader(Assets.getText(Paths.fragShader("bloom")))` *(postCreate)*
- [shader] `camChar.addShader(bloomShader)` *(postCreate)*
- [sprite/décor] `clown1.visible = false` *(postCreate)*
- [sprite/décor] `clownf.visible = false` *(postCreate)*
- [sprite/décor] `clown0.visible = false` *(postCreate)*
- [sprite/décor] `coolText.visible = false` *(postCreate)*
- [caméra] `camZooming = true` *(postCreate)*

**Non suivi** (20)
- [sprite/décor] `futo.visible = conner.visible = daisy.visible = false` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `s.color = FlxColor.fromHSL(3, 0.3, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [autre] `charname = 'talia'` *(postCreate)*
- [HUD] `strumLines.members[1].cpu = true` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [HUD] `strumLines.members[2].cpu = true` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*
- [sprite/décor] `s.color = FlxColor.fromHSL(298, 0.3, 0.4, 0.4)` *(postCreate)*
- [sprite/décor] `s.color = FlxColor.fromHSL(s.color.hue, 0.9, 0.4, 0.4)` *(postCreate)*
- [HUD] `camGame.alpha = camNewHUD.alpha = 0` *(postCreate)*
- [sprite/décor] `fire.visible = eventbackground.visible = false` *(postCreate)*
- [sprite/décor] `daiclone.visible = taeclone.visible = bfclone.visible = camChar.visible = false` *(postCreate)*
- [sprite/décor] `daiclone.alpha = taeclone.alpha = bfclone.alpha = camChar.alpha = 0` *(postCreate)*
- [sprite/décor] `pico.visible = gffake.visible = canvas1.visible = canvas2.visible = canvas3.visible = white.visible = false` *(postCreate)*
- [sprite/décor] `clown1.visible = clownf.visible = clown0.visible = coolText.visible = false` *(postCreate)*

**Pendant la chanson**
- `onPostNoteCreation` : sprite/décor 5
- `postUpdate` : sprite/décor 2, HUD 2, caméra 1
- `update` : shader 1
- `bump` : caméra 1, HUD 1
- `hardbump` (appelée 18 fois par le chart) : caméra 1, HUD 1
- `onPostCountdown` : caméra 2, sprite/décor 1, HUD 1
- `stepHit` : caméra 58, sprite/décor 38, HUD 24, tween/timer 17, shader 5, personnage 2
- `beatHit` : HUD 4, caméra 3, sprite/décor 1

### `songs/our hope/cutscene.hx`
**Non suivi** (18)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37/2, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!) Basically! Talia dying but luckily that Tiny helped BF out in time and now BF distract the Error`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [vidéo/cinématique] `PlayState.instance.inCutscene = false`
- [autre] `PlayState.instance.startCountdown()`

**Pendant la chanson**
- `update` : sprite/décor 3, vidéo/cinématique 1

### `songs/our hope/end-cutscene.hx`
**Non suivi** (17)
- [vidéo/cinématique] `PlayState.instance.inCutscene = true`
- [autre] `cutCamera = new FlxCamera()`
- [autre] `cutCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(cutCamera, false)`
- [sprite/décor] `bgsub = new FlxSprite(0,0).makeGraphic(FlxG.width * 2.85, FlxG.height * 2.85, 0xFF000000)`
- [autre] `bgsub.cameras = [cutCamera]`
- [sprite/décor] `bgsub.visible = true`
- [sprite/décor] `add(bgsub)`
- [autre] `longshort = new FunkinText(525, 547, 0, "BOTPLAY", 37/2, true)`
- [sprite/décor] `longshort.visible = true`
- [autre] `longshort.alignment = 'center'`
- [autre] `longshort.cameras = [cutCamera]`
- [sprite/décor] `add(longshort)`
- [autre] `longshort.text = "(Long Story Short Time!!) Basically! Shadow Talia is an old Talia’s AI that has been deleted thought her mistake while dea`
- [autre] `longshort.screenCenter(FlxAxes.XY)`
- [tween/timer] `new FlxTimer().start(1, function(tmr:FlxTimer) { skippable = true; })`
- [autre] `PlayState.instance.nextSong()`

**Pendant la chanson**
- `update` : tween/timer 1

## No More Sense (IM DEAD) (`no more sense-im dead`)

Décor `none`, persos nondead, taeyai, nonsense, **1 ligne(s) de notes en plus** (persos non affichés)

### `data/stages/none.hx`
**Appliqué** (4)
- [personnage] `gf.visible = false`
- [personnage] `dad.cameraOffset.x -= 100`
- [caméra] `defaultCamZoom = 0.5`
- [personnage] `boyfriend.visible = false`

**Ignoré** (2)
- [personnage] `dad.playAnim('enter')`
- [HUD] `cpuStrums.forEach(function(spr:FlxSprite) { spr.alpha = 0; spr.x -= FlxG.width; })` *(postCreate)*

### `songs/no more sense/scripts/events.hx`
**Ignoré** (40)
- [autre] `videoCamera = new FlxCamera()`
- [autre] `videoCamera.bgColor = 0`
- [caméra] `FlxG.cameras.add(videoCamera, false)`
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)`
- [autre] `video.cameras = [videoCamera]`
- [sprite/décor] `add(video)`
- [autre] `cameraMovementEnabled = true` *(postCreate)*
- [sprite/décor] `bell = new FlxSprite(0, 0).loadGraphic(Paths.image('stages/nomore/bell'))` *(postCreate)*
- [sprite/décor] `bell.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `bell.alpha = 0` *(postCreate)*
- [autre] `bell.screenCenter(FlxAxes.XY)` *(postCreate)*
- [HUD] `bell.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `add(bell)` *(postCreate)*
- [sprite/décor] `scream = new FlxSprite(0, 0).loadGraphic(Paths.image('stages/nomore/tom'))` *(postCreate)*
- [sprite/décor] `scream.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `scream.alpha = 0` *(postCreate)*
- [autre] `scream.screenCenter(FlxAxes.XY)` *(postCreate)*
- [HUD] `scream.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `add(scream)` *(postCreate)*
- [sprite/décor] `neko = new FlxSprite(0, 0).loadGraphic(Paths.image('stages/nomore/neko'))` *(postCreate)*
- [sprite/décor] `neko.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `neko.alpha = 0` *(postCreate)*
- [sprite/décor] `neko.scale.set(0.3,0.3)` *(postCreate)*
- [autre] `neko.screenCenter(FlxAxes.XY)` *(postCreate)*
- [HUD] `neko.cameras = [camHUD]` *(postCreate)*
- [sprite/décor] `add(neko)` *(postCreate)*
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].setPosition(100,200)` *(postCreate)*
- [HUD] `strumLines.members[3].characters[0].setPosition(1100,100)` *(postCreate)*
- [HUD] `strumLines.members[3].characters[0].visible = false` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].visible = false` *(postCreate)*
- [sprite/décor] `outside = new FlxSprite(-600, 40).loadGraphic(Paths.image('stages/nonsense/Outside'))` *(postCreate)*
- [sprite/décor] `outside.scrollFactor.set(0.8, 0.8)` *(postCreate)*
- [sprite/décor] `inside = new FlxSprite(-800, -370).loadGraphic(Paths.image('stages/nonsense/BACKGROUND'))` *(postCreate)*
- [sprite/décor] `inside.scrollFactor.set(0.9, 0.9)` *(postCreate)*
- [sprite/décor] `inside.alpha = 0` *(postCreate)*
- [sprite/décor] `outside.alpha = 0` *(postCreate)*
- [sprite/décor] `insert(index, inside)` *(postCreate)*
- [sprite/décor] `insert(index, outside)` *(postCreate)*
- [shader] `monochrome = new CustomShader("monochrome")` *(postCreate)*

**Non suivi** (3)
- [sprite/décor] `inside.alpha = outside.alpha = 0` *(postCreate)*
- [sprite/décor] `s.visible = false` *(postCreate)*
- [sprite/décor] `s.visible = false` *(postCreate)*

**Pendant la chanson**
- `screaming` : sprite/décor 5, HUD 1, tween/timer 1
- `ring` : sprite/décor 1, tween/timer 1
- `videomidsong` : tween/timer 1
- `postUpdate` : HUD 1
- `stepHit` : sprite/décor 14, tween/timer 8, HUD 8, shader 6, caméra 3, personnage 2

## Data Diva (`data diva`)

Décor `monika`, persos monika, veethree, 23 événements HScript Call

### `data/stages/monika.hx`
**Appliqué** (22)
- [personnage] `var index = members.indexOf(boyfriend)`
- [sprite/décor] `bg = new FlxBackdrop(Paths.image('stages/monika/bg'), FlxAxes.XY,-80,-20)`
- [sprite/décor] `bg.scale.set(4,4)`
- [sprite/décor] `bg.velocity.set(-150,150)`
- [sprite/décor] `bg2 = new FlxBackdrop(Paths.image('stages/monika/bg'), FlxAxes.X,FlxAxes.Y)`
- [sprite/décor] `bg2.scale.set(4,4)`
- [sprite/décor] `bg2.velocity.set(-150,150)`
- [sprite/décor] `insert(index, bg)`
- [personnage] `var index = members.indexOf(boyfriend)`
- [sprite/décor] `floor = new FunkinSprite(-280, -310).loadGraphic(Paths.image('stages/monika/floor'))`
- [sprite/décor] `floor.scale.set(3,3)`
- [autre] `floor.screenCenter(FlxAxes.XY)`
- [sprite/décor] `floor.y += 600`
- [sprite/décor] `insert(index, floor)`
- [personnage] `boyfriend.x += 330`
- [personnage] `boyfriend.y -= 250`
- [personnage] `dad.x -= 460`
- [personnage] `dad.y -= 40`
- [personnage] `dad.scrollFactor.set(1.3,1.3)`
- [personnage] `boyfriend.scrollFactor.set(1.3,1.3)`
- [sprite/décor] `floor.scrollFactor.set(1.3,1.3)`
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*

**Ignoré** (12)
- [personnage] `canvas1 = new FlxShapeCircle(boyfriend.x + 150,boyfriend.y + 580,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas1)` *(postCreate)*
- [sprite/décor] `canvas1.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas1.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas1.alpha = 0.2` *(postCreate)*
- [personnage] `canvas2 = new FlxShapeCircle(dad.x + 200,dad.y + 300,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas2)` *(postCreate)*
- [sprite/décor] `canvas2.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas2.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas2.alpha = 0.2` *(postCreate)*
- [sprite/décor] `canvas1.scrollFactor.set(1.3,1.3)` *(postCreate)*
- [sprite/décor] `canvas2.scrollFactor.set(1.3,1.3)` *(postCreate)*

**Pendant la chanson**
- `bump` (appelée 10 fois par le chart) : caméra 1, HUD 1
- `flash` (appelée 11 fois par le chart) : HUD 1
- `beatHit` : caméra 2, HUD 2
- `postUpdate` : caméra 1

## That Me! (`thats me`)

Décor `kemo`, persos kemotae, taeyaitps, 97 événements HScript Call

### `data/stages/kemo.hx`
**Appliqué** (8)
- [personnage] `boyfriend.cameraOffset.x -= 200` *(postCreate)*
- [personnage] `boyfriend.cameraOffset.y -= 60` *(postCreate)*
- [personnage] `dad.y += 440` *(postCreate)*
- [personnage] `dad.x += 240` *(postCreate)*
- [personnage] `boyfriend.x += 400` *(postCreate)*
- [personnage] `boyfriend.y += 440` *(postCreate)*
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.7, 0.7)` *(postCreate)*

**Ignoré** (14)
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(253,310)` *(postCreate)*
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camGame.addShader(idkshade)` *(postCreate)*
- [autre] `var i = 0` *(postCreate)*
- [autre] `i++` *(postCreate)*
- [shader] `dad.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `dad.shader.color = [-4, 249 / 255,1.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `dad.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `dad.shader.flipped = false` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader("garage-shadow")` *(postCreate)*
- [shader] `boyfriend.shader.color = [-3, 249 / 255,1.5, 0.33 * ((3 - i) / 2)]` *(postCreate)*
- [shader] `boyfriend.shader.shadowLength = 25 * ((3 - i) / 2)` *(postCreate)*
- [shader] `boyfriend.shader.flipped = true` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : caméra 1

### `songs/thats me/scripts/events.hx`
**Ignoré** (22)
- [personnage] `var index = members.indexOf(dad)` *(postCreate)*
- [sprite/décor] `eventbackground = new FlxSprite(0,0).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [sprite/décor] `eventbackground.alpha = 0` *(postCreate)*
- [autre] `eventbackground.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground.scale.set(4,4)` *(postCreate)*
- [autre] `eventbackground.screenCenter(FlxAxes.X)` *(postCreate)*
- [sprite/décor] `eventbackground.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `insert(index, eventbackground)` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `bleh = new FlxSprite(0,0).loadGraphic(Paths.image('cat/cat'+FlxG.random.int(0,4)))` *(postCreate)*
- [sprite/décor] `bleh.scrollFactor.set(0, 0)` *(postCreate)*
- [sprite/décor] `bleh.alpha = 0` *(postCreate)*
- [autre] `bleh.cameras = [camfx]` *(postCreate)*
- [autre] `bleh.setGraphicSize(FlxG.width, FlxG.height)` *(postCreate)*
- [autre] `bleh.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(bleh)` *(postCreate)*

**Pendant la chanson**
- `meow` (appelée 11 fois par le chart) : sprite/décor 4, tween/timer 1
- `bump` (appelée 86 fois par le chart) : caméra 1, HUD 1
- `onSongStart` : personnage 1
- `stepHit` : HUD 8, caméra 4, tween/timer 1, sprite/décor 1
- `beatHit` : caméra 5, HUD 5

## Cyberfunk Attack (`cyberfunk attack`)

Décor `funk`, persos tofu, Spyrodile, fahsai, **1 ligne(s) de notes en plus** (persos non affichés)

### `data/stages/funk.hx`
**Appliqué** (21)
- [HUD] `var index = members.indexOf(strumLines.members[2].characters[0])`
- [sprite/décor] `white = new FlxSprite(-800, -800)`
- [sprite/décor] `white.scrollFactor.set(0, 0)`
- [autre] `white.blend = 1`
- [sprite/décor] `insert(index, white)`
- [HUD] `var index = members.indexOf(strumLines.members[2].characters[0])`
- [sprite/décor] `nerd = new FlxSprite(950,905).loadGraphic(Paths.image("characters/the_nerd_one"))`
- [sprite/décor] `nerd.frames = Paths.getSparrowAtlas('characters/the_nerd_one')`
- [sprite/décor] `nerd.scale.set(0.4,0.4)`
- [sprite/décor] `nerd.alpha = 0.4`
- [autre] `nerd.flipX = true`
- [sprite/décor] `insert(index, nerd)`
- [HUD] `var index = members.indexOf(strumLines.members[0].characters[0])`
- [sprite/décor] `stamps = new FlxSprite(295 ,-110).loadGraphic(Paths.image('stamps'))`
- [sprite/décor] `stamps.scale.set(0.2,0.2)`
- [sprite/décor] `stamps.alpha = 0.4`
- [sprite/décor] `insert(index, stamps)`
- [sprite/décor] `mag = new FlxSprite(265,370).loadGraphic(Paths.image('magg'))`
- [sprite/décor] `mag.scale.set(0.7,0.7)`
- [sprite/décor] `mag.alpha = 0.4`
- [sprite/décor] `insert(index, mag)`

**Ignoré** (12)
- [sprite/décor] `white.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFffffff)`
- [autre] `nerd.antialiasing = true`
- [sprite/décor] `nerd.animation.addByPrefix('idle', "the nerd one down0", 24,false)`
- [sprite/décor] `stamps.scrollFactor.set(1,1)`
- [sprite/décor] `stamps.origin.y = 1330`
- [sprite/décor] `mag.scrollFactor.set(1,1)`
- [sprite/décor] `mag.origin.y = 840`
- [HUD] `strumLines.members[0].characters[0].setPosition(40,545)` *(postCreate)*
- [HUD] `strumLines.members[1].characters[0].setPosition(-255,800)` *(postCreate)*
- [HUD] `strumLines.members[2].characters[0].setPosition(1125,625)` *(postCreate)*
- [HUD] `strumLines.members[3].characters[0].setPosition(1430,715)` *(postCreate)*
- [caméra] `FlxG.camera.followLerp = 99` *(postCreate)*

**Pendant la chanson**
- `beatHit` : sprite/décor 5, caméra 5, HUD 5
- `update` : HUD 3, sprite/décor 1
- `stepHit` : HUD 21, caméra 18, tween/timer 12, sprite/décor 12

## Sweet Love (`sweet love`)

Décor `shiver`, persos shiver, veethree, 10 événements HScript Call

### `data/stages/shiver.hx`
**Appliqué** (15)
- [personnage] `var index = members.indexOf(boyfriend)`
- [sprite/décor] `white = new FlxSprite(-800, -800)`
- [sprite/décor] `white.scrollFactor.set(0, 0)`
- [autre] `white.blend = 1`
- [sprite/décor] `insert(index, white)`
- [personnage] `boyfriend.y += 90` *(postCreate)*
- [personnage] `dad.x -= 400` *(postCreate)*
- [personnage] `dad.y -= 60` *(postCreate)*
- [personnage] `var index = members.indexOf(boyfriend)` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.X)` *(postCreate)*
- [autre] `eventbackground2.cameras=[camfx]` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*

**Ignoré** (50)
- [sprite/décor] `white.makeGraphic(FlxG.width * 2.3, FlxG.width * 2.3, 0xFFffffff)`
- [personnage] `canvas1 = new FlxShapeCircle(boyfriend.x + 120,dad.y + 640,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas1)` *(postCreate)*
- [sprite/décor] `canvas1.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas1.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas1.alpha = 0.3` *(postCreate)*
- [personnage] `canvas2 = new FlxShapeCircle(dad.x + 110,dad.y + 640,100,lineStyle,FlxColor.BLACK)` *(postCreate)*
- [sprite/décor] `insert(index,canvas2)` *(postCreate)*
- [sprite/décor] `canvas2.scale.x = 2` *(postCreate)*
- [sprite/décor] `canvas2.scale.y = 0.5` *(postCreate)*
- [sprite/décor] `canvas2.alpha = 0.3` *(postCreate)*
- [sprite/décor] `fyre=new FlxSprite().loadGraphic(Paths.image('fyre'))` *(postCreate)*
- [sprite/décor] `fyre.setPosition(-294 ,70)` *(postCreate)*
- [sprite/décor] `fyre.alpha=0.4` *(postCreate)*
- [sprite/décor] `fyre.scale.set(0.4,0.4)` *(postCreate)*
- [sprite/décor] `fyre.scrollFactor.set(0.7,0.7)` *(postCreate)*
- [sprite/décor] `insert(index,fyre)` *(postCreate)*
- [sprite/décor] `bigman=new FlxSprite().loadGraphic(Paths.image('bigman'))` *(postCreate)*
- [sprite/décor] `bigman.setPosition(236 ,205 )` *(postCreate)*
- [sprite/décor] `bigman.alpha=0.4` *(postCreate)*
- [sprite/décor] `bigman.scale.set(0.7,0.7)` *(postCreate)*
- [sprite/décor] `bigman.scrollFactor.set(0.7,0.7)` *(postCreate)*
- [sprite/décor] `insert(index,bigman)` *(postCreate)*
- [shader] `blur = new CustomShader("radialBlur")` *(postCreate)*
- [autre] `blur.cx = 0.5` *(postCreate)*
- [autre] `blur.cy = 0.5` *(postCreate)*
- [autre] `blur.blurWidth = 0.0` *(postCreate)*
- [autre] `camfx = new FlxCamera()` *(postCreate)*
- [autre] `camfx.bgColor = 0` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.remove(camNewHUD, false)` *(postCreate)*
- [caméra] `FlxG.cameras.add(camfx, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camHUD, false)` *(postCreate)*
- [HUD] `FlxG.cameras.add(camNewHUD, false)` *(postCreate)*
- [sprite/décor] `vignette=new FlxSprite().loadGraphic(Paths.image('stages/room/vignette'))` *(postCreate)*
- [autre] `vignette.screenCenter()` *(postCreate)*
- [autre] `vignette.cameras=[camfx]` *(postCreate)*
- [sprite/décor] `vignette.alpha=0` *(postCreate)*
- [sprite/décor] `vignette.scrollFactor.set()` *(postCreate)*
- [sprite/décor] `add(vignette)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [autre] `videoCamera = new FlxCamera()` *(postCreate)*
- [autre] `videoCamera.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(videoCamera, false)` *(postCreate)*
- [vidéo/cinématique] `video = new FlxVideoSprite(FlxAxes.X,FlxAxes.Y)` *(postCreate)*
- [autre] `video.cameras = [videoCamera]` *(postCreate)*
- [sprite/décor] `video.alpha = 0` *(postCreate)*
- [sprite/décor] `add(video)` *(postCreate)*
- [caméra] `camGame.zoom = 3` *(postCreate)*

**Pendant la chanson**
- `videomidsong` : tween/timer 1
- `onPostCountdown` : sprite/décor 1, tween/timer 1
- `onSongStart` : caméra 1
- `bump` (appelée 10 fois par le chart) : caméra 1, HUD 1
- `hardbump` : caméra 1, HUD 1
- `stepHit` : caméra 12, HUD 3, tween/timer 2
- `beatHit` : sprite/décor 2, tween/timer 2, caméra 2, HUD 2

## Me (`me`)

Décor `me`, persos soft, tans, veethree, **2 ligne(s) de notes en plus** (persos non affichés)

### `data/stages/me.hx`
**Appliqué** (12)
- [HUD] `var index = members.indexOf(strumLines.members[0].characters[0])`
- [sprite/décor] `bg = new FlxSprite(-200, -200).loadGraphic(Paths.image('stages/me/stage'))`
- [sprite/décor] `bg.scale.set(1.7,1.5)`
- [sprite/décor] `insert(index, bg)`
- [autre] `var index = members.indexOf(bg)`
- [sprite/décor] `shiver = new FlxSprite(400, 0).loadGraphic(Paths.image("stages/me/shiver"))`
- [sprite/décor] `shiver.frames = Paths.getSparrowAtlas('stages/me/shiver')`
- [sprite/décor] `shiver.visible = false`
- [sprite/décor] `insert(index,shiver)`
- [personnage] `boyfriend.setPosition(1365,200)`
- [personnage] `dad.setPosition(-285,270)`
- [personnage] `gf.setPosition(1535,335)`

**Ignoré** (13)
- [sprite/décor] `bg.scrollFactor.set(1,1)`
- [personnage] `shadum = new Character(570, 260, "shadow taeyai")`
- [sprite/décor] `shadum.y -= 1000`
- [sprite/décor] `insert(index, shadum)`
- [autre] `shiver.antialiasing = true`
- [sprite/décor] `shiver.animation.addByPrefix('idle', "tile0", 24,false)`
- [HUD] `strumLines.members[3].characters[0].setPosition(-725,355)`
- [HUD] `strumLines.members[4].characters[0].setPosition(530,-180)`
- [HUD] `strumLines.members[4].characters[0].visible = false`
- [HUD] `strumLines.members[4].characters = [shadum]`
- [personnage] `tansMid = gf.getGraphicMidpoint()` *(postCreate)*
- [caméra] `camGame.zoom = 1` *(postCreate)*
- [caméra] `camGame.scroll.set(800,230)` *(postCreate)*

**Non suivi** (6)
- [sprite/décor] `s.visible = false`
- [caméra] `s.cameras = [camGame]` *(postCreate)*
- [sprite/décor] `s.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `s.x += 420` *(postCreate)*
- [sprite/décor] `s.y -= 200` *(postCreate)*
- [sprite/décor] `s.alpha = 0` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : caméra 1
- `beatHit` : sprite/décor 1
- `postUpdate` : caméra 10

### `songs/me/scripts/events.hx`
**Pendant la chanson**
- `onSongStart` : caméra 1
- `onCountdown` : tween/timer 2
- `stepHit` : caméra 41, tween/timer 3, HUD 3, sprite/décor 2
- `beatHit` : caméra 5, HUD 5

## Glitcher (`glitcher`)

Décor `glitcher`, persos hex_night, veethree

### `data/stages/glitcher.hx`
**Ignoré** (14)
- [shader] `glitchbg.shader = new CustomShader('virus', "130")` *(postCreate)*
- [shader] `glitchbg.shader.uEnabled = true` *(postCreate)*
- [shader] `glitchbg.shader.screenSize = [1280, 720]` *(postCreate)*
- [shader] `glitchbg.shader.colorMultiplier = 0.1` *(postCreate)*
- [shader] `glitchbg.shader.colorTransform = true` *(postCreate)*
- [sprite/décor] `wire.visible = true` *(postCreate)*
- [shader] `wire.shader = new CustomShader('glitch')` *(postCreate)*
- [shader] `wire.shader.enabled = true` *(postCreate)*
- [sprite/décor] `wire.alpha = 0.3` *(postCreate)*
- [shader] `dad.shader = new CustomShader('virus', "130")` *(postCreate)*
- [shader] `dad.shader.uEnabled = true` *(postCreate)*
- [shader] `dad.shader.colorMultiplier = 0.1` *(postCreate)*
- [shader] `dad.shader.colorTransform = true` *(postCreate)*
- [shader] `boyfriend.shader = new CustomShader('hex-shadow')` *(postCreate)*

**Pendant la chanson**
- `update` : shader 2
- `stepHit` : shader 3, tween/timer 3, caméra 2, HUD 1

### `songs/glitcher/scripts/glitch.hx`
**Ignoré** (3)
- [shader] `screenshader = new CustomShader('glitchscreen')` *(postCreate)*
- [autre] `screenshader.uEnabled = true` *(postCreate)*
- [shader] `camGame.addShader(screenshader)` *(postCreate)*

## Cyber Trouble (`cyber trouble`)

Décor `exe`, persos clown conner, bf

### `data/stages/exe.hx`
**Appliqué** (40)
- [caméra] `defaultCamZoom = 1`
- [sprite/décor] `portchar = new FlxSprite(portposition[0] - 1200, -216).loadGraphic(Paths.image('stages/exe/CC'))`
- [sprite/décor] `portchar.alpha = 0`
- [sprite/décor] `portchar.scale.set(0.8,0.8)`
- [HUD] `portchar.cameras = [camHUD]`
- [sprite/décor] `add(portchar)`
- [sprite/décor] `sky = new FlxSprite(-621.1, -395.65).loadGraphic(Paths.image('stages/exe/Phase3/glitch'))`
- [sprite/décor] `add(sky)`
- [sprite/décor] `treeback = new FlxSprite(-621.1, -395.65).loadGraphic(Paths.image('stages/exe/Phase3/normal/TTTrees'))`
- [sprite/décor] `add(treeback)`
- [sprite/décor] `topbushes = new FlxSprite(-621.1, -395.65).loadGraphic(Paths.image('stages/exe/Phase3/normal/TopBushes'))`
- [sprite/décor] `add(topbushes)`
- [sprite/décor] `fgTree1 = new FlxSprite(-621.1, -395.65).loadGraphic(Paths.image('stages/exe/Phase3/normal/FGTree1'))`
- [sprite/décor] `fgTree1.scrollFactor.set(0.7,0.7)`
- [sprite/décor] `fgTree2 = new FlxSprite(-621.1, -395.65).loadGraphic(Paths.image('stages/exe/Phase3/normal/FGTree2'))`
- [sprite/décor] `fgTree2.scrollFactor.set(0.7,0.7)`
- [sprite/décor] `p3staticbg = new FlxSprite(0, 0).loadGraphic(Paths.image("stages/exe/NewTitleMenuBG"))`
- [sprite/décor] `p3staticbg.frames = Paths.getSparrowAtlas('stages/exe/NewTitleMenuBG')`
- [sprite/décor] `p3staticbg.animation.play('idle')`
- [autre] `p3staticbg.screenCenter()`
- [sprite/décor] `p3staticbg.scale.x = 4.5`
- [sprite/décor] `p3staticbg.scale.y = 4.5`
- [sprite/décor] `p3staticbg.visible = false`
- [sprite/décor] `add(p3staticbg)`
- [sprite/décor] `backtreesXeno = new FlxSprite(-621.1, -395.65).loadGraphic(Paths.image('stages/exe/Phase3/xeno/BackTrees'))`
- [sprite/décor] `backtreesXeno.visible = false`
- [sprite/décor] `add(backtreesXeno)`
- [sprite/décor] `grassXeno = new FlxSprite(-621.1, -395.65).loadGraphic(Paths.image('stages/exe/Phase3/xeno/Grass'))`
- [sprite/décor] `grassXeno.visible = false`
- [sprite/décor] `add(grassXeno)`
- [sprite/décor] `add(fgTree1)`
- [sprite/décor] `add(fgTree2)`
- [personnage] `remove(dad)`
- [personnage] `remove(boyfriend)`
- [personnage] `dad.setPosition(150, -60)`
- [personnage] `boyfriend.setPosition(1000, -60)`
- [personnage] `insert(members.indexOf(fgTree1), dad)`
- [personnage] `insert(members.indexOf(fgTree1), boyfriend)`
- [personnage] `dad.color = FlxColor.fromHSL(dad.color.hue, dad.color.saturation, 0.7, 0.7)` *(postCreate)*
- [personnage] `boyfriend.color = FlxColor.fromHSL(boyfriend.color.hue, boyfriend.color.saturation, 0.7, 0.7)` *(postCreate)*

**Ignoré** (33)
- [autre] `thread = EngineUtil.startMultithreadedScript(Paths.script('songs/cyber trouble/char-thread'))`
- [autre] `startedCountdown = true`
- [autre] `preloadCharacter('shadow taeyai',150,0,false)`
- [autre] `preloadCharacter('daisy-wrath',1000,100,false)`
- [autre] `preloadCharacter('futo',-200,100,false)`
- [autre] `var i = 0`
- [sprite/décor] `portchar.scrollFactor.set(1,1)`
- [sprite/décor] `sky.scrollFactor.set(1,1)`
- [shader] `sky.shader = new CustomShader('glitch')`
- [shader] `sky.shader.enabled = true`
- [sprite/décor] `backbush= new FlxSprite(-621.1, -395.65).loadGraphic(Paths.image('stages/exe/Phase3/normal/BackBush'))`
- [sprite/décor] `backbush.scrollFactor.set(1,1)`
- [sprite/décor] `add(backbush)`
- [autre] `treeback.active = false`
- [autre] `topbushes.active = false`
- [autre] `fgTree1.active = false`
- [autre] `fgTree2.active = false`
- [sprite/décor] `p3staticbg.animation.addByPrefix('idle', "TitleMenuSSBG instance 1", 24)`
- [autre] `backtreesXeno.active = false`
- [autre] `grassXeno.active = false`
- [personnage] `shadum = new Character(150, -1000, "shadow taeyai")` *(postCreate)*
- [personnage] `insert(members.indexOf(dad), shadum)` *(postCreate)*
- [personnage] `daisy = new Character(1100, -300, "daisy-wrath")` *(postCreate)*
- [personnage] `insert(members.indexOf(dad), daisy)` *(postCreate)*
- [personnage] `futo = new Character(-1500, -130, "futo")` *(postCreate)*
- [personnage] `insert(members.indexOf(dad), futo)` *(postCreate)*
- [caméra] `camGame.zoom = 3` *(postCreate)*
- [caméra] `FlxTween.tween(camGame, {zoom: 0.8}, 2, {ease: FlxEase.expoOut})` *(postCreate)*
- [sprite/décor] `shadum.color = FlxColor.fromHSL(shadum.color.hue, shadum.color.saturation, 0.7, 0.7)` *(postCreate)*
- [sprite/décor] `daisy.color = FlxColor.fromHSL(daisy.color.hue, daisy.color.saturation, 0.7, 0.7)` *(postCreate)*
- [sprite/décor] `futo.color = FlxColor.fromHSL(futo.color.hue, futo.color.saturation, 0.7, 0.7)` *(postCreate)*
- [shader] `idkshade = new FunkinShader(Assets.getText(Paths.fragShader("blend")))` *(postCreate)*
- [shader] `camGame.addShader(idkshade)` *(postCreate)*

**Non suivi** (5)
- [autre] `i++`
- [shader] `c.shader = new CustomShader("garage-shadow")`
- [shader] `c.shader.color = [1, 120 / 236, 0.9, 0.33 * ((3 - i) / 2)]`
- [shader] `c.shader.shadowLength = 25 * ((3 - i) / 2)`
- [shader] `c.shader.flipped = c.flipX`

**Pendant la chanson**
- `update` : shader 1
- `postUpdate` : sprite/décor 2
- `goofyAhhStatic` : sprite/décor 11, HUD 1
- `reverse` : HUD 2
- `noteback` : HUD 2
- `stepHit` : sprite/décor 51, caméra 28, tween/timer 21, personnage 20, HUD 8
- `beatHit` : caméra 3, HUD 3

### `songs/cyber trouble/scripts/glitch.hx`
**Ignoré** (3)
- [shader] `screenshader = new CustomShader('glitchscreen')` *(postCreate)*
- [autre] `screenshader.uEnabled = true` *(postCreate)*
- [shader] `camGame.addShader(screenshader)` *(postCreate)*

## credits (CREDITS) (`credits-credits`)

Décor `credits`, persos dad

### `data/stages/credits.hx`
**Appliqué** (7)
- [personnage] `dad.visible = false` *(postCreate)*
- [sprite/décor] `eventbackground2 = new FlxSprite(200,-200).makeGraphic(1280,720, 0xFF000000)` *(postCreate)*
- [autre] `eventbackground2.updateHitbox()` *(postCreate)*
- [sprite/décor] `eventbackground2.scale.set(10,10)` *(postCreate)*
- [autre] `eventbackground2.cameras = [camCredits]` *(postCreate)*
- [autre] `eventbackground2.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `add(eventbackground2)` *(postCreate)*

**Ignoré** (26)
- [caméra] `camGame.visible = false` *(postCreate)*
- [HUD] `camHUD.visible = false` *(postCreate)*
- [autre] `camCredits = new FlxCamera()` *(postCreate)*
- [autre] `camCredits.bgColor = 0` *(postCreate)*
- [caméra] `FlxG.cameras.add(camCredits, false)` *(postCreate)*
- [sprite/décor] `eventbackground2.alpha = 1` *(postCreate)*
- [sprite/décor] `eventbackground2.scrollFactor.set(1,1)` *(postCreate)*
- [sprite/décor] `logo=new FlxSprite()` *(postCreate)*
- [autre] `logo.screenCenter()` *(postCreate)*
- [autre] `logo.cameras=[camCredits]` *(postCreate)*
- [sprite/décor] `logo.alpha=0` *(postCreate)*
- [sprite/décor] `add(logo)` *(postCreate)*
- [autre] `titleCre = new FunkinText(-2200, 217, 0, 'Credits', 74, true)` *(postCreate)*
- [autre] `titleCre.screenCenter()` *(postCreate)*
- [sprite/décor] `titleCre.y = logo.y + 470` *(postCreate)*
- [sprite/décor] `titleCre.alpha=0` *(postCreate)*
- [autre] `titleCre.cameras=[camCredits]` *(postCreate)*
- [sprite/décor] `add(titleCre)` *(postCreate)*
- [autre] `credreal = new FunkinText(0, 0, 0, 'Credits', 42, true)` *(postCreate)*
- [autre] `credreal.alignment = "center"` *(postCreate)*
- [autre] `credreal.cameras=[camCredits]` *(postCreate)*
- [autre] `credreal.screenCenter(FlxAxes.XY)` *(postCreate)*
- [sprite/décor] `credreal.x -= 430` *(postCreate)*
- [sprite/décor] `credreal.y = titleCre.y + 300` *(postCreate)*
- [sprite/décor] `add(credreal)` *(postCreate)*
- [autre] `credreal.text = ' Directed by TaeYai\n \nWritten by\nTaeYai, TOFU\n [Editor] TaeYai, Natah_DN\n \n[Art Director] JzBoy, TOFU, Jeng\n [Voice ` *(postCreate)*

**Non suivi** (3)
- [HUD] `dad.visible = camGame.visible = camHUD.visible = false` *(postCreate)*
- [sprite/décor] `logo.loadGraphic(Paths.image('menus/titlescreen/logo-th'))` *(postCreate)*
- [sprite/décor] `logo.loadGraphic(Paths.image('menus/titlescreen/logo'))` *(postCreate)*

**Pendant la chanson**
- `onCountdown` : sprite/décor 1
- `onStartSong` : tween/timer 2
- `update` : sprite/décor 2, audio 1

## Liquated (`liquated`)

Décor `camellia`, persos bob, taeyai-player, gf

