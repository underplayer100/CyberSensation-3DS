/* Lecture continue de fichiers DSP ADPCM depuis la SD : le DSP de la 3DS décode lui-même,
 * le processeur ne fait que lire ~32 Ko/s par piste. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fnf.h"

static bool stream_fill(Stream* s, int b) {
    if (s->nextChunk >= s->chunkCount && s->loop) s->nextChunk = 0;
    if (s->nextChunk >= s->chunkCount) return false;
    long off = 60L + (long)s->nextChunk * (long)s->channels * (long)(4 + s->chunkBytes);
    fseek(s->f, off, SEEK_SET);
    uint32_t samples = s->chunkSamples;
    uint32_t left = s->samples - s->nextChunk * s->chunkSamples;
    if (left < samples) samples = left;
    for (uint32_t c = 0; c < s->channels; c++) {
        int16_t hist[2];
        if (fread(hist, 2, 2, s->f) != 2) return false;
        if (fread(s->data[b][c], 1, s->chunkBytes, s->f) != s->chunkBytes) return false;
        DSP_FlushDataCache(s->data[b][c], s->chunkBytes);
        ndspAdpcmData* ad = &s->ad[b][c];
        ad->index = s->data[b][c][0];
        ad->history0 = hist[0];
        ad->history1 = hist[1];
        ndspWaveBuf* wb = &s->wb[b][c];
        memset(wb, 0, sizeof(*wb));
        wb->data_adpcm = s->data[b][c];
        wb->nsamples = samples;
        wb->adpcm_data = ad;
        wb->looping = false;
        ndspChnWaveBufAdd(s->chn[c], wb);
    }
    s->bufChunk[b] = (int)s->nextChunk;
    s->nextChunk++;
    return true;
}

bool stream_open(Stream* s, const char* path, int firstChannel) {
    memset(s, 0, sizeof(*s));
    s->f = fopen(path, "rb");
    if (!s->f) return false;
    setvbuf(s->f, NULL, _IOFBF, 32 * 1024);
    char magic[4];
    uint32_t h[6];
    if (fread(magic, 1, 4, s->f) != 4 || memcmp(magic, "FADP", 4) != 0 || fread(h, 4, 6, s->f) != 6 ||
        fread(s->coefs, 2, 16, s->f) != 16) {
        fclose(s->f);
        s->f = NULL;
        return false;
    }
    s->rate = h[1];
    s->channels = h[2] > 2 ? 2 : h[2];
    s->samples = h[3];
    s->chunkSamples = h[4];
    s->chunkCount = h[5];
    s->chunkBytes = s->chunkSamples / 14 * 8;
    s->volume = 1.0f;
    for (uint32_t c = 0; c < s->channels; c++) {
        int ch = firstChannel + (int)c;
        s->chn[c] = ch;
        ndspChnReset(ch);
        ndspChnSetInterp(ch, NDSP_INTERP_LINEAR);
        ndspChnSetRate(ch, (float)s->rate);
        ndspChnSetFormat(ch, NDSP_FORMAT_MONO_ADPCM);
        ndspChnSetAdpcmCoefs(ch, s->coefs);
        for (int b = 0; b < STREAM_BUFS; b++) s->data[b][c] = linearAlloc(s->chunkBytes);
    }
    stream_volume(s, 1.0f);
    for (int b = 0; b < STREAM_BUFS; b++) s->bufChunk[b] = -1;
    s->open = true;
    return true;
}

void stream_volume(Stream* s, float v) {
    if (!s->open && s->channels == 0) return;
    s->volume = v;
    for (uint32_t c = 0; c < s->channels; c++) {
        float mix[12] = {0};
        if (s->channels == 1) {
            mix[0] = v;
            mix[1] = v;
        } else {
            mix[c] = v;
        }
        ndspChnSetMix(s->chn[c], mix);
    }
}

void stream_start(Stream* s) {
    if (!s->open) return;
    ndspChnSetPaused(s->chn[0], true);
    if (s->channels > 1) ndspChnSetPaused(s->chn[1], true);
    for (int b = 0; b < STREAM_BUFS; b++) stream_fill(s, b);
    s->started = true;
}

/* à appeler juste après stream_start sur toutes les pistes, pour qu'elles partent ensemble */
void stream_pause(Stream* s, bool paused) {
    if (!s->open) return;
    for (uint32_t c = 0; c < s->channels; c++) ndspChnSetPaused(s->chn[c], paused);
}

void stream_update(Stream* s) {
    if (!s->open || !s->started) return;
    for (int b = 0; b < STREAM_BUFS; b++) {
        if (s->bufChunk[b] < 0) continue;
        bool done = true;
        for (uint32_t c = 0; c < s->channels; c++)
            if (s->wb[b][c].status != NDSP_WBUF_DONE) done = false;
        if (done) {
            s->bufChunk[b] = -1;
            stream_fill(s, b);
        }
    }
}

double stream_position_ms(Stream* s) {
    if (!s->open || !s->started) return -1.0;
    u16 seq = ndspChnGetWaveBufSeq(s->chn[0]);
    int chunk = -1;
    for (int b = 0; b < STREAM_BUFS; b++)
        if (s->bufChunk[b] >= 0 && s->wb[b][0].sequence_id == seq &&
            (s->wb[b][0].status == NDSP_WBUF_PLAYING || s->wb[b][0].status == NDSP_WBUF_QUEUED))
            chunk = s->bufChunk[b];
    if (chunk < 0) {
        if (stream_finished(s)) return (double)s->samples * 1000.0 / s->rate;
        /* entre deux blocs : prendre le plus ancien bloc en attente */
        int best = -1;
        for (int b = 0; b < STREAM_BUFS; b++)
            if (s->bufChunk[b] >= 0 && (best < 0 || s->bufChunk[b] < best)) best = s->bufChunk[b];
        if (best < 0) return (double)s->nextChunk * s->chunkSamples * 1000.0 / s->rate;
        return (double)best * s->chunkSamples * 1000.0 / s->rate;
    }
    double pos = (double)chunk * s->chunkSamples + ndspChnGetSamplePos(s->chn[0]);
    return pos * 1000.0 / s->rate;
}

bool stream_finished(Stream* s) {
    if (!s->open || !s->started) return false;
    if (s->nextChunk < s->chunkCount) return false;
    for (int b = 0; b < STREAM_BUFS; b++)
        if (s->bufChunk[b] >= 0 && s->wb[b][0].status != NDSP_WBUF_DONE) return false;
    return true;
}

void stream_close(Stream* s) {
    if (!s->open) {
        if (s->f) fclose(s->f);
        return;
    }
    for (uint32_t c = 0; c < s->channels; c++) {
        ndspChnWaveBufClear(s->chn[c]);
        ndspChnReset(s->chn[c]);
    }
    svcSleepThread(20 * 1000 * 1000);
    for (uint32_t c = 0; c < s->channels; c++)
        for (int b = 0; b < STREAM_BUFS; b++)
            if (s->data[b][c]) linearFree(s->data[b][c]);
    if (s->f) fclose(s->f);
    memset(s, 0, sizeof(*s));
}
