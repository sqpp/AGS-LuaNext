#include "pch.h"
#define LUA_BUILD_AS_DLL
#define THIS_IS_THE_PLUGIN
#ifndef LUAJIT_LIB_PATH_COMMENT
// NOTE: library linking to Lua/LuaJIT should be configured in the project settings.
// The previous absolute pragma has been removed for portability.
#endif
#include <stdio.h>
#include <string.h>
#include <string>
#include "../include/agsplugin.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luajit.h"
}


// Shared engine pointer (defined in plugin_exports.cpp)
extern IAGSEngine* engine;

#ifdef _WIN32
// Absolute path to the game's project "lib" folder (native DLLs). Exposed to Lua as AGS_NATIVE_LIB_DIR.
// ffi.load("curl") only checks exe dir / cwd / PATH; Lua should ffi.load(AGS_NATIVE_LIB_DIR.."\\curl.dll").
// SetDllDirectory(lib) helps dependent DLLs next to curl.dll.
static char g_agsNativeLibDir[2048] = {};

// fullPathInOut: full path to a marker file; on success becomes "...\lib" directory (nul-terminated).
static bool ComputeProjectLibDirFromResolvedFile(char* fullPathInOut, size_t pathCap) {
    char* s = strrchr(fullPathInOut, '\\');
    if (!s)
        s = strrchr(fullPathInOut, '/');
    if (!s)
        return false;
    *s = '\0';

    s = strrchr(fullPathInOut, '\\');
    if (!s)
        s = strrchr(fullPathInOut, '/');
    char* lastSeg = s ? (s + 1) : fullPathInOut;
    if (s && _stricmp(lastSeg, "lua") == 0) {
        size_t tail = (size_t)((s + 1) - fullPathInOut);
        if (tail < pathCap)
            strcpy_s(s + 1, pathCap - tail, "lib");
    }

    return fullPathInOut[0] != '\0';
}

static bool CanonicalDirectoryInPlace(char* path, size_t cap) {
    char out[4096] = {};
    DWORD n = GetFullPathNameA(path, static_cast<DWORD>(sizeof(out)), out, nullptr);
    if (n == 0 || n >= sizeof(out) || n >= cap)
        return false;
    for (char* p = out; *p; ++p) {
        if (*p == '/')
            *p = '\\';
    }
    return strcpy_s(path, cap, out) == 0;
}

static void ResolveAndPublishNativeLibDir(lua_State* L) {
    g_agsNativeLibDir[0] = '\0';
    if (!engine)
        return;

    static const char* kMarkers[] = {
        "..\\..\\lib\\libcurl.lua",
        "..\\lib\\libcurl.lua",
        "..\\..\\..\\lib\\libcurl.lua",
        "..\\..\\lua\\libcurl.lua",
        "..\\lua\\libcurl.lua",
        "..\\..\\..\\lua\\libcurl.lua",
        "..\\..\\lib\\curl.dll",
        "..\\..\\lib\\libcurl.dll",
        "..\\lib\\curl.dll",
        "..\\lib\\libcurl.dll",
        "..\\..\\..\\lib\\curl.dll",
        "..\\..\\..\\lib\\libcurl.dll",
    };

    char buf[2048] = {};
    for (const char* rel : kMarkers) {
        buf[0] = '\0';
        engine->GetPathToFileInCompiledFolder(rel, buf);
        if (!buf[0])
            continue;
        if (!ComputeProjectLibDirFromResolvedFile(buf, sizeof(buf)))
            continue;
        if (!CanonicalDirectoryInPlace(buf, sizeof(buf)))
            continue;
        if (strcpy_s(g_agsNativeLibDir, sizeof(g_agsNativeLibDir), buf) != 0)
            continue;

        {
            char probe[2600] = {};
            if (strcpy_s(probe, sizeof(probe), g_agsNativeLibDir) == 0
                && strcat_s(probe, sizeof(probe), "\\curl.dll") == 0) {
                const DWORD a = GetFileAttributesA(probe);
                if (a == INVALID_FILE_ATTRIBUTES) {
                    engine->Log(AGSLOG_LEVEL_ERROR,
                        "[AgsLuaNext] curl.dll not at %s (GetLastError=%lu)",
                        probe,
                        static_cast<unsigned long>(GetLastError()));
                }
                else {
                    engine->Log(AGSLOG_LEVEL_INFO, "[AgsLuaNext] Found %s", probe);
                }
            }
        }

        SetDllDirectoryA(g_agsNativeLibDir);
        engine->Log(AGSLOG_LEVEL_INFO, "[AgsLuaNext] Native lib dir: %s", g_agsNativeLibDir);
        if (L) {
            lua_pushstring(L, g_agsNativeLibDir);
            lua_setglobal(L, "AGS_NATIVE_LIB_DIR");
        }
        return;
    }

    engine->Log(AGSLOG_LEVEL_WARN,
        "[AgsLuaNext] Could not resolve project lib/ (put AGS_NATIVE_LIB_DIR unset; ffi.load(\"curl\") needs exe dir or PATH).");
}
#endif

