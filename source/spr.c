#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fnf.h"

static bool rd(FILE* f, void* p, size_t n) { return fread(p, 1, n, f) == n; }

bool spr_load(Spr* s, const char* path, const char* texDir) {
    memset(s, 0, sizeof(*s));
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    char magic[4];
    uint32_t hdr[4];
    if (!rd(f, magic, 4) || memcmp(magic, "FSPR", 4) != 0 || !rd(f, hdr, sizeof(hdr))) {
        fclose(f);
        return false;
    }
    s->pageCount = (int)hdr[1];
    s->frameCount = (int)hdr[2];
    s->animCount = (int)hdr[3];
    s->pages = calloc((size_t)s->pageCount + 1, sizeof(C3D_Tex));
    s->frames = calloc((size_t)s->frameCount + 1, sizeof(SprFrame));
    s->anims = calloc((size_t)s->animCount + 1, sizeof(SprAnim));
    uint16_t* pageSize = calloc((size_t)s->pageCount * 2 + 2, sizeof(uint16_t));
    bool ok = true;
    for (int i = 0; i < s->pageCount && ok; i++) {
        char name[48];
        ok = rd(f, name, 48) && rd(f, &pageSize[i * 2], 4);
        if (!ok) break;
        char tp[512];
        snprintf(tp, sizeof(tp), "%s/%s", texDir, name);
        FILE* tf = fopen(tp, "rb");
        if (!tf) {
            ok = false;
            break;
        }
        Tex3DS_Texture t3 = Tex3DS_TextureImportStdio(tf, &s->pages[i], NULL, false);
        fclose(tf);
        if (!t3) {
            ok = false;
            break;
        }
        Tex3DS_TextureFree(t3);
        C3D_TexSetFilter(&s->pages[i], GPU_LINEAR, GPU_LINEAR);
        C3D_TexSetWrap(&s->pages[i], GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
    }
    for (int i = 0; i < s->frameCount && ok; i++) {
        uint8_t b[26];
        ok = rd(f, b, 26);
        if (!ok) break;
        SprFrame* fr = &s->frames[i];
        uint16_t x, y, w, h;
        fr->page = b[0];
        memcpy(&x, b + 2, 2);
        memcpy(&y, b + 4, 2);
        memcpy(&w, b + 6, 2);
        memcpy(&h, b + 8, 2);
        memcpy(&fr->ox, b + 10, 4);
        memcpy(&fr->oy, b + 14, 4);
        memcpy(&fr->fw, b + 18, 4);
        memcpy(&fr->fh, b + 22, 4);
        if (fr->page >= s->pageCount) fr->page = 0;
        float pw = (float)s->pages[fr->page].width, ph = (float)s->pages[fr->page].height;
        fr->sub.width = w;
        fr->sub.height = h;
        fr->sub.left = x / pw;
        fr->sub.right = (x + w) / pw;
        fr->sub.top = 1.0f - y / ph;
        fr->sub.bottom = 1.0f - (y + h) / ph;
        fr->img.tex = &s->pages[fr->page];
        fr->img.subtex = &fr->sub;
    }
    for (int i = 0; i < s->animCount && ok; i++) {
        SprAnim* a = &s->anims[i];
        uint8_t b[18];
        ok = rd(f, a->name, 32) && rd(f, b, 18);
        if (!ok) break;
        a->name[31] = 0;
        memcpy(&a->fps, b, 4);
        a->loop = b[4] != 0;
        memcpy(&a->ox, b + 8, 4);
        memcpy(&a->oy, b + 12, 4);
        memcpy(&a->count, b + 16, 2);
        a->frames = calloc((size_t)a->count + 2, sizeof(uint16_t));
        ok = rd(f, a->frames, (size_t)a->count * 2);
        if (a->count % 2) {
            uint16_t pad;
            rd(f, &pad, 2);
        }
        for (int k = 0; k < a->count; k++)
            if (a->frames[k] >= s->frameCount) a->frames[k] = 0;
    }
    uint32_t mc = 0;
    if (ok && rd(f, &mc, 4) && mc < 256) {
        s->meta = calloc(mc + 1, sizeof(SprMeta));
        for (uint32_t i = 0; i < mc; i++) {
            char t[4];
            uint8_t v[32];
            if (!rd(f, s->meta[i].key, 16) || !rd(f, t, 4) || !rd(f, v, 32)) break;
            s->meta[i].key[15] = 0;
            s->meta[i].type = t[0];
            if (t[0] == 'f') memcpy(&s->meta[i].f, v, 4);
            else {
                memcpy(s->meta[i].s, v, 32);
                s->meta[i].s[31] = 0;
            }
            s->metaCount++;
        }
    }
    free(pageSize);
    fclose(f);
    if (!ok) spr_free(s);
    return ok;
}

void spr_free(Spr* s) {
    for (int i = 0; i < s->pageCount; i++)
        if (s->pages && s->pages[i].data) C3D_TexDelete(&s->pages[i]);
    for (int i = 0; i < s->animCount; i++)
        if (s->anims) free(s->anims[i].frames);
    free(s->pages);
    free(s->frames);
    free(s->anims);
    free(s->meta);
    memset(s, 0, sizeof(*s));
}

int spr_anim(const Spr* s, const char* name) {
    for (int i = 0; i < s->animCount; i++)
        if (strcmp(s->anims[i].name, name) == 0) return i;
    return -1;
}

float spr_metaf(const Spr* s, const char* key, float def) {
    for (int i = 0; i < s->metaCount; i++)
        if (s->meta[i].type == 'f' && strcmp(s->meta[i].key, key) == 0) return s->meta[i].f;
    return def;
}

const char* spr_metas(const Spr* s, const char* key, const char* def) {
    for (int i = 0; i < s->metaCount; i++)
        if (s->meta[i].type == 's' && strcmp(s->meta[i].key, key) == 0) return s->meta[i].s;
    return def;
}
