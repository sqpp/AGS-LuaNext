cmake_minimum_required(VERSION 3.20)
project(AgsLuaNext VERSION 1.2.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes
include_directories(include)

# LuaJIT dependency discovery
if(WIN32)
    include_directories(third_party/luajit/src)
    link_directories(third_party/luajit/src)
    set(LUA_LIBS lua51.lib)
else()
    # On Linux/macOS:
    # 1. Try to find the STATIC local build first (for self-contained plugins)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(LUA_LIB_NAME "libluajit.a")
    else()
        set(LUA_LIB_NAME "libluajit32.a")
    endif()

    find_library(LUA_LIBS_STATIC NAMES ${LUA_LIB_NAME}
        HINTS "${CMAKE_SOURCE_DIR}/third_party/luajit/src"
        NO_DEFAULT_PATH)
    
    if(LUA_LIBS_STATIC)
        set(LUA_LIBS ${LUA_LIBS_STATIC})
        message(STATUS "Using STATIC LuaJIT: ${LUA_LIBS}")
        include_directories(third_party/luajit/src)
    else()
        message(STATUS "Local static LuaJIT not found. Falling back to system discovery...")
        
        # 2. Try PkgConfig (common on Linux/macOS)
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            # macOS Brew often puts pc files in these locations
            if(APPLE)
                set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig:/opt/homebrew/lib/pkgconfig:/usr/local/opt/luajit/lib/pkgconfig")
            endif()
            pkg_check_modules(LUA luajit QUIET)
            if(LUA_FOUND)
                set(LUA_LIBS ${LUA_LIBRARIES})
                include_directories(${LUA_INCLUDE_DIRS})
                link_directories(${LUA_LIBRARY_DIRS})
            endif()
        endif()

        # 3. Last resort fallback to standard library search (with Homebrew hints)
        if(NOT LUA_LIBS)
            find_library(LUA_LIBS NAMES luajit-5.1.2 luajit-5.1 luajit
                HINTS /usr/local/lib /opt/homebrew/lib /usr/local/opt/luajit/lib)
            
            # Find the header if the library was found
            if(LUA_LIBS)
                find_path(LUA_INCLUDE_DIR NAMES lua.h
                    HINTS /usr/local/include/luajit-2.1 /opt/homebrew/include/luajit-2.1 /usr/local/opt/luajit/include/luajit-2.1)
                if(LUA_INCLUDE_DIR)
                    include_directories(${LUA_INCLUDE_DIR})
                endif()
            else()
                # Absolute last fallback
                find_package(Lua51 REQUIRED)
                set(LUA_LIBS ${LUA_LIBRARIES})
                include_directories(${LUA_INCLUDE_DIR})
            endif()
        endif()
    endif()
    
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Source files
set(SOURCES
    src/AgsLuaNext.cpp
    src/plugin_exports.cpp
    src/dllmain.cpp
)

add_library(AgsLuaNext SHARED ${SOURCES})

# Precompiled Headers (Visual Studio style)
if(WIN32 AND MSVC)
    target_precompile_headers(AgsLuaNext PRIVATE include/pch.h)
endif()

target_link_libraries(AgsLuaNext PRIVATE ${LUA_LIBS})

# Proper naming for AGS plugins
if(WIN32)
    set_target_properties(AgsLuaNext PROPERTIES PREFIX "")
else()
    # Force lowercase name on Linux/macOS so AGS can find it without using symlinks!
    set_target_properties(AgsLuaNext PROPERTIES OUTPUT_NAME "agsluanext")
endif()
