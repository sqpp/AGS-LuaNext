#include "pch.h"
#include "../include/agsplugin.h"

// Minimal AGS plugin exports to satisfy linker. Implement full behavior as needed.

extern "C" {

DLLEXPORT const char* AGS_GetPluginName(void)
{
    return "AgsLuaNext";
}

DLLEXPORT int AGS_EditorStartup(void* /*editor*/)
{
    return 0; // not used
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

DLLEXPORT void AGS_EngineStartup(IAGSEngine* engine)
{
    // store engine pointer if needed by plugin
    (void)engine;
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
    return 2; // advertise Plugin V2 interface
}

}