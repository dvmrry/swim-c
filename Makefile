CC = clang
SWIFTC = xcrun swiftc
CFLAGS = -fobjc-arc -Wall -Wextra -Wpedantic -std=c17
FRAMEWORKS = -framework Cocoa -framework WebKit -framework QuartzCore -framework Network
SWIFT_MODULE_CACHE = /tmp/swim-swift-module-cache

SRC_C = browser.c input.c commands.c storage.c config.c userscript.c theme.c focus.c
SRC_M = main.m ui.m
SRC_SWIFT = Zoom.swift
SRC = $(SRC_C) $(SRC_M) $(SRC_SWIFT)
OBJ_C = $(SRC_C:.c=.o)
OBJ_M = $(SRC_M:.m=.o)
OBJ_SWIFT = $(SRC_SWIFT:.swift=.o)
OBJ = $(OBJ_C) $(OBJ_M) $(OBJ_SWIFT)
HEADERS = browser.h input.h commands.h ui.h storage.h config.h userscript.h theme.h focus.h zoom.h
SWIFT_BRIDGE = SwiftBridge.h

JS_INC = focus_js.inc old-reddit_js.inc youtube-adblock_js.inc dark-mode_js.inc

all: swim

swim: $(OBJ) Info.plist
	$(SWIFTC) $(FRAMEWORKS) -Xlinker -sectcreate -Xlinker __TEXT -Xlinker __info_plist -Xlinker Info.plist $(OBJ) -o swim

$(OBJ_C) $(OBJ_M): $(HEADERS) $(JS_INC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.m
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.swift $(SWIFT_BRIDGE)
	$(SWIFTC) -module-cache-path $(SWIFT_MODULE_CACHE) -parse-as-library -import-objc-header $(SWIFT_BRIDGE) -c $< -o $@

# Tests build as separate binaries — never linked into the release build
TESTS = test/storage_test test/zoom_test

test: $(TESTS)
	@./test/storage_test && ./test/zoom_test

test/storage_test: test/storage_test.c storage.c storage.h
	$(CC) -Wall -Wextra -std=c17 -I. test/storage_test.c -o $@

test/zoom_test: test/zoom_test.o Zoom.o
	$(SWIFTC) test/zoom_test.o Zoom.o -o $@

test/zoom_test.o: test/zoom_test.c zoom.h storage.h
	$(CC) -Wall -Wextra -std=c17 -I. -c $< -o $@

# Convert .js to C string literals for embedding
%_js.inc: js/%.js
	@echo "Generating $@"
	@sed 's/\\/\\\\/g; s/"/\\"/g; s/^/"/; s/$$/\\n"/' $< > $@

clean:
	rm -f swim $(JS_INC) $(TESTS) *.o test/*.o

.PHONY: clean all test
