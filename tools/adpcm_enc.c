/* Encodeur DSP ADPCM pour le moteur FNF 3DS.
 * Entrée : PCM 16 bits entrelacé. Sortie : fichier FADP découpé en blocs ; chaque bloc donne
 * l'état du décodeur au début (pour que la 3DS puisse lire chaque bloc indépendamment).
 *
 * En-tête : "FADP" u32 version, u32 rate, u32 channels, u32 samples, u32 chunkSamples,
 *           u32 chunkCount, s16 coefs[16]
 * Bloc    : pour chaque canal : s16 hist1, s16 hist2, u8 data[chunkSamples/14*8]
 *
 * gcc -O2 -o adpcm_enc.exe adpcm_enc.c
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_FRAMES 1024
#define CHUNK_SAMPLES (CHUNK_FRAMES * 14)
#define CHUNK_BYTES (CHUNK_FRAMES * 8)

static const int16_t COEFS[8][2] = {
    {0, 0}, {1024, 0}, {2048, 0}, {2560, -512}, {3072, -1024}, {3584, -1536}, {3968, -1920}, {4096, -2048},
};

static int clamp16(int v) { return v < -32768 ? -32768 : v > 32767 ? 32767 : v; }

/* Encode 14 échantillons ; renvoie l'erreur, écrit les 8 octets et met à jour l'historique. */
static long long try_frame(const int16_t* s, int pair, int shift, int* h1, int* h2, uint8_t* out) {
    int c1 = COEFS[pair][0], c2 = COEFS[pair][1];
    int y1 = *h1, y2 = *h2;
    long long err = 0;
    out[0] = (uint8_t)((pair << 4) | shift);
    for (int i = 0; i < 14; i++) {
        int pred = (c1 * y1 + c2 * y2 + 1024) >> 11;
        int diff = s[i] - pred;
        int scale = 1 << shift;
        int n = diff >= 0 ? (diff + scale / 2) / scale : -((-diff + scale / 2) / scale);
        if (n > 7) n = 7;
        if (n < -8) n = -8;
        int dec = clamp16((((n * scale) << 11) + 1024 + c1 * y1 + c2 * y2) >> 11);
        long long e = (long long)(s[i] - dec);
        err += e * e;
        y2 = y1;
        y1 = dec;
        if (i & 1) out[1 + i / 2] |= (uint8_t)(n & 15);
        else out[1 + i / 2] = (uint8_t)((n & 15) << 4);
    }
    *h1 = y1;
    *h2 = y2;
    return err;
}

static void encode_frame(const int16_t* s, int* h1, int* h2, uint8_t* out) {
    long long best = -1;
    uint8_t bestOut[8];
    int bh1 = 0, bh2 = 0;
    for (int p = 0; p < 8; p++) {
        for (int sh = 0; sh <= 12; sh++) {
            uint8_t tmp[8];
            int a = *h1, b = *h2;
            long long e = try_frame(s, p, sh, &a, &b, tmp);
            if (best < 0 || e < best) {
                best = e;
                memcpy(bestOut, tmp, 8);
                bh1 = a;
                bh2 = b;
            }
            if (e == 0) break;
        }
    }
    memcpy(out, bestOut, 8);
    *h1 = bh1;
    *h2 = bh2;
}

static void w32(FILE* f, uint32_t v) { fwrite(&v, 4, 1, f); }
static void w16(FILE* f, int16_t v) { fwrite(&v, 2, 1, f); }

int main(int argc, char** argv) {
    if (argc < 5) {
        fprintf(stderr, "usage: adpcm_enc in.raw out.adp rate channels\n");
        return 1;
    }
    FILE* in = fopen(argv[1], "rb");
    if (!in) return 2;
    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    fseek(in, 0, SEEK_SET);
    int rate = atoi(argv[3]), ch = atoi(argv[4]);
    uint32_t samples = (uint32_t)(size / 2 / ch);
    int16_t* pcm = malloc((size_t)size + 16);
    if (fread(pcm, 1, (size_t)size, in) != (size_t)size) return 3;
    fclose(in);
    uint32_t chunks = (samples + CHUNK_SAMPLES - 1) / CHUNK_SAMPLES;
    FILE* out = fopen(argv[2], "wb");
    if (!out) return 4;
    fwrite("FADP", 1, 4, out);
    w32(out, 1);
    w32(out, (uint32_t)rate);
    w32(out, (uint32_t)ch);
    w32(out, samples);
    w32(out, CHUNK_SAMPLES);
    w32(out, chunks);
    for (int p = 0; p < 8; p++) {
        w16(out, COEFS[p][0]);
        w16(out, COEFS[p][1]);
    }
    int h1[2] = {0, 0}, h2[2] = {0, 0};
    int16_t frame[14];
    uint8_t* buf = malloc(CHUNK_BYTES);
    for (uint32_t c = 0; c < chunks; c++) {
        for (int k = 0; k < ch; k++) {
            w16(out, (int16_t)h1[k]);
            w16(out, (int16_t)h2[k]);
            for (int f = 0; f < CHUNK_FRAMES; f++) {
                for (int i = 0; i < 14; i++) {
                    uint32_t idx = c * CHUNK_SAMPLES + (uint32_t)f * 14 + (uint32_t)i;
                    frame[i] = idx < samples ? pcm[(size_t)idx * ch + k] : 0;
                }
                encode_frame(frame, &h1[k], &h2[k], buf + f * 8);
            }
            fwrite(buf, 1, CHUNK_BYTES, out);
        }
    }
    fclose(out);
    free(buf);
    free(pcm);
    return 0;
}
