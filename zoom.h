#ifndef SWIM_ZOOM_H
#define SWIM_ZOOM_H

// Per-domain zoom persistence (~/.config/swim/zoom.json).
// Map of hostname -> page zoom factor. Hosts at the default 1.0 are not stored.

void   zoom_load(const char *path);          // load map from disk (once, at startup)
double zoom_get(const char *host);           // saved zoom for host, or 0.0 if none
void   zoom_set(const char *host, double z); // set + persist; z==1.0 forgets host

#endif
