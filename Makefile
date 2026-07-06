CC = clang
CFLAGS = -fobjc-arc -Wall -Wextra -Wpedantic -std=c17
FRAMEWORKS = -framework Cocoa -framework WebKit -framework QuartzCore -framework Network

SRC_C = browser.c input.c commands.c storage.c config.c userscript.c theme.c focus.c zoom.c
SRC_M = main.m ui.m serve.m
SRC = $(SRC_C) $(SRC_M)
HEADERS = browser.h input.h commands.h ui.h storage.h config.h userscript.h theme.h focus.h serve.h zoom.h

JS_INC = focus_js.inc extract_js.inc interact_js.inc console_js.inc requests_js.inc \
         old-reddit_js.inc youtube-adblock_js.inc dark-mode_js.inc

all: swim swim-mcp

swim: $(SRC) $(HEADERS) $(JS_INC) Info.plist
	$(CC) $(CFLAGS) $(FRAMEWORKS) -sectcreate __TEXT __info_plist Info.plist $(SRC) -o swim

swim-mcp: swim-mcp.c
	$(CC) -Wall -Wextra -Wpedantic -std=c17 swim-mcp.c -o swim-mcp

# Tests build as separate binaries — never linked into the release build
TESTS = test/storage_test test/zoom_test

test: $(TESTS)
	@./test/storage_test && ./test/zoom_test

test/storage_test: test/storage_test.c storage.c storage.h
	$(CC) -Wall -Wextra -std=c17 -I. test/storage_test.c -o $@

test/zoom_test: test/zoom_test.c zoom.c zoom.h storage.h
	$(CC) -Wall -Wextra -std=c17 -I. test/zoom_test.c -o $@

# Convert .js to C string literals for embedding
%_js.inc: js/%.js
	@echo "Generating $@"
	@sed 's/\\/\\\\/g; s/"/\\"/g; s/^/"/; s/$$/\\n"/' $< > $@

clean:
	rm -f swim swim-mcp $(JS_INC) $(TESTS)

.PHONY: clean all test
