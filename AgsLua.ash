// AgsLuaNext: LuaJIT for AGS 3.6.2

managed struct Lua {
    /// Initializes the LuaJIT VM (called automatically by Run if not initialized)
    import static void Init();
    
    /// Runs a string of LuaJIT code
    import static void Run(string code);
};
