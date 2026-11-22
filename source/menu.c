/* Menus repris du mod : écran titre, menu principal, mode histoire, freeplay, options.
 * Les positions sont celles des scripts du mod (écran FNF 1280x720), ramenées à l'écran 3DS. */
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fnf.h"

#define MS (400.0f / 1280.0f)
#define MY0 ((SCREEN_H - 720.0f * MS) / 2.0f)
#define FX(x) ((x) * MS)
#define FY(y) (MY0 + (y) * MS)

Settings gSettings = {false, true};
bool gAutoMenu = false;
void screenshot(const char* path);

/* robot de test des menus : touches simulées et captures à des images précises */
static u32 auto_down(void) {
    u32 real = hidKeysDown();
    if (!gAutoMenu) return real;
    static int f;
    static const struct { int frame; u32 key; } SCRIPT[] = {
        {120, 0}, {150, KEY_A}, {300, 0}, {320, KEY_DUP}, {360, KEY_A}, {480, 0}, {500, KEY_DRIGHT}, {560, 0},
        {580, KEY_A}, {700, 0}, {720, KEY_B}, {760, KEY_B}, {820, KEY_DDOWN}, {850, KEY_A}, {990, 0},
        {1000, KEY_DDOWN}, {1060, 0}, {1080, KEY_DRIGHT}, {1150, 0}, {1170, KEY_B}, {1230, KEY_DDOWN},
        {1260, KEY_A}, {1340, 0}, {1360, -1u}};
    f++;
    for (unsigned i = 0; i < sizeof(SCRIPT) / sizeof(SCRIPT[0]); i++) {
        if (SCRIPT[i].frame != f) continue;
        if (SCRIPT[i].key == (u32)-1) {
            flog("AUTOTEST FINI menus");
            exit(0);
        }
        if (SCRIPT[i].key == 0) {
            char p[64];
            snprintf(p, sizeof(p), "sdmc:/3ds/fnf3ds/shot_m%04d.bmp", f);
            gspWaitForVBlank();
            screenshot(p);
            flog("capture %s", p);
            return 0;
        }
        return SCRIPT[i].key;
    }
    return 0;
}

static Spr sTitle, sUi, sArt, sStory, sIcons;
static C2D_Font fFree, fVcr;
static Stream sMusic;
static bool sAudio;
static C3D_RenderTarget *sTop, *sBottom;
static SongEntry* sSongs;
static int sSongCount;

/* ---------------------------------------------------------------- outils */
static float ease_expo_out(float t) { return t >= 1 ? 1 : 1 - powf(2, -10 * t); }
static float ease_cube_out(float t) {
    float u = 1 - t;
    return 1 - u * u * u;
}
static float lerpf(float a, float b, float t) { return a + (b - a) * t; }

typedef struct {
    float from, to, t, dur;
    int ease;   /* 0 expo, 1 cube, 2 linéaire */
} Tw;

static void tw_set(Tw* w, float to, float dur, int ease) {
    float cur = w->dur > 0 && w->t < w->dur ? 0 : 0;
    (void)cur;
    float now;
    if (w->dur <= 0 || w->t >= w->dur) now = w->to;
    else {
        float k = w->t / w->dur;
        k = w->ease == 0 ? ease_expo_out(k) : w->ease == 1 ? ease_cube_out(k) : k;
        now = lerpf(w->from, w->to, k);
    }
    w->from = now;
    w->to = to;
    w->t = 0;
    w->dur = dur;
    w->ease = ease;
}
static void tw_jump(Tw* w, float v) {
    w->from = w->to = v;
    w->t = w->dur = 0;
}
static float tw_get(Tw* w) {
    if (w->dur <= 0 || w->t >= w->dur) return w->to;
    float k = w->t / w->dur;
    k = w->ease == 0 ? ease_expo_out(k) : w->ease == 1 ? ease_cube_out(k) : k;
    return lerpf(w->from, w->to, k);
}
static void tw_step(Tw* w, float dt) { w->t += dt; }

static const SprFrame* frame_of(const Spr* s, const char* anim, int idx) {
    int a = spr_anim(s, anim);
    if (a < 0 || s->anims[a].count == 0) return NULL;
    return &s->frames[s->anims[a].frames[idx % s->anims[a].count]];
}

/* image à (x,y) en coordonnées FNF, coin haut-gauche, échelle autour du centre */
static void img(const SprFrame* fr, float x, float y, float scale, float alpha, float angleDeg) {
    if (!fr || alpha <= 0.003f) return;
    float w = fr->sub.width * scale, h = fr->sub.height * scale;
    float cx = FX(x) + (fr->ox + fr->sub.width / 2.0f) * 1.0f, cy = FY(y) + (fr->oy + fr->sub.height / 2.0f);
    C2D_ImageTint tint;
    C2D_AlphaImageTint(&tint, alpha > 1 ? 1 : alpha);
    C2D_DrawParams p = {.pos = {cx, cy, w, h}, .center = {w / 2, h / 2}, .depth = 0.5f,
        .angle = angleDeg * (float)M_PI / 180.0f};
    C2D_DrawImage(fr->img, &p, alpha < 1 ? &tint : NULL);
}

/* texte avec une police du mod ; size = taille FNF en pixels ; align 0 gauche, 1 centre, 2 droite */
static float text_w(C2D_Font font, float size, const char* s) {
    C2D_Text t;
    C2D_TextFontParse(&t, font, gTextBuf, s);
    float lf = font ? C2D_FontGetInfo(font)->lineFeed : 30;
    float sc = size * MS / lf;
    float w, h;
    C2D_TextGetDimensions(&t, sc, sc, &w, &h);
    return w / MS;
}

static void textf(C2D_Font font, float x, float y, float size, int align, uint32_t color, const char* fmt, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    C2D_Text t;
    C2D_TextFontParse(&t, font, gTextBuf, buf);
    C2D_TextOptimize(&t);
    float lf = font ? C2D_FontGetInfo(font)->lineFeed : 30;
    float sc = size * MS / lf;
    float w, h;
    C2D_TextGetDimensions(&t, sc, sc, &w, &h);
    float px = FX(x) - (align == 1 ? w / 2 : align == 2 ? w : 0);
    C2D_DrawText(&t, C2D_WithColor, px, FY(y), 0.9f, sc, sc, color);
}

static uint32_t rgba(int r, int g, int b, float a) {
    if (a < 0) a = 0;
    if (a > 1) a = 1;
    return C2D_Color32((u8)r, (u8)g, (u8)b, (u8)(a * 255));
}

