_G.ffi = require("ffi")

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