static lua_State* L = NULL;

const char* LUA_BRIDGE_CODE = R"AGSLUA(_G.ffi = require("ffi")

ffi.cdef[[
typedef void BITMAP;
typedef void* HWND;
typedef void* LPDIRECTDRAW2;
typedef void* LPDIRECTDRAWSURFACE2;
typedef void* LPDIRECTSOUND;
typedef void* LPDIRECTINPUTDEVICE;

typedef struct { int32_t x, y; int32_t transparency; int32_t reserved[4]; short num, baseline, view, loop, frame, wait, moving; char cycling, overall_speed, on, flags; } AGSObject;
typedef struct { int32_t r, g, b, padding; } AGSColor;
typedef struct { int32_t score, usedmode, disabled_user_interface, gscript_timer, debug_mode, globalvars[50], messagetime, usedinv, inv_top, inv_numdisp, inv_numorder, inv_numinline, text_speed, sierra_inv_color, talkanim_speed, inv_item_wid, inv_item_hit, speech_text_shadow, swap_portrait_side, speech_textwindow_gui, follow_change_room_timer, totalscore, skip_display, no_multiloop_repeat, roomscript_finished, used_inv_on, no_textbg_when_voice, max_dialogoption_width, no_hicolor_fadein, bgspeech_game_speed, bgspeech_stay_on_display, unfactor_speech_from_textlength, mp3_loop_before_end, speech_music_drop, in_cutscene, fast_forward, room_width, room_height; } AGSGameOptions;
typedef struct { int32_t defview, talkview, view, room, prevroom, x, y, wait, flags; short following, followinfo; int32_t idleview; short idledelay, idleleft, transparency, baseline; int32_t activeinv, talkcolor, thinkview, reserved[2]; short walkspeed_y, pic_yoffs; int32_t z, reserved2[5]; short loop, frame, walking, animating, walkspeed, animspeed, inv[301], actx, acty; char name[40], scrname[20], on; } AGSCharacter;
typedef struct { int32_t pic; short xoffs, yoffs, speed; int32_t flags, sound, reserved[2]; } AGSViewFrame;

typedef struct IAGSEngine IAGSEngine;
struct IAGSEngine {
  struct {
    void (__stdcall *AbortGame)(IAGSEngine*, const char*);
    const char* (__stdcall *GetEngineVersion)(IAGSEngine*);
    void (__stdcall *RegisterScriptFunction)(IAGSEngine*, const char*, void*);
    HWND (__stdcall *GetWindowHandle)(IAGSEngine*);
    LPDIRECTDRAW2 (__stdcall *GetDirectDraw2)(IAGSEngine*);
    LPDIRECTDRAWSURFACE2 (__stdcall *GetBitmapSurface)(IAGSEngine*, BITMAP*);
    BITMAP* (__stdcall *GetScreen)(IAGSEngine*);
    void (__stdcall *RequestEventHook)(IAGSEngine*, int32_t);
    int (__stdcall *GetSavedData)(IAGSEngine*, char*, int32_t);
    BITMAP* (__stdcall *GetVirtualScreen)(IAGSEngine*);
    void (__stdcall *DrawText)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t, char*);
    void (__stdcall *GetScreenDimensions)(IAGSEngine*, int32_t*, int32_t*, int32_t*);
    unsigned char** (__stdcall *GetRawBitmapSurface)(IAGSEngine*, BITMAP*);
    void (__stdcall *ReleaseBitmapSurface)(IAGSEngine*, BITMAP*);
    void (__stdcall *GetMousePosition)(IAGSEngine*, int32_t*, int32_t*);
    int (__stdcall *GetCurrentRoom)(IAGSEngine*);
    int (__stdcall *GetNumBackgrounds)(IAGSEngine*);
    int (__stdcall *GetCurrentBackground)(IAGSEngine*);
    BITMAP* (__stdcall *GetBackgroundScene)(IAGSEngine*, int32_t);
    void (__stdcall *GetBitmapDimensions)(IAGSEngine*, BITMAP*, int32_t*, int32_t*, int32_t*);
    int (__stdcall *FWrite)(IAGSEngine*, void*, int32_t, int32_t);
    int (__stdcall *FRead)(IAGSEngine*, void*, int32_t, int32_t);
    void (__stdcall *DrawTextWrapped)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t, int32_t, const char*);
    void (__stdcall *SetVirtualScreen)(IAGSEngine*, BITMAP*);
    int (__stdcall *LookupParserWord)(IAGSEngine*, const char*);
    void (__stdcall *BlitBitmap)(IAGSEngine*, int32_t, int32_t, BITMAP*, int32_t);
    void (__stdcall *PollSystem)(IAGSEngine*);
    int (__stdcall *GetNumCharacters)(IAGSEngine*);
    AGSCharacter* (__stdcall *GetCharacter)(IAGSEngine*, int32_t);
    AGSGameOptions* (__stdcall *GetGameOptions)(IAGSEngine*);
    AGSColor* (__stdcall *GetPalette)(IAGSEngine*);
    void (__stdcall *SetPalette)(IAGSEngine*, int32_t, int32_t, AGSColor*);
    int (__stdcall *GetPlayerCharacter)(IAGSEngine*);
    void (__stdcall *RoomToViewport)(IAGSEngine*, int32_t*, int32_t*);
    void (__stdcall *ViewportToRoom)(IAGSEngine*, int32_t*, int32_t*);
    int (__stdcall *GetNumObjects)(IAGSEngine*);
    AGSObject* (__stdcall *GetObject)(IAGSEngine*, int32_t);
    BITMAP* (__stdcall *GetSpriteGraphic)(IAGSEngine*, int32_t);
    BITMAP* (__stdcall *CreateBlankBitmap)(IAGSEngine*, int32_t, int32_t, int32_t);
    void (__stdcall *FreeBitmap)(IAGSEngine*, BITMAP*);
    BITMAP* (__stdcall *GetRoomMask)(IAGSEngine*, int32_t);
    AGSViewFrame* (__stdcall *GetViewFrame)(IAGSEngine*, int32_t, int32_t, int32_t);
    int (__stdcall *GetWalkbehindBaseline)(IAGSEngine*, int32_t);
    void* (__stdcall *GetScriptFunctionAddress)(IAGSEngine*, const char*);
    int (__stdcall *GetBitmapTransparentColor)(IAGSEngine*, BITMAP*);
    int (__stdcall *GetAreaScaling)(IAGSEngine*, int32_t, int32_t);
    int (__stdcall *IsGamePaused)(IAGSEngine*);
    int (__stdcall *GetRawPixelColor)(IAGSEngine*, int32_t);
    int (__stdcall *GetSpriteWidth)(IAGSEngine*, int32_t);
    int (__stdcall *GetSpriteHeight)(IAGSEngine*, int32_t);
    void (__stdcall *GetTextExtent)(IAGSEngine*, int32_t, const char*, int32_t*, int32_t*);
    void (__stdcall *PrintDebugConsole)(IAGSEngine*, const char*);
    void (__stdcall *PlaySoundChannel)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t, const char*);
    int (__stdcall *IsChannelPlaying)(IAGSEngine*, int32_t);
    void (__stdcall *MarkRegionDirty)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t);
    void* (__stdcall *GetMouseCursor)(IAGSEngine*, int32_t);
    void (__stdcall *GetRawColorComponents)(IAGSEngine*, int32_t, int32_t, int32_t*, int32_t*, int32_t*, int32_t*);
    int (__stdcall *MakeRawColorPixel)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t, int32_t);
    int (__stdcall *GetFontType)(IAGSEngine*, int32_t);
    int (__stdcall *CreateDynamicSprite)(IAGSEngine*, int32_t, int32_t, int32_t);
    void (__stdcall *DeleteDynamicSprite)(IAGSEngine*, int32_t);
    int (__stdcall *IsSpriteAlphaBlended)(IAGSEngine*, int32_t);
    void (__stdcall *UnrequestEventHook)(IAGSEngine*, int32_t);
    void (__stdcall *BlitSpriteTranslucent)(IAGSEngine*, int32_t, int32_t, BITMAP*, int32_t);
    void (__stdcall *BlitSpriteRotated)(IAGSEngine*, int32_t, int32_t, BITMAP*, int32_t);
    void* (__stdcall *GetDirectSound)(IAGSEngine*);
    void (__stdcall *DisableSound)(IAGSEngine*);
    int (__stdcall *CanRunScriptFunctionNow)(IAGSEngine*);
    int (__stdcall *CallGameScriptFunction)(IAGSEngine*, const char*, int32_t, int32_t, ...);
    void (__stdcall *NotifySpriteUpdated)(IAGSEngine*, int32_t);
    void (__stdcall *SetSpriteAlphaBlended)(IAGSEngine*, int32_t, int32_t);
    void (__stdcall *QueueGameScriptFunction)(IAGSEngine*, const char*, int32_t, int32_t, ...);
    int (__stdcall *RegisterManagedObject)(IAGSEngine*, void*, void*);
    void (__stdcall *AddManagedObjectReader)(IAGSEngine*, const char*, void*);
    void (__stdcall *RegisterUnserializedObject)(IAGSEngine*, int32_t, void*, void*);
    void* (__stdcall *GetManagedObjectAddressByKey)(IAGSEngine*, int32_t);
    int (__stdcall *GetManagedObjectKeyByAddress)(IAGSEngine*, void*);
    const char* (__stdcall *CreateScriptString)(IAGSEngine*, const char*);
    int (__stdcall *IncrementManagedObjectRefCount)(IAGSEngine*, void*);
    int (__stdcall *DecrementManagedObjectRefCount)(IAGSEngine*, void*);
    void (__stdcall *SetMousePosition)(int32_t, int32_t);
    void (__stdcall *SimulateMouseClick)(IAGSEngine*, int32_t);
    int (__stdcall *GetMovementPathWaypointCount)(IAGSEngine*, int32_t);
    int (__stdcall *GetMovementPathLastWaypoint)(IAGSEngine*, int32_t);
    void (__stdcall *GetMovementPathWaypointLocation)(IAGSEngine*, int32_t, int32_t, int32_t*, int32_t*);
    void (__stdcall *GetMovementPathWaypointSpeed)(IAGSEngine*, int32_t, int32_t, int32_t*, int32_t*);
    const char* (__stdcall *GetGraphicsDriverID)(IAGSEngine*);
    int (__stdcall *IsRunningUnderDebugger)(IAGSEngine*);
    void (__stdcall *BreakIntoDebugger)(IAGSEngine*);
    void (__stdcall *GetPathToFileInCompiledFolder)(IAGSEngine*, const char*, char*);
    void* (__stdcall *GetDirectInputKeyboard)(IAGSEngine*);
    void* (__stdcall *GetDirectInputMouse)(IAGSEngine*);
    void* (__stdcall *ReplaceFontRenderer)(IAGSEngine*, int32_t, void*);
    void (__stdcall *GetRenderStageDesc)(IAGSEngine*, void*);
    void (__stdcall *GetGameInfo)(IAGSEngine*, void*);
    void* (__stdcall *ReplaceFontRenderer2)(IAGSEngine*, int32_t, void*);
    void (__stdcall *NotifyFontUpdated)(IAGSEngine*, int32_t);
    size_t (__stdcall *ResolveFilePath)(IAGSEngine*, const char*, char*, size_t);
    void* (__stdcall *OpenFileStream)(IAGSEngine*, const char*, int32_t, int32_t);
    void* (__stdcall *GetFileStreamByHandle)(IAGSEngine*, int32_t);
    void (__stdcall *Log)(IAGSEngine*, int32_t, const char*, ...);
    void* (__stdcall *CreateDynamicArray)(IAGSEngine*, size_t, size_t, bool);
    size_t (__stdcall *GetDynamicArrayLength)(IAGSEngine*, const void*);
    size_t (__stdcall *GetDynamicArraySize)(IAGSEngine*, const void*);
  } *lpVtbl;
};
]]

