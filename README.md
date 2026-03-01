# Lumen

Phase 1: moteur UI custom drawn multi-plateforme (C++20), sans dependance externe.

## Prerequis

- CMake >= 3.16
- Compilateur C++20
- OpenGL

## Linux (X11 + GLX)

Debian/Ubuntu:

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libx11-dev libgl1-mesa-dev
```

Fedora:

```bash
sudo dnf install -y gcc-c++ cmake libX11-devel mesa-libGL-devel
```

Arch:

```bash
sudo pacman -S --needed base-devel cmake libx11 mesa
```

Build + run:

```bash
cmake -S /path/to/lumen -B /path/to/lumen/build
cmake --build /path/to/lumen/build -j
/path/to/lumen/build/lumen
```

Note: si l'environnement est Wayland uniquement, XWayland doit etre actif (backend X11/GLX).

## macOS (Cocoa + NSOpenGLView)

Prerequis:
- Xcode ou Command Line Tools
- CMake >= 3.16

Build + run:

```bash
cmake -S /path/to/lumen -B /path/to/lumen/build
cmake --build /path/to/lumen/build -j
/path/to/lumen/build/lumen
```

Note: OpenGL est deprecated sur macOS, mais reste utilisable pour cette phase.

## Windows (Win32 + WGL)

Prerequis:
- Visual Studio 2022 (C++ Desktop) ou Build Tools
- CMake >= 3.16

Build (Developer Command Prompt):

```bat
cmake -S C:\path\to\lumen -B C:\path\to\lumen\build
cmake --build C:\path\to\lumen\build --config Release
```

Run:

```bat
C:\path\to\lumen\build\Release\lumen.exe
```

Si vous utilisez un generateur Ninja, remplacez la commande build par:

```bat
cmake --build C:\path\to\lumen\build
```
