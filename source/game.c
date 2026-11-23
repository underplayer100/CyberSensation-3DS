/* Partie : décor + personnages + flèches, synchronisés sur la position audio. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fnf.h"

#define HIT_WINDOW 166.0f

bool gAutoTest = false;
float gAutoSeconds = 60;
void flog(const char* fmt, ...);
void screenshot(const char* path);
#define MAX_STAGE_SPRITES 48
#define MAX_CHARS 6         /* 0 bf, 1 dad, 2 gf, 3+ persos créés par les scripts (new Character) */

typedef struct {
    float time;
    uint8_t lane, type;
    float sLen;
    bool hit, missed, holding, holdDone;
    bool hidden;            /* ligne de notes sans flèches à l'écran (ligne en plus) : chantée, pas dessinée */
} Note;

typedef struct {
    float time;
    uint8_t kind;
    float value;
} Event;

typedef struct {
    Spr spr;
    bool loaded;
    float x, y, scx, scy;   /* position de dessin (scène + décalage global) et défilement */
    float stageX, stageY;   /* position donnée par la scène (setPosition) */
    float gx, gy;           /* décalage global du XML du personnage */
    bool flip, swapLR, isPlayer;
    bool drawSwap;          /* isFlippedOffsets : swapLR, inversé si un script change flipX */
    int anim;
    float animTime;
    float holdTimer;        /* ms depuis la dernière note chantée */
    float holdTime;
    bool singing, danced;
    int interval;
    float camx, camy;
    float baseW, baseH;     /* taille du premier cadre (monde) */
    float scale, ow, oh;    /* échelle du XML, origine Flixel (centre du 1er cadre de l'atlas, non mis à l'échelle) */
    bool hidden;
    float tint;             /* color = gris : 1 normal, 0.4 assombri à 40 % */
    float alpha;            /* alpha du perso (scripts : apparitions, fondus) */
    float twFrom, twTo, twStart, twDur;
    float mvFrom[2], mvTo[2], mvStart, mvDur; /* déplacement (FlxTween sur x/y), mvDur < 0 : aucun */
    int aIdle, aLeft, aRight, aSing[4], aMiss[4], aAlt[4];
    bool alt;               /* événement Alt Animation Toggle */
    char noteSkin[32];
} Character;

typedef struct {
    Spr spr;
    bool isChar;
    int charIndex;          /* 0 bf, 1 dad, 2 gf */
    float x, y, scx, scy, scale, alpha;
    bool flip;
    float animTime;
    int tile;               /* FlxBackdrop : 1 répété en X, 2 en Y */
    float spx, spy, vx, vy; /* espacement entre copies, vitesse (px/s, monde) */
    uint8_t kind;           /* 0 sprite, 2 rectangle uni (makeGraphic) */
    uint8_t layer;          /* 1 : caméra ajoutée par un script, au-dessus du jeu, en coordonnées écran */
    uint8_t before;         /* extra.stg : perso devant lequel insérer (0 bf, 1 dad, 2 gf, 255 à la fin) */
    char name[33];          /* nom de l'élément, cible des événements "ev" de fix.txt */
    uint32_t color;         /* rectangle : 0xAARRGGBB */
    float w, h;             /* rectangle : taille monde (échelle comprise) */
    float anchor;           /* >= 0 : animation calée sur la position de la chanson (ms), sinon horloge propre */
    float twFrom, twTo, twStart, twDur; /* fondu d'alpha en cours (twDur < 0 : aucun) */
    bool hidden;            /* visible = false */
} StageItem;

typedef struct {
    float bpm, speed;
    char stage[32], chars[3][32], name[48];
    Note* notes[2];         /* 0 adversaire, 1 joueur */
    int noteCount[2];
    Event* events;
    int eventCount;
} Chart;

static float rdf(const uint8_t* p) {
    float f;
    memcpy(&f, p, 4);
    return f;
}

static bool chart_load(Chart* c, const char* path) {
    memset(c, 0, sizeof(*c));
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* b = malloc((size_t)n);
    if (fread(b, 1, (size_t)n, f) != (size_t)n || memcmp(b, "FSNG", 4) != 0) {
        free(b);
        fclose(f);
        return false;
    }
    fclose(f);
    const uint8_t* p = b + 8;
    c->bpm = rdf(p);
    c->speed = rdf(p + 4);
    p += 8;
    memcpy(c->stage, p, 32);
    p += 32;
    for (int i = 0; i < 3; i++) {
        memcpy(c->chars[i], p, 32);
        p += 32;
    }
    memcpy(c->name, p, 48);
    p += 48;
    uint32_t nd, nb, ne;
    memcpy(&nd, p, 4);
    memcpy(&nb, p + 4, 4);
    memcpy(&ne, p + 8, 4);
    p += 12;
    uint32_t counts[2] = {nd, nb};
    for (int s = 0; s < 2; s++) {
        c->noteCount[s] = (int)counts[s];
        c->notes[s] = calloc(counts[s] + 1, sizeof(Note));
        for (uint32_t i = 0; i < counts[s]; i++, p += 12) {
            Note* no = &c->notes[s][i];
            no->time = rdf(p);
            no->lane = p[4] & 3;
            no->type = p[5] & 0x7f;
            no->hidden = (p[5] & 0x80) != 0;
            no->sLen = rdf(p + 8);
        }
    }
    c->eventCount = (int)ne;
    c->events = calloc(ne + 1, sizeof(Event));
    for (uint32_t i = 0; i < ne; i++, p += 12) {
        c->events[i].time = rdf(p);
        c->events[i].kind = p[4];
        c->events[i].value = rdf(p + 8);
    }
    free(b);
    return true;
}

static void chart_free(Chart* c) {
    free(c->notes[0]);
    free(c->notes[1]);
    free(c->events);
}

/* ---------------------------------------------------------------- conductor (changements de BPM) */
typedef struct {
    float time, beat, bpm;
} BpmPoint;
static BpmPoint gBpm[64];
static int gBpmCount;

static void conductor_init(const Chart* c) {
    gBpmCount = 1;
    gBpm[0] = (BpmPoint){0, 0, c->bpm};
    for (int i = 0; i < c->eventCount && gBpmCount < 64; i++) {
        if (c->events[i].kind != 1) continue;
        BpmPoint* last = &gBpm[gBpmCount - 1];
        float t = c->events[i].time;
        float beat = last->beat + (t - last->time) * last->bpm / 60000.0f;
        gBpm[gBpmCount++] = (BpmPoint){t, beat, c->events[i].value};
    }
}

static float conductor_beat(float ms) {
    int k = 0;
    for (int i = 1; i < gBpmCount; i++)
        if (ms >= gBpm[i].time) k = i;
    return gBpm[k].beat + (ms - gBpm[k].time) * gBpm[k].bpm / 60000.0f;
}

static float conductor_crochet(float ms) {
    int k = 0;
    for (int i = 1; i < gBpmCount; i++)
        if (ms >= gBpm[i].time) k = i;
    return 60000.0f / gBpm[k].bpm;
}

/* ---------------------------------------------------------------- personnages (règles de Codename Engine) */
static const char* SING[4] = {"singLEFT", "singDOWN", "singUP", "singRIGHT"};

static void char_play(Character* ch, int anim) {
    if (anim < 0) return;
    ch->anim = anim;
    ch->animTime = 0;
}

/* isPlayer = valeur "flip" de la scène (vrai pour boyfriend), comme dans PlayState.create */
static bool char_load(Character* ch, const char* name, bool isPlayer) {
    memset(ch, 0, sizeof(*ch));
    snprintf(ch->noteSkin, sizeof(ch->noteSkin), "default");
    if (!name[0]) return false;
    char path[384], tex[320];
    snprintf(path, sizeof(path), "%s/chars/%s.spr", gModDir, name);
    snprintf(tex, sizeof(tex), "%s/tex", gModDir);
    if (!spr_load(&ch->spr, path, tex)) return false;
    ch->loaded = true;
    ch->tint = 1.0f;
    ch->alpha = 1.0f;
    ch->twDur = -1.0f;
    ch->mvDur = -1.0f;
    ch->isPlayer = isPlayer;
    bool playerOffsets = spr_metaf(&ch->spr, "isPlayer", 0) != 0;
    bool flipX = spr_metaf(&ch->spr, "flipX", 0) != 0;
    ch->flip = flipX != isPlayer;            /* fixChar : if (isPlayer) flipX = !flipX */
    ch->swapLR = isPlayer != playerOffsets;  /* gauche/droite échangés, décalages en miroir */
    ch->drawSwap = ch->swapLR;
    ch->holdTime = spr_metaf(&ch->spr, "holdTime", 4);
    ch->camx = spr_metaf(&ch->spr, "camx", 0);
    ch->camy = spr_metaf(&ch->spr, "camy", 0);
    ch->gx = spr_metaf(&ch->spr, "x", 0);
    ch->gy = spr_metaf(&ch->spr, "y", 0);
    ch->scale = spr_metaf(&ch->spr, "scale", 1);
    ch->ow = spr_metaf(&ch->spr, "ow", 0);
    ch->oh = spr_metaf(&ch->spr, "oh", 0);
    snprintf(ch->noteSkin, sizeof(ch->noteSkin), "%s", spr_metas(&ch->spr, "notesprite", "default"));
    ch->aIdle = spr_anim(&ch->spr, "idle");
    ch->aLeft = spr_anim(&ch->spr, "danceLeft");
    ch->aRight = spr_anim(&ch->spr, "danceRight");
    int iv = (int)spr_metaf(&ch->spr, "interval", 0);
    ch->interval = iv > 0 ? iv : (ch->aLeft >= 0 && ch->aRight >= 0) ? 1 : 2;
    for (int i = 0; i < 4; i++) {
        int src = ch->swapLR ? (i == 0 ? 3 : i == 3 ? 0 : i) : i;
        char m[40];
        ch->aSing[i] = spr_anim(&ch->spr, SING[src]);
        snprintf(m, sizeof(m), "%smiss", SING[src]);
        ch->aMiss[i] = spr_anim(&ch->spr, m);
        snprintf(m, sizeof(m), "%s-alt", SING[src]);
        ch->aAlt[i] = spr_anim(&ch->spr, m);
        if (ch->aSing[i] < 0) ch->aSing[i] = ch->aIdle;
    }
    int first = ch->aIdle >= 0 ? ch->aIdle : (ch->aRight >= 0 ? ch->aRight : 0);
    if (ch->spr.animCount > 0 && ch->spr.anims[first].count > 0) {
        SprFrame* fr = &ch->spr.frames[ch->spr.anims[first].frames[0]];
        ch->baseW = fr->fw / WORLD_S;
        ch->baseH = fr->fh / WORLD_S;
    }
    char_play(ch, first);
    return true;
}

