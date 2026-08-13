# Yvonta Kickstart Desktop Toolbar

A lightweight, borderless, semi-transparent desktop toolbar and advertisement widget designed for macOS and Linux. It is intended for quick access to the AI functionalities of Yvonta, providing a seamless way to execute queries, launch shortcuts, and view integrated content right from your desktop layer.

![Screenshot](screenshot.png)

## Features

* **Quick AI Access:** Designed for fast access to Yvonta's AI functionalities and search workflows.
* **Cross-Platform:** Supports macOS (AppKit / Objective-C) and Linux (GTK3 / WebKit2GTK).
* **Desktop Widget Design:** Borderless, semi-transparent layout that stays pinned on the desktop.
* **Embedded Web View:** Integrated browser frame to display remote promotional banners or web content with full external link-handling support.

---

## License

This project is released under the **CC0 1.0 Universal (Public Domain)** license. You can copy, modify, distribute, and perform the work, even for commercial purposes, all without asking permission.

---

## Compilation Instructions

Ensure you save the source code into a file named `kickstart.c` before running the compilation commands below[cite: 1].

### macOS

Requires Xcode Command Line Tools to be installed.

```bash
clang -x objective-c -fobjc-arc kickstart.c -framework Cocoa -framework WebKit -o kickstart
```[cite: 1]

### Linux

Requires `clang`, `gtk+-3.0`, and `webkit2gtk` development packages to be installed[cite: 1].

```bash
clang kickstart.c -o kickstart $(pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.1)
```[cite: 1]

```