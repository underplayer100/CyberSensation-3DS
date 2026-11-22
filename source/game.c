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

typedef struct {
    float time;
    uint8_t lane, type;
    float sLen;
    bool hit, missed, holding, holdDone;
} Note;

typedef struct {
    float time;
    uint8_t kind;
    float value;
} Event;

typedef struct {
    Spr spr;
    bool loaded;
    float x, y, scx, scy;   /* position monde et défilement (parallaxe) */
    bool flip, swapLR, isPlayerSide, gfDance;
    int anim;
    float animTime;
    bool animDone;
    float holdTimer;        /* ms depuis la dernière note chantée */
    float holdTime;
    bool singing, danced;
    float camx, camy;
    float baseW, baseH;     /* taille du premier cadre (monde) */
    int aIdle, aLeft, aRight, aSing[4], aMiss[4];
} Character;

typedef struct {
    Spr spr;
    bool isChar;
    int charIndex;          /* 0 bf, 1 dad, 2 gf */
    float x, y, scx, scy, scale, alpha;
    bool flip;
    float animTime;
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
            no->type = p[5];
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

/* ---------------------------------------------------------------- personnages */
static const char* SING[4] = {"singLEFT", "singDOWN", "singUP", "singRIGHT"};

static void char_play(Character* ch, int anim) {
    if (anim < 0) return;
    ch->anim = anim;
    ch->animTime = 0;
    ch->animDone = false;
}

static bool char_load(Character* ch, const char* name, bool playerSide) {
    memset(ch, 0, sizeof(*ch));
    if (!name[0]) return false;
    char path[384], tex[320];
    snprintf(path, sizeof(path), "%s/chars/%s.spr", gModDir, name);
    snprintf(tex, sizeof(tex), "%s/tex", gModDir);
    if (!spr_load(&ch->spr, path, tex)) return false;
    ch->loaded = true;
    ch->isPlayerSide = playerSide;
    bool isPlayer = spr_metaf(&ch->spr, "isPlayer", 0) != 0;
    bool flipX = spr_metaf(&ch->spr, "flipX", 0) != 0;
    ch->flip = flipX != playerSide;
    ch->swapLR = isPlayer != playerSide;
    ch->holdTime = spr_metaf(&ch->spr, "holdTime", 4);
    ch->camx = spr_metaf(&ch->spr, "camx", 0);
    ch->camy = spr_metaf(&ch->spr, "camy", 0);
    ch->gfDance = spr_metaf(&ch->spr, "gfDance", 0) != 0;
    ch->aIdle = spr_anim(&ch->spr, "idle");
    ch->aLeft = spr_anim(&ch->spr, "danceLeft");
    ch->aRight = spr_anim(&ch->spr, "danceRight");
    for (int i = 0; i < 4; i++) {
        int src = ch->swapLR ? (i == 0 ? 3 : i == 3 ? 0 : i) : i;
        char m[40];
        ch->aSing[i] = spr_anim(&ch->spr, SING[src]);
        snprintf(m, sizeof(m), "%smiss", SING[src]);
        ch->aMiss[i] = spr_anim(&ch->spr, m);
        if (ch->aSing[i] < 0) ch->aSing[i] = ch->aIdle;
    }
    int first = ch->aIdle >= 0 ? ch->aIdle : (ch->aLeft >= 0 ? ch->aLeft : 0);
    if (ch->spr.animCount > 0 && ch->spr.anims[first].count > 0) {
        SprFrame* fr = &ch->spr.frames[ch->spr.anims[first].frames[0]];
        float sc = WORLD_S * spr_metaf(&ch->spr, "scale", 1);
        ch->baseW = fr->fw / WORLD_S;
        ch->baseH = fr->fh / WORLD_S;
        (void)sc;
    }
    char_play(ch, first);
    return true;
}

static void char_dance(Character* ch, int beat) {
    if (!ch->loaded || ch->singing) return;
    if (ch->gfDance || (ch->aLeft >= 0 && ch->aRight >= 0)) {
        char_play(ch, (beat % 2) ? ch->aLeft : ch->aRight);
    } else if (beat % 2 == 0) {
        char_play(ch, ch->aIdle);
    }
}

static void char_sing(Character* ch, int lane, bool miss) {
    if (!ch->loaded) return;
    int a = miss ? ch->aMiss[lane] : ch->aSing[lane];
    if (a < 0) a = ch->aSing[lane];
    char_play(ch, a);
    ch->singing = true;
    ch->holdTimer = 0;
}

static void char_update(Character* ch, float dt, float stepCrochet, bool keysHeld) {
    if (!ch->loaded) return;
    SprAnim* a = &ch->spr.anims[ch->anim];
    ch->animTime += dt;
    float len = a->fps > 0 ? a->count * 1000.0f / a->fps : 0;
    if (!a->loop && ch->animTime >= len) ch->animDone = true;
    if (ch->singing) {
        ch->holdTimer += dt;
        if (ch->holdTimer >= stepCrochet * ch->holdTime && !(ch->isPlayerSide && keysHeld)) {
            ch->singing = false;
            int idle = ch->aIdle >= 0 ? ch->aIdle : ch->aRight;
            char_play(ch, idle);
            ch->animTime = len > 0 ? 100000.0f : 0;   /* reste sur la dernière image jusqu'au prochain temps */
            ch->animTime = 0;
        }
    }
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

static void draw_frame(const SprFrame* fr, float sx, float sy, float scale, bool flip, float alpha) {
    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, alpha);
    float w = fr->sub.width * scale, h = fr->sub.height * scale;
    float ox = flip ? (fr->fw - fr->ox - fr->sub.width) : fr->ox;
    C2D_DrawParams p = {
        .pos = {sx + ox * scale + (flip ? w : 0), sy + fr->oy * scale, flip ? -w : w, h},
        .center = {0, 0}, .depth = 0.5f, .angle = 0};
    if (sx + ox * scale > SCREEN_W + 8 || sx + ox * scale + w < -8 || sy + fr->oy * scale > SCREEN_H + 8 ||
        sy + fr->oy * scale + h < -8)
        return;
    C2D_DrawImage(fr->img, &p, alpha < 1.0f ? &tint : NULL);
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

static void char_draw(const Character* ch, const Camera* cam) {
    if (!ch->loaded) return;
    const SprFrame* fr = anim_frame(&ch->spr, ch->anim, ch->animTime, false);
    if (!fr) return;
    const SprAnim* a = &ch->spr.anims[ch->anim];
    float ox = ch->swapLR ? -a->ox : a->ox;
    float sx, sy;
    world_to_screen(cam, ch->x - ox, ch->y - a->oy, ch->scx, ch->scy, &sx, &sy);
    draw_frame(fr, sx, sy, cam->zoom, ch->flip, 1.0f);
}

static void char_camera(const Character* ch, float* x, float* y) {
    float mx = ch->x + ch->baseW / 2, my = ch->y + ch->baseH / 2;
    float cx = ch->swapLR ? -ch->camx : ch->camx;
    *x = mx + (ch->isPlayerSide ? -100.0f : 150.0f) + cx;
    *y = my - 100.0f + ch->camy;
}

/* ---------------------------------------------------------------- HUD */
static Spr gNotes, gMisc, gIcons;
static bool gUiLoaded;

static void ui_load(void) {
    if (gUiLoaded) return;
    char p[320], t[320];
    snprintf(t, sizeof(t), "%s/tex", gModDir);
    snprintf(p, sizeof(p), "%s/ui/notes.spr", gModDir);
    spr_load(&gNotes, p, t);
    snprintf(p, sizeof(p), "%s/ui/misc.spr", gModDir);
    spr_load(&gMisc, p, t);
    snprintf(p, sizeof(p), "%s/ui/icons.spr", gModDir);
    spr_load(&gIcons, p, t);
    gUiLoaded = true;
}

#define HUD_S WORLD_S
#define HUD_Y0 ((SCREEN_H - 720.0f * HUD_S) / 2.0f)
#define LANE_W (160.0f * 0.7f)

static float lane_x(int side, int lane) {
    float base = 1280.0f * (side ? 0.75f : 0.25f) - LANE_W * 2.0f;
    return (base + lane * LANE_W + LANE_W / 2.0f) * HUD_S;
}
static const float STRUM_Y = 50.0f;

static void draw_centered(const Spr* s, int anim, int frameIdx, float cx, float cy, float scale, float alpha) {
    if (anim < 0 || anim >= s->animCount || s->anims[anim].count == 0) return;
    const SprAnim* a = &s->anims[anim];
    const SprFrame* fr = &s->frames[a->frames[frameIdx % a->count]];
    float w = fr->fw * scale, h = fr->fh * scale;
    draw_frame(fr, cx - w / 2, cy - h / 2, scale, false, alpha);
}

static void draw_hold(int lane, float cx, float y0, float y1, float alpha) {
    /* morceau répété étiré entre y0 et y1, puis bout de la tenue */
    if (y1 <= y0) return;
    int piece = 16 + lane, end = 20 + lane;
    if (piece >= gNotes.animCount || gNotes.anims[piece].count == 0) return;
    const SprFrame* pf = &gNotes.frames[gNotes.anims[piece].frames[0]];
    const SprFrame* ef = gNotes.anims[end].count ? &gNotes.frames[gNotes.anims[end].frames[0]] : NULL;
    float endH = ef ? ef->sub.height : 0;
    float bodyEnd = y1 - endH;
    if (bodyEnd > y0) {
        C2D_ImageTint tint;
        C2D_AlphaImageTint(&tint, alpha);
        C2D_DrawParams p = {.pos = {cx - pf->sub.width / 2.0f, y0, pf->sub.width, bodyEnd - y0}, .depth = 0.5f};
        C2D_DrawImage(pf->img, &p, &tint);
    }
    if (ef) {
        float ey = bodyEnd > y0 ? bodyEnd : y0;
        float eh = y1 - ey;
        C2D_ImageTint tint;
        C2D_AlphaImageTint(&tint, alpha);
        C2D_DrawParams p = {.pos = {cx - ef->sub.width / 2.0f, ey, ef->sub.width, eh}, .depth = 0.5f};
        C2D_DrawImage(ef->img, &p, &tint);
    }
}

/* ---------------------------------------------------------------- partie */
typedef struct {
    int anim;          /* 0 statique, 1 appui, 2 confirmation */
    float time;
} Strum;

static const u32 LANE_KEYS[4] = {
    KEY_DLEFT | KEY_Y | KEY_CPAD_LEFT,
    KEY_DDOWN | KEY_B | KEY_CPAD_DOWN,
    KEY_DUP | KEY_X | KEY_CPAD_UP,
    KEY_DRIGHT | KEY_A | KEY_CPAD_RIGHT,
};

static bool stage_load(StageItem** items, int* count, float* zoom, float charPos[3][4], const char* name) {
    *items = NULL;
    *count = 0;
    *zoom = 1.0f;
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
            *zoom = rdf(h + 8);
            for (int i = 0; i < 3; i++) {
                float v[4];
                if (fread(v, 4, 4, f) == 4) memcpy(charPos[i], v, sizeof(v));
            }
            uint32_t cnt = 0;
            if (fread(&cnt, 4, 1, f) == 1) {
                for (uint32_t i = 0; i < cnt && n < MAX_STAGE_SPRITES; i++) {
                    uint8_t e[58];
                    if (fread(e, 1, 58, f) != 58) break;
                    StageItem* s = &it[n];
                    char nm[33];
                    memcpy(nm, e + 1, 32);
                    nm[32] = 0;
                    if (e[0] == 1) {
                        s->isChar = true;
                        s->charIndex = strcmp(nm, "bf") == 0 ? 0 : strcmp(nm, "dad") == 0 ? 1 : 2;
                        haveChar[s->charIndex] = true;
                        n++;
                        continue;
                    }
                    s->x = rdf(e + 33);
                    s->y = rdf(e + 37);
                    s->scx = rdf(e + 41);
                    s->scy = rdf(e + 45);
                    s->scale = rdf(e + 49);
                    s->alpha = rdf(e + 53);
                    s->flip = e[57] != 0;
                    char sp[400];
                    snprintf(sp, sizeof(sp), "%s/stages/%s.spr", gModDir, nm);
                    if (spr_load(&s->spr, sp, tex)) n++;
                }
            }
        }
        fclose(f);
    }
    /* personnages absents du décor : gf derrière, puis dad, puis bf */
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

static int text_score(char* buf, size_t n, int score, int misses, float acc) {
    return snprintf(buf, n, "Score: %d | Rates: %d | Precision: %.2f%%", score, misses, acc);
}

GameResult game_play(const SongEntry* song, C3D_RenderTarget* top, C3D_RenderTarget* bottom,
    int* outScore, float* outAccuracy) {
    char path[384];
    Chart chart;
    snprintf(path, sizeof(path), "%s/songs/%s/chart.sng", gModDir, song->id);
    /* écran de chargement */
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32(0, 0, 0, 255));
    C2D_SceneBegin(top);
    C2D_TextBufClear(gTextBuf);
    draw_text_centered(200, 110, 0.7f, C2D_Color32(255, 255, 255, 255), "Chargement...");
    C2D_TargetClear(bottom, C2D_Color32(0, 0, 0, 255));
    C3D_FrameEnd(0);

    if (!chart_load(&chart, path)) return GAME_QUIT;
    conductor_init(&chart);
    ui_load();

    StageItem* stage;
    int stageCount;
    float stageZoom, charPos[3][4];
    stage_load(&stage, &stageCount, &stageZoom, charPos, chart.stage);

    Character chars[3];   /* 0 bf, 1 dad, 2 gf */
    char_load(&chars[0], chart.chars[1], true);
    char_load(&chars[1], chart.chars[0], false);
    char_load(&chars[2], chart.chars[2], false);
    for (int i = 0; i < 3; i++) {
        float cx = 0, cy = 0;
        if (chars[i].loaded) {
            cx = spr_metaf(&chars[i].spr, "x", 0);
            cy = spr_metaf(&chars[i].spr, "y", 0);
        }
        chars[i].x = charPos[i][0] + cx;
        chars[i].y = charPos[i][1] + cy;
        chars[i].scx = charPos[i][2];
        chars[i].scy = charPos[i][3];
    }
    const char* iconP = chars[0].loaded ? spr_metas(&chars[0].spr, "icon", "bf") : "bf";
    const char* iconD = chars[1].loaded ? spr_metas(&chars[1].spr, "icon", "dad") : "dad";
    int aIconP = spr_anim(&gIcons, iconP), aIconD = spr_anim(&gIcons, iconD);

    Stream inst, voices;
    snprintf(path, sizeof(path), "%s/songs/%s/inst.adp", gModDir, song->id);
    bool haveInst = stream_open(&inst, path, 0);
    snprintf(path, sizeof(path), "%s/songs/%s/voices.adp", gModDir, song->id);
    bool haveVoices = stream_open(&voices, path, 2);

    Camera cam = {0, 0, stageZoom};
    float camTX, camTY;
    char_camera(chars[1].loaded ? &chars[1] : &chars[0], &camTX, &camTY);
    cam.x = camTX;
    cam.y = camTY;
    float camZoomBump = 0, hudZoomBump = 0;

    Strum strums[2][4];
    memset(strums, 0, sizeof(strums));
    float health = 1.0f;
    int score = 0, misses = 0, combo = 0, hits = 0;
    float accSum = 0;
    int nextNote[2] = {0, 0};
    int nextEvent = 0;
    float ratingTime = -1;
    int ratingAnim = -1;
    int lastBeat = -1;
    bool started = false, paused = false, dead = false;
    int pauseSel = 0;
    GameResult result = GAME_DONE;
    float crochet0 = conductor_crochet(0);
    double songPos = -crochet0 * 5.0;
    u64 lastTick = svcGetSystemTick();
    double lastAudio = -1, audioBase = 0;
    u64 audioTick = 0;
    bool altAnim = false;
    (void)altAnim;

    while (aptMainLoop()) {
        u64 now = svcGetSystemTick();
        float dt = (float)((now - lastTick) * 1000.0 / SYSCLOCK_ARM11);
        lastTick = now;
        if (dt > 100) dt = 100;
        hidScanInput();
        u32 down = hidKeysDown(), held = hidKeysHeld();
        if (gAutoTest) {
            /* robot : appuie sur chaque note du joueur au bon moment et tient les tenues */
            down = held = 0;
            for (int i = nextNote[1]; i < chart.noteCount[1]; i++) {
                Note* n = &chart.notes[1][i];
                if (n->time > songPos + 5) break;
                if (!n->hit && !n->missed && n->time >= songPos - 30) down |= LANE_KEYS[n->lane] & (KEY_DLEFT | KEY_DDOWN | KEY_DUP | KEY_DRIGHT);
                if (n->hit && n->holding) held |= LANE_KEYS[n->lane] & (KEY_DLEFT | KEY_DDOWN | KEY_DUP | KEY_DRIGHT);
            }
            held |= down;
        }

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
        } else if (down & KEY_START && !dead) {
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
                    (!haveInst && songPos > chart.notes[1][chart.noteCount[1] - 1].time + 2000)) {
                    result = GAME_DONE;
                    break;
                }
            }
        }
        float pos = (float)songPos;
        float crochet = conductor_crochet(pos > 0 ? pos : 0);
        float stepCrochet = crochet / 4.0f;

        /* ---- événements */
        while (nextEvent < chart.eventCount && chart.events[nextEvent].time <= pos) {
            Event* e = &chart.events[nextEvent++];
            if (e->kind == 0) {
                int t = (int)e->value;
                Character* c = t == 1 ? &chars[0] : t == 2 ? &chars[2] : &chars[1];
                if (c->loaded) char_camera(c, &camTX, &camTY);
            } else if (e->kind == 2) {
                altAnim = e->value != 0;
            }
        }

        /* ---- temps forts */
        int beat = (int)floorf(conductor_beat(pos));
        if (beat != lastBeat && !paused) {
            lastBeat = beat;
            for (int i = 0; i < 3; i++) char_dance(&chars[i], beat < 0 ? -beat : beat);
            if (started && beat % 4 == 0) {
                camZoomBump += 0.015f;
                hudZoomBump += 0.03f;
            }
        }

        if (!paused && !dead) {
            /* ---- adversaire : notes jouées automatiquement */
            for (int i = nextNote[0]; i < chart.noteCount[0]; i++) {
                Note* n = &chart.notes[0][i];
                if (n->time > pos) break;
                if (!n->hit) {
                    n->hit = true;
                    char_sing(&chars[1], n->lane, false);
                    strums[0][n->lane].anim = 2;
                    strums[0][n->lane].time = 0;
                    if (haveVoices) stream_volume(&voices, 1.0f);
                }
                if (n->time + n->sLen < pos) nextNote[0] = i + 1;
                else {
                    /* tenue : l'adversaire continue de chanter */
                    if (n->sLen > 0 && pos < n->time + n->sLen) {
                        chars[1].holdTimer = 0;
                        strums[0][n->lane].anim = 2;
                    }
                }
            }
            for (int l = 0; l < 4; l++)
                if (strums[0][l].anim == 2 && strums[0][l].time > 150) strums[0][l].anim = 0;

            /* ---- joueur */
            bool anyHeld = false;
            for (int l = 0; l < 4; l++) {
                bool pressed = (down & LANE_KEYS[l]) != 0;
                bool isHeld = (held & LANE_KEYS[l]) != 0;
                if (isHeld) anyHeld = true;
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
                        float a;
                        if (diff <= 45) r = 1, pts = 350, a = 1.0f;
                        else if (diff <= 90) r = 2, pts = 200, a = 0.75f;
                        else if (diff <= 135) r = 3, pts = 100, a = 0.45f;
                        else r = 4, pts = 50, a = 0.1f;
                        score += pts;
                        combo++;
                        hits++;
                        accSum += a;
                        health += 0.023f;
                        ratingAnim = r;
                        ratingTime = 0;
                        char_sing(&chars[0], l, false);
                        strums[1][l].anim = 2;
                        strums[1][l].time = 0;
                        if (haveVoices) stream_volume(&voices, 1.0f);
                    } else {
                        strums[1][l].anim = 1;
                        strums[1][l].time = 0;
                    }
                }
                if (!isHeld && strums[1][l].anim != 0 && strums[1][l].time > 60) strums[1][l].anim = 0;
            }
            /* tenues et notes ratées */
            for (int i = nextNote[1]; i < chart.noteCount[1]; i++) {
                Note* n = &chart.notes[1][i];
                if (n->time > pos + HIT_WINDOW) break;
                if (n->hit && n->holding) {
                    if (held & LANE_KEYS[n->lane]) {
                        if (pos < n->time + n->sLen) {
                            chars[0].holdTimer = 0;
                            strums[1][n->lane].anim = 2;
                            health += 0.0002f * dt;
                        } else {
                            n->holding = false;
                            n->holdDone = true;
                        }
                    } else if (pos < n->time + n->sLen - stepCrochet) {
                        n->holding = false;   /* lâchée trop tôt */
                        n->holdDone = true;
                    } else {
                        n->holding = false;
                        n->holdDone = true;
                    }
                }
                if (!n->hit && !n->missed && pos > n->time + HIT_WINDOW) {
                    n->missed = true;
                    misses++;
                    combo = 0;
                    hits++;
                    health -= 0.0475f;
                    score -= 10;
                    char_sing(&chars[0], n->lane, true);
                    if (haveVoices) stream_volume(&voices, 0.0f);
                }
            }
            while (nextNote[1] < chart.noteCount[1]) {
                Note* n = &chart.notes[1][nextNote[1]];
                bool over = (n->missed || n->hit) && !n->holding && pos > n->time + n->sLen + 200;
                if (!over) break;
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

            for (int i = 0; i < 3; i++) char_update(&chars[i], dt, stepCrochet, i == 0 && anyHeld);
            for (int i = 0; i < stageCount; i++) stage[i].animTime += dt;
            if (ratingTime >= 0) ratingTime += dt;
        }

        /* ---- caméra */
        float lerp = 1.0f - powf(0.96f, dt / 16.67f);
        cam.x += (camTX - cam.x) * lerp;
        cam.y += (camTY - cam.y) * lerp;
        float back = 1.0f - powf(0.95f, dt / 16.67f);
        camZoomBump -= camZoomBump * back;
        hudZoomBump -= hudZoomBump * back;
        cam.zoom = stageZoom + camZoomBump;

        /* ---- dessin écran du haut */
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
            const SprFrame* fr = anim_frame(&s->spr, 0, s->animTime, true);
            if (!fr) continue;
            const SprAnim* a = &s->spr.anims[0];
            float sx, sy;
            world_to_screen(&cam, s->x - a->ox, s->y - a->oy, s->scx, s->scy, &sx, &sy);
            draw_frame(fr, sx, sy, cam.zoom * s->scale, s->flip, s->alpha);
        }

        /* HUD : flèches */
        float hz = 1.0f + hudZoomBump;
