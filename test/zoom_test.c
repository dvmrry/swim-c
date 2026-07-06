// make test — per-domain zoom persistence tests. Builds as a separate
// binary; never linked into the release build.
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

void storage_ensure_dir(void) {}  // stub: keep the test standalone
#include "zoom.c"

static int near(double a, double b) { return fabs(a - b) < 0.005; }

int main(void) {
    const char *path = "/tmp/swim_zoom_test.json";
    remove(path);

    zoom_load(path);                     // missing file -> empty map
    assert(near(zoom_get("example.com"), 0.0));

    zoom_set("example.com", 1.50);
    zoom_set("news.ycombinator.com", 0.80);
    assert(near(zoom_get("example.com"), 1.50));
    assert(near(zoom_get("news.ycombinator.com"), 0.80));

    zoom_set("example.com", 1.00);       // default zoom forgets the host
    assert(near(zoom_get("example.com"), 0.0));

    zoom_load(path);                     // disk round-trip
    assert(near(zoom_get("news.ycombinator.com"), 0.80));
    assert(near(zoom_get("example.com"), 0.0));

    remove(path);
    printf("zoom_test: PASS\n");
    return 0;
}