local addr = GetAgsEngineAddress()
local engine = ffi.cast("IAGSEngine*", addr)

_G.ags = {}
local function bridge(name)
    _G.ags[name] = function(...) return engine.lpVtbl[name](engine, ...) end
end

local functions = {
    "AbortGame", "GetEngineVersion", "RegisterScriptFunction", "GetWindowHandle",
    "GetDirectDraw2", "GetBitmapSurface", "GetScreen", "RequestEventHook",
    "GetSavedData", "GetVirtualScreen", "DrawText", "GetScreenDimensions",
    "GetRawBitmapSurface", "ReleaseBitmapSurface", "GetMousePosition", "GetCurrentRoom",
    "GetNumBackgrounds", "GetCurrentBackground", "GetBackgroundScene", "GetBitmapDimensions",
    "FWrite", "FRead", "DrawTextWrapped", "SetVirtualScreen", "LookupParserWord",
    "BlitBitmap", "PollSystem", "GetNumCharacters", "GetCharacter", "GetGameOptions",
    "GetPalette", "SetPalette", "GetPlayerCharacter", "RoomToViewport", "ViewportToRoom",
    "GetNumObjects", "GetObject", "GetSpriteGraphic", "CreateBlankBitmap", "FreeBitmap",
    "GetRoomMask", "GetViewFrame", "GetWalkbehindBaseline", "GetScriptFunctionAddress",
    "GetBitmapTransparentColor", "GetAreaScaling", "IsGamePaused", "GetRawPixelColor",
    "GetSpriteWidth", "GetSpriteHeight", "GetTextExtent", "PrintDebugConsole",
    "PlaySoundChannel", "IsChannelPlaying", "MarkRegionDirty", "GetMouseCursor",
    "GetRawColorComponents", "MakeRawColorPixel", "GetFontType", "CreateDynamicSprite",
    "DeleteDynamicSprite", "IsSpriteAlphaBlended", "UnrequestEventHook", "BlitSpriteTranslucent",
    "BlitSpriteRotated", "GetDirectSound", "DisableSound", "CanRunScriptFunctionNow",
    "CallGameScriptFunction", "NotifySpriteUpdated", "SetSpriteAlphaBlended",
    "QueueGameScriptFunction", "RegisterManagedObject", "AddManagedObjectReader",
    "RegisterUnserializedObject", "GetManagedObjectAddressByKey", "GetManagedObjectKeyByAddress",
    "CreateScriptString", "IncrementManagedObjectRefCount", "DecrementManagedObjectRefCount",
    "SetMousePosition", "SimulateMouseClick", "GetMovementPathWaypointCount",
    "GetMovementPathLastWaypoint", "GetMovementPathWaypointLocation", "GetMovementPathWaypointSpeed",
    "GetGraphicsDriverID", "IsRunningUnderDebugger", "BreakIntoDebugger",
    "GetPathToFileInCompiledFolder", "GetDirectInputKeyboard", "GetDirectInputMouse",
    "ReplaceFontRenderer", "GetRenderStageDesc", "GetGameInfo", "ReplaceFontRenderer2",
    "NotifyFontUpdated", "ResolveFilePath", "OpenFileStream", "GetFileStreamByHandle",
    "Log", "CreateDynamicArray", "GetDynamicArrayLength", "GetDynamicArraySize"
}

