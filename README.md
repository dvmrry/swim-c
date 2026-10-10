# swim (C) — archived

> **Archived.** This is the original C/Objective-C implementation, frozen at tag
> `c-final`. swim continues as a Swift rewrite at
> [github.com/dvmrry/swim](https://github.com/dvmrry/swim) — see
> [`docs/SWIFT-KICKOFF.md`](docs/SWIFT-KICKOFF.md) for the spec it was built from.

Minimalist vi-mode web browser for macOS, written in C/Objective-C (Cocoa + WebKit).

## Build

```
make        # builds ./swim
make clean  # removes binary
```

## Project Structure

- `main.m` — app lifecycle, event monitor, action dispatch, command handlers
- `ui.m` / `ui.h` — window, tab bar, status bar, command bar, webview management
- `input.c` / `input.h` — modal key handling (NORMAL/INSERT/COMMAND/HINT/PASSTHROUGH), key trie
- `commands.c` / `commands.h` — command registry with tab completion
- `browser.c` / `browser.h` — tab collection, URL/title state
- `storage.c` / `storage.h` — bookmarks, history, session persistence
- `config.c` / `config.h` — INI config parser, key bindings
- `js/` — reference copies of injected userscripts