#define HX(x) (((x) - SCREEN_W / 2.0f) * hz + SCREEN_W / 2.0f)
#define HY(y) (((y) - SCREEN_H / 2.0f) * hz + SCREEN_H / 2.0f)
        float strumY = HUD_Y0 + (STRUM_Y + LANE_W / 2.0f) * HUD_S;
        float pxPerMs = 0.45f * chart.speed * HUD_S;
        for (int s = 0; s < 2; s++) {
            for (int l = 0; l < 4; l++) {
                int anim = strums[s][l].anim == 2 ? 8 + l : strums[s][l].anim == 1 ? 4 + l : l;
                int fi = (int)(strums[s][l].time * 24 / 1000.0f);
                if (anim >= gNotes.animCount || gNotes.anims[anim].count == 0) anim = l;
                if (fi >= gNotes.anims[anim].count) fi = gNotes.anims[anim].count - 1;
                draw_centered(&gNotes, anim, fi, HX(lane_x(s, l)), HY(strumY), hz, 1.0f);
            }
            Note* ns = chart.notes[s];
            int from = nextNote[s] > 8 ? nextNote[s] - 8 : 0;
            for (int i = from; i < chart.noteCount[s]; i++) {
                Note* n = &ns[i];
                float y = strumY + (n->time - pos) * pxPerMs;
                if (y > SCREEN_H + 40) break;
                float cx = lane_x(s, n->lane);
                if (n->sLen > 0 && !n->holdDone && !(n->missed && pos > n->time + n->sLen)) {
                    float y0 = n->hit ? strumY : y;
                    float y1 = strumY + (n->time + n->sLen - pos) * pxPerMs;
                    if (y1 > y0) draw_hold(n->lane, HX(cx), HY(y0), HY(y1), n->missed ? 0.3f : 0.6f);
                }
                if (n->hit) continue;
                if (y < -40) continue;
                draw_centered(&gNotes, 12 + n->lane, 0, HX(cx), HY(y), hz, n->missed ? 0.3f : 1.0f);
            }
        }

        /* barre de vie + icônes */
        int aBar = spr_anim(&gMisc, "healthBar");
        float barY = HUD_Y0 + 720.0f * 0.89f * HUD_S;
        if (aBar >= 0) {
            const SprFrame* bf = &gMisc.frames[gMisc.anims[aBar].frames[0]];
            float bw = bf->sub.width, bh = bf->sub.height;
            float bx = (SCREEN_W - bw) / 2;
            draw_frame(bf, HX(bx), HY(barY - bh / 2), hz, false, 1.0f);
            float inner = bw - 2;
            float split = bx + 1 + inner * (1.0f - health / 2.0f);
            C2D_DrawRectSolid(HX(bx + 1), HY(barY - bh / 2 + 1), 0.5f, (split - bx - 1) * hz, (bh - 2) * hz,
                C2D_Color32(255, 0, 0, 255));
            C2D_DrawRectSolid(HX(split), HY(barY - bh / 2 + 1), 0.5f, (bx + 1 + inner - split) * hz, (bh - 2) * hz,
                C2D_Color32(102, 255, 51, 255));
            float iconScale = hz * (1.0f + hudZoomBump * 2);
            if (aIconD >= 0) draw_centered(&gIcons, aIconD, health > 1.6f ? 1 : 0, HX(split - 14), HY(barY), iconScale, 1.0f);
            if (aIconP >= 0) {
                /* icône du joueur retournée vers la gauche */
                const SprAnim* a = &gIcons.anims[aIconP];
                const SprFrame* fr = &gIcons.frames[a->frames[(health < 0.4f ? 1 : 0) % a->count]];
                draw_frame(fr, HX(split + 14) - fr->fw * iconScale / 2, HY(barY) - fr->fh * iconScale / 2, iconScale, true, 1.0f);
            }
        }
        char sc[128];
        text_score(sc, sizeof(sc), score, misses, hits ? accSum * 100.0f / hits : 0.0f);
        draw_text_centered(200, barY + 9, 0.38f, C2D_Color32(255, 255, 255, 255), "%s", sc);

        /* jugement */
        if (ratingTime >= 0 && ratingTime < 600) {
            static const char* RN[5] = {"", "sick", "good", "bad", "shit"};
            int ra = spr_anim(&gMisc, RN[ratingAnim]);
            float al = ratingTime < 300 ? 1.0f : 1.0f - (ratingTime - 300) / 300.0f;
            if (ra >= 0) draw_centered(&gMisc, ra, 0, 200, 110 - ratingTime * 0.02f, 1.0f, al);
        }
        /* compte à rebours */
        if (!started) {
            int k = (int)floorf(-songPos / crochet0);
            static const char* CD[3] = {"go", "set", "ready"};
            if (k >= 0 && k < 3) {
                int ca = spr_anim(&gMisc, CD[k]);
                if (ca >= 0) draw_centered(&gMisc, ca, 0, 200, 120, 1.0f, 1.0f);
            }
        }
        if (dead) {
            C2D_DrawRectSolid(0, 0, 0.6f, SCREEN_W, SCREEN_H, C2D_Color32(0, 0, 0, 200));
            draw_text_centered(200, 90, 0.9f, C2D_Color32(255, 60, 60, 255), "GAME OVER");
            draw_text_centered(200, 140, 0.5f, C2D_Color32(255, 255, 255, 255), "A : recommencer    B : quitter");
        }

        /* ---- écran du bas */
        C2D_TargetClear(bottom, C2D_Color32(20, 16, 40, 255));
        C2D_SceneBegin(bottom);
        draw_text_centered(160, 12, 0.7f, C2D_Color32(255, 255, 255, 255), "%s", chart.name);
        draw_text(12, 50, 0.55f, C2D_Color32(255, 255, 255, 255), "Score : %d", score);
        draw_text(12, 75, 0.55f, C2D_Color32(255, 255, 255, 255), "Rates : %d", misses);
        draw_text(12, 100, 0.55f, C2D_Color32(255, 255, 255, 255), "Combo : %d", combo);
        draw_text(12, 125, 0.55f, C2D_Color32(255, 255, 255, 255), "Precision : %.2f%%",
            hits ? accSum * 100.0f / hits : 0.0f);
        float len = haveInst ? inst.samples * 1000.0f / inst.rate : 1;
        float prog = started ? pos / len : 0;
        if (prog > 1) prog = 1;
        C2D_DrawRectSolid(12, 160, 0.5f, 296, 10, C2D_Color32(60, 60, 90, 255));
        C2D_DrawRectSolid(12, 160, 0.5f, 296 * prog, 10, C2D_Color32(146, 113, 253, 255));
        draw_text(12, 180, 0.45f, C2D_Color32(180, 180, 220, 255), "Croix/ABXY/stick : fleches   START : pause");
        if (paused) {
            C2D_DrawRectSolid(0, 0, 0.6f, 320, 240, C2D_Color32(0, 0, 0, 210));
            static const char* PM[3] = {"Reprendre", "Recommencer", "Quitter"};
            for (int i = 0; i < 3; i++)
                draw_text_centered(160, 70 + i * 35, 0.7f,
                    i == pauseSel ? C2D_Color32(255, 230, 80, 255) : C2D_Color32(200, 200, 200, 255), "%s", PM[i]);
        }
        C3D_FrameEnd(0);
        if (gAutoTest) {
            static int frames;
            static u64 t0;
            static float cpuMax, gpuMax;
            frames++;
            float cpu = C3D_GetProcessingTime(), gpu = C3D_GetDrawingTime();
            if (cpu > cpuMax) cpuMax = cpu;
            if (gpu > gpuMax) gpuMax = gpu;
            if (!t0) t0 = now;
            double el = (now - t0) * 1.0 / SYSCLOCK_ARM11;
            if (frames % 120 == 0) {
                flog("t=%.1fs pos=%.0fms img/s=%.1f cpu=%.1fms(max %.1f) gpu=%.1fms(max %.1f) vie=%.2f rates=%d score=%d lin=%luKo",
                    el, pos, frames / el, cpu, cpuMax, gpu, gpuMax, health, misses, score,
                    (unsigned long)(linearSpaceFree() / 1024));
                cpuMax = gpuMax = 0;
            }
            if (frames == 60 || frames == 600 || frames == 1500) {
                gspWaitForVBlank();
                char sp[300];
                snprintf(sp, sizeof(sp), "sdmc:/3ds/fnf3ds/shot_%05d.bmp", frames);
                screenshot(sp);
            }
            if (el > gAutoSeconds) {
                result = GAME_QUIT;
                break;
            }
        }
    }

    stream_close(&inst);
    stream_close(&voices);
    for (int i = 0; i < 3; i++)
        if (chars[i].loaded) spr_free(&chars[i].spr);
    for (int i = 0; i < stageCount; i++)
        if (!stage[i].isChar) spr_free(&stage[i].spr);
    free(stage);
    chart_free(&chart);
    if (outScore) *outScore = score;
    if (outAccuracy) *outAccuracy = hits ? accSum * 100.0f / hits : 0.0f;
    return result;
}