for _, name in ipairs(functions) do
    bridge(name)
end

-- Smart Wrapping for string returns
_G.ags.GetVersion = function() return ffi.string(engine.lpVtbl.GetEngineVersion(engine)) end
_G.ags.GetEngineVersion = _G.ags.GetVersion
_G.ags.GetGraphicsDriverID = function() return ffi.string(engine.lpVtbl.GetGraphicsDriverID(engine)) end
)AGSLUA";

static void NormalizePathSlashes(char* buf) {
    if (!buf) return;
    for (; *buf; ++buf) {
        if (*buf == '\\')
            *buf = '/';
    }
}

static void AppendPackagePath(lua_State* Ls, const char* absPattern) {
    if (!absPattern || !absPattern[0])
        return;
    lua_getglobal(Ls, "package");
    lua_getfield(Ls, -1, "path");
    const char* cur = lua_tostring(Ls, -1);
    std::string combined = cur ? cur : "";
    combined += ";";
    combined += absPattern;
    lua_pop(Ls, 1);
    lua_pushstring(Ls, combined.c_str());
    lua_setfield(Ls, -2, "path");
    lua_pop(Ls, 1);
}

static void AppendPackageCpath(lua_State* Ls, const char* absPattern) {
    if (!absPattern || !absPattern[0])
        return;
    lua_getglobal(Ls, "package");
    lua_getfield(Ls, -1, "cpath");
    const char* cur = lua_tostring(Ls, -1);
    std::string combined = cur ? cur : "";
    combined += ";";
    combined += absPattern;
    lua_pop(Ls, 1);
    lua_pushstring(Ls, combined.c_str());
    lua_setfield(Ls, -2, "cpath");
    lua_pop(Ls, 1);
}