/* ---------------------------------------------------------------- son */
static void music_play(const char* path, bool loop) {
    if (!sAudio) return;
    stream_close(&sMusic);
    if (stream_open(&sMusic, path, 0)) {
        sMusic.loop = loop;
        stream_start(&sMusic);
        stream_pause(&sMusic, false);
    }
}
static void music_menu(void) {
    char p[320];
    snprintf(p, sizeof(p), "%s/ui/menu.adp", gModDir);
    music_play(p, true);
}
static void sfx(const char* name) {
    if (!sAudio) return;
    char p[320];
    snprintf(p, sizeof(p), "%s/ui/sfx_%s.adp", gModDir, name);
    sfx_play(p, 1.0f);
}
static void audio_tick(void) {
    stream_update(&sMusic);
    sfx_update();
}

void hud_text(float x, float y, float size, int align, uint32_t color, bool outline, const char* fmt, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (outline) {
        uint32_t k = C2D_Color32(0, 0, 0, (u8)((color >> 24) & 0xFF));
        float d = 1.2f / MS;
        textf(fVcr, x - d, y, size, align, k, "%s", buf);
        textf(fVcr, x + d, y, size, align, k, "%s", buf);
        textf(fVcr, x, y - d, size, align, k, "%s", buf);
        textf(fVcr, x, y + d, size, align, k, "%s", buf);
    }
    textf(fVcr, x, y, size, align, color, "%s", buf);
}

/* ---------------------------------------------------------------- scores */
typedef struct {
    char id[64];
    int score;
} Best;
static Best* sBest;
static int sBestCount;

static void scores_load(void) {
    char p[320];
    snprintf(p, sizeof(p), "%s/scores.txt", gModDir);
    FILE* f = fopen(p, "r");
    sBest = calloc(128, sizeof(Best));
    sBestCount = 0;
    if (!f) return;
    char line[128];
    while (fgets(line, sizeof(line), f) && sBestCount < 128) {
        char* bar = strrchr(line, '|');
        if (!bar) continue;
        *bar = 0;
        snprintf(sBest[sBestCount].id, 64, "%s", line);
        sBest[sBestCount].score = atoi(bar + 1);
        sBestCount++;
    }
    fclose(f);
}
static int best_get(const char* id) {
    for (int i = 0; i < sBestCount; i++)
        if (strcmp(sBest[i].id, id) == 0) return sBest[i].score;
    return 0;
}
static void best_set(const char* id, int score) {
    int i;
    for (i = 0; i < sBestCount; i++)
        if (strcmp(sBest[i].id, id) == 0) break;
    if (i == sBestCount) {
        if (sBestCount >= 128) return;
        snprintf(sBest[sBestCount].id, 64, "%s", id);
        sBest[sBestCount++].score = 0;
    }
    if (score <= sBest[i].score) return;
    sBest[i].score = score;
    char p[320];
    snprintf(p, sizeof(p), "%s/scores.txt", gModDir);
    FILE* f = fopen(p, "w");
    if (!f) return;
    for (int k = 0; k < sBestCount; k++) fprintf(f, "%s|%d\n", sBest[k].id, sBest[k].score);
    fclose(f);
}

static void settings_save(void) {
    FILE* f = fopen("sdmc:/3ds/fnf3ds/settings.txt", "w");
    if (!f) return;
    fprintf(f, "downscroll=%d\nghost=%d\n", gSettings.downscroll, gSettings.ghostTapping);
    fclose(f);
}
static void settings_load(void) {
    FILE* f = fopen("sdmc:/3ds/fnf3ds/settings.txt", "r");
    if (!f) return;
    char line[64];
    while (fgets(line, sizeof(line), f)) {
        int v;
        if (sscanf(line, "downscroll=%d", &v) == 1) gSettings.downscroll = v;
        if (sscanf(line, "ghost=%d", &v) == 1) gSettings.ghostTapping = v;
    }
    fclose(f);
}

/* ---------------------------------------------------------------- fond animé du freeplay (shader bgMenu) */
#define BGW 128
#define BGH 64
static C3D_Tex sBgTex;
static C2D_Image sBgImg;
static Tex3DS_SubTexture sBgSub;
static bool sBgReady;

static float gradient(float p) {
    static const float px[5] = {0.00f, 0.86f, 0.955f, 0.99f, 1.00f};
    static const float py[5] = {0.0f, 0.1f, 0.40f, 1.0f, 0.0f};
    if (p < px[0]) return py[0];
    for (int i = 1; i < 5; i++)
        if (p < px[i]) return lerpf(py[i - 1], py[i], (p - px[i - 1]) / (px[i] - px[i - 1]));
    return py[4];
}
static float waveN(float ux, float uy, float t, float s1, float s2, float t1, float t2, float f1, float f2, float h1,
    float h2) {
    float x1 = sinf((t * s1 + t1 + ux) * f1) * h1;
    float x2 = sinf((t * s2 + t2 + ux) * f2) * h2;
    return gradient(uy / (0.5f + x1 + x2)) * 0.27f;
}