static void char_place(Character* ch, float sx, float sy, float scx, float scy) {
    ch->stageX = sx;
    ch->stageY = sy;
    /* playAnim : offset = (isPlayer != playerOffsets ? gx : -gx, -gy) ; Flixel dessine à x - offset */
    ch->x = sx + (ch->swapLR ? -ch->gx : ch->gx);
    ch->y = sy + ch->gy;
    ch->scx = scx;
    ch->scy = scy;
}

static void char_dance_now(Character* ch) {
    if (ch->aLeft >= 0 && ch->aRight >= 0) {
        ch->danced = !ch->danced;
        char_play(ch, ch->danced ? ch->aLeft : ch->aRight);
    } else {
        char_play(ch, ch->aIdle);
    }
    ch->singing = false;
}

/* beatHit -> tryDance */
static void char_beat(Character* ch, int beat, float stepCrochet, bool locked) {
    if (!ch->loaded || locked) return;
    if (beat % ch->interval != 0) return;
    if (ch->singing) {
        if (ch->holdTimer >= stepCrochet * ch->holdTime) char_dance_now(ch);
        return;
    }
    char_dance_now(ch);
}

static void char_sing(Character* ch, int lane, bool miss, bool alt) {
    if (!ch->loaded) return;
    int a = miss ? ch->aMiss[lane] : ((alt || ch->alt) && ch->aAlt[lane] >= 0) ? ch->aAlt[lane] : ch->aSing[lane];
    if (a < 0) a = ch->aSing[lane];
    char_play(ch, a);
    ch->singing = true;
    ch->holdTimer = 0;
}

static void char_update(Character* ch, float dt) {
    if (!ch->loaded) return;
    ch->animTime += dt;
    if (ch->singing) ch->holdTimer += dt;
}

/* ---------------------------------------------------------------- caméra / dessin monde */
typedef struct {
    float x, y, zoom;
} Camera;

static void world_to_screen(const Camera* cam, float wx, float wy, float scx, float scy, float* sx, float* sy) {
    float scrollX = cam->x - 640.0f, scrollY = cam->y - 360.0f;
    float fx = wx - scrollX * scx, fy = wy - scrollY * scy;
    *sx = (fx - 640.0f) * cam->zoom * WORLD_S + SCREEN_W / 2.0f;
    *sy = (fy - 360.0f) * cam->zoom * WORLD_S + SCREEN_H / 2.0f;
}

/* light < 1 : couleur multipliée par light (fondu vers le noir de citro2d), comme sprite.color = gris dans Flixel */
static void draw_frame_lit(const SprFrame* fr, float sx, float sy, float scale, bool flip, float alpha, float light) {
    if (alpha <= 0.003f) return;
    C2D_ImageTint tint;
    if (alpha > 1) alpha = 1;
    if (light > 1.001f) /* colorTransform offset : fondu vers le blanc */
        C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, (u8)(alpha * 255)), light > 2 ? 1.0f : light - 1.0f);
    else if (light < 0.999f)
        C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 0, (u8)(alpha * 255)), 1.0f - (light < 0 ? 0 : light));
    else
        C2D_AlphaImageTint(&tint, alpha);
    float w = fr->sub.width * scale, h = fr->sub.height * scale;
    float ox = flip ? (fr->fw - fr->ox - fr->sub.width) : fr->ox;
    float x0 = sx + ox * scale, y0 = sy + fr->oy * scale;
    if (x0 > SCREEN_W + 8 || x0 + w < -8 || y0 > SCREEN_H + 8 || y0 + h < -8) return;
    /* citro2d prend |w| à partir de pos.x : une largeur négative retourne l'image sur place */
    C2D_DrawParams p = {.pos = {x0, y0, flip ? -w : w, h}, .center = {0, 0}, .depth = 0.5f};
    C2D_DrawImage(fr->img, &p, alpha < 1.0f || light < 0.999f || light > 1.001f ? &tint : NULL);
}

static void draw_frame(const SprFrame* fr, float sx, float sy, float scale, bool flip, float alpha) {
    draw_frame_lit(fr, sx, sy, scale, flip, alpha, 1.0f);
}

static const SprFrame* anim_frame(const Spr* s, int anim, float timeMs, bool loop) {
    if (anim < 0 || anim >= s->animCount) return NULL;
    const SprAnim* a = &s->anims[anim];
    if (a->count == 0) return NULL;
    int i = a->fps > 0 ? (int)(timeMs * a->fps / 1000.0f) : 0;
    if (loop || a->loop) i %= a->count;
    else if (i >= a->count) i = a->count - 1;
    return &s->frames[a->frames[i]];
}

/* FlxSprite.drawFrameComplex : écran = pos - offset + origin + scale * (pixel - origin - frameOffset).
   Le convertisseur a déjà multiplié frameOffset (a->ox, a->oy) par scale.
   Décalages inversés (isFlippedOffsets) : flipX et scale.x sont niés pendant le dessin, ce qui
   donne un cadre placé en miroir autour de l'origine avec frameOffset de signe opposé. */
static void char_draw(const Character* ch, const Camera* cam) {
    if (!ch->loaded || ch->hidden) return;
    const SprFrame* fr = anim_frame(&ch->spr, ch->anim, ch->animTime, false);
    if (!fr) return;
    const SprAnim* a = &ch->spr.anims[ch->anim];
    float s = ch->scale, ow = ch->ow, oh = ch->oh;
    float bx = ch->drawSwap ?ch->x + ow + s * ow + a->ox - fr->fw / WORLD_S : ch->x + ow - s * ow - a->ox;
    float by = ch->y + oh - s * oh - a->oy;
    float sx, sy;
    world_to_screen(cam, bx, by, ch->scx, ch->scy, &sx, &sy);
    draw_frame_lit(fr, sx, sy, cam->zoom, ch->flip, ch->alpha, ch->tint);
}

/* getCameraPosition : getMidpoint (x + width / 2, width = frameWidth * scale après updateHitbox)
   + (joueur ? -100 : 150) + globalOffset + cameraOffset */
static void char_camera(const Character* ch, float* x, float* y) {
    float hw = ch->ow > 0 ? ch->ow * ch->scale : ch->baseW / 2, hh = ch->oh > 0 ? ch->oh * ch->scale : ch->baseH / 2;
    *x = ch->stageX + hw + (ch->isPlayer ? -100.0f : 150.0f) + ch->gx + ch->camx;
    *y = ch->stageY + hh - 100.0f + ch->gy + ch->camy;
}

/* "slot <perso> <position>" de fix.txt : position de scène utilisée par chaque emplacement (0 bf, 1 dad, 2 gf) */
static void read_slots(const char* songId, int posOf[3]) {
    char path[384], line[96];
    snprintf(path, sizeof(path), "%s/songs/%s/fix.txt", gModDir, songId);
    FILE* f = fopen(path, "r");
    if (!f) return;
    int who, p;
    while (fgets(line, sizeof(line), f))
        if (sscanf(line, "slot %d %d", &who, &p) == 2 && who >= 0 && who < 3 && p >= 0 && p < 3) posOf[who] = p;
    fclose(f);
}

/* fix.txt : repositionnements simples extraits des scripts du décor et de la chanson */
typedef struct {
    bool on;
    float x, y;
} CamFix;

