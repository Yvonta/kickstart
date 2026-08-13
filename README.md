# Yvonta Kickstart Desktop Toolbar

A lightweight, borderless, semi-transparent desktop toolbar and advertisement widget designed for macOS and Linux. It sits neatly on your desktop layer, providing quick search/query execution, a logo button link, and an embedded WebKit advertisement or content banner.

## Features

* **Cross-Platform:** Supports macOS (AppKit / Objective-C) and Linux (GTK3 / WebKit2GTK).
* **Desktop Widget Design:** Borderless, semi-transparent layout that stays pinned on the desktop layer.
* **Quick Query Bar:** Direct text entry to submit queries and launch external search/automation workflows.
* **Embedded Web View:** Integrated browser frame to display remote promotional banners or web content with full external link-handling support.

---

## License

This project is released under the **CC0 1.0 Universal (Public Domain)** license. You can copy, modify, distribute, and perform the work, even for commercial purposes, all without asking permission.

---

## Compilation Instructions

Ensure you save the source code into a file named `kickstart.c` before running the compilation commands below.

### macOS

Requires Xcode Command Line Tools to be installed.

```bash
clang -x objective-c -fobjc-arc kickstart.c -framework Cocoa -framework WebKit -o kickstart

```

### Linux

Requires `clang`, `gtk+-3.0`, and `webkit2gtk` development packages to be installed.

```bash
clang kickstart.c -o kickstart $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.1)

```