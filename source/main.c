/* FNF 3DS : moteur Friday Night Funkin' pour Old 3DS, lit les mods convertis par tools/convert.py */
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fnf.h"

char gModDir[256] = "sdmc:/3ds/fnf3ds/cyber";
C2D_TextBuf gTextBuf;
extern bool gAutoTest;
extern float gAutoSeconds;

void flog(const char* fmt, ...) {
    FILE* f = fopen("sdmc:/3ds/fnf3ds/log.txt", "a");
    if (!f) return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fputc(10, f);
    fclose(f);
}

/* capture de l'écran du haut (BGR8, tourné) en BMP 24 bits */
void screenshot(const char* path) {
    u16 fw, fh;
    u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &fw, &fh);
    FILE* f = fopen(path, "wb");
    if (!f) return;
    u32 W = 400, H = 240, row = W * 3, size = 54 + row * H;
    u8 hdr[54] = {'B', 'M'};
    memcpy(hdr + 2, &size, 4);
    u32 off = 54, dib = 40, planes_bpp = 1 | (24 << 16), img = row * H;
    memcpy(hdr + 10, &off, 4);
    memcpy(hdr + 14, &dib, 4);
    memcpy(hdr + 18, &W, 4);
    memcpy(hdr + 22, &H, 4);
    memcpy(hdr + 26, &planes_bpp, 4);
    memcpy(hdr + 34, &img, 4);
    fwrite(hdr, 1, 54, f);
    u8* line = malloc(row);
    for (int y = (int)H - 1; y >= 0; y--) {
        for (u32 x = 0; x < W; x++) {
            const u8* s = fb + (x * fw + (fw - 1 - (u32)y)) * 3;
            line[x * 3] = s[0];
            line[x * 3 + 1] = s[1];
            line[x * 3 + 2] = s[2];
        }
        fwrite(line, 1, row, f);
    }
    free(line);
    fclose(f);
}

static void vdraw(float x, float y, float size, uint32_t color, bool center, const char* fmt, va_list ap) {
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    C2D_Text t;
    C2D_TextParse(&t, gTextBuf, buf);
    C2D_TextOptimize(&t);
    if (center) {
        float w, h;
        C2D_TextGetDimensions(&t, size, size, &w, &h);
        x -= w / 2;
    }
    C2D_DrawText(&t, C2D_WithColor, x, y, 0.9f, size, size, color);
}

void draw_text(float x, float y, float size, uint32_t color, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vdraw(x, y, size, color, false, fmt, ap);
    va_end(ap);
}

void draw_text_centered(float cx, float y, float size, uint32_t color, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vdraw(cx, y, size, color, true, fmt, ap);
    va_end(ap);
}

int load_songs(SongEntry** out) {
    char p[300];
    snprintf(p, sizeof(p), "%s/songs.lst", gModDir);
    FILE* f = fopen(p, "r");
    if (!f) return 0;
    int cap = 64, n = 0;
    SongEntry* s = calloc((size_t)cap, sizeof(SongEntry));
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (!line[0]) continue;
        if (n == cap) {
            cap *= 2;
            s = realloc(s, (size_t)cap * sizeof(SongEntry));
        }
        SongEntry* e = &s[n];
        memset(e, 0, sizeof(*e));
        char* fields[9] = {0};
        int k = 0;
        char* tok = line;
        fields[k++] = tok;
        while (k < 9 && (tok = strchr(tok, '|'))) {
            *tok++ = 0;
            fields[k++] = tok;
        }
        snprintf(e->id, sizeof(e->id), "%s", fields[0]);
        snprintf(e->name, sizeof(e->name), "%s", fields[1] ? fields[1] : fields[0]);
        snprintf(e->icon, sizeof(e->icon), "%s", fields[2] ? fields[2] : "");
        int r = 146, g = 113, b = 253;
        if (fields[3] && fields[3][0]) sscanf(fields[3], "%d,%d,%d", &r, &g, &b);
        e->color = C2D_Color32((u8)r, (u8)g, (u8)b, 255);
        snprintf(e->base, sizeof(e->base), "%s", fields[4] ? fields[4] : e->id);
        snprintf(e->diff, sizeof(e->diff), "%s", fields[5] ? fields[5] : "REGULAR");
        snprintf(e->composer, sizeof(e->composer), "%s", fields[6] ? fields[6] : "");
        e->cover = fields[7] ? atoi(fields[7]) : 4;
        snprintf(e->baseName, sizeof(e->baseName), "%s", fields[8] ? fields[8] : e->name);
        n++;
    }
    fclose(f);
    *out = s;
    return n;
}