static void apply_fixes(const char* songId, Character chars[3], float charPos[3][4], float* zoom, CamFix camFix[3]) {
    char path[384], line[96];
    snprintf(path, sizeof(path), "%s/songs/%s/fix.txt", gModDir, songId);
    FILE* f = fopen(path, "r");
    if (!f) return;
    while (fgets(line, sizeof(line), f)) {
        char op[8], kind[8];
        int who;
        float a, b;
        if (sscanf(line, "pos %d %f %f", &who, &a, &b) == 3 && who >= 0 && who < 3) {
            charPos[who][0] = a;
            charPos[who][1] = b;
        } else if (sscanf(line, "%7s %7s %d %f", op, kind, &who, &a) == 4 && who >= 0 && who < 3 &&
                   (!strcmp(op, "set") || !strcmp(op, "add") || !strcmp(op, "sub"))) {
            Character* c = &chars[who];
            float* t = !strcmp(kind, "x") ? &charPos[who][0] : !strcmp(kind, "y") ? &charPos[who][1]
                     : !strcmp(kind, "camx") ? &c->camx : !strcmp(kind, "camy") ? &c->camy : NULL;
            if (!t) continue;
            if (!strcmp(op, "set")) *t = a;
            else *t += !strcmp(op, "add") ? a : -a;
        } else if (sscanf(line, "flip %d %f", &who, &a) == 2 && who >= 0 && who < 3) {
            /* flipX change après fixChar : l'image suit flipX, les décalages passent en miroir */
            bool fl = a != 0;
            if (fl != chars[who].flip) {
                chars[who].flip = fl;
                chars[who].drawSwap = !chars[who].drawSwap;
            }
        } else if (sscanf(line, "scroll %d %f %f", &who, &a, &b) == 3 && who >= 0 && who < 3) {
            charPos[who][2] = a;
            charPos[who][3] = b;
        } else if (sscanf(line, "alpha %d %f", &who, &a) == 2 && who >= 0 && who < 3) {
            chars[who].alpha = a;
        } else if (sscanf(line, "tint %d %f", &who, &a) == 2 && who >= 0 && who < 3) {
            chars[who].tint = a;
        } else if (sscanf(line, "hide %d", &who) == 1 && who >= 0 && who < 3) {
            chars[who].hidden = true;
        } else if (sscanf(line, "zoom %f", &a) == 1) {
            *zoom = a;
        } else if (sscanf(line, "camfix %d %f %f", &who, &a, &b) == 3 && who >= 0 && who < 3) {
            camFix[who] = (CamFix){true, a, b};
        }
    }
    fclose(f);
}

/* ---------------------------------------------------------------- HUD (coordonnées FNF 1280x720) */
static Spr gNotesDef, gNotesSkin[2], gMisc, gIcons, gSplash;
static bool gUiLoaded, gSkinLoaded[2];

static void ui_load(void) {
    if (gUiLoaded) return;
    char p[320], t[320];
    snprintf(t, sizeof(t), "%s/tex", gModDir);
    snprintf(p, sizeof(p), "%s/ui/notes.spr", gModDir);
    spr_load(&gNotesDef, p, t);
    snprintf(p, sizeof(p), "%s/ui/misc.spr", gModDir);
    spr_load(&gMisc, p, t);
    snprintf(p, sizeof(p), "%s/ui/icons.spr", gModDir);
    spr_load(&gIcons, p, t);
    snprintf(p, sizeof(p), "%s/ui/splash.spr", gModDir);
    spr_load(&gSplash, p, t);
    gUiLoaded = true;
}

static const Spr* skin_for(int side) { return gSkinLoaded[side] ? &gNotesSkin[side] : &gNotesDef; }

#define HUD_Y0 ((SCREEN_H - 720.0f * WORLD_S) / 2.0f)
#define LANE_W (160.0f * 0.7f)

typedef struct {
    float zoom;
    bool down;
} Hud;

/* FNF -> écran, avec zoom du HUD autour du centre et miroir vertical en downscroll */
static float hx(const Hud* h, float x) { return ((x * WORLD_S) - SCREEN_W / 2.0f) * h->zoom + SCREEN_W / 2.0f; }
static float hy(const Hud* h, float y) {
    if (h->down) y = 720.0f - y;
    return ((HUD_Y0 + y * WORLD_S) - SCREEN_H / 2.0f) * h->zoom + SCREEN_H / 2.0f;
}

/* calculateStartingXPos + swagWidth * i, puis centre de la flèche */
static float lane_cx(int side, int lane) {
    float start = 1280.0f * (side ? 0.75f : 0.25f) - (LANE_W * 1.5f + LANE_W * 0.5f);
    return start + LANE_W * lane + LANE_W / 2.0f;
}
#define STRUM_CY (50.0f + LANE_W / 2.0f)

static void draw_centered(const Hud* h, const Spr* s, int anim, int frameIdx, float cx, float cy, float scale, float alpha,
    bool flip) {
    if (anim < 0 || anim >= s->animCount || s->anims[anim].count == 0) return;
    const SprAnim* a = &s->anims[anim];
    const SprFrame* fr = &s->frames[a->frames[frameIdx % a->count]];
    float sc = scale * h->zoom;
    float w = fr->fw * sc, hh = fr->fh * sc;
    draw_frame(fr, hx(h, cx) - w / 2, hy(h, cy) - hh / 2, sc, flip, alpha);
}

/* tenue : corps étiré du début à la fin, bout de la tenue au bout (retourné en downscroll) */
static void draw_hold(const Hud* h, const Spr* sk, int lane, float cx, float yStart, float yEnd, float alpha) {
    if (yEnd <= yStart) return;
    int piece = 16 + lane, end = 20 + lane;
    if (piece >= sk->animCount || sk->anims[piece].count == 0) return;
    const SprFrame* pf = &sk->frames[sk->anims[piece].frames[0]];
    const SprFrame* ef = end < sk->animCount && sk->anims[end].count ? &sk->frames[sk->anims[end].frames[0]] : NULL;
    float z = h->zoom;
    float endH = ef ? ef->sub.height / WORLD_S : 0;
    float bodyEnd = yEnd - endH > yStart ? yEnd - endH : yStart;
    float x = hx(h, cx);
    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, alpha);
    float a0 = hy(h, yStart), a1 = hy(h, bodyEnd);
    if (bodyEnd > yStart) {
        float top = a0 < a1 ? a0 : a1, ht = fabsf(a1 - a0);
        C2D_DrawParams p = {.pos = {x - pf->sub.width * z / 2.0f, top, pf->sub.width * z, ht}, .depth = 0.5f};
        C2D_DrawImage(pf->img, &p, &tint);
    }
    if (ef) {
        float b0 = hy(h, bodyEnd), b1 = hy(h, yEnd);
        float top = b0 < b1 ? b0 : b1, ht = fabsf(b1 - b0);
        bool flipY = h->down;
        C2D_DrawParams p = {.pos = {x - ef->sub.width * z / 2.0f, top, ef->sub.width * z, flipY ? -ht : ht},
            .depth = 0.5f};
        C2D_DrawImage(ef->img, &p, &tint);
    }
}

/* ---------------------------------------------------------------- partie */
typedef struct {
    int anim;          /* 0 statique, 1 appui, 2 confirmation */
    float time;
} Strum;

typedef struct {
    bool on;
    int lane, side, anim;
    float t;
} Splash;

typedef struct {
    bool on;
    int anim;          /* index dans gMisc */
    float x, y, vx, vy, ay, t, fadeStart, fadeDur;
} Popup;

#define MAX_POPUPS 24

/* boutons de chaque flèche : réglages du joueur (Options > Touches) */
#define LANE_KEYS gSettings.laneKeys

/* élément de décor : v1 58 octets, v2 + répétition (78), v3 + rectangle, calque, insertion, ancre (98) */
static bool item_read(StageItem* s, const uint8_t* e, const char* tex) {
    memset(s, 0, sizeof(*s));
    memcpy(s->name, e + 1, 32);
    s->name[32] = 0;
    s->twDur = -1.0f;
    s->anchor = -1.0f;
    s->before = 255;
    if (e[0] == 1) {
        s->isChar = true;
        s->charIndex = strcmp(s->name, "bf") == 0 ? 0 : strcmp(s->name, "dad") == 0 ? 1 : 2;
        return true;
    }
    s->kind = e[0];
    s->x = rdf(e + 33);
    s->y = rdf(e + 37);
    s->scx = rdf(e + 41);
    s->scy = rdf(e + 45);
    s->scale = rdf(e + 49);
    s->alpha = rdf(e + 53);
    s->flip = (e[57] & 1) != 0;
    s->hidden = (e[57] & 2) != 0;  /* visible = false au départ */
    s->tile = e[58];
    s->spx = rdf(e + 62);
    s->spy = rdf(e + 66);
    s->vx = rdf(e + 70);
    s->vy = rdf(e + 74);
    memcpy(&s->color, e + 78, 4);
    s->w = rdf(e + 82);
    s->h = rdf(e + 86);
    s->layer = e[90];
    s->before = e[91];
    s->anchor = rdf(e + 94);
    if (s->kind == 2) return true;
    char sp[400];
    snprintf(sp, sizeof(sp), "%s/stages/%s.spr", gModDir, s->name);
    return spr_load(&s->spr, sp, tex);
}

