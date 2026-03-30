#include "pch.h"
#include "../include/agsplugin.h"

#ifndef AGSLUANEXT_VERSION
#define AGSLUANEXT_VERSION "1.0.0"
#endif

// Global engine pointer
IAGSEngine* engine = nullptr;

// Bridge to AgsLuaNext.cpp
extern "C" {
    void Lua_Init();
    void Lua_Run(const char* code);
}

extern "C" {

DLLEXPORT const char* AGS_GetPluginName(void)
{
    return "AgsLuaNext";
}

DLLEXPORT int AGS_EditorStartup(void* /*editor*/)
{
    return 0;
}

DLLEXPORT void AGS_EditorShutdown(void)
{
}

DLLEXPORT void AGS_EditorProperties(HWND /*hwnd*/)
{
}

DLLEXPORT int AGS_EditorSaveGame(char* /*data*/, int /*len*/)
{
    return 0;
}

DLLEXPORT void AGS_EditorLoadGame(char* /*data*/, int /*len*/)
{
}

DLLEXPORT void AGS_EngineStartup(IAGSEngine* lpEngine)
{
    engine = lpEngine;
    engine->Log(AGSLOG_LEVEL_INFO, "[AgsLuaNext] Loaded plugin version %s", AGSLUANEXT_VERSION);

    // Register script functions with the engine (must be cdecl functions)
    engine->RegisterScriptFunction("Lua::Init^0", (void*)Lua_Init);
    engine->RegisterScriptFunction("Lua::Run^1", (void*)Lua_Run);
    
    // Auto-initialize the bridge on startup
    Lua_Init();
}

DLLEXPORT void AGS_EngineShutdown(void)
{
}

DLLEXPORT intptr_t AGS_EngineOnEvent(int /*evt*/, intptr_t /*data*/)
{
    return 0;
}

DLLEXPORT int AGS_EngineDebugHook(const char* /*script*/, int /*line*/, int /*level*/)
{
    return 0;
}

DLLEXPORT void AGS_EngineInitGfx(const char* /*driverID*/, void* /*data*/)
{
}

DLLEXPORT int AGS_PluginV2()
{
    return 2;
}

}