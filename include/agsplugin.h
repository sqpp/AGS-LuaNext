//=============================================================================
// Adventure Game Studio (AGS) Plugin SDK (Full Protected VTable v30)
//=============================================================================
#ifndef _AGS_PLUGIN_H
#define _AGS_PLUGIN_H
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#if !defined(_WINGDI_) && !defined(BITMAP_DEFINED)
typedef void BITMAP;
#define BITMAP_DEFINED
#endif

#if !defined(_WINDOWS_) && !defined(HWND) && !defined(_HWND_DEFINED)
typedef void* HWND;
#define _HWND_DEFINED
#endif

#ifdef _WIN32
#define AGSIFUNC(type) virtual type __stdcall
#define DLLEXPORT __declspec(dllexport)
#else
#define AGSIFUNC(type) virtual type
#define DLLEXPORT 
#endif

typedef void* LPDIRECTDRAW2;
typedef void* LPDIRECTDRAWSURFACE2;
typedef void* LPDIRECTSOUND;
typedef void* LPDIRECTINPUTDEVICE;

// AGSEngine Log Levels
#define AGSLOG_LEVEL_ALERT    1
#define AGSLOG_LEVEL_CRITICAL 2
#define AGSLOG_LEVEL_ERROR    3
#define AGSLOG_LEVEL_WARN     4
#define AGSLOG_LEVEL_INFO     5
#define AGSLOG_LEVEL_DEBUG    6

class IAGSEngine {
public:
    int32_t version;
    int32_t pluginId;

