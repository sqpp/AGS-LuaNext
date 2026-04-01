#ifdef _WIN32
#include "pch.h"
#endif

#include "../include/agsplugin.h"
#include <stdio.h>

#ifndef AGSLUANEXT_VERSION
#define AGSLUANEXT_VERSION "1.2.1"
#endif

// Global engine pointer
IAGSEngine* engine = nullptr;

// Bridge to AgsLuaNext.cpp
extern "C" {
    void Lua_Init();
    void Lua_Run(const char* code);
}

extern "C" {

#ifdef _WIN32
#define PLUGIN_EXPORT DLLEXPORT
#else
#define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

PLUGIN_EXPORT const char* AGS_GetPluginName(void)
{
    return "AgsLuaNext";
}

PLUGIN_EXPORT int AGS_EditorStartup(void* /*editor*/)
{
    return 0;
}

PLUGIN_EXPORT void AGS_EditorShutdown(void)
{
}

PLUGIN_EXPORT void AGS_EditorProperties(HWND /*hwnd*/)
{
}

PLUGIN_EXPORT int AGS_EditorSaveGame(char* /*data*/, int /*len*/)
{
    return 0;
}

PLUGIN_EXPORT void AGS_EditorLoadGame(char* /*data*/, int /*len*/)
{
}

PLUGIN_EXPORT void AGS_EngineStartup(IAGSEngine* lpEngine)
{
    engine = lpEngine;
    
    printf("[AgsLuaNext] 1. Engine Startup entry point hit.\n");
    fflush(stdout);

    if (!engine) return;
    
    printf("[AgsLuaNext] 2. Engine pointer valid. Registering functions...\n");
    fflush(stdout);

    engine->RegisterScriptFunction("Lua::Init^0", (void*)Lua_Init);
    engine->RegisterScriptFunction("Lua::Run^1", (void*)Lua_Run);

    printf("[AgsLuaNext] 3. Engine Startup complete! Version: %s\n", AGSLUANEXT_VERSION);
    fflush(stdout);
}

PLUGIN_EXPORT void AGS_EngineShutdown(void)
{
}

PLUGIN_EXPORT intptr_t AGS_EngineOnEvent(int /*evt*/, intptr_t /*data*/)
{
    return 0;
}

PLUGIN_EXPORT int AGS_EngineDebugHook(const char* /*script*/, int /*line*/, int /*level*/)
{
    return 0;
}

PLUGIN_EXPORT void AGS_EngineInitGfx(const char* /*driverID*/, void* /*data*/)
{
}

PLUGIN_EXPORT int AGS_PluginV2()
{
    return 2;
}

}