static void TryCompiledFolderPattern(lua_State* Ls, const char* relativePath, bool forCpath) {
    if (!engine || !Ls || !relativePath)
        return;
    char buf[2048];
    buf[0] = '\0';
    engine->GetPathToFileInCompiledFolder(relativePath, buf);
    if (!buf[0])
        return;
    NormalizePathSlashes(buf);
    if (forCpath)
        AppendPackageCpath(Ls, buf);
    else
        AppendPackagePath(Ls, buf);
}

static void TryResolveScriptPath(lua_State* Ls, const char* scriptPath, bool forCpath) {
    if (!engine || !Ls || !scriptPath)
        return;
    char buf[2048];
    size_t n = engine->ResolveFilePath(scriptPath, buf, sizeof(buf));
    if (n == 0 || n >= sizeof(buf) || !buf[0])
        return;
    NormalizePathSlashes(buf);
    if (forCpath)
        AppendPackageCpath(Ls, buf);
    else
        AppendPackagePath(Ls, buf);
}

static void ConfigureLuaModuleSearchPaths(lua_State* Ls) {
    if (!engine || !Ls)
        return;

    static const char* kLuaCompiledRel[] = {
        "..\\lua\\?.lua",
        "..\\..\\lua\\?.lua",
        "..\\..\\..\\lua\\?.lua",
        "..\\lua\\?\\init.lua",
        "..\\..\\lua\\?\\init.lua",
        "..\\..\\..\\lua\\?\\init.lua",
        "..\\lib\\?.lua",
        "..\\..\\lib\\?.lua",
        "..\\..\\..\\lib\\?.lua",
        "..\\lib\\?\\init.lua",
        "..\\..\\lib\\?\\init.lua",
        "..\\..\\..\\lib\\?\\init.lua",
        "..\\?.lua",
        "..\\..\\?.lua",
        "..\\..\\..\\?.lua",
    };
    for (const char* rel : kLuaCompiledRel)
        TryCompiledFolderPattern(Ls, rel, false);

    static const char* kDllCompiledRel[] = {
        "..\\lib\\?.dll",
        "..\\..\\lib\\?.dll",
        "..\\..\\..\\lib\\?.dll",
    };
    for (const char* rel : kDllCompiledRel)
        TryCompiledFolderPattern(Ls, rel, true);

    static const char* kResolvePath[] = {
        "lua/?.lua",
        "lua/?/init.lua",
        "lib/?.lua",
        "lib/?/init.lua",
    };
    for (const char* sp : kResolvePath)
        TryResolveScriptPath(Ls, sp, false);

    static const char* kResolveCpath[] = {
        "lib/?.dll",
    };
    for (const char* sp : kResolveCpath)
        TryResolveScriptPath(Ls, sp, true);

    engine->Log(AGSLOG_LEVEL_INFO,
        "[AgsLuaNext] package.path / package.cpath updated (lua, lib, extra .. levels, ResolveFilePath).");
}