static bool stage_load(StageItem** items, int* count, float* zoom, float charPos[3][4], const char* name) {
    *items = NULL;
    *count = 0;
    *zoom = 1.05f;
    float defs[3][4] = {{770, 100, 1, 1}, {100, 100, 1, 1}, {400, 130, 0.95f, 0.95f}};
    memcpy(charPos, defs, sizeof(defs));
    char path[384], tex[320];
    snprintf(path, sizeof(path), "%s/stages/%s.stg", gModDir, name);
    snprintf(tex, sizeof(tex), "%s/tex", gModDir);
    FILE* f = fopen(path, "rb");
    StageItem* it = calloc(MAX_STAGE_SPRITES + 3, sizeof(StageItem));
    int n = 0;
    bool haveChar[3] = {false, false, false};
    if (f) {
        uint8_t h[12];
        if (fread(h, 1, 12, f) == 12 && memcmp(h, "FSTG", 4) == 0) {
            uint32_t ver;
            memcpy(&ver, h + 4, 4);
            size_t isz = ver >= 3 ? 98 : ver >= 2 ? 78 : 58;
            *zoom = rdf(h + 8);
            for (int i = 0; i < 3; i++) {
                float v[4];
                if (fread(v, 4, 4, f) == 4) memcpy(charPos[i], v, sizeof(v));
            }
            uint32_t cnt = 0;
            if (fread(&cnt, 4, 1, f) == 1) {
                for (uint32_t i = 0; i < cnt && n < MAX_STAGE_SPRITES; i++) {
                    uint8_t e[98] = {0};
                    if (fread(e, 1, isz, f) != isz) break;
                    if (isz < 98) { /* anciens formats : pas d'ancre de temps, rien à insérer */
                        memset(e + isz, 0, 98 - isz);
                        float none = -1.0f;
                        memcpy(e + 94, &none, 4);
                        e[91] = 255;
                    }
                    if (item_read(&it[n], e, tex)) {
                        if (it[n].isChar) haveChar[it[n].charIndex] = true;
                        n++;
                    }
                }
            }
        }
        fclose(f);
    }
    int order[3] = {2, 1, 0};
    for (int k = 0; k < 3; k++)
        if (!haveChar[order[k]]) {
            it[n].isChar = true;
            it[n].charIndex = order[k];
            n++;
        }
    *items = it;
    *count = n;
    return true;
}

static const char* rank_of(float acc) {
    if (acc < 0) return "N/A";
    if (acc >= 1.0f) return "S++";
    if (acc >= 0.95f) return "S";
    if (acc >= 0.9f) return "A";
    if (acc >= 0.85f) return "B";
    if (acc >= 0.8f) return "C";
    if (acc >= 0.7f) return "D";
    if (acc >= 0.5f) return "E";
    return "F";
}

static void play_ui_sfx(const char* name, float vol) {
    char p[320];
    snprintf(p, sizeof(p), "%s/ui/sfx_%s.adp", gModDir, name);
    sfx_play(p, vol);
}

static float frand(float a, float b) { return a + (b - a) * (float)rand() / (float)RAND_MAX; }
static float ease_circ_out(float t) { return sqrtf(1 - (t - 1) * (t - 1)); }
static float ease_cube_in_out(float t) { return t < 0.5f ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2; }

static void add_popup(Popup* pops, int anim, float x, float y, float vx, float vy, float ay, float fadeStart, float fadeDur) {
    if (anim < 0) return;
    for (int k = 0; k < MAX_POPUPS; k++)
        if (!pops[k].on) {
            pops[k] = (Popup){true, anim, x, y, vx, vy, ay, 0, fadeStart, fadeDur};
            return;
        }
}

static void miss_sound(void) {
    char mn[16];
    snprintf(mn, sizeof(mn), "missnote%d", 1 + rand() % 3);
    play_ui_sfx(mn, frand(0.1f, 0.2f));
}

/* songs/<id>/extra.stg : sprites créés par les scripts de la chanson, insérés devant un perso ou à la fin */
static void extras_load(StageItem** items, int* count, const char* songId) {
    char path[384], tex[320];
    snprintf(path, sizeof(path), "%s/songs/%s/extra.stg", gModDir, songId);
    snprintf(tex, sizeof(tex), "%s/tex", gModDir);
    FILE* f = fopen(path, "rb");
    if (!f) return;
    uint8_t h[12];
    uint32_t cnt = 0;
    if (fread(h, 1, 12, f) == 12 && memcmp(h, "FSTX", 4) == 0) memcpy(&cnt, h + 8, 4);
    if (cnt > 64) cnt = 64;
    StageItem* it = realloc(*items, (*count + cnt + 1) * sizeof(StageItem));
    if (!it) {
        fclose(f);
        return;
    }
    int n = *count;
    for (uint32_t i = 0; i < cnt; i++) {
        uint8_t e[98];
        StageItem s;
        if (fread(e, 1, 98, f) != 98) break;
        if (!item_read(&s, e, tex)) continue;
        int at = n;
        if (s.before < 3 && !s.layer)
            for (int k = 0; k < n; k++)
                if (it[k].isChar && it[k].charIndex == s.before) {
                    at = k;
                    break;
                }
        memmove(&it[at + 1], &it[at], (n - at) * sizeof(StageItem));
        it[at] = s;
        n++;
    }
    fclose(f);
    *items = it;
    *count = n;
}

/* événements de pas extraits des stepHit/beatHit (fix.txt : "ev <pas> <genre> <cible> <valeur> <durée s>") */
typedef struct {
    float step;
    uint8_t kind;            /* 0 alpha, 1 teinte, 2 flash, 3 chanteur, 4 déplacement, 5 miroir, 6 visible */
    int target;              /* >= 0 élément du décor, -1 - k perso k ; chanteur : ligne de notes (0 joueur, 1 adversaire, 2 gf) */
    float value, dur, y;     /* déplacement : x = value, y, durée ; 1e30 = inchangé */
    uint32_t color;
} StepEvent;

static int timeline_load(const char* songId, const StageItem* items, int count, StepEvent** out) {
    char path[384], line[128];
    snprintf(path, sizeof(path), "%s/songs/%s/fix.txt", gModDir, songId);
    *out = NULL;
    FILE* f = fopen(path, "r");
    if (!f) return 0;
    int n = 0, cap = 0;
    StepEvent* ev = NULL;
    while (fgets(line, sizeof(line), f)) {
        char kind[8], tgt[40];
        float step, v, d = 0, d2 = 0;
        if (sscanf(line, "ev %f %7s %39s %f %f %f", &step, kind, tgt, &v, &d, &d2) < 4) continue;
        StepEvent e = {step, 0, -1, v, d, 0, 0};
        if (!strcmp(kind, "flash")) {
            e.kind = 2;
            e.color = (uint32_t)strtoul(tgt, NULL, 16);
            e.dur = v;
        } else if (!strcmp(kind, "sing")) {
            e.kind = 3;
            e.target = atoi(tgt);
        } else {
            e.kind = !strcmp(kind, "tint") ? 1 : !strcmp(kind, "move") ? 4 : !strcmp(kind, "flip") ? 5
                   : !strcmp(kind, "vis") ? 6 : 0;
            if (e.kind == 4) e.y = d, e.dur = d2;
            if (tgt[0] == '@') e.target = -1 - (tgt[1] - '0');
            else {
                e.target = -1000;
                for (int i = 0; i < count; i++)
                    if (!items[i].isChar && !strcmp(items[i].name, tgt)) {
                        e.target = i;
                        break;
                    }
                if (e.target == -1000) continue;
            }
        }
        if (n == cap) {
            cap = cap ? cap * 2 : 32;
            ev = realloc(ev, cap * sizeof(StepEvent));
        }
        ev[n++] = e;
    }
    fclose(f);
    /* tri par pas (insertion : stable, garde l'ordre du script pour un même pas) */
    for (int i = 1; i < n; i++) {
        StepEvent e = ev[i];
        int j = i - 1;
        while (j >= 0 && ev[j].step > e.step) ev[j + 1] = ev[j], j--;
        ev[j + 1] = e;
    }
    *out = ev;
    return n;
}

typedef struct {
    uint32_t color;
    float alpha, dur;
} Flash;

/* applique un événement de pas ; pos = position de la chanson (début des fondus) */
static void step_event_apply(const StepEvent* e, Character* chars, int slot[3], StageItem* stage, Flash* fl, float pos) {
    if (gAutoTest && e->kind >= 3)
        flog("ev pas=%.0f genre=%d cible=%d v=%.1f y=%.1f d=%.2f", e->step, e->kind, e->target, e->value, e->y, e->dur);
    if (e->kind == 2) {
        fl->color = e->color;
        fl->alpha = 1.0f;
        fl->dur = e->dur > 0 ? e->dur * 1000.0f : 1000.0f;
        return;
    }
    if (e->kind == 3) { /* dad = X : X chante les notes de cette ligne, la caméra le suit */
        int k = (int)e->value;
        if (e->target >= 0 && e->target < 3 && k >= 0 && k < MAX_CHARS && chars[k].loaded) slot[e->target] = k;
        return;
    }
    if (e->target < 0) {
        Character* c = &chars[-1 - e->target];
        if (!c->loaded) return;
        switch (e->kind) {
        case 1: c->tint = e->value; break;
        case 4:
            c->mvFrom[0] = c->stageX, c->mvFrom[1] = c->stageY;
            c->mvTo[0] = e->value > 1e29f ? c->stageX : e->value;
            c->mvTo[1] = e->y > 1e29f ? c->stageY : e->y;
            c->mvStart = pos, c->mvDur = e->dur * 1000.0f;
            break;
        case 5:
            if ((e->value != 0) != c->flip) {
                c->flip = e->value != 0;
                c->drawSwap = !c->drawSwap;
            }
            break;
        case 6: c->hidden = e->value == 0; break;
        default: c->twFrom = c->alpha, c->twTo = e->value, c->twStart = pos, c->twDur = e->dur * 1000.0f;
        }
        return;
    }
    StageItem* si = &stage[e->target];
    if (e->kind == 6) si->hidden = e->value == 0;
    else if (e->kind == 0) si->twFrom = si->alpha, si->twTo = e->value, si->twStart = pos, si->twDur = e->dur * 1000.0f;
}

