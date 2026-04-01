#ifdef _WIN32
#include "pch.h"
#define LUA_BUILD_AS_DLL
#define THIS_IS_THE_PLUGIN
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luajit.h"
}

#include "../include/agsplugin.h"
#include <filesystem>

namespace fs = std::filesystem;

// Global engine pointer defined in plugin_exports.cpp
extern IAGSEngine* engine;

static lua_State* L = NULL;

// Direct log function so Lua doesn't have to find the engine vtable through pointers
extern "C" {
    void Lua_NativeLog(const char* msg) {
        if (engine) engine->Log(AGSLOG_LEVEL_INFO, "[AgsLuaNext Log] %s", msg);
        printf("[AgsLuaLog] %s\n", msg);
        fflush(stdout);
    }
}

// Safely log to AGS and stdout for emergency debugging
static void Trace(const char* msg) {
    if (engine) engine->Log(AGSLOG_LEVEL_INFO, "[AgsLuaNext Debug] %s", msg);
    printf("[AgsLuaNext Debug] %s\n", msg);
    fflush(stdout);
}

// ----------------------------------------------------------------------------
// LUA BRIDGE CODE (Embedded Lua side)
// ----------------------------------------------------------------------------

#define LUA_BRIDGE_CODE_HEADER \
"local ffi = require('ffi')\n" \
"ffi.cdef[[\n" \
"typedef void* HWND;\n" \
"void Lua_NativeLog(const char* msg);\n" \
"\n" \
"typedef struct IAGSEngine IAGSEngine;\n" \
"struct IAGSEngine {\n" \
"  struct {\n" \
"    void (*AbortGame)(IAGSEngine*, const char*);\n" \
"    const char* (*GetEngineVersion)(IAGSEngine*);\n" \
"    void (*RegisterScriptFunction)(IAGSEngine*, const char*, void*);\n" \
"    void (*Log)(IAGSEngine*, int32_t, const char*, ...);\n" \
"    void (*PrintDebugConsole)(IAGSEngine*, const char*);\n" \
"    int32_t (*IsPluginLoaded)(IAGSEngine*, const char*);\n" \
"  } *lpVtbl;\n" \
"};\n" \
"]]\n"

#define LUA_BRIDGE_CODE_FOOTER \
"local function GetAgsEngineAddress()\n" \
"    local reg = debug.getregistry()\n" \
"    local address = reg['AGS_ENGINE']\n" \
"    if not address then return nil end\n" \
"    return ffi.cast('IAGSEngine*', address)\n" \
"end\n" \
"\n" \
"local ags_raw = GetAgsEngineAddress()\n" \
"if ags_raw then\n" \
"    _G.ags = ags_raw.lpVtbl\n" \
"    _G.ags_ptr = ags_raw\n" \
"    _G.print = function(...) \n" \
"        local args = {...}\n" \
"        local s = ''\n" \
"        for i, v in ipairs(args) do s = s .. tostring(v) .. (i < #args and '\\t' or '') end\n" \
"        ffi.C.Lua_NativeLog(s)\n" \
"    end\n" \
"    _G.Log = _G.print\n" \
"end\n"

#define LUA_BRIDGE_CODE (LUA_BRIDGE_CODE_HEADER LUA_BRIDGE_CODE_FOOTER)

// ----------------------------------------------------------------------------
// NATIVE C++ FUNCTIONS
// ----------------------------------------------------------------------------

extern "C" {

void Lua_Init() {
    if (L) return;
    
    Trace("Beginning Lua_Init for Bridge...");
    if (!engine) return;
    
    L = luaL_newstate();
    if (!L) return;
    
    luaL_openlibs(L);

    // Store engine pointer in Lua registry safely
    lua_pushlightuserdata(L, (void*)engine);
    lua_setfield(L, LUA_REGISTRYINDEX, "AGS_ENGINE");

    // Load the bridge code
    if (luaL_loadstring(L, LUA_BRIDGE_CODE) != 0) {
        const char* err = lua_tostring(L, -1);
        if (engine) engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] FAILED to load bridge string: %s", err);
        return;
    }

    // Execute the bridge code
    if (lua_pcall(L, 0, 0, 0) != 0) {
        const char* err = lua_tostring(L, -1);
        if (engine) engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] FAILED to execute bridge: %s", err);
        return;
    }
    Trace("Bridge execution successful!");

    // Auto-load lua/init.lua if present
    if (fs::exists("lua/init.lua"))
    {
        Trace("Found lua/init.lua, loading...");
        if (luaL_loadfile(L, "lua/init.lua") == 0) {
            if (lua_pcall(L, 0, 0, 0) != 0) {
                const char* err = lua_tostring(L, -1);
                if (engine) engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] init.lua Error: %s", err);
            } else {
                Trace("init.lua success!");
            }
        }
    }
}

void Lua_Run(const char* code) {
    if (!L) Lua_Init();
    if (!L) return;

    if (luaL_loadstring(L, code) == 0) {
        if (lua_pcall(L, 0, 0, 0) != 0) {
            const char* err = lua_tostring(L, -1);
            if (engine) engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] Script Error: %s", err);
        }
    } else {
        const char* err = lua_tostring(L, -1);
        if (engine) engine->Log(AGSLOG_LEVEL_ERROR, "[AgsLuaNext] Compile Error: %s", err);
    }
}

} // extern "C"