extern "C" {
    // This function is registered with Lua to return the engine address
    int GetAgsEngineAddress(lua_State* L) {
        lua_pushlightuserdata(L, (void*)engine);
        return 1;
    }

    // Native print function to route Lua print() to AGS engine
    int Lua_Print(lua_State* L) {
        int nargs = lua_gettop(L);
        std::string out = "";
        for (int i = 1; i <= nargs; i++) {
            if (i > 1) out += "\t";
            if (lua_isstring(L, i)) {
                out += lua_tostring(L, i);
            }
            else if (lua_isboolean(L, i)) {
                out += lua_toboolean(L, i) ? "true" : "false";
            }
            else if (lua_isnil(L, i)) {
                out += "nil";
            }
            else {
                out += luaL_typename(L, i);
            }
        }
        if (engine && !out.empty()) {
            // AGS 4 can swallow PrintDebugConsole depending on context; engine log is more reliable.
            engine->Log(AGSLOG_LEVEL_INFO, "%s", out.c_str());
            engine->PrintDebugConsole(out.c_str());
        }
        return 0;
    }

    void Lua_Init() {
        if (L) return;
        L = luaL_newstate();
        if (!L) return;
        luaL_openlibs(L);

        // Register the helper to get the engine pointer
        lua_register(L, "GetAgsEngineAddress", GetAgsEngineAddress);

        // Register the native print function to bypass FFI for debug output
        lua_register(L, "print", Lua_Print);
        // Many scripts expect `Log(...)` instead of `print(...)`.
        // Alias it to our routed print.
        lua_getglobal(L, "print");
        lua_setglobal(L, "Log");

        // Execute the bridge code to set up the FFI mapping
        if (luaL_dostring(L, LUA_BRIDGE_CODE) != 0) {
            const char* err = lua_tostring(L, -1);
            if (engine && err) {
                engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] LUA_BRIDGE_CODE error: %s", err);
                engine->AbortGame(err);
            }
            lua_pop(L, 1);
        }

#ifdef _WIN32
        ResolveAndPublishNativeLibDir(L);
#endif
        ConfigureLuaModuleSearchPaths(L);

        if (luaL_dostring(L,
            "local ok, err = pcall(require, \"init\")\n"
            "if not ok then print(\"[AgsLuaNext] require('init') failed: \" .. tostring(err)) end") != 0) {
            const char* err = lua_tostring(L, -1);
            if (engine && err)
                engine->PrintDebugConsole(err);
            lua_pop(L, 1);
        }
    }

    void Lua_Run(const char* code) {
        if (!L) Lua_Init();
        if (!L || !code) return;

        if (luaL_dostring(L, code) != 0) {
            const char* err = lua_tostring(L, -1);
            if (engine && err) {
                engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] Lua.Run error: %s", err);
                engine->AbortGame(err);
            }
            lua_pop(L, 1);
        }
    }

    void Lua_Load(const char* filename) {
        if (!L) Lua_Init();
        if (!L || !filename) return;

        if (luaL_dofile(L, filename) != 0) {
            const char* err = lua_tostring(L, -1);
            if (engine && err) {
                engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] Lua.Load error: %s", err);
                engine->AbortGame(err);
            }
            lua_pop(L, 1);
        }
    }
}
