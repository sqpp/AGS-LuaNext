#ifdef _WIN32
#include "pch.h"
#define LUA_BUILD_AS_DLL
#define THIS_IS_THE_PLUGIN
#include <windows.h>
#else
#define _stricmp strcasecmp
#define strcpy_s(dest, n, src) strncpy(dest, src, n)
#define strcat_s(dest, n, src) strncat(dest, src, n)
#endif

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "../include/agsplugin.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luajit.h"
}

// Shared engine pointer (defined in plugin_exports.cpp)
extern IAGSEngine* engine;

static char g_agsNativeLibDir[2048] = {};

#ifdef _WIN32
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
                if (a != INVALID_FILE_ATTRIBUTES) {
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

const char* LUA_BRIDGE_CODE_HEADER = R"AGSLUA(
local ffi = require("ffi")

-- Dynamic calling convention: __stdcall on Windows x86, empty otherwise.
local IS_WIN_X86 = (ffi.os == "Windows" and ffi.arch == "x86")
local CC = IS_WIN_X86 and "__stdcall " or ""

ffi.cdef([[
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
    void (]] .. CC .. [[ *AbortGame)(IAGSEngine*, const char*);
    const char* (]] .. CC .. [[ *GetEngineVersion)(IAGSEngine*);
    void (]] .. CC .. [[ *RegisterScriptFunction)(IAGSEngine*, const char*, void*);
    HWND (]] .. CC .. [[ *GetWindowHandle)(IAGSEngine*);
    LPDIRECTDRAW2 (]] .. CC .. [[ *GetDirectDraw2)(IAGSEngine*);
    LPDIRECTDRAWSURFACE2 (]] .. CC .. [[ *GetBitmapSurface)(IAGSEngine*, BITMAP*);
    BITMAP* (]] .. CC .. [[ *GetScreen)(IAGSEngine*);
    void (]] .. CC .. [[ *RequestEventHook)(IAGSEngine*, int32_t);
    int (]] .. CC .. [[ *GetSavedData)(IAGSEngine*, char*, int32_t);
    BITMAP* (]] .. CC .. [[ *GetVirtualScreen)(IAGSEngine*);
    void (]] .. CC .. [[ *DrawText)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t, char*);
    void (]] .. CC .. [[ *GetScreenDimensions)(IAGSEngine*, int32_t*, int32_t*, int32_t*);
    unsigned char** (]] .. CC .. [[ *GetRawBitmapSurface)(IAGSEngine*, BITMAP*);
    void (]] .. CC .. [[ *ReleaseBitmapSurface)(IAGSEngine*, BITMAP*);
    void (]] .. CC .. [[ *GetMousePosition)(IAGSEngine*, int32_t*, int32_t*);
    int (]] .. CC .. [[ *GetCurrentRoom)(IAGSEngine*);
    int (]] .. CC .. [[ *GetNumBackgrounds)(IAGSEngine*);
    int (]] .. CC .. [[ *GetCurrentBackground)(IAGSEngine*);
    BITMAP* (]] .. CC .. [[ *GetBackgroundScene)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *GetBitmapDimensions)(IAGSEngine*, BITMAP*, int32_t*, int32_t*, int32_t*);
    int (]] .. CC .. [[ *FWrite)(IAGSEngine*, void*, int32_t, int32_t);
    int (]] .. CC .. [[ *FRead)(IAGSEngine*, void*, int32_t, int32_t);
    void (]] .. CC .. [[ *DrawTextWrapped)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t, int32_t, const char*);
    void (]] .. CC .. [[ *SetVirtualScreen)(IAGSEngine*, BITMAP*);
    int (]] .. CC .. [[ *LookupParserWord)(IAGSEngine*, const char*);
    void (]] .. CC .. [[ *BlitBitmap)(IAGSEngine*, int32_t, int32_t, BITMAP*, int32_t);
    void (]] .. CC .. [[ *PollSystem)(IAGSEngine*);
    int (]] .. CC .. [[ *GetNumCharacters)(IAGSEngine*);
    AGSCharacter* (]] .. CC .. [[ *GetCharacter)(IAGSEngine*, int32_t);
    AGSGameOptions* (]] .. CC .. [[ *GetGameOptions)(IAGSEngine*);
    AGSColor* (]] .. CC .. [[ *GetPalette)(IAGSEngine*);
    void (]] .. CC .. [[ *SetPalette)(IAGSEngine*, int32_t, int32_t, AGSColor*);
    int (]] .. CC .. [[ *GetPlayerCharacter)(IAGSEngine*);
    void (]] .. CC .. [[ *RoomToViewport)(IAGSEngine*, int32_t*, int32_t*);
    void (]] .. CC .. [[ *ViewportToRoom)(IAGSEngine*, int32_t*, int32_t*);
    int (]] .. CC .. [[ *GetNumObjects)(IAGSEngine*);
    AGSObject* (]] .. CC .. [[ *GetObject)(IAGSEngine*, int32_t);
    BITMAP* (]] .. CC .. [[ *GetSpriteGraphic)(IAGSEngine*, int32_t);
    BITMAP* (]] .. CC .. [[ *CreateBlankBitmap)(IAGSEngine*, int32_t, int32_t, int32_t);
    void (]] .. CC .. [[ *FreeBitmap)(IAGSEngine*, BITMAP*);
    BITMAP* (]] .. CC .. [[ *GetRoomMask)(IAGSEngine*, int32_t);
    AGSViewFrame* (]] .. CC .. [[ *GetViewFrame)(IAGSEngine*, int32_t, int32_t, int32_t);
    int (]] .. CC .. [[ *GetWalkbehindBaseline)(IAGSEngine*, int32_t);
    void* (]] .. CC .. [[ *GetScriptFunctionAddress)(IAGSEngine*, const char*);
    int (]] .. CC .. [[ *GetBitmapTransparentColor)(IAGSEngine*, BITMAP*);
    int (]] .. CC .. [[ *GetAreaScaling)(IAGSEngine*, int32_t, int32_t);
    int (]] .. CC .. [[ *IsGamePaused)(IAGSEngine*);
    int (]] .. CC .. [[ *GetRawPixelColor)(IAGSEngine*, int32_t);
    int (]] .. CC .. [[ *GetSpriteWidth)(IAGSEngine*, int32_t);
    int (]] .. CC .. [[ *GetSpriteHeight)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *GetTextExtent)(IAGSEngine*, int32_t, const char*, int32_t*, int32_t*);
    void (]] .. CC .. [[ *PrintDebugConsole)(IAGSEngine*, const char*);
    void (]] .. CC .. [[ *PlaySoundChannel)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t, const char*);
    int (]] .. CC .. [[ *IsChannelPlaying)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *MarkRegionDirty)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t);
    void* (]] .. CC .. [[ *GetMouseCursor)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *GetRawColorComponents)(IAGSEngine*, int32_t, int32_t, int32_t*, int32_t*, int32_t*, int32_t*);
    int (]] .. CC .. [[ *MakeRawColorPixel)(IAGSEngine*, int32_t, int32_t, int32_t, int32_t, int32_t);
    int (]] .. CC .. [[ *GetFontType)(IAGSEngine*, int32_t);
    int (]] .. CC .. [[ *CreateDynamicSprite)(IAGSEngine*, int32_t, int32_t, int32_t);
    void (]] .. CC .. [[ *DeleteDynamicSprite)(IAGSEngine*, int32_t);
    int (]] .. CC .. [[ *IsSpriteAlphaBlended)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *UnrequestEventHook)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *BlitSpriteTranslucent)(IAGSEngine*, int32_t, int32_t, BITMAP*, int32_t);
    void (]] .. CC .. [[ *BlitSpriteRotated)(IAGSEngine*, int32_t, int32_t, BITMAP*, int32_t);
    void* (]] .. CC .. [[ *GetDirectSound)(IAGSEngine*);
    void (]] .. CC .. [[ *DisableSound)(IAGSEngine*);
    int (]] .. CC .. [[ *CanRunScriptFunctionNow)(IAGSEngine*);
    int (]] .. CC .. [[ *CallGameScriptFunction)(IAGSEngine*, const char*, int32_t, int32_t, ...);
    void (]] .. CC .. [[ *NotifySpriteUpdated)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *SetSpriteAlphaBlended)(IAGSEngine*, int32_t, int32_t);
    void (]] .. CC .. [[ *QueueGameScriptFunction)(IAGSEngine*, const char*, int32_t, int32_t, ...);
    int (]] .. CC .. [[ *RegisterManagedObject)(IAGSEngine*, void*, void*);
    void (]] .. CC .. [[ *AddManagedObjectReader)(IAGSEngine*, const char*, void*);
    void (]] .. CC .. [[ *RegisterUnserializedObject)(IAGSEngine*, int32_t, void*, void*);
    void* (]] .. CC .. [[ *GetManagedObjectAddressByKey)(IAGSEngine*, int32_t);
    int (]] .. CC .. [[ *GetManagedObjectKeyByAddress)(IAGSEngine*, void*);
    const char* (]] .. CC .. [[ *CreateScriptString)(IAGSEngine*, const char*);
    int (]] .. CC .. [[ *IncrementManagedObjectRefCount)(IAGSEngine*, void*);
    int (]] .. CC .. [[ *DecrementManagedObjectRefCount)(IAGSEngine*, void*);
    void (]] .. CC .. [[ *SetMousePosition)(int32_t, int32_t);
    void (]] .. CC .. [[ *SimulateMouseClick)(IAGSEngine*, int32_t);
    int (]] .. CC .. [[ *GetMovementPathWaypointCount)(IAGSEngine*, int32_t);
    int (]] .. CC .. [[ *GetMovementPathLastWaypoint)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *GetMovementPathWaypointLocation)(IAGSEngine*, int32_t, int32_t, int32_t*, int32_t*);
    void (]] .. CC .. [[ *GetMovementPathWaypointSpeed)(IAGSEngine*, int32_t, int32_t, int32_t*, int32_t*);
    const char* (]] .. CC .. [[ *GetGraphicsDriverID)(IAGSEngine*);
    int (]] .. CC .. [[ *IsRunningUnderDebugger)(IAGSEngine*);
    void (]] .. CC .. [[ *BreakIntoDebugger)(IAGSEngine*);
    void (]] .. CC .. [[ *GetPathToFileInCompiledFolder)(IAGSEngine*, const char*, char*);
    void* (]] .. CC .. [[ *GetDirectInputKeyboard)(IAGSEngine*);
    void* (]] .. CC .. [[ *GetDirectInputMouse)(IAGSEngine*);
    void* (]] .. CC .. [[ *ReplaceFontRenderer)(IAGSEngine*, int32_t, void*);
    void (]] .. CC .. [[ *GetRenderStageDesc)(IAGSEngine*, void*);
    void (]] .. CC .. [[ *GetGameInfo)(IAGSEngine*, void*);
    void* (]] .. CC .. [[ *ReplaceFontRenderer2)(IAGSEngine*, int32_t, void*);
    void (]] .. CC .. [[ *NotifyFontUpdated)(IAGSEngine*, int32_t);
    size_t (]] .. CC .. [[ *ResolveFilePath)(IAGSEngine*, const char*, char*, size_t);
    void* (]] .. CC .. [[ *OpenFileStream)(IAGSEngine*, const char*, int32_t, int32_t);
    void* (]] .. CC .. [[ *GetFileStreamByHandle)(IAGSEngine*, int32_t);
    void (]] .. CC .. [[ *Log)(IAGSEngine*, int32_t, const char*, ...);
    void* (]] .. CC .. [[ *CreateDynamicArray)(IAGSEngine*, size_t, size_t, bool);
    size_t (]] .. CC .. [[ *GetDynamicArrayLength)(IAGSEngine*, const void*);
    size_t (]] .. CC .. [[ *GetDynamicArraySize)(IAGSEngine*, const void*);
  } *lpVtbl;
};
]])

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
        "..\\lib\\?.so",
        "..\\..\\lib\\?.so"
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
        "lib/?.so"
    };
    for (const char* sp : kResolveCpath)
        TryResolveScriptPath(Ls, sp, true);
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

        lua_register(L, "GetAgsEngineAddress", GetAgsEngineAddress);
        lua_register(L, "print", Lua_Print);
        lua_getglobal(L, "print");
        lua_setglobal(L, "Log");

        if (luaL_dostring(L, LUA_BRIDGE_CODE_HEADER) != 0) {
            const char* err = lua_tostring(L, -1);
            if (engine && err) {
                engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] LUA_BRIDGE_CODE error: %s", err);
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
            }
            lua_pop(L, 1);
        }
    }
}