/* fix.txt "char <n> |<nom>| <x> <y> <joueur> <devant>" : persos créés par les scripts (new Character) */
static void extra_chars_load(const char* songId, Character* chars, StageItem** items, int* count) {
    char path[384], line[160];
    snprintf(path, sizeof(path), "%s/songs/%s/fix.txt", gModDir, songId);
    FILE* f = fopen(path, "r");
    if (!f) return;
    while (fgets(line, sizeof(line), f)) {
        int k, player, before;
        float x, y;
        char name[64];
        if (sscanf(line, "char %d |%63[^|]| %f %f %d %d", &k, name, &x, &y, &player, &before) != 6) continue;
        if (k < 3 || k >= MAX_CHARS || !char_load(&chars[k], name, player != 0)) continue;
        char_place(&chars[k], x, y, 1, 1);
        StageItem* it = realloc(*items, (*count + 1) * sizeof(StageItem));
        if (!it) continue;
        int at = *count;
        for (int i = 0; i < *count; i++)
            if (it[i].isChar && it[i].charIndex == before) {
                at = i;
                break;
            }
        memmove(&it[at + 1], &it[at], (*count - at) * sizeof(StageItem));
        memset(&it[at], 0, sizeof(StageItem));
        it[at].isChar = true;
        it[at].charIndex = k;
        *items = it;
        (*count)++;
    }
    fclose(f);
}

/* fondu linéaire d'alpha (les courbes FlxEase sont approchées par une droite) */
static float tween_value(float from, float to, float start, float dur, float now, bool* done) {
    float t = dur > 0 ? (now - start) / dur : 1.0f;
    *done = t >= 1.0f;
    if (t > 1) t = 1;
    if (t < 0) t = 0;
    return from + (to - from) * t;
}