static void bg_update(float time) {
    if (!sBgReady) {
        C3D_TexInit(&sBgTex, BGW, BGH, GPU_RGBA8);
        C3D_TexSetFilter(&sBgTex, GPU_LINEAR, GPU_LINEAR);
        sBgSub = (Tex3DS_SubTexture){400, 225, 0.0f, 1.0f, 100.0f / BGW, 1.0f - 56.25f / BGH};
        sBgImg.tex = &sBgTex;
        sBgImg.subtex = &sBgSub;
        sBgReady = true;
    }
    /* image 100x56 (rapport 16:9) dans une texture 128x64, rangée en tuiles 8x8 de la 3DS */
    u32* data = (u32*)sBgTex.data;
    const int W = 100, H = 57;
    for (int y = 0; y < H; y++) {
        float uvY = 1.0f - (y + 0.5f) / 56.25f;   /* coordonnées du shader : y vers le haut */
        for (int x = 0; x < W; x++) {
            float ux = (x + 0.5f) / 100.0f;
            float w = waveN(ux, uvY - 0.25f, time, 0.03f, 0.06f, 0.00f, 0.02f, 8.0f, 3.7f, 0.06f, 0.05f) +
                      waveN(ux, uvY - 0.25f, time, 0.04f, 0.07f, 0.16f, -0.37f, 6.7f, 2.89f, 0.06f, 0.05f) +
                      waveN(ux, 0.75f - uvY, time, 0.035f, 0.055f, -0.09f, 0.27f, 7.4f, 2.51f, 0.06f, 0.05f) +
                      waveN(ux, 0.75f - uvY, time, 0.032f, 0.09f, 0.08f, -0.22f, 6.5f, 3.89f, 0.06f, 0.05f);
            float k = (ux + (1.0f - uvY)) * 0.55f;
            float r = lerpf(0.05f, 0.1f, k) + w, g = lerpf(0.05f, 0.65f, k) + w, b = lerpf(0.3f, 0.85f, k) + w;
            u8 R = (u8)(fminf(r, 1) * 255), G = (u8)(fminf(g, 1) * 255), B = (u8)(fminf(b, 1) * 255);
            int ty = BGH - 1 - y;
            u32 m = (x & 1) | ((ty & 1) << 1) | ((x & 2) << 1) | ((ty & 2) << 2) | ((x & 4) << 2) | ((ty & 4) << 3);
            u32 off = (u32)(((ty >> 3) * (BGW >> 3) + (x >> 3)) * 64) + m;
            data[off] = ((u32)R << 24) | ((u32)G << 16) | ((u32)B << 8) | 0xFF;
        }
    }
    C3D_TexFlush(&sBgTex);
}

/* ---------------------------------------------------------------- transitions */
static float sFade;   /* noir par-dessus, 1 -> 0 en entrant dans un écran */
static void fade_draw(float dt) {
    if (sFade > 0) {
        C2D_DrawRectSolid(0, 0, 0.95f, SCREEN_W, SCREEN_H, rgba(0, 0, 0, sFade));
        sFade -= dt / 300.0f;
    }
}
static void flash_draw(float* flash, float dt, float durMs) {
    if (*flash > 0) {
        C2D_DrawRectSolid(0, 0, 0.94f, SCREEN_W, SCREEN_H, rgba(255, 255, 255, *flash));
        *flash -= dt / durMs;
    }
}

static void bottom_info(const char* title, const char* l1, const char* l2) {
    C2D_TargetClear(sBottom, C2D_Color32(14, 12, 30, 255));
    C2D_SceneBegin(sBottom);
    C2D_DrawRectSolid(0, 0, 0.5f, 320, 36, C2D_Color32(247, 1, 116, 255));
    draw_text_centered(160, 8, 0.6f, C2D_Color32(255, 255, 255, 255), "%s", title);
    if (l1) draw_text_centered(160, 150, 0.48f, C2D_Color32(200, 220, 255, 255), "%s", l1);
    if (l2) draw_text_centered(160, 175, 0.48f, C2D_Color32(200, 220, 255, 255), "%s", l2);
    draw_text_centered(160, 215, 0.4f, C2D_Color32(120, 120, 160, 255), "Cyber Sensation: Malware Breakout V1");
}

static float sMusicBeat(void) {
    double ms = sMusic.open ? stream_position_ms(&sMusic) : (double)osGetTime();
    if (ms < 0) ms = 0;
    return (float)(ms * 102.0 / 60000.0);
}

