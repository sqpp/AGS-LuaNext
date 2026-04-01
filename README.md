# AgsLuaNext

LuaJIT plugin for Adventure Game Studio.

## Current Version: 1.0.0

## What it provides

- `Lua.Init()` and `Lua.Run(string code)` in AGS script.
- Embedded Lua bridge with `_G.ags` FFI wrappers for engine access.
- Auto-loading of `lua/init.lua` during Lua initialization.
- Logging from Lua `print(...)` to AGS plugin log output.

## Quick start

1. Copy `AgsLuaNext.dll` and `lua51.dll` into your AGS Editor folder.
2. Copy `AgsLuaNext.dll` and `lua51.dll` into your game project folder (same level as `.agf`).
3. Enable the plugin in AGS (`Plugins` -> `Use this plugin`).
4. Import `AgsLua.ash` into your project scripts.
5. Create `lua/init.lua`.

## Folder layout

Use this layout in your game project:

```text
YourGame/
  YourGame.agf
  AgsLuaNext.dll
  lua51.dll
  lua/
    init.lua
  lib/
    (optional native DLLs and Lua helper modules)
```

## Script usage

In `GlobalScript.asc`:

```c
function game_start()
{
  Lua.Init();
  Lua.Run("require('your_module')");
}
```

Notes:
- `lua/init.lua` is loaded automatically once when Lua initializes.
- `Lua.Init()` is safe to call more than once; initialization happens once per process.
- Use `print("message")` in Lua to write to AGS plugin logs.

## Native DLLs (`lib/`)

If Lua code uses `ffi.load(...)`:
- keep native DLLs in your project `lib/` folder,
- load them by path built from `AGS_NATIVE_LIB_DIR` when available, or by explicit path.

This avoids relying on the current working directory.

## Building

Visual Studio 2022:

1. Open a VS x86 developer prompt and run `msvcbuild.bat` in `third_party/luajit/src`.
2. Open `AgsLuaNext.slnx`.
3. Build `Win32` (`x86`) - required for AGS.

## License

Same license terms as LuaJIT. (MIT) See [LuaJIT COPYRIGHT](third_party/luajit/src/luajit.h).
