# swim — Roadmap

## Direction (decided 2026-06)

swim is a **personal daily-driver browser** — native, fast, keyboard-driven,
macOS-only. It is not an agent-browser platform. That race belongs to
Playwright-MCP / browser-use / CDP-based tooling; swim's JS-injection
automation can't structurally compete (no isTrusted events, no
WebSocket/service-worker visibility, main-frame only) and shouldn't try.

In practice:

- **Invest in:** polish, stability, and removing daily friction for a human.
- **Don't build:** the automation platform (see Not Planned below).
- **Automation removed:** the HTTP API (`--serve`), MCP sidecar, and JS
  automation hooks were removed in 2026-07.
- **Tripwire resolved:** full automation-layer history is preserved on the
  archive remote.

## Current State

Minimalist vi-mode browser for macOS. C/Objective-C, Cocoa + WKWebView.
Single developer, AI-assisted. One binary: swim.

### Shipped

- Modal editing (NORMAL/INSERT/COMMAND/HINT/PASSTHROUGH)
- Tabs, bookmarks, marks, history, session save/restore
- Find-in-page (/, n, N), hint follow/tab/yank (f, F, ;y)
- Adblock (WKContentRuleList), userscripts, reader/focus mode
- Themes, config (INI), custom keybindings (key trie)
- Downloads (~/Downloads with dedup), file upload (native NSOpenPanel)
- Zoom with per-domain persistence (~/.config/swim/zoom.json)
- :open/:tabopen completion over bookmarks + history (Tab cycles, deduped)
- Clipboard integration (yy, yp, pp, Pp)
- Navigate up/root (gu, gU), search engine shortcuts
- Dark mode (prefers-color-scheme), tab mute, devtools
- Proxy (HTTP/SOCKS5, overrides system), private tabs
- Variable substitution ({url}, {title}, {clipboard})
- Count prefix (5j), repeat last action (.)
- App mode (`--app <url>`: chromeless single-window)
- Profiles (`--profile <name>` config overlays)

## Daily-Driver Backlog

1. **Dogfood.** Use swim as the actual default browser for a week; let real
   friction write this list. Everything below is an educated guess.
2. **Session autosave.** The session file is written only on clean quit
   (and `:session save`). Save on tab open/close too, so a crash or
   force-kill loses nothing. History already saves per-navigation.
3. **Frecency completion.** Rank completion candidates by frequency ×
   recency instead of most-recent-first, and render a small visible
   candidate list under the command bar. The history → completion →
   navigation loop is the core of a daily driver.
4. **History headroom.** Revisit MAX_ENTRIES (4096) and eviction once
   frecency lands — frequency data changes what's worth keeping.
5. **Micro-hardening + `make test`.** Remaining small audit items (strtol,
   escape-buffer sizing, @run-at guard, EINTR retry) and a test target that
   locks in storage-parser and zoom behavior. Test binaries stay out of the
   release build.

## Not Planned — Automation & AI Platform (dropped 2026-06)

The original Phase 2: an SSE event stream, session recording/replay, a
workflow sidecar (swim-auto), and an `:ai` copilot. Dropped — it
reimplements what CDP-based tools already do better, on a JS-injection
foundation with a hard ceiling. Recorded here so it doesn't get
re-invented. The HTTP API, MCP sidecar, and automation hooks were deleted
in 2026-07; history is preserved on the archive remote. If full network
inspection is ever genuinely needed, the WebKit Inspector Protocol
(private API) is the only route worth taking — not more JS monkey-patching.

## Exploratory

- **Swift strangler.** Port modules to Swift inside the existing binary —
  swiftc and clang link fine from the Makefile, no Xcode. Candidate order:
  zoom.c (one evening, proves the toolchain) → storage.c (Codable deletes
  the hand-rolled JSON parser) → input/commands/config/theme →
  main.m/ui.m. Motivation is craft
  plus deleting the memory-unsafety bug class, not urgency.
- **libswim.** Unchanged gate: only when a second consumer actually exists.

## Non-Goals

- Agent-browser platform / automation endpoint growth
- Extension API compatibility (WebExtensions, Chrome extensions)
- Feature parity with Chrome/Firefox (minimalism is the product)
- Web-based browser chrome (swim's own UI stays native Cocoa)

## Architectural Rules

1. **Core never imports platform headers.** browser.h, input.h, commands.h,
   config.h, storage.h stay pure C. Platform code lives only in main.m and
   ui.m.
2. **Webview is opaque.** The `void *webview` in Tab is the only bridge
   between core and platform.
3. **Actions and commands are the internal boundary.** Everything the user
   does flows through handle_action or registry_exec.
4. **The wrapper is thin.** Platform layer: create window, create webview,
   navigate, run JS, screenshot, handle events. The core does the rest.

## Size Budget

Under 500KB and ~10K LOC. If it's getting bigger, something is being
over-engineered. The 300MB browsers got there one "reasonable" feature at
a time.

---

*Last updated: 2026-07-06*
