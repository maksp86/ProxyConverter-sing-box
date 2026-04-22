# ProxyConverter-sing-box (Throne-based)

A C++ proxy-link converter that reuses parsing and conversion logic from [Throne](https://github.com/throneproj/Throne).

## Features

- Input sources:
  - `--input-lines <file|->`
  - `--input-json <file|->`
  - positional URL arguments
- Output targets:
  - `sing-box` (default)
  - `xray`
- Xray-specific options:
  - `--start-port`
  - `--change-ports`

## Build

```bash
git submodule update --init --recursive
cmake -S . -B build -DPROXY_CONVERTER_HEADLESS=ON
cmake --build build --config Release -j
```

## Portable bundle

`CMakeLists.txt` now contains an install/deploy path for Qt runtime files. This means you can build a portable directory without hand-written deploy scripts:

```bash
cmake --install build --config Release --prefix dist/proxy-converter
```

With a dynamic Qt build this produces a folder with `proxy-converter` plus the required Qt runtime libraries. On CI the same path is used to publish `zip` and `tar.gz` artifacts.

## Single binary

`PROXY_CONVERTER_SINGLE_BINARY=ON` only becomes a true single executable when Qt itself is built statically. If `Qt6::Core` is a shared library, CMake will warn and the output stays a portable folder, not a single file.

Practical options:

- Real single executable: build against a static Qt and keep `PROXY_CONVERTER_SINGLE_BINARY=ON`.
- One distributable file without static linking: package the portable folder into a self-extracting archive.
  On Windows this is usually a `7z` SFX stub plus the installed folder.
  On Linux the equivalent is `makeself`.

The second option is still "one file", but technically it is a launcher that unpacks the bundled Qt DLLs or `.so` files before starting `proxy-converter`.

## GitHub Actions

`.github/workflows/build.yml` was adapted from the upstream `Throne` matrix, but reduced to what this CLI project actually needs:

- Windows x64 and Linux x64 builds
- Qt installation through `install-qt-action`
- `cmake --build` plus `cmake --install`
- portable artifacts uploaded on every push, PR, or manual run
- optional release publishing on `workflow_dispatch` when `tag` is filled

## Usage examples

```bash
./build/proxy-converter --target sing-box \
  "vmess://..." "vless://..." -o -

cat links.txt | ./build/proxy-converter --input-lines - --target xray --change-ports
```
