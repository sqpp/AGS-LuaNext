# AgsLuaNext

A high-performance, self-contained LuaJIT plugin for Adventure Game Studio (AGS).

## Features

- **LuaJIT Integration**: Experience native-speed execution of game logic via LuaJIT's Just-In-Time compiler.
- **Full Engine Access**: Provides raw, FFI-based access to 100+ functions of the AGS Engine API (v3.0+).
- **Embedded Bridge**: The entire engine bridge is embedded directly into the plugin for a zero-configuration experience.
- **Modern C++**: Built using MSVC with C++20 standards.

## Project Structure

- `src/`: Core C++ implementation and plugin exports.
- `include/`: Plugin headers and engine interface definitions.
- `third_party/luajit/`: contains LuaJIT source and pre-built libraries for easy building.
- `bridge.lua`: The Lua-side logic for the engine bridge.
- `AgsLua.ash`: The AGS script header for the plugin.

## Building

The project is designed for Visual Studio 2022 (MSVC v143+).

1. Open `AgsLuaNext.slnx` in Visual Studio.
2. Ensure `luajit.lib` is available in `third_party/luajit/lib/(Win32|x64)/`.
3. Build the solution for your desired platform (Win32 is standard for AGS).

## Distribution

To distribute the plugin, include `AgsLuaNext.dll` and `lua51.dll` in your AGS game project.

## License

This project is released under the same license terms as LuaJIT. See [LuaJIT COPYRIGHT](third_party/luajit/src/luajit.h) for details.
