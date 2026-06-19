#include "zoom.h"
#include "storage.h"  // storage_ensure_dir
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ZOOM_MAX 512

typedef struct {
    char host[256];
    double zoom;
} ZoomEntry;

static ZoomEntry entries[ZOOM_MAX];
static int count;
static char filepath[512];

static int zoom_find(const char *host) {
    for (int i = 0; i < count; i++)
        if (strcmp(entries[i].host, host) == 0) return i;
    return -1;
}

void zoom_load(const char *path) {
    snprintf(filepath, sizeof(filepath), "%s", path);
    count = 0;

    FILE *f = fopen(path, "r");
    if (!f) return;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len < 0) { fclose(f); return; }
    char *buf = malloc(len + 1);
    if (!buf) { fclose(f); return; }
    size_t got = fread(buf, 1, len, f);
    buf[got] = '\0';
    fclose(f);

    // Parse {"host":zoom,...}. Hostnames contain no JSON-special characters,
    // so a naive quote/colon scan is safe.
    char *p = buf;
    while (count < ZOOM_MAX && (p = strchr(p, '"'))) {
        p++;
        char *q = strchr(p, '"');
        if (!q) break;
        size_t hlen = (size_t)(q - p);
        if (hlen >= sizeof(entries[0].host)) hlen = sizeof(entries[0].host) - 1;
        memcpy(entries[count].host, p, hlen);
        entries[count].host[hlen] = '\0';
        char *colon = strchr(q, ':');
        if (!colon) break;
        entries[count].zoom = atof(colon + 1);
        if (entries[count].zoom > 0.0) count++;
        p = colon + 1;
    }
    free(buf);
}

static void zoom_save(void) {
    if (!filepath[0]) return;
    storage_ensure_dir();
    FILE *f = fopen(filepath, "w");
    if (!f) return;
    fprintf(f, "{");
    for (int i = 0; i < count; i++)
        fprintf(f, "%s\"%s\":%.2f", i ? "," : "", entries[i].host, entries[i].zoom);
    fprintf(f, "}\n");
    fclose(f);
}

double zoom_get(const char *host) {
    if (!host || !host[0]) return 0.0;
    int i = zoom_find(host);
    return i >= 0 ? entries[i].zoom : 0.0;
}

void zoom_set(const char *host, double z) {
    if (!host || !host[0]) return;
    int i = zoom_find(host);

    // Default zoom → forget the host (keeps zoom.json minimal).
    if (fabs(z - 1.0) < 0.001) {
        if (i >= 0) { entries[i] = entries[--count]; zoom_save(); }
        return;
    }
    if (i < 0) {
        if (count >= ZOOM_MAX) return;
        i = count++;
        snprintf(entries[i].host, sizeof(entries[i].host), "%s", host);
    }
    entries[i].zoom = z;
    zoom_save();
}