    // MANDATORY VTABLE ORDER (v1 - v30)
    AGSIFUNC(void) AbortGame (const char *reason) = 0;
    AGSIFUNC(const char*) GetEngineVersion () = 0;
    AGSIFUNC(void) RegisterScriptFunction (const char *name, void *address) = 0;
    AGSIFUNC(HWND) GetWindowHandle() = 0;
    AGSIFUNC(LPDIRECTDRAW2) GetDirectDraw2 () = 0;
    AGSIFUNC(LPDIRECTDRAWSURFACE2) GetBitmapSurface (BITMAP *) = 0;
    AGSIFUNC(BITMAP *) GetScreen () = 0;
    AGSIFUNC(void) RequestEventHook (int32_t event) = 0;
    AGSIFUNC(int)  GetSavedData (char *buffer, int32_t bufsize) = 0;
    AGSIFUNC(BITMAP *) GetVirtualScreen () = 0;
    AGSIFUNC(void) DrawText (int32_t x, int32_t y, int32_t font, int32_t color, char *text) = 0;
    AGSIFUNC(void) GetScreenDimensions (int32_t *width, int32_t *height, int32_t *coldepth) = 0;
    AGSIFUNC(unsigned char**) GetRawBitmapSurface (BITMAP *) = 0;
    AGSIFUNC(void) ReleaseBitmapSurface (BITMAP *) = 0;
    AGSIFUNC(void) GetMousePosition (int32_t *x, int32_t *y) = 0;
    AGSIFUNC(int)  GetCurrentRoom () = 0;
    AGSIFUNC(int)  GetNumBackgrounds () = 0;
    AGSIFUNC(int)  GetCurrentBackground () = 0;
    AGSIFUNC(BITMAP *) GetBackgroundScene (int32_t) = 0;
    AGSIFUNC(void) GetBitmapDimensions (BITMAP *bmp, int32_t *width, int32_t *height, int32_t *coldepth) = 0;
    AGSIFUNC(int)  FWrite (void *out_buf, int32_t len, int32_t fhandle) = 0;
    AGSIFUNC(int)  FRead (void *in_buf, int32_t len, int32_t fhandle) = 0;
    AGSIFUNC(void) DrawTextWrapped (int32_t x, int32_t y, int32_t width, int32_t font, int32_t color, const char *text) = 0;
    AGSIFUNC(void) SetVirtualScreen (BITMAP *) = 0;
    AGSIFUNC(int)  LookupParserWord (const char *word) = 0;
    AGSIFUNC(void) BlitBitmap (int32_t x, int32_t y, BITMAP *, int32_t masked) = 0;
    AGSIFUNC(void) PollSystem () = 0;
    AGSIFUNC(int)  GetNumCharacters () = 0;
    AGSIFUNC(void*) GetCharacter (int32_t) = 0;
    AGSIFUNC(void*) GetGameOptions () = 0;
    AGSIFUNC(void*) GetPalette() = 0;
    AGSIFUNC(void) SetPalette (int32_t, int32_t, void*) = 0;
    AGSIFUNC(int)  GetPlayerCharacter () = 0;
    AGSIFUNC(void) RoomToViewport (int32_t*, int32_t*) = 0;
    AGSIFUNC(void) ViewportToRoom (int32_t*, int32_t*) = 0;
    AGSIFUNC(int)  GetNumObjects () = 0;
    AGSIFUNC(void*) GetObject (int32_t) = 0;
    AGSIFUNC(BITMAP *) GetSpriteGraphic (int32_t) = 0;
    AGSIFUNC(BITMAP *) CreateBlankBitmap (int32_t, int32_t, int32_t) = 0;
    AGSIFUNC(void) FreeBitmap (BITMAP *) = 0;
    AGSIFUNC(BITMAP *) GetRoomMask(int32_t) = 0;
    AGSIFUNC(void*) GetViewFrame(int32_t, int32_t, int32_t) = 0;
    AGSIFUNC(int)  GetWalkbehindBaseline(int32_t) = 0;
    AGSIFUNC(void*) GetScriptFunctionAddress(const char*) = 0;
    AGSIFUNC(int)  GetBitmapTransparentColor(BITMAP *) = 0;
    AGSIFUNC(int)  GetAreaScaling (int32_t, int32_t) = 0;
    AGSIFUNC(int)  IsGamePaused() = 0;
    AGSIFUNC(int)  GetRawPixelColor (int32_t) = 0;
    AGSIFUNC(int)  GetSpriteWidth (int32_t) = 0;
    AGSIFUNC(int)  GetSpriteHeight (int32_t) = 0;
    AGSIFUNC(void) GetTextExtent (int32_t, const char*, int32_t*, int32_t*) = 0;
    AGSIFUNC(void) PrintDebugConsole (const char*) = 0;
    AGSIFUNC(void) PlaySoundChannel (int32_t, int32_t, int32_t, int32_t, const char*) = 0;
    AGSIFUNC(int)  IsChannelPlaying (int32_t) = 0;
    AGSIFUNC(void) MarkRegionDirty(int32_t, int32_t, int32_t, int32_t) = 0;
    AGSIFUNC(void*) GetMouseCursor(int32_t) = 0;
    AGSIFUNC(void) GetRawColorComponents(int32_t, int32_t, int32_t*, int32_t*, int32_t*, int32_t*) = 0;
    AGSIFUNC(int)  MakeRawColorPixel(int32_t, int32_t, int32_t, int32_t, int32_t) = 0;
    AGSIFUNC(int)  GetFontType(int32_t) = 0;
    AGSIFUNC(int)  CreateDynamicSprite(int32_t, int32_t, int32_t) = 0;
    AGSIFUNC(void) DeleteDynamicSprite(int32_t) = 0;
    AGSIFUNC(int)  IsSpriteAlphaBlended(int32_t) = 0;
    AGSIFUNC(void) UnrequestEventHook(int32_t) = 0;
    AGSIFUNC(void) BlitSpriteTranslucent(int32_t, int32_t, BITMAP *, int32_t) = 0;
    AGSIFUNC(void) BlitSpriteRotated(int32_t, int32_t, BITMAP *, int32_t) = 0;
    AGSIFUNC(void*) GetDirectSound() = 0;
    AGSIFUNC(void) DisableSound() = 0;
    AGSIFUNC(int)  CanRunScriptFunctionNow() = 0;
    AGSIFUNC(int)  CallGameScriptFunction(const char*, int32_t, int32_t, ...) = 0;
    AGSIFUNC(void) NotifySpriteUpdated(int32_t) = 0;
    AGSIFUNC(void) SetSpriteAlphaBlended(int32_t, int32_t) = 0;
    AGSIFUNC(void) QueueGameScriptFunction(const char*, int32_t, int32_t, ...) = 0;
    AGSIFUNC(int)  RegisterManagedObject(void*, void*) = 0;
    AGSIFUNC(void) AddManagedObjectReader(const char*, void*) = 0;
    AGSIFUNC(void) RegisterUnserializedObject(int32_t, void*, void*) = 0;
    AGSIFUNC(void*) GetManagedObjectAddressByKey(int32_t) = 0;
    AGSIFUNC(int)  GetManagedObjectKeyByAddress(void*) = 0;
    AGSIFUNC(const char*) CreateScriptString(const char*) = 0;
    AGSIFUNC(int)  IncrementManagedObjectRefCount(void*) = 0;
    AGSIFUNC(int)  DecrementManagedObjectRefCount(void*) = 0;
    AGSIFUNC(void) SetMousePosition(int32_t, int32_t) = 0;
    AGSIFUNC(void) SimulateMouseClick(int32_t) = 0;
    AGSIFUNC(int)  GetMovementPathWaypointCount(int32_t) = 0;
    AGSIFUNC(int)  GetMovementPathLastWaypoint(int32_t) = 0;
    AGSIFUNC(void) GetMovementPathWaypointLocation(int32_t, int32_t, int32_t*, int32_t*) = 0;
    AGSIFUNC(void) GetMovementPathWaypointSpeed(int32_t, int32_t, int32_t*, int32_t*) = 0;
    AGSIFUNC(const char*) GetGraphicsDriverID() = 0;
    AGSIFUNC(int)  IsRunningUnderDebugger() = 0;
    AGSIFUNC(void) BreakIntoDebugger() = 0;
    AGSIFUNC(void) GetPathToFileInCompiledFolder(const char*, char*) = 0;
    AGSIFUNC(void*) GetDirectInputKeyboard() = 0;
    AGSIFUNC(void*) GetDirectInputMouse() = 0;
    AGSIFUNC(void*) ReplaceFontRenderer(int32_t, void*) = 0;
    AGSIFUNC(void) GetRenderStageDesc(void*) = 0;
    AGSIFUNC(void) GetGameInfo(void*) = 0;
    AGSIFUNC(void*) ReplaceFontRenderer2(int32_t, void*) = 0;
    AGSIFUNC(void) NotifyFontUpdated(int32_t) = 0;
    AGSIFUNC(size_t) ResolveFilePath(const char*, char*, size_t) = 0;
    AGSIFUNC(void*) OpenFileStream(const char*, int32_t, int32_t) = 0;
    AGSIFUNC(void*) GetFileStreamByHandle(int32_t) = 0;
    AGSIFUNC(void) Log(int32_t level, const char* fmt, ...) = 0;
    AGSIFUNC(void*) CreateDynamicArray(size_t, size_t, bool) = 0;
};

extern "C" {
    DLLEXPORT const char * AGS_GetPluginName(void);
    DLLEXPORT int    AGS_EditorStartup (void *);
    DLLEXPORT void   AGS_EditorShutdown (void);
    DLLEXPORT void   AGS_EditorProperties (HWND);
    DLLEXPORT int    AGS_EditorSaveGame (char *, int);
    DLLEXPORT void   AGS_EditorLoadGame (char *, int);
    DLLEXPORT void   AGS_EngineStartup (IAGSEngine *);
    DLLEXPORT void   AGS_EngineShutdown (void);
    DLLEXPORT intptr_t AGS_EngineOnEvent (int, intptr_t);
    DLLEXPORT int    AGS_EngineDebugHook(const char *, int, int);
    DLLEXPORT void   AGS_EngineInitGfx(const char* driverID, void *data); 
    DLLEXPORT int    AGS_PluginV2 ();
}
#endif
