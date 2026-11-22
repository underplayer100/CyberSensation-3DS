#pragma once
#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <stdbool.h>
#include <stdint.h>

#define SCREEN_W 400
#define SCREEN_H 240
#define WORLD_S (400.0f / 1280.0f)

/* ---------------- sprites (.spr) */
typedef struct {
    uint8_t page;
    C2D_Image img;
    Tex3DS_SubTexture sub;
    float ox, oy, fw, fh;   /* décalage du morceau découpé dans le cadre complet, taille du cadre */
} SprFrame;

typedef struct {
    char name[32];
    float fps;
    bool loop;
    float ox, oy;           /* décalage d'animation (unités monde FNF) */
    uint16_t count;
    uint16_t* frames;
} SprAnim;

typedef struct {
    char key[16];
    char type;
    float f;
    char s[32];
} SprMeta;

typedef struct {
    int pageCount, frameCount, animCount, metaCount;
    C3D_Tex* pages;
    SprFrame* frames;
    SprAnim* anims;
    SprMeta* meta;
} Spr;

bool spr_load(Spr* s, const char* path, const char* texDir);
void spr_free(Spr* s);
int spr_anim(const Spr* s, const char* name);
float spr_metaf(const Spr* s, const char* key, float def);
const char* spr_metas(const Spr* s, const char* key, const char* def);

/* ---------------- audio (.adp, DSP ADPCM en continu) */
#define STREAM_BUFS 4
typedef struct {
    FILE* f;
    uint32_t rate, channels, samples, chunkSamples, chunkCount, chunkBytes;
    uint16_t coefs[16];
    int chn[2];
    uint8_t* data[STREAM_BUFS][2];
    ndspWaveBuf wb[STREAM_BUFS][2];
    ndspAdpcmData ad[STREAM_BUFS][2];
    int bufChunk[STREAM_BUFS];
    uint32_t nextChunk;
    bool open, started, loop;
    float volume;
} Stream;

bool stream_open(Stream* s, const char* path, int firstChannel);
void stream_start(Stream* s);
void stream_update(Stream* s);
double stream_position_ms(Stream* s);   /* -1 si pas encore lancé */
bool stream_finished(Stream* s);
void stream_volume(Stream* s, float v);
void stream_close(Stream* s);
void stream_pause(Stream* s, bool paused);
void sfx_enable(bool on);
void sfx_play(const char* path, float volume);
void sfx_update(void);
void sfx_stop_all(void);

/* ---------------- jeu */
typedef struct {
    char id[64];
    char name[64];
    char icon[32];
    uint32_t color;
    char base[64];      /* dossier de la chanson (regroupe les difficultés) */
    char diff[16];
    char composer[48];
    char baseName[64];  /* nom affiché sans difficulté */
    int cover;
} SongEntry;

/* touches par défaut des 4 flèches (gauche, bas, haut, droite) */
#define DEFAULT_LANE_KEYS {KEY_DLEFT | KEY_Y | KEY_CPAD_LEFT, KEY_DDOWN | KEY_B | KEY_CPAD_DOWN, \
                           KEY_DUP | KEY_X | KEY_CPAD_UP, KEY_DRIGHT | KEY_A | KEY_CPAD_RIGHT}

typedef struct {
    bool downscroll;
    bool ghostTapping;
    u32 laneKeys[4];        /* boutons de chaque flèche, configurables dans Options > Touches */
} Settings;
extern Settings gSettings;

extern char gModDir[256];
extern C2D_TextBuf gTextBuf;
void draw_text(float x, float y, float size, uint32_t color, const char* fmt, ...);
void draw_text_centered(float cx, float y, float size, uint32_t color, const char* fmt, ...);

typedef enum { GAME_QUIT, GAME_RESTART, GAME_DONE } GameResult;
/* texte avec la police VCR du mod, coordonnées FNF (1280x720), align 0 gauche 1 centre 2 droite */
void hud_text(float x, float y, float size, int align, uint32_t color, bool outline, const char* fmt, ...);
void menu_run(C3D_RenderTarget* top, C3D_RenderTarget* bottom, bool audioOk);
void flog(const char* fmt, ...);
GameResult game_play(const SongEntry* song, C3D_RenderTarget* top, C3D_RenderTarget* bottom,
    int* outScore, float* outAccuracy);