/* ---------------------------------------------------------------- écran titre */
static bool title_screen(void) {
    u64 last = svcGetSystemTick();
    bool pressed = false;
    float pressT = 0, flash = 0, tm = 0;
    sFade = 1;
    while (aptMainLoop()) {
        u64 now = svcGetSystemTick();
        float dt = (float)((now - last) * 1000.0 / SYSCLOCK_ARM11);
        last = now;
        tm += dt;
        hidScanInput();
        u32 down = auto_down();
        audio_tick();
        if (!pressed && (down & (KEY_A | KEY_START))) {
            pressed = true;
            flash = 1;
            sfx("confirm");
        }
        if (pressed) {
            pressT += dt;
            if (pressT > 1500) return true;
        }
        if ((down & KEY_SELECT) && !pressed) return false;
        float beat = sMusicBeat();
        float bf = beat - floorf(beat);
        float lerp = sinf(bf * (float)M_PI / 2);   /* sineOut */

        C2D_TextBufClear(gTextBuf);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(sTop, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(sTop);
        img(frame_of(&sTitle, "fade", 0), 0, 0, 1, 1, 0);
        img(frame_of(&sTitle, "lazers-effect", 0), 0, 0, 1, 0.5f + 0.25f * sinf((float)M_PI * beat / 4), 0);
        img(frame_of(&sTitle, "lazers-effectless", 0), 0, 0, 1, 1, 0);
        img(frame_of(&sTitle, "logo", 0), 280, 11, 1, 1, 0);
        const char* an = pressed ? "press" : "idle";
        int fi = (int)(tm * 24 / 1000);
        const SprFrame* te = frame_of(&sTitle, an, fi);
        if (te) {
            float tx = (1280 - te->fw / MS) / 2, ty = 720 * 0.8f;
            img(te, tx, ty, 1, 1, 0);
            if (!pressed) img(te, tx, ty, lerpf(1.025f, 1, lerp), (1 - lerp) * 0.5f, 0);
        }
        textf(fVcr, 5, 720 - 22, 16, 0, C2D_Color32(255, 255, 255, 255), "Cyber Sensation: Malware Breakout V1");
        flash_draw(&flash, dt, 1000);
        fade_draw(dt);
        bottom_info("FNF 3DS", "A / START : commencer", "SELECT : quitter");
        C3D_FrameEnd(0);
    }
    return false;
}

/* ---------------------------------------------------------------- jeu (une chanson ou une liste) */
static int find_song(const char* base, const char* diff) {
    for (int i = 0; i < sSongCount; i++)
        if (strcmp(sSongs[i].base, base) == 0 && (!diff || strcmp(sSongs[i].diff, diff) == 0)) return i;
    for (int i = 0; i < sSongCount; i++)
        if (strcmp(sSongs[i].base, base) == 0) return i;
    return -1;
}

static void play_list(const char** bases, int n, const char* storyId) {
    stream_close(&sMusic);
    sfx_stop_all();
    int total = 0;
    for (int k = 0; k < n; k++) {
        int i = find_song(bases[k], "REGULAR");
        if (i < 0) continue;
        GameResult r;
        int sc = 0;
        float acc = 0;
        do {
            r = game_play(&sSongs[i], sTop, sBottom, &sc, &acc);
        } while (r == GAME_RESTART);
        if (r != GAME_DONE) {
            total = -1;
            break;
        }
        best_set(sSongs[i].id, sc);
        total += sc;
    }
    if (storyId && total >= 0) best_set(storyId, total);
    music_menu();
    sFade = 1;
}

/* ---------------------------------------------------------------- options */
static void options_screen(void) {
    int sel = 0;
    u64 last = svcGetSystemTick();
    sFade = 1;
    while (aptMainLoop()) {
        u64 now = svcGetSystemTick();
        float dt = (float)((now - last) * 1000.0 / SYSCLOCK_ARM11);
        last = now;
        hidScanInput();
        u32 down = auto_down();
        audio_tick();
        if (down & (KEY_DUP | KEY_DDOWN)) {
            sel ^= 1;
            sfx("scroll");
        }
        if (down & (KEY_A | KEY_DLEFT | KEY_DRIGHT)) {
            if (sel == 0) gSettings.downscroll = !gSettings.downscroll;
            else gSettings.ghostTapping = !gSettings.ghostTapping;
            sfx("confirm");
            settings_save();
        }
        if (down & KEY_B) {
            sfx("cancel");
            return;
        }
        C2D_TextBufClear(gTextBuf);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(sTop, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(sTop);
        if (sBgReady) {
            C2D_DrawParams p = {.pos = {0, 0, 400, 240}, .depth = 0.1f};
            C2D_DrawImage(sBgImg, &p, NULL);
        }
        textf(fFree, 640, 80, 64, 1, C2D_Color32(255, 255, 255, 255), "OPTIONS");
        const char* lab[2] = {"Downscroll", "Ghost tapping"};
        bool val[2] = {gSettings.downscroll, gSettings.ghostTapping};
        for (int i = 0; i < 2; i++) {
            uint32_t c = i == sel ? C2D_Color32(255, 255, 255, 255) : C2D_Color32(255, 255, 255, 130);
            textf(fVcr, 300, 280 + i * 110, 48, 0, c, "%s", lab[i]);
            textf(fVcr, 980, 280 + i * 110, 48, 2, val[i] ? C2D_Color32(2, 244, 240, 255) : C2D_Color32(247, 1, 116, 255),
                "%s", val[i] ? "OUI" : "NON");
            if (i == sel) C2D_DrawRectSolid(FX(300), FY(280 + i * 110 + 56), 0.5f, FX(680), 2, C2D_Color32(247, 1, 116, 255));
        }
        fade_draw(dt);
        bottom_info("Options", "A : changer   B : retour",
            gSettings.ghostTapping ? "Ghost tapping : pas de penalite si tu appuies dans le vide" : "Appuyer dans le vide compte comme un rate");
        C3D_FrameEnd(0);
        bg_update(osGetTime() / 1000.0f);
    }
}

/* ---------------------------------------------------------------- mode histoire */
typedef struct {
    const char* name;
    const char* desc;
    const char* cover;
    const char* main[10];
    const char* side[10];
} Chapter;

static const Chapter CHAPTERS[5] = {
    {"Chapter 1", "The Beginning", "chap1", {"open system", "wear a mask", NULL}, {NULL}},
    {"Chapter 2", "Nonsensical Outbreak", "chap2", {"sensical", "sensational", "resensation", NULL}, {"guideline", NULL}},
    {"Chapter 3", "Cyber Mayhem", "chap3", {"you and me", "fun till end", "light it up", NULL}, {NULL}},
    {"Chapter 4", "Breakout", "chap4",
        {"bobalolazing", "feathered firewall", "trolameme", "julius", "shini", "fun gang", "new hope", "hoax", NULL},
        {"brand new life", "soft machine", "new day", "yawarakai", "underground", "karma", NULL}},
    {"Final Chapter", "The End", "chap5", {"errorians", "our hope", NULL},
        {"virus", "crowd control", "malware", "kowase", "your hope", "our hope", NULL}},
};

static int list_len(const char* const* l) {
    int n = 0;
    while (n < 10 && l[n]) n++;
    return n;
}

static void story_screen(void) {
    int cur = 0, pickSel = 0;
    bool pick = false;
    Tw cx[5], ca[5], cs[5], chapY, descY, sideY;
    const float GAP = 470;
    for (int i = 0; i < 5; i++) {
        tw_jump(&cx[i], 99 + i * GAP + 1280);
        tw_jump(&ca[i], 0);
        tw_jump(&cs[i], 1);
        tw_set(&cx[i], 444 + i * GAP, 0.7f, 1);
        tw_set(&ca[i], i == 0 ? 1 : 0.5f, 0.7f, 1);
        if (i) tw_set(&cs[i], 290.0f / 392.0f, 0.7f, 0);
    }
    tw_jump(&chapY, 9 - 720);
    tw_set(&chapY, 9, 0.7f, 1);
    tw_jump(&descY, 655 + 720);
    tw_set(&descY, 655, 0.7f, 1);
    tw_jump(&sideY, 720);
    u64 last = svcGetSystemTick();
    float tm = 0;
    sFade = 1;
    while (aptMainLoop()) {
        u64 now = svcGetSystemTick();
        float dt = (float)((now - last) * 1000.0 / SYSCLOCK_ARM11);
        last = now;
        tm += dt;
        float ds = dt / 1000.0f;
        for (int i = 0; i < 5; i++) {
            tw_step(&cx[i], ds);
            tw_step(&ca[i], ds);
            tw_step(&cs[i], ds);
        }
        tw_step(&chapY, ds);
        tw_step(&descY, ds);
        tw_step(&sideY, ds);
        hidScanInput();
        u32 down = auto_down();
        audio_tick();
        const Chapter* ch = &CHAPTERS[cur];
        bool hasSide = ch->side[0] != NULL;
        if (!pick) {
            int mv = (down & (KEY_DRIGHT | KEY_R)) ? 1 : (down & (KEY_DLEFT | KEY_L)) ? -1 : 0;
            if (mv) {
                cur = (cur + mv + 5) % 5;
                sfx("scroll");
                for (int i = 0; i < 5; i++) {
                    tw_set(&cx[i], cur == 0 ? 444 + i * GAP : (3 + i * GAP) - cur * GAP + GAP - 45, 0.2f, 0);
                    tw_set(&ca[i], i == cur ? 1 : 0.5f, 0.2f, 2);
                    tw_set(&cs[i], i == cur ? 1 : 290.0f / 392.0f, 0.2f, 0);
                }
            }
            if (down & KEY_B) {
                sfx("cancel");
                return;
            }
            if (down & KEY_A) {
                sfx("selected");
                if (hasSide) {
                    pick = true;
                    pickSel = 0;
                    tw_set(&sideY, 0, 0.7f, 1);
                    tw_set(&chapY, 9 - 720, 0.7f, 1);
                } else {
                    char id[32];
                    snprintf(id, sizeof(id), "story:%d", cur);
                    play_list((const char**)ch->main, list_len(ch->main), id);
                    tw_jump(&sideY, 720);
                }
            }
        } else {
            if (down & (KEY_DLEFT | KEY_DRIGHT)) {
                pickSel ^= 1;
                sfx("scroll");
            }
            if (down & KEY_B) {
                pick = false;
                sfx("cancel");
                tw_set(&sideY, 720, 0.7f, 1);
                tw_set(&chapY, 9, 0.7f, 1);
            }
            if (down & KEY_A) {
                sfx("selected");
                const char* const* l = pickSel ? ch->side : ch->main;
                char id[32];
                snprintf(id, sizeof(id), "story:%d:%d", cur, pickSel);
                play_list((const char**)l, list_len(l), id);
                pick = false;
                tw_jump(&sideY, 720);
                tw_jump(&chapY, 9);
            }
        }

        C2D_TextBufClear(gTextBuf);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(sTop, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(sTop);
        const SprFrame* pf = frame_of(&sStory, "pause_fade", 0);
        if (pf) img(pf, (1280 - pf->fw / MS) / 2, (720 - pf->fh / MS) / 2, 1, 1, 0);
        const SprFrame* ho = frame_of(&sStory, "hologram", 0);
        if (ho) {
            float drift = sinf(tm / 1500.0f) * 12;
            img(ho, (1280 - ho->fw / MS) / 2 + drift, (720 - ho->fh / MS) / 2, 1, 0.3f, 0);
        }
        for (int i = 0; i < 5; i++) {
            const SprFrame* c = frame_of(&sStory, CHAPTERS[i].cover, 0);
            if (!c) continue;
            float s = tw_get(&cs[i]);
            float cyy = (720 - 554) / 2.0f;
            float yoff = pick ? 720 : 0;
            img(c, tw_get(&cx[i]), cyy + yoff * (1 - tw_get(&sideY) / 720) * 0 + (pick ? -tw_get(&sideY) * 0 : 0), s,
                tw_get(&ca[i]) * (pick ? 0.25f : 1), 0);
        }
        const SprFrame* tf = frame_of(&sTitle, "fade", 0);
        if (tf) img(tf, 0, -100, 1, 0.4f, 0);
        textf(fVcr, 640, tw_get(&chapY), 64, 1, C2D_Color32(255, 255, 255, 255), "%s", ch->name);
        textf(fVcr, 640, tw_get(&descY), 48, 1, C2D_Color32(255, 255, 255, 255), "%s", ch->desc);
        if (pick || tw_get(&sideY) < 719) {
            float sy = tw_get(&sideY);
            C2D_DrawRectSolid(0, 0, 0.6f, 400, 240, rgba(0, 0, 0, 0.6f * (1 - sy / 720)));
            const SprFrame* m = frame_of(&sStory, "side_main", 0);
            const SprFrame* sd = frame_of(&sStory, "side_side", 0);
            if (m) img(m, 40, 150 + sy, pickSel == 0 ? 0.8f : 0.7f, pickSel == 0 ? 1 : 0.5f, 0);
            if (sd) img(sd, 640, 150 + sy, pickSel == 1 ? 0.8f : 0.7f, pickSel == 1 ? 1 : 0.5f, 0);
            textf(fVcr, 640, 30 + sy, 64, 1, C2D_Color32(255, 255, 255, 255), "PICK A STORY");
            textf(fVcr, 330, 620 + sy, 40, 1, pickSel == 0 ? C2D_Color32(2, 244, 240, 255) : C2D_Color32(180, 180, 180, 255),
                "MAIN STORY");
            textf(fVcr, 950, 620 + sy, 40, 1, pickSel == 1 ? C2D_Color32(247, 1, 116, 255) : C2D_Color32(180, 180, 180, 255),
                "SIDE STORY");
        }
        fade_draw(dt);

        /* écran du bas : chansons du chapitre */
        C2D_TargetClear(sBottom, C2D_Color32(14, 12, 30, 255));
        C2D_SceneBegin(sBottom);
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 36, C2D_Color32(247, 1, 116, 255));
        draw_text_centered(160, 8, 0.6f, C2D_Color32(255, 255, 255, 255), "%s - %s", ch->name, ch->desc);
        const char* const* l = (pick && pickSel) ? ch->side : ch->main;
        draw_text(12, 44, 0.45f, C2D_Color32(2, 244, 240, 255), (pick && pickSel) ? "Side story :" : "Main story :");
        for (int k = 0; k < list_len(l); k++) {
            int si = find_song(l[k], "REGULAR");
            draw_text(24 + (k / 8) * 150, 64 + (k % 8) * 17, 0.42f, C2D_Color32(230, 230, 255, 255), "%s",
                si >= 0 ? sSongs[si].baseName : l[k]);
        }
        char id[32];
        if (pick) snprintf(id, sizeof(id), "story:%d:%d", cur, pickSel);
        else snprintf(id, sizeof(id), "story:%d", cur);
        draw_text(12, 200, 0.45f, C2D_Color32(255, 230, 120, 255), "Meilleur score : %d", best_get(id));
        draw_text(12, 220, 0.4f, C2D_Color32(150, 150, 190, 255),
            pick ? "Gauche/Droite : histoire   A : jouer   B : retour" : "Gauche/Droite : chapitre   A : choisir   B : retour");
        C3D_FrameEnd(0);
    }
}

/* ---------------------------------------------------------------- freeplay */
typedef struct {
    int first, count;   /* entrées de sSongs (une par difficulté) */
    Tw tx, ty, ts, ta, bx, by, bs, ix, iy, is;
} FpItem;

static void freeplay_screen(void) {
    static int cur = 0, curDiff = 0;
    FpItem* it = calloc((size_t)sSongCount + 1, sizeof(FpItem));
    int n = 0;
    for (int i = 0; i < sSongCount; i++) {
        if (n > 0 && strcmp(sSongs[it[n - 1].first].base, sSongs[i].base) == 0) {
            it[n - 1].count++;
            continue;
        }
        it[n].first = i;
        it[n].count = 1;
        n++;
    }
    if (n == 0) {
        free(it);
        return;
    }
    if (cur >= n) cur = 0;
    Tw coverX[9], coverY[9], coverA[9], coverR[9], diffY, diffA;
    for (int i = 0; i < 9; i++) {
        tw_jump(&coverX[i], -100);
        tw_jump(&coverY[i], -100);
        tw_jump(&coverA[i], 0);
        tw_jump(&coverR[i], 100);
    }
    tw_jump(&diffY, 320);
    tw_jump(&diffA, 1);
    static const char* COV[9] = {"cover_mayhem", "cover_sha", "cover_ct", "cover_old", "cover_def", "cover_soft",
        "cover_tans", "cover_dust", "cover_hope"};

    bool first = true;
    float preview = 0;
    bool previewing = false;
    u64 last = svcGetSystemTick();
    float tm = 0;
    sFade = 1;
    while (aptMainLoop()) {
        u64 now = svcGetSystemTick();
        float dt = (float)((now - last) * 1000.0 / SYSCLOCK_ARM11);
        last = now;
        tm += dt;
        float ds = dt / 1000.0f;
        hidScanInput();
        u32 down = auto_down();
        audio_tick();
        int mv = (down & (KEY_DDOWN | KEY_CPAD_DOWN)) ? 1 : (down & (KEY_DUP | KEY_CPAD_UP)) ? -1 : 0;
        if (down & KEY_R) mv = 5;
        if (down & KEY_L) mv = -5;
        if (mv || first) {
            cur = ((cur + mv) % n + n) % n;
            if (!first) sfx("scroll");
            if (curDiff >= it[cur].count) curDiff = 0;
            for (int i = 0; i < n; i++) {
                FpItem* f = &it[i];
                float d = (float)(i - cur);
                float tx = i == cur ? 983.12f : 983.12f + d * 90 + 200, ty = i == cur ? 280.05f : 160 + d * 128 + 128;
                float bx = i == cur ? 777 : 777 + d * 90 + 240, by = i == cur ? 244 : 119.05f + d * 128 + 128;
                float ix = i == cur ? 983.12f - 130 : 983.12f - 70 + d * 90 + 200, iy = i == cur ? 280.05f - 54 : 100 + d * 128 + 128;
                if (first) {
                    tw_jump(&f->tx, tx), tw_jump(&f->ty, ty), tw_jump(&f->bx, bx), tw_jump(&f->by, by);
                    tw_jump(&f->ix, ix), tw_jump(&f->iy, iy);
                    tw_jump(&f->ts, i == cur ? 1 : 2.0f / 3), tw_jump(&f->bs, i == cur ? 2.0f / 3 : 0.5f);
                    tw_jump(&f->is, i == cur ? 0.6f : 0.4f), tw_jump(&f->ta, i == cur ? 1 : 0.5f);
                } else {
                    tw_set(&f->tx, tx, 0.2f, 0), tw_set(&f->ty, ty, 0.2f, 0), tw_set(&f->bx, bx, 0.2f, 0);
                    tw_set(&f->by, by, 0.2f, 0), tw_set(&f->ix, ix, 0.2f, 0), tw_set(&f->iy, iy, 0.2f, 0);
                    tw_set(&f->ts, i == cur ? 1 : 2.0f / 3, 0.2f, 0), tw_set(&f->bs, i == cur ? 2.0f / 3 : 0.5f, 0.2f, 0);
                    tw_set(&f->is, i == cur ? 0.6f : 0.4f, 0.2f, 0), tw_set(&f->ta, i == cur ? 1 : 0.5f, 0.2f, 2);
                }
            }
            int cv = sSongs[it[cur].first].cover;
            for (int k = 0; k < 9; k++) {
                bool on = k == cv;
                tw_set(&coverX[k], on ? 140 : -100, 0.2f, 2);
                tw_set(&coverY[k], on ? 200 : -100, 0.2f, 2);
                tw_set(&coverA[k], on ? 1 : 0, 0.2f, 2);
                tw_set(&coverR[k], on ? -10 : 100, 0.2f, 2);
            }
            tw_jump(&diffY, 310);
            tw_jump(&diffA, 0.5f);
            tw_set(&diffY, 320, 0.2f, 2);
            tw_set(&diffA, 1, 0.2f, 2);
            preview = 0;
            previewing = false;
            first = false;
        }
        if (down & (KEY_DLEFT | KEY_DRIGHT | KEY_CPAD_LEFT | KEY_CPAD_RIGHT)) {
            if (it[cur].count > 1) {
                curDiff = (curDiff + ((down & (KEY_DRIGHT | KEY_CPAD_RIGHT)) ? 1 : it[cur].count - 1)) % it[cur].count;
                sfx("scroll");
                preview = 0;
                previewing = false;
            }
        }
        /* aperçu de la musique après un court arrêt sur une chanson */
        preview += dt;
        if (!previewing && preview > 1000) {
            previewing = true;
            char p[400];
            snprintf(p, sizeof(p), "%s/songs/%s/inst.adp", gModDir, sSongs[it[cur].first + curDiff].id);
            music_play(p, true);
        }
        if (down & KEY_B) {
            sfx("cancel");
            if (previewing) music_menu();
            break;
        }
        if (down & KEY_A) {
            SongEntry* s = &sSongs[it[cur].first + curDiff];
            stream_close(&sMusic);
            sfx_stop_all();
            GameResult r;
            int sc = 0;
            float acc = 0;
            do {
                r = game_play(s, sTop, sBottom, &sc, &acc);
            } while (r == GAME_RESTART);
            if (r == GAME_DONE) best_set(s->id, sc);
            music_menu();
            previewing = false;
            preview = -100000;
            sFade = 1;
            last = svcGetSystemTick();
            continue;
        }
        for (int i = 0; i < n; i++) {
            FpItem* f = &it[i];
            Tw* all[10] = {&f->tx, &f->ty, &f->ts, &f->ta, &f->bx, &f->by, &f->bs, &f->ix, &f->iy, &f->is};
            for (int k = 0; k < 10; k++) tw_step(all[k], ds);
        }
        for (int k = 0; k < 9; k++) {
            tw_step(&coverX[k], ds), tw_step(&coverY[k], ds), tw_step(&coverA[k], ds), tw_step(&coverR[k], ds);
        }
        tw_step(&diffY, ds);
        tw_step(&diffA, ds);

        bg_update(tm / 1000.0f * 2.0f);
        C2D_TextBufClear(gTextBuf);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(sTop, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(sTop);
        C2D_DrawParams bp = {.pos = {0, 0, 400, 240}, .depth = 0.1f};
        C2D_DrawImage(sBgImg, &bp, NULL);
        const SprFrame* bar = frame_of(&sUi, "freeplay_songbar", 0);
        for (int i = 0; i < n; i++) {
            FpItem* f = &it[i];
            float ty = tw_get(&f->ty);
            if (ty < -200 || ty > 900) continue;
            float al = tw_get(&f->ta);
            float bs = tw_get(&f->bs);
            if (bar) {
                /* la barre est mise à l'échelle depuis son coin (updateHitbox) */
                float w = bar->fw / MS, h = bar->fh / MS;
                img(bar, tw_get(&f->bx) - (w - w * bs) / 2 * 0, tw_get(&f->by) - (h - h * bs) / 2, bs, al, 0);
            }
            SongEntry* s = &sSongs[f->first];
            float ts = tw_get(&f->ts);
            textf(fFree, tw_get(&f->tx), ty, 42 * ts, 0, rgba(255, 255, 255, al), "%s", s->baseName);
            int ia = spr_anim(&sIcons, s->icon);
            if (ia >= 0) {
                const SprFrame* ic = &sIcons.frames[sIcons.anims[ia].frames[0]];
                float isc = tw_get(&f->is);
                img(ic, tw_get(&f->ix), tw_get(&f->iy), isc, al, 0);
            }
        }
        for (int k = 0; k < 9; k++) {
            const SprFrame* c = frame_of(&sUi, COV[k], 0);
            if (c) img(c, tw_get(&coverX[k]), tw_get(&coverY[k]), 1, tw_get(&coverA[k]), tw_get(&coverR[k]));
        }
        SongEntry* s = &sSongs[it[cur].first + curDiff];
        if (it[cur].count > 1)
            textf(fFree, 1050, tw_get(&diffY), 24, 0, rgba(255, 255, 255, tw_get(&diffA)), "< %s >", s->diff);
        else
            textf(fFree, 1050, tw_get(&diffY), 24, 0, rgba(255, 255, 255, tw_get(&diffA)), "%s", s->diff);
        img(frame_of(&sUi, "freeplay_overlay", 0), 0, 0, 1, 1, 0);
        textf(fFree, 10, 10, 42, 0, C2D_Color32(255, 255, 255, 255), "%s - %s", s->baseName, s->diff);
        textf(fFree, 10, 53, 32, 0, C2D_Color32(255, 255, 255, 255), "%s", s->composer);
        textf(fFree, 10, 720 - 46, 32, 0, C2D_Color32(255, 255, 255, 255), "PERSONAL BEST: %d", best_get(s->id));
        fade_draw(dt);

        C2D_TargetClear(sBottom, C2D_Color32(14, 12, 30, 255));
        C2D_SceneBegin(sBottom);
        C2D_DrawRectSolid(0, 0, 0.5f, 320, 36, C2D_Color32(247, 1, 116, 255));
        draw_text_centered(160, 8, 0.6f, C2D_Color32(255, 255, 255, 255), "FREEPLAY  %d / %d", cur + 1, n);
        for (int k = -3; k <= 3; k++) {
            int i = cur + k;
            if (i < 0 || i >= n) continue;
            draw_text_centered(160, 100 + k * 18, k == 0 ? 0.55f : 0.45f,
                k == 0 ? C2D_Color32(2, 244, 240, 255) : C2D_Color32(170, 170, 210, 255), "%s", sSongs[it[i].first].baseName);
        }
        draw_text_centered(160, 200, 0.4f, C2D_Color32(150, 150, 190, 255), "Haut/Bas : chanson   Gauche/Droite : difficulte");
        draw_text_centered(160, 218, 0.4f, C2D_Color32(150, 150, 190, 255), "A : jouer   B : retour   L/R : +-5");
        C3D_FrameEnd(0);
    }
    free(it);
}

/* ---------------------------------------------------------------- menu principal */
static int main_menu(void) {
    static const char* OPT[5] = {"Story Mode", "Freeplay", "Options", "Credits", "Extra"};
    static int sel = 1;
    static const char* ARTS[3] = {"desk", "grrrr", "shadum"};
    const char* art = ARTS[(osGetTime() / 7) % 3];
    float t[5] = {0};
    float sqx = 0, sqy = 0, sqa = 30;
    bool selected = false;
    float selT = 0, flash = 0;
    u64 last = svcGetSystemTick();
    sFade = 1;
    float textW[5];
    C2D_TextBufClear(gTextBuf);
    for (int i = 0; i < 5; i++) textW[i] = text_w(fVcr, 48, OPT[i]);
    float ty0 = 720 - 120 - 4 * 60;
    sqx = 1280 - 100 - textW[sel] - 32;
    sqy = ty0 + sel * 60 + 24 - 8;
    while (aptMainLoop()) {
        u64 now = svcGetSystemTick();
        float dt = (float)((now - last) * 1000.0 / SYSCLOCK_ARM11);
        last = now;
        float ds = dt / 1000.0f;
        hidScanInput();
        u32 down = auto_down();
        audio_tick();
        if (!selected) {
            if (down & (KEY_DUP | KEY_CPAD_UP)) {
                sel = (sel + 4) % 5;
                sfx("scroll");
            }
            if (down & (KEY_DDOWN | KEY_CPAD_DOWN)) {
                sel = (sel + 1) % 5;
                sfx("scroll");
            }
            if (down & KEY_A) {
                selected = true;
                selT = 0;
                flash = 1;
                sfx("selected");
            }
            if (down & KEY_B) {
                sfx("cancel");
                return -1;
            }
        } else {
            selT += dt;
            if (selT > 900) return sel;
        }
        float k = 1 - powf(0.5f, dt / 16.67f);
        for (int i = 0; i < 5; i++) t[i] = lerpf(t[i], i == sel ? 1 : 0, k);
        if (!selected) {
            sqx = lerpf(sqx, 1280 - 100 - textW[sel] - 32, k);
            sqy = lerpf(sqy, ty0 + sel * 60 + 24 - 8, k);
            sqa += ds * 75;
        } else {
            sqa += ds * 100;
        }

        C2D_TextBufClear(gTextBuf);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(sTop, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(sTop);
        const SprFrame* a = frame_of(&sArt, art, 0);
        if (a) img(a, (1280 - a->fw / MS) / 2, -720 * 0.25f + (720 * 1.4f - a->fh / MS) / 2 * 0, 1, 1, 0);
        img(frame_of(&sUi, "mainmenu_fade", 0), 0, 0, 1, 1, 0);
        const SprFrame* lg = frame_of(&sTitle, "logo", 0);
        if (lg) {
            float lw = lg->fw / MS * 2 / 3, lh = lg->fh / MS * 2 / 3;
            /* img() met à l'échelle autour du centre : on recentre */
            img(lg, 1280 - lw - 25 - (lg->fw / MS - lw) / 2, 25 - (lg->fh / MS - lh) / 2, 2.0f / 3, 1, 0);
        }
        for (int i = 0; i < 5; i++) {
            float y = ty0 + i * 60;
            float x = 1280 - 100 - textW[i];
            float al = 1;
            if (selected && i != sel) al = 1 - selT / 500;
            textf(fVcr, x, y, 48, 0, rgba(255, 255, 255, al), "%s", OPT[i]);
            float bw = t[i] * textW[i] - 4;
            if (bw > 0) {
                float ba = ease_cube_out(t[i]);
                C2D_DrawRectSolid(FX(1280 - 100 - bw), FY(y + 52), 0.6f, FX(bw), 6 * MS + 0.5f, rgba(247, 1, 116, ba));
            }
        }
        /* carré cyan qui tourne */
        float sq = 16 * MS;
        C2D_DrawRectangle(FX(sqx) + sq / 2, FY(sqy) + sq / 2, 0.7f, sq, sq, C2D_Color32(2, 244, 240, 255),
            C2D_Color32(2, 244, 240, 255), C2D_Color32(2, 244, 240, 255), C2D_Color32(2, 244, 240, 255));
        {
            /* dessiné tourné : deux triangles */
            float cx0 = FX(sqx) + sq / 2, cy0 = FY(sqy) + sq / 2, r = sq * 0.7071f;
            float an = sqa * (float)M_PI / 180.0f;
            float px[4], py[4];
            for (int c = 0; c < 4; c++) {
                px[c] = cx0 + r * cosf(an + c * (float)M_PI / 2 + (float)M_PI / 4);
                py[c] = cy0 + r * sinf(an + c * (float)M_PI / 2 + (float)M_PI / 4);
            }
            uint32_t cc = C2D_Color32(2, 244, 240, 255);
            C2D_DrawTriangle(px[0], py[0], cc, px[1], py[1], cc, px[2], py[2], cc, 0.71f);
            C2D_DrawTriangle(px[0], py[0], cc, px[2], py[2], cc, px[3], py[3], cc, 0.71f);
        }
        textf(fVcr, 5, 720 - 22, 16, 0, C2D_Color32(255, 255, 255, 255), "Cyber Sensation: Malware Breakout V1");
        flash_draw(&flash, dt, 900);
        fade_draw(dt);
        bottom_info("Menu principal", "Haut/Bas : choisir   A : valider", "B : ecran titre");
        C3D_FrameEnd(0);
    }
    return -2;
}

/* ---------------------------------------------------------------- boucle des menus */
static void extra_screen(void) {
    u64 last = svcGetSystemTick();
    sFade = 1;
    while (aptMainLoop()) {
        u64 now = svcGetSystemTick();
        float dt = (float)((now - last) * 1000.0 / SYSCLOCK_ARM11);
        last = now;
        hidScanInput();
        audio_tick();
        if (auto_down() & (KEY_A | KEY_B)) return;
        C2D_TextBufClear(gTextBuf);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(sTop, C2D_Color32(0, 0, 0, 255));
        C2D_SceneBegin(sTop);
        if (sBgReady) {
            C2D_DrawParams p = {.pos = {0, 0, 400, 240}, .depth = 0.1f};
            C2D_DrawImage(sBgImg, &p, NULL);
        }
        textf(fFree, 640, 250, 56, 1, C2D_Color32(255, 255, 255, 255), "EXTRA");
        textf(fVcr, 640, 360, 30, 1, C2D_Color32(255, 255, 255, 255), "Les bonus du mod sont sur un Google Drive :");
        textf(fVcr, 640, 410, 30, 1, C2D_Color32(2, 244, 240, 255), "ouvre le jeu sur PC pour y acceder.");
        fade_draw(dt);
        bottom_info("Extra", "A / B : retour", NULL);
        C3D_FrameEnd(0);
    }
}

static bool load_all(void) {
    char p[320], t[320];
    snprintf(t, sizeof(t), "%s/tex", gModDir);
#define L(spr, name)                                          \
    snprintf(p, sizeof(p), "%s/ui/%s.spr", gModDir, name); \
    spr_load(spr, p, t)
    L(&sTitle, "title");
    L(&sUi, "menu");
    L(&sArt, "art");
    L(&sStory, "story");
    L(&sIcons, "icons");
    snprintf(p, sizeof(p), "%s/ui/freeplay.bcfnt", gModDir);
    fFree = C2D_FontLoad(p);
    snprintf(p, sizeof(p), "%s/ui/vcr.bcfnt", gModDir);
    fVcr = C2D_FontLoad(p);
    return true;
}

int load_songs(SongEntry** out);

void menu_run(C3D_RenderTarget* top, C3D_RenderTarget* bottom, bool audioOk) {
    sTop = top;
    sBottom = bottom;
    sAudio = audioOk;
    settings_load();
    load_all();
    scores_load();
    sSongCount = load_songs(&sSongs);
    bg_update(0);
    music_menu();
    while (aptMainLoop()) {
        if (!title_screen()) break;
        int r;
        while ((r = main_menu()) >= 0) {
            if (r == 0) story_screen();
            else if (r == 1) freeplay_screen();
            else if (r == 2) options_screen();
            else if (r == 3) {
                const char* cr[1] = {"credits"};
                int i = find_song("credits", NULL);
                if (i >= 0) {
                    stream_close(&sMusic);
                    GameResult g;
                    int sc;
                    float acc;
                    do g = game_play(&sSongs[i], sTop, sBottom, &sc, &acc);
                    while (g == GAME_RESTART);
                    music_menu();
                }
                (void)cr;
            } else if (r == 4) extra_screen();
        }
        if (r == -2) break;
    }
    stream_close(&sMusic);
    sfx_stop_all();
}