GameResult game_play(const SongEntry* song, C3D_RenderTarget* top, C3D_RenderTarget* bottom,
    int* outScore, float* outAccuracy) {
    char path[384];
    Chart chart;
    snprintf(path, sizeof(path), "%s/songs/%s/chart.sng", gModDir, song->id);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(top);
    C2D_TextBufClear(gTextBuf);
    draw_text_centered(200, 110, 0.7f, C2D_Color32(255, 255, 255, 255), "Loading...");
    C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));
    C3D_FrameEnd(0);

    if (!chart_load(&chart, path)) return GAME_QUIT;
    conductor_init(&chart);
    ui_load();

    StageItem* stage;
    int stageCount;
    float stageZoom, charPos[3][4];
    stage_load(&stage, &stageCount, &stageZoom, charPos, chart.stage);

    Character chars[MAX_CHARS];   /* 0 bf, 1 dad, 2 gf, 3+ persos des scripts */
    memset(chars, 0, sizeof(chars));
    int slot[3] = {0, 1, 2};      /* perso qui chante chaque ligne : 0 joueur, 1 adversaire, 2 gf (dad = X) */
    /* isPlayer = stage.isCharFlipped(position) : vrai seulement à la position "boyfriend" */
    int posOf[3] = {0, 1, 2};
    read_slots(song->id, posOf);
    char_load(&chars[0], chart.chars[1], posOf[0] == 0);
    char_load(&chars[1], chart.chars[0], posOf[1] == 0);
    char_load(&chars[2], chart.chars[2], posOf[2] == 0);
    CamFix camFix[3] = {{false, 0, 0}, {false, 0, 0}, {false, 0, 0}};
    float slotPos[3][4];
    for (int i = 0; i < 3; i++) memcpy(slotPos[i], charPos[posOf[i]], sizeof(slotPos[i]));
    apply_fixes(song->id, chars, slotPos, &stageZoom, camFix);
    for (int i = 0; i < 3; i++) char_place(&chars[i], slotPos[i][0], slotPos[i][1], slotPos[i][2], slotPos[i][3]);
    extras_load(&stage, &stageCount, song->id);
    extra_chars_load(song->id, chars, &stage, &stageCount);
    StepEvent* stepEv;
    int stepEvCount = timeline_load(song->id, stage, stageCount, &stepEv), nextStepEv = 0;
    Flash flash = {0, 0, 1};
    /* pas <= -1000 : état initial réglé par create/postCreate (persos des scripts) */
    while (nextStepEv < stepEvCount && stepEv[nextStepEv].step <= -1000)
        step_event_apply(&stepEv[nextStepEv++], chars, slot, stage, &flash, 0);

    /* flèches propres au personnage de chaque côté (notesprite) */
    char tex[320];
    snprintf(tex, sizeof(tex), "%s/tex", gModDir);
    for (int side = 0; side < 2; side++) {
        Character* c = side ? &chars[0] : &chars[1];
        gSkinLoaded[side] = false;
        if (c->loaded && strcmp(c->noteSkin, "default") != 0) {
            snprintf(path, sizeof(path), "%s/ui/notes_%s.spr", gModDir, c->noteSkin);
            gSkinLoaded[side] = spr_load(&gNotesSkin[side], path, tex);
        }
    }

    const char* iconP = chars[0].loaded ? spr_metas(&chars[0].spr, "icon", "bf") : "bf";
    const char* iconD = chars[1].loaded ? spr_metas(&chars[1].spr, "icon", "dad") : "dad";
    int aIconP = spr_anim(&gIcons, iconP), aIconD = spr_anim(&gIcons, iconD);
    if (aIconP < 0) aIconP = spr_anim(&gIcons, "face");   /* HealthIcon : icône absente -> face */
    if (aIconD < 0) aIconD = spr_anim(&gIcons, "face");

    Stream inst, voices;
    snprintf(path, sizeof(path), "%s/songs/%s/inst.adp", gModDir, song->id);
    bool haveInst = stream_open(&inst, path, 0);
    snprintf(path, sizeof(path), "%s/songs/%s/voices.adp", gModDir, song->id);
    bool haveVoices = stream_open(&voices, path, 2);

    Camera cam = {0, 0, stageZoom};
    float camTX, camTY;
    int camSlot = 1;  /* ligne suivie par la caméra : 0 joueur, 1 adversaire, 2 gf */
    char_camera(chars[1].loaded ? &chars[1] : &chars[0], &camTX, &camTY);
    if (camFix[0].on) camTX = camFix[0].x, camTY = camFix[0].y;
    cam.x = camTX;
    cam.y = camTY;
    float hudZoom = 1.0f;

    Strum strums[2][4];
    memset(strums, 0, sizeof(strums));
    Splash splashes[8];
    memset(splashes, 0, sizeof(splashes));
    Popup pops[MAX_POPUPS];
    memset(pops, 0, sizeof(pops));
    float health = 1.0f;
    int score = 0, misses = 0, combo = 0, pressedNotes = 0;
    float accSum = 0;
    int nextNote[2] = {0, 0};
    int nextEvent = 0;
    int lastBeat = -1000, lastTick = -1;
    float iconScale = 1.0f;
    bool started = false, paused = false, dead = false;
    int pauseSel = 0;
    GameResult result = GAME_DONE;
    float crochet0 = conductor_crochet(0);
    double songPos = -crochet0 * 5.0;
    float introTime = 0;
    u64 lastTk = svcGetSystemTick();
    double lastAudio = -1, audioBase = 0;
    u64 audioTick = 0;
    srand((unsigned)osGetTime());
    Hud hud = {1.0f, gSettings.downscroll};
    static const char* RN[5] = {"", "sick", "good", "bad", "shit"};
    static const char* SC[4] = {"purple", "blue", "green", "red"};

    while (aptMainLoop()) {
        u64 now = svcGetSystemTick();
        float dt = (float)((now - lastTk) * 1000.0 / SYSCLOCK_ARM11);
        lastTk = now;
        if (dt > 100) dt = 100;
        hidScanInput();
        u32 down = hidKeysDown(), held = hidKeysHeld();
        if (gAutoTest) {
            down = held = 0;
            for (int i = nextNote[1]; i < chart.noteCount[1]; i++) {
                Note* n = &chart.notes[1][i];
                if (n->time > songPos + 5) break;
                if (!n->hit && !n->missed && n->time >= songPos - 30) down |= LANE_KEYS[n->lane];
                if (n->hit && n->holding) held |= LANE_KEYS[n->lane];
            }
            held |= down;
        }
        sfx_update();

        if (paused) {
            if (down & (KEY_DUP | KEY_DDOWN)) pauseSel = (pauseSel + ((down & KEY_DDOWN) ? 1 : 2)) % 3;
            if (down & KEY_A) {
                if (pauseSel == 0) {
                    paused = false;
                    stream_pause(&inst, false);
                    stream_pause(&voices, false);
                } else {
                    result = pauseSel == 1 ? GAME_RESTART : GAME_QUIT;
                    break;
                }
            }
            if (down & KEY_START) {
                paused = false;
                stream_pause(&inst, false);
                stream_pause(&voices, false);
            }
            dt = 0;
        } else if ((down & KEY_START) && !dead) {
            paused = true;
            pauseSel = 0;
            stream_pause(&inst, true);
            stream_pause(&voices, true);
        }
        if (dead) {
            if (down & KEY_A) {
                result = GAME_RESTART;
                break;
            }
            if (down & KEY_B) {
                result = GAME_QUIT;
                break;
            }
        }

        /* ---- temps de la chanson */
        if (!paused && !dead) {
            introTime += dt;
            if (!started) {
                songPos += dt;
                if (songPos >= 0) {
                    started = true;
                    stream_start(&inst);
                    stream_start(&voices);
                    stream_pause(&inst, false);
                    stream_pause(&voices, false);
                    songPos = 0;
                    audioTick = now;
                }
            } else {
                stream_update(&inst);
                stream_update(&voices);
                double ap = haveInst ? stream_position_ms(&inst) : songPos + dt;
                if (ap != lastAudio) {
                    lastAudio = ap;
                    audioBase = ap;
                    audioTick = now;
                }
                double interp = audioBase + (now - audioTick) * 1000.0 / SYSCLOCK_ARM11;
                if (interp > audioBase + 40) interp = audioBase + 40;
                if (interp > songPos) songPos = interp;
                if (!haveInst) songPos += dt;
                if ((haveInst && stream_finished(&inst)) ||
                    (!haveInst && chart.noteCount[1] > 0 && songPos > chart.notes[1][chart.noteCount[1] - 1].time + 2000)) {
                    result = GAME_DONE;
                    break;
                }
            }
        }
        float pos = (float)songPos;
        float crochet = conductor_crochet(pos > 0 ? pos : 0);
        float stepCrochet = crochet / 4.0f;

        /* événements de pas des scripts : fondus d'alpha, teintes, flashs */
        if (pos >= 0) {
            float stepNow = conductor_beat(pos) * 4.0f;
            while (nextStepEv < stepEvCount && stepNow >= stepEv[nextStepEv].step)
                step_event_apply(&stepEv[nextStepEv++], chars, slot, stage, &flash, pos);
        }
        for (int i = 0; i < MAX_CHARS; i++) {
            Character* c = &chars[i];
            bool done;
            if (c->twDur >= 0) {
                c->alpha = tween_value(c->twFrom, c->twTo, c->twStart, c->twDur, pos, &done);
                if (done) c->twDur = -1;
            }
            if (c->mvDur >= 0) {
                float x = tween_value(c->mvFrom[0], c->mvTo[0], c->mvStart, c->mvDur, pos, &done);
                float y = tween_value(c->mvFrom[1], c->mvTo[1], c->mvStart, c->mvDur, pos, &done);
                char_place(c, x, y, c->scx, c->scy);
                if (done) c->mvDur = -1;
            }
        }
        for (int i = 0; i < stageCount; i++) {
            StageItem* si = &stage[i];
            if (si->isChar || si->twDur < 0) continue;
            bool done;
            si->alpha = tween_value(si->twFrom, si->twTo, si->twStart, si->twDur, pos, &done);
            if (done) si->twDur = -1;
        }
        if (flash.alpha > 0) flash.alpha -= dt / flash.dur;

        /* ---- compte à rebours : 3, 2 (ready), 1 (set), go */
        int tick = (int)floorf((float)(songPos + crochet0 * 5.0) / crochet0) - 1;
        if (!started && !paused && tick != lastTick && tick >= 0 && tick <= 3) {
            static const char* SND[4] = {"intro3", "intro2", "intro1", "introGo"};
            play_ui_sfx(SND[tick], 1.0f);
            lastTick = tick;
        }

        /* ---- événements */
        while (nextEvent < chart.eventCount && chart.events[nextEvent].time <= pos) {
            Event* e = &chart.events[nextEvent++];
            if (e->kind == 0) {
                int t = (int)e->value;
                camSlot = t == 1 ? 0 : t == 2 ? 2 : 1;
            } else if (e->kind == 2) {
                int v = (int)e->value, t = v / 2;
                Character* c = &chars[slot[t == 1 ? 0 : t == 2 ? 2 : 1]];
                c->alt = (v & 1) != 0;
            }
        }

        bool anyHeld = false;
        for (int l = 0; l < 4; l++)
            if (held & LANE_KEYS[l]) anyHeld = true;

        /* ---- temps forts */
        int beat = (int)floorf(conductor_beat(pos));
        if (beat != lastBeat && !paused) {
            lastBeat = beat;
            int b = beat < 0 ? -beat : beat;
            for (int i = 0; i < MAX_CHARS; i++) char_beat(&chars[i], b, stepCrochet, i == slot[0] && anyHeld);
            if (started && beat % 4 == 0) {
                cam.zoom += 0.015f;
                hudZoom += 0.03f;
            }
            iconScale = 1.2f;
        }

        if (!paused && !dead) {
            /* ---- adversaire */
            for (int i = nextNote[0]; i < chart.noteCount[0]; i++) {
                Note* n = &chart.notes[0][i];
                if (n->time > pos) break;
                if (!n->hit) {
                    n->hit = true;
                    if (n->type != 2)
                        char_sing(&chars[slot[n->type == 3 ? 2 : n->type == 4 ? 0 : 1]], n->lane, false, n->type == 1);
                    if (!n->hidden) {
                        strums[0][n->lane].anim = 2;
                        strums[0][n->lane].time = 0;
                    }
                    if (haveVoices) stream_volume(&voices, 1.0f);
                }
                if (n->time + n->sLen < pos) nextNote[0] = i + 1;
                else if (n->sLen > 0 && !n->hidden) {
                    chars[slot[1]].holdTimer = 0;
                    strums[0][n->lane].anim = 2;
                }
            }
            for (int l = 0; l < 4; l++)
                if (strums[0][l].anim == 2 && strums[0][l].time > crochet * 0.5f) strums[0][l].anim = 0;

            /* ---- joueur */
            for (int l = 0; l < 4; l++) {
                bool pressed = (down & LANE_KEYS[l]) != 0;
                bool isHeld = (held & LANE_KEYS[l]) != 0;
                if (pressed) {
                    Note* best = NULL;
                    for (int i = nextNote[1]; i < chart.noteCount[1]; i++) {
                        Note* n = &chart.notes[1][i];
                        if (n->time - pos > HIT_WINDOW) break;
                        if (n->hit || n->missed || n->lane != l) continue;
                        if (fabsf(n->time - pos) <= HIT_WINDOW) {
                            best = n;
                            break;
                        }
                    }
                    if (best) {
                        float diff = fabsf(best->time - pos);
                        best->hit = true;
                        best->holding = best->sLen > 0;
                        int r, pts;
                        float a, hg;
                        if (diff <= 45) r = 1, pts = 300, a = 1.0f, hg = 0.023f;
                        else if (diff <= 90) r = 2, pts = 200, a = 0.75f, hg = 0.015f;
                        else if (diff <= 135) r = 3, pts = 100, a = 0.45f, hg = 0.0f;
                        else r = 4, pts = 50, a = 0.25f, hg = -0.05f;
                        score += pts;
                        combo++;
                        pressedNotes++;
                        accSum += a;
                        health += hg;
                        if (best->type != 2)
                            char_sing(&chars[slot[best->type == 3 ? 2 : best->type == 4 ? 1 : 0]], l, false,
                                best->type == 1);
                        strums[1][l].anim = 2;
                        strums[1][l].time = 0;
                        if (haveVoices) stream_volume(&voices, 1.0f);
                        /* jugement qui saute puis retombe ; chiffres du combo à partir de 10 */
                        add_popup(pops, spr_anim(&gMisc, RN[r]), 664 + frand(0, 10), 300 + frand(-10, 10), frand(-10, 10),
                            -frand(140, 175), 550, 200, crochet);
                        if (combo >= 10) {
                            char digits[12];
                            snprintf(digits, sizeof(digits), "%03d", combo);
                            for (int d = 0; digits[d]; d++) {
                                char nm[8];
                                snprintf(nm, sizeof(nm), "num%c", digits[d]);
                                add_popup(pops, spr_anim(&gMisc, nm), 614 + 43 * d, 440, frand(-5, 5), -frand(140, 160),
                                    frand(200, 300), 0, crochet * 2);
                            }
                        }
                        if (r == 1)
                            for (int k = 0; k < 8; k++)
                                if (!splashes[k].on) {
                                    char nm[16];
                                    snprintf(nm, sizeof(nm), "%d%s", 1 + rand() % 2, SC[l]);
                                    splashes[k] = (Splash){true, l, 1, spr_anim(&gSplash, nm), 0};
                                    break;
                                }
                    } else {
                        strums[1][l].anim = 1;
                        strums[1][l].time = 0;
                        if (!gSettings.ghostTapping) {
                            health -= 0.04f;
                            misses++;
                            combo = 0;
                            score -= 10;
                            char_sing(&chars[slot[0]], l, true, false);
                            miss_sound();
                        }
                    }
                }
                if (!isHeld && strums[1][l].anim != 0) strums[1][l].anim = 0;
            }
            /* tenues et notes ratées */
            for (int i = nextNote[1]; i < chart.noteCount[1]; i++) {
                Note* n = &chart.notes[1][i];
                if (n->time > pos + HIT_WINDOW) break;
                if (n->hit && n->holding) {
                    if ((held & LANE_KEYS[n->lane]) && pos < n->time + n->sLen) {
                        chars[slot[0]].holdTimer = 0;
                        strums[1][n->lane].anim = 2;
                        health += 0.00023f * dt;
                    } else {
                        n->holding = false;
                        n->holdDone = true;
                    }
                }
                if (!n->hit && !n->missed && pos > n->time + HIT_WINDOW) {
                    n->missed = true;
                    misses++;
                    combo = 0;
                    pressedNotes++;
                    health -= 0.0475f;
                    score -= 10;
                    char_sing(&chars[slot[0]], n->lane, true, false);
                    if (haveVoices) stream_volume(&voices, 0.0f);
                    miss_sound();
                }
            }
            while (nextNote[1] < chart.noteCount[1]) {
                Note* n = &chart.notes[1][nextNote[1]];
                if (!((n->missed || n->hit) && !n->holding && pos > n->time + n->sLen + 200)) break;
                nextNote[1]++;
            }
            if (health > 2) health = 2;
            if (health <= 0) {
                health = 0;
                dead = true;
                stream_pause(&inst, true);
                stream_pause(&voices, true);
            }
            for (int s = 0; s < 2; s++)
                for (int l = 0; l < 4; l++) strums[s][l].time += dt;
            for (int i = 0; i < MAX_CHARS; i++) char_update(&chars[i], dt);
            for (int i = 0; i < stageCount; i++) stage[i].animTime += dt;
            for (int k = 0; k < 8; k++)
                if (splashes[k].on && (splashes[k].t += dt) > 400) splashes[k].on = false;
            for (int k = 0; k < MAX_POPUPS; k++) {
                Popup* p = &pops[k];
                if (!p->on) continue;
                float s = dt / 1000.0f;
                p->vy += p->ay * s;
                p->x += p->vx * s;
                p->y += p->vy * s;
                p->t += dt;
                if (p->t > p->fadeStart + p->fadeDur) p->on = false;
            }
        }

        /* ---- caméra : suivi 0,04 et retour du zoom 0,05 par image à 60 i/s */
        float follow = 1.0f - powf(1.0f - 0.04f, dt / 16.67f);
        /* moveCamera : chaque image, sur le perso qui chante la ligne visée (il peut changer ou bouger) */
        {
            int fi = camSlot == 0 ? 1 : camSlot == 1 ? 0 : 2;  /* camFix : indice de ligne de notes */
            Character* c = &chars[slot[camSlot]];
            if (camFix[fi].on) camTX = camFix[fi].x, camTY = camFix[fi].y;
            else if (c->loaded && !c->hidden) char_camera(c, &camTX, &camTY);
        }
        cam.x += (camTX - cam.x) * follow;
        cam.y += (camTY - cam.y) * follow;
        float back = 1.0f - powf(0.95f, dt / 16.67f);
        cam.zoom += (stageZoom - cam.zoom) * back;
        hudZoom += (1.0f - hudZoom) * back;
        iconScale += (1.0f - iconScale) * (1.0f - powf(0.85f, dt / 16.67f));
        hud.zoom = hudZoom;

        /* ---- écran du haut */
        C2D_TextBufClear(gTextBuf);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(top);
        for (int i = 0; i < stageCount; i++) {
            StageItem* s = &stage[i];
            if (s->isChar) {
                char_draw(&chars[s->charIndex], &cam);
                continue;
            }
            if (s->alpha <= 0.003f || s->hidden) continue;
            if (s->kind == 2) {
                /* rectangle uni (makeGraphic) */
                float x0, y0, x1, y1;
                if (s->layer) {
                    x0 = s->x * WORLD_S, y0 = HUD_Y0 + s->y * WORLD_S;
                    x1 = x0 + s->w * WORLD_S, y1 = y0 + s->h * WORLD_S;
                } else {
                    world_to_screen(&cam, s->x, s->y, s->scx, s->scy, &x0, &y0);
                    x1 = x0 + s->w * cam.zoom * WORLD_S, y1 = y0 + s->h * cam.zoom * WORLD_S;
                }
                if (x0 < -4) x0 = -4;
                if (y0 < -4) y0 = -4;
                if (x1 > SCREEN_W + 4) x1 = SCREEN_W + 4;
                if (y1 > SCREEN_H + 4) y1 = SCREEN_H + 4;
                if (x1 <= x0 || y1 <= y0) continue;
                float al = s->alpha > 1 ? 1 : s->alpha;
                C2D_DrawRectSolid(x0, y0, 0.5f, x1 - x0, y1 - y0,
                    C2D_Color32((s->color >> 16) & 255, (s->color >> 8) & 255, s->color & 255, (u8)(al * 255)));
                continue;
            }
            /* ancre >= 0 : images jouées à la position de la chanson (shader pré-calculé) */
            bool anchored = s->anchor >= 0;
            const SprFrame* fr = anim_frame(&s->spr, 0, anchored ? pos - s->anchor : s->animTime, !anchored);
            if (!fr) continue;
            const SprAnim* a = &s->spr.anims[0];
            float sx, sy;
            float t = s->animTime / 1000.0f;
            float z;
            if (s->layer) {
                sx = (s->x - a->ox) * WORLD_S, sy = HUD_Y0 + (s->y - a->oy) * WORLD_S;
                z = s->scale;
            } else {
                world_to_screen(&cam, s->x - a->ox + s->vx * t, s->y - a->oy + s->vy * t, s->scx, s->scy, &sx, &sy);
                z = cam.zoom * s->scale;
            }
            if (!s->tile) {
                draw_frame(fr, sx, sy, z, s->flip, s->alpha);
                continue;
            }
            /* FlxBackdrop : copies espacées de (largeur * échelle + espacement) jusqu'aux bords de l'écran */
            float stepX = fr->fw * z + s->spx * cam.zoom * WORLD_S, stepY = fr->fh * z + s->spy * cam.zoom * WORLD_S;
            if (stepX < 4) stepX = 4;
            if (stepY < 4) stepY = 4;
            float x0 = sx, y0 = sy;
            int nx = 1, ny = 1;
            if (s->tile & 1) {
                x0 = fmodf(sx, stepX);
                if (x0 > 0) x0 -= stepX;
                nx = (int)((SCREEN_W - x0) / stepX) + 1;
            }
            if (s->tile & 2) {
                y0 = fmodf(sy, stepY);
                if (y0 > 0) y0 -= stepY;
                ny = (int)((SCREEN_H - y0) / stepY) + 1;
            }
            for (int ty = 0; ty < ny && ty < 32; ty++)
                for (int tx = 0; tx < nx && tx < 32; tx++)
                    draw_frame(fr, x0 + tx * stepX, y0 + ty * stepY, z, s->flip, s->alpha);
        }

        /* camGame.flash : couleur plein écran qui s'efface (sous le HUD) */
        if (flash.alpha > 0)
            C2D_DrawRectSolid(0, 0, 0.5f, SCREEN_W, SCREEN_H,
                C2D_Color32((flash.color >> 16) & 255, (flash.color >> 8) & 255, flash.color & 255,
                    (u8)((flash.alpha > 1 ? 1 : flash.alpha) * 255)));

        Hud flat = {hud.zoom, false};
        for (int k = 0; k < MAX_POPUPS; k++) {
            Popup* p = &pops[k];
            if (!p->on) continue;
            float al = p->t < p->fadeStart ? 1 : 1 - (p->t - p->fadeStart) / p->fadeDur;
            const SprFrame* fr = &gMisc.frames[gMisc.anims[p->anim].frames[0]];
            draw_frame(fr, hx(&flat, p->x), hy(&flat, p->y), hud.zoom, false, al);
        }

        /* flèches : apparition y-10 et alpha 0 -> 1 en 1 s (circOut), décalées de 0,5 + 0,2 i s */
        float pxPerMs = 0.45f * chart.speed;
        for (int s = 0; s < 2; s++) {
            const Spr* sk = skin_for(s);
            for (int l = 0; l < 4; l++) {
                float t0 = introTime / 1000.0f - (0.5f + 0.2f * l);
                float k = t0 <= 0 ? 0 : t0 >= 1 ? 1 : ease_circ_out(t0);
                int anim = strums[s][l].anim == 2 ? 8 + l : strums[s][l].anim == 1 ? 4 + l : l;
                if (anim >= sk->animCount || sk->anims[anim].count == 0) anim = l;
                int fi = (int)(strums[s][l].time * 24 / 1000.0f);
                if (anim < sk->animCount && sk->anims[anim].count && fi >= sk->anims[anim].count) fi = sk->anims[anim].count - 1;
                draw_centered(&hud, sk, anim, fi, lane_cx(s, l), STRUM_CY - 10 + 10 * k, 1.0f, k, false);
            }
            Note* ns = chart.notes[s];
            int from = nextNote[s] > 8 ? nextNote[s] - 8 : 0;
            for (int i = from; i < chart.noteCount[s]; i++) {
                Note* n = &ns[i];
                if (n->hidden) continue;
                float y = STRUM_CY + (n->time - pos) * pxPerMs;
                if (y > 720 + 150) break;
                float cx = lane_cx(s, n->lane);
                if (n->sLen > 0 && !n->holdDone && !(n->missed && pos > n->time + n->sLen)) {
                    float y0 = n->hit ? STRUM_CY : y;
                    float y1 = STRUM_CY + (n->time + n->sLen - pos) * pxPerMs;
                    if (y1 > y0) draw_hold(&hud, sk, n->lane, cx, y0, y1, n->missed ? 0.3f : 0.6f);
                }
                if (n->hit || y < -150) continue;
                draw_centered(&hud, sk, 12 + n->lane, 0, cx, y, 1.0f, n->missed ? 0.3f : 1.0f, false);
            }
        }
        for (int k = 0; k < 8; k++) {
            Splash* sp = &splashes[k];
            if (!sp->on || sp->anim < 0) continue;
            draw_centered(&hud, &gSplash, sp->anim, (int)(sp->t * 24 / 1000), lane_cx(sp->side, sp->lane), STRUM_CY, 1.0f,
                0.6f, false);
        }

        /* barre de vie (y = 0,9 x hauteur), icônes et textes */
        int aBar = spr_anim(&gMisc, "healthBar");
        if (aBar >= 0) {
            const SprFrame* bfr = &gMisc.frames[gMisc.anims[aBar].frames[0]];
            float bw = bfr->fw / WORLD_S, bh = bfr->fh / WORLD_S;
            float bx = (1280 - bw) / 2, by = 720 * 0.9f;
            float z = hud.zoom;
            float barMid = by + bh / 2;
            float tY = hy(&hud, barMid) - bh * WORLD_S * z / 2;
            draw_frame(bfr, hx(&hud, bx), tY, z, false, 1.0f);
            float ix = bx + 4, iw = bw - 8;
            float center = ix + iw * (1.0f - health / 2.0f);
            float y0 = tY + 4 * WORLD_S * z, hh = (bh - 8) * WORLD_S * z;
            if (hh < 1) hh = 1;
            C2D_DrawRectSolid(hx(&hud, ix), y0, 0.5f, (center - ix) * WORLD_S * z, hh, C2D_Color32(255, 0, 0, 255));
            C2D_DrawRectSolid(hx(&hud, center), y0, 0.5f, (ix + iw - center) * WORLD_S * z, hh, C2D_Color32(102, 255, 51, 255));
            if (aIconD >= 0) {
                float w = gIcons.frames[gIcons.anims[aIconD].frames[0]].fw / WORLD_S;
                int nD = gIcons.anims[aIconD].count;
                int fD = health > 1.6f ? (nD > 1 ? 1 : 0) : (health < 0.4f && nD > 2) ? 2 : 0;
                draw_centered(&hud, &gIcons, aIconD, fD, center - (w - 26) + w / 2, barMid, iconScale, 1.0f, false);
            }
            if (aIconP >= 0) {
                float w = gIcons.frames[gIcons.anims[aIconP].frames[0]].fw / WORLD_S;
                int nP = gIcons.anims[aIconP].count;
                int fP = health < 0.4f ? (nP > 1 ? 1 : 0) : (health > 1.6f && nP > 2) ? 2 : 0;
                draw_centered(&hud, &gIcons, aIconP, fP, center - 26 + w / 2, barMid, iconScale, 1.0f, true);
            }
            float acc = pressedNotes ? accSum / pressedNotes : -1;
            float ty = hud.down ? 720 - (by + 30) - 18 : by + 30;
            uint32_t white = C2D_Color32(255, 255, 255, 255);
            char accs[32];
            if (acc < 0) snprintf(accs, sizeof(accs), "-%%");
            else snprintf(accs, sizeof(accs), "%.2f%%", acc * 100);
            hud_text(bx + 10, ty, 26, 0, white, true, "Accuracy:%s - [%s]", accs, rank_of(acc));
            hud_text(640, ty, 26, 1, white, true, "Misses:%d", misses);
            hud_text(bx + bw - 10, ty, 26, 2, white, true, "Score:%d", score);
        }

        /* ready / set / go : descendent de 100 px et s'effacent sur un temps */
        if (tick >= 1 && tick <= 3 && songPos < crochet0) {
            static const char* CD[4] = {NULL, "ready", "set", "go"};
            float in = (float)(songPos + crochet0 * 5.0) / crochet0 - 1 - tick;
            int ca = spr_anim(&gMisc, CD[tick]);
            float k = ease_cube_in_out(in < 0 ? 0 : in > 1 ? 1 : in);
            Hud one = {1.0f, false};
            if (ca >= 0) draw_centered(&one, &gMisc, ca, 0, 640, 360 + 100 * k, 1.0f, 1 - k, false);
        }
        if (dead) {
            C2D_DrawRectSolid(0, 0, 0.6f, SCREEN_W, SCREEN_H, C2D_Color32(0, 0, 0, 200));
            draw_text_centered(200, 90, 0.9f, C2D_Color32(255, 60, 60, 255), "GAME OVER");
            draw_text_centered(200, 140, 0.5f, C2D_Color32(255, 255, 255, 255), "A : retry    B : exit");
        }

        /* ---- écran du bas */
        C2D_TargetClear(bottom, C2D_Color32(14, 12, 30, 255));
        C2D_SceneBegin(bottom);
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 36, C2D_Color32(247, 1, 116, 255));
        draw_text_centered(160, 8, 0.6f, C2D_Color32(255, 255, 255, 255), "%s", chart.name);
        float accP = pressedNotes ? accSum * 100.0f / pressedNotes : 0.0f;
        draw_text(16, 50, 0.55f, C2D_Color32(255, 255, 255, 255), "Score : %d", score);
        draw_text(16, 75, 0.55f, C2D_Color32(255, 255, 255, 255), "Misses : %d", misses);
        draw_text(16, 100, 0.55f, C2D_Color32(255, 255, 255, 255), "Combo : %d", combo);
        draw_text(16, 125, 0.55f, C2D_Color32(255, 255, 255, 255), "Accuracy : %.2f%%  [%s]", accP,
            rank_of(pressedNotes ? accSum / pressedNotes : -1));
        float len = haveInst ? inst.samples * 1000.0f / inst.rate : 1;
        float prog = started ? pos / len : 0;
        if (prog > 1) prog = 1;
        C2D_DrawRectSolid(16, 160, 0.5f, 288, 8, C2D_Color32(60, 60, 90, 255));
        C2D_DrawRectSolid(16, 160, 0.5f, 288 * prog, 8, C2D_Color32(2, 244, 240, 255));
        draw_text(16, 180, 0.42f, C2D_Color32(180, 180, 220, 255), "Arrow buttons : Options > Touches");
        draw_text(16, 198, 0.42f, C2D_Color32(180, 180, 220, 255), "START : pause");
        if (paused) {
            C2D_DrawRectSolid(0, 0, 0.6f, 320, 240, C2D_Color32(0, 0, 0, 210));
            static const char* PM[3] = {"Resume", "Restart Song", "Exit to menu"};
            for (int i = 0; i < 3; i++)
                draw_text_centered(160, 70 + i * 35, 0.7f,
                    i == pauseSel ? C2D_Color32(255, 230, 80, 255) : C2D_Color32(200, 200, 200, 255), "%s", PM[i]);
        }
        C3D_FrameEnd(0);
        if (gAutoTest) {
            static int frames;
            static u64 t0;
            frames++;
            if (!t0) t0 = now;
            double el = (now - t0) * 1.0 / SYSCLOCK_ARM11;
            if (frames % 120 == 0)
                flog("t=%.1fs pos=%.0fms img/s=%.1f cpu=%.1fms gpu=%.1fms vie=%.2f misses=%d score=%d lin=%luKo", el, pos,
                    frames / el, C3D_GetProcessingTime(), C3D_GetDrawingTime(), health, misses, score,
                    (unsigned long)(linearSpaceFree() / 1024));
            if (frames == 60 || frames == 200 || frames == 700 || frames == 1500) {
                gspWaitForVBlank();
                char sp[300];
                snprintf(sp, sizeof(sp), "sdmc:/3ds/fnf3ds/shot_%05d.bmp", frames);
                screenshot(sp);
            }
            /* captures à des positions fixes de la chanson : mêmes instants que tools/pc_ref (jeu PC) */
            static const int AT_MS[] = {5000, 15000, 30000, 45000, 60000, 90000, 120000, 140000, 150000};
            static int nextShot;
            if (nextShot < (int)(sizeof(AT_MS) / sizeof(AT_MS[0])) && pos >= AT_MS[nextShot]) {
                gspWaitForVBlank();
                char sp[300];
                snprintf(sp, sizeof(sp), "sdmc:/3ds/fnf3ds/shot_p%05d.bmp", AT_MS[nextShot]);
                screenshot(sp);
                nextShot++;
            }
            if (el > gAutoSeconds) {
                result = GAME_QUIT;
                break;
            }
        }
    }

    stream_close(&inst);
    stream_close(&voices);
    sfx_stop_all();
    for (int i = 0; i < MAX_CHARS; i++)
        if (chars[i].loaded) spr_free(&chars[i].spr);
    for (int side = 0; side < 2; side++)
        if (gSkinLoaded[side]) {
            spr_free(&gNotesSkin[side]);
            gSkinLoaded[side] = false;
        }
    for (int i = 0; i < stageCount; i++)
        if (!stage[i].isChar) spr_free(&stage[i].spr);
    free(stage);
    free(stepEv);
    chart_free(&chart);
    if (outScore) *outScore = score;
    if (outAccuracy) *outAccuracy = pressedNotes ? accSum * 100.0f / pressedNotes : 0.0f;
    return result;
}
