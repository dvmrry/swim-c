// make test — storage parser regression tests. Builds as a separate
// binary; never linked into the release build.
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "storage.c"

static const char *PATH = "/tmp/swim_storage_test.json";

static void write_file(const char *body) {
    FILE *f = fopen(PATH, "w");
    assert(f);
    fputs(body, f);
    fclose(f);
}

int main(void) {
    Storage s;

    // A title-less entry must not borrow fields from the next object
    write_file(
        "[\n"
        "  {\"url\":\"https://a.com\",\"title\":\"FIRST\",\"time\":100},\n"
        "  {\"url\":\"https://b.com\",\"time\":200},\n"
        "  {\"url\":\"https://c.com\",\"title\":\"THIRD\",\"time\":300}\n"
        "]\n");
    storage_init(&s, PATH);
    storage_load(&s);
    assert(s.count == 3);
    assert(strcmp(s.entries[1].url, "https://b.com") == 0);
    assert(s.entries[1].title[0] == '\0');
    assert(s.entries[1].timestamp == 200);
    assert(strcmp(s.entries[2].title, "THIRD") == 0);
    storage_free(&s);

    // Overflowing timestamp clamps (strtol) instead of undefined behavior
    write_file("[{\"url\":\"https://x.com\",\"title\":\"X\","
               "\"time\":99999999999999999999}]\n");
    storage_init(&s, PATH);
    storage_load(&s);
    assert(s.count == 1);
    assert(s.entries[0].timestamp == LONG_MAX);
    storage_free(&s);

    // Truncated input must not crash and loads nothing
    write_file("[{\"url\":\"https://trunc");
    storage_init(&s, PATH);
    storage_load(&s);
    assert(s.count == 0);
    storage_free(&s);

    // Escaped quotes and backslashes survive a save/load round-trip
    storage_init(&s, PATH);
    storage_add(&s, "https://q.com/?x=\"quoted\"\\path", "A \"title\"");
    storage_save(&s);
    storage_free(&s);
    storage_init(&s, PATH);
    storage_load(&s);
    assert(s.count == 1);
    assert(strcmp(s.entries[0].url, "https://q.com/?x=\"quoted\"\\path") == 0);
    assert(strcmp(s.entries[0].title, "A \"title\"") == 0);
    storage_free(&s);

    remove(PATH);
    printf("storage_test: PASS\n");
    return 0;
}
