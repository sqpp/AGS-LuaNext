# DOWNLOAD AgsLuaNext 1.1.2 (LuaJIT for AGS 3.6+)

## AgsLuaNext

AgsLuaNext is a high-performance LuaJIT bridge for Adventure Game Studio. It embeds the entire 107-function v30 Engine API directly into the Lua environment via FFI, allowing you to write game logic in Lua at native speeds with raw access to the engine.

### Features

- **LuaJIT Performance**: Experience the speed of Just-In-Time compilation within your AGS games.
- **Cross-Platform**: Support for **Windows** (.dll), **Linux** (.so), and **macOS** (.dylib).
- **Full Engine Access**: Over 100 engine functions are automatically exposed to Lua via the `ags` table (e.g., `ags.GetCharacter`, `ags.GetScreenDimensions`).
- **Auto-Initialization**: Automatically loads `lua/init.lua` from your project folder on startup.
- **Engine Logging**: Standard Lua `print(...)` and `Log(...)` calls are routed directly to the AGS Editor's Log pane and `PrintDebugConsole`.
- **FFI Power**: Direct access to the C-level FFI library (`_G.ffi`) for calling external DLLs or defining custom C structs.
- **Smart Path Resolution**: Finds your scripts in `lua/` or `lib/` folders, even when running from the debugger or standalone.
- **Universal Binaries (Mac)**: Built with support for both Apple Silicon and Intel Macs.

---

## Official Forum Link:
https://www.adventuregamestudio.co.uk/forums/modules-plugins-tools/module-agsluanext-v1-0-lua-bridge/msg636689526/#msg636689526

---

## Quick start

1. Copy `AgsLuaNext.dll` (or platform equivalent) and `lua51.dll` into your AGS Editor folder.
2. Copy the plugin and Lua runtime into your game project folder (same level as `.agf`).
3. Enable the plugin in AGS (`Plugins` -> `Use this plugin`).
4. Import `AgsLua.ash` into your project scripts.
5. Create `lua/init.lua`.

---

## Module API (AGS Script)

The native interface is accessible via the `Lua` managed struct. 

```ags
/// Initializes the LuaJIT VM. 
/// Called automatically by Lua.Run() if not already initialized.
static void Lua.Init();

/// Runs a string of LuaJIT code immediately.
static void Lua.Run(String code);
```

---

## Lua Environment API

Once inside the Lua environment, you have access to several global tables and variables:

```lua
-- The 'ags' table contains all raw engine mappings.
ags.PrintDebugConsole("Hello from Lua!")
local x, y = ags.GetScreenDimensions()

-- The 'ffi' library for advanced memory access and DLL calls.
local ffi = require("ffi")

-- Standard redirected output
print("This goes to the AGS Log window")
Log("So does this")
```

---

## Building

AgsLuaNext uses **CMake** for cross-platform builds.

### **Windows (Visual Studio 2022)**
```powershell
# Open a VS x86 developer prompt
cmake -B build_win32 -S . -A Win32
cmake --build build_win32 --config Release
```

### **Linux**
Requires `libluajit-5.1-dev`.
```bash
cmake -B build_linux -S .
make -C build_linux
```

### **macOS**
Supports Universal Binaries (arm64 + x86_64).
```bash
cmake -B build_mac -S . -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build_mac --config Release
```

---

## License

AgsLuaNext is released under the **MIT License**. (Same license terms as LuaJIT). See [LuaJIT COPYRIGHT](third_party/luajit/src/luajit.h).