static bool find_mod(void) {
    DIR* d = opendir("sdmc:/3ds/fnf3ds");
    if (!d) return false;
    struct dirent* e;
    bool found = false;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.') continue;
        char p[300];
        snprintf(p, sizeof(p), "sdmc:/3ds/fnf3ds/%s/songs.lst", e->d_name);
        FILE* f = fopen(p, "r");
        if (f) {
            fclose(f);
            snprintf(gModDir, sizeof(gModDir), "sdmc:/3ds/fnf3ds/%s", e->d_name);
            found = true;
            break;
        }
    }
    closedir(d);
    return found;
}

int main(void) {
    gfxInitDefault();
    osSetSpeedupEnable(true);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(4096);
    C2D_Prepare();
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
    gTextBuf = C2D_TextBufNew(4096);
    bool audioOk = R_SUCCEEDED(ndspInit());
    if (audioOk) ndspSetOutputMode(NDSP_OUTPUT_STEREO);

    bool modOk = find_mod();
    SongEntry* songs = NULL;
    int songCount = modOk ? load_songs(&songs) : 0;
    int sel = 0, scroll = 0;
    char lastResult[128] = "";
    FILE* at = fopen("sdmc:/3ds/fnf3ds/autotest.txt", "r");
    if (at) {
        char want[64] = "";
        if (fscanf(at, "%63s %f", want, &gAutoSeconds) < 1) want[0] = 0;
        fclose(at);
        if (strcmp(want, "menus") == 0) {
            extern bool gAutoMenu;
            gAutoMenu = true;
            flog("AUTOTEST menus");
            menu_run(top, bottom, audioOk);
            goto quit;
        }
        gAutoTest = true;
        flog("AUTOTEST debut, mod=%s, %d chansons, son=%s", gModDir, songCount, audioOk ? "oui" : "NON");
        for (int i = 0; i < songCount; i++)
            if (strcmp(songs[i].id, want) == 0) sel = i;
        if (songCount > 0) {
            int sc = 0;
            float acc = 0;
            GameResult r = game_play(&songs[sel], top, bottom, &sc, &acc);
            flog("AUTOTEST FINI resultat=%d score=%d precision=%.2f", (int)r, sc, acc);
        } else {
            flog("AUTOTEST FINI aucune chanson");
        }
        goto quit;
    }

    if (modOk && songCount > 0) {
        menu_run(top, bottom, audioOk);
    } else {
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & (KEY_START | KEY_B)) break;
            C2D_TextBufClear(gTextBuf);
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            C2D_TargetClear(top, C2D_Color32(40, 20, 60, 255));
            C2D_SceneBegin(top);
            draw_text_centered(200, 90, 0.6f, C2D_Color32(255, 255, 255, 255), "Aucun mod trouve");
            draw_text_centered(200, 120, 0.45f, C2D_Color32(255, 255, 255, 255), "sdmc:/3ds/fnf3ds/<mod>/songs.lst");
            C2D_TargetClear(bottom, C2D_Color32(20, 16, 40, 255));
            C3D_FrameEnd(0);
        }
    }
    (void)sel;
    (void)scroll;
    (void)lastResult;

quit:
    free(songs);
    C2D_TextBufDelete(gTextBuf);
    if (audioOk) ndspExit();
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
