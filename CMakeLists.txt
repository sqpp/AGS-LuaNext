cmake_minimum_required(VERSION 3.20)
project(AgsLuaNext VERSION 1.2.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes
include_directories(include)

# LuaJIT dependency
if(WIN32)
    include_directories(third_party/luajit/src)
    link_directories(third_party/luajit/src)
    set(LUA_LIBS lua51.lib)
else()
    # On Linux/macOS:
    # We strictly want the STATIC library so the .so is self-contained.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(LUA_LIB_NAME "libluajit.a")
    else()
        set(LUA_LIB_NAME "libluajit32.a")
    endif()

    find_library(LUA_LIBS NAMES ${LUA_LIB_NAME}
        HINTS "${CMAKE_SOURCE_DIR}/third_party/luajit/src"
        NO_DEFAULT_PATH)
    
    if(NOT LUA_LIBS)
        message(FATAL_ERROR "Could not find local static LuaJIT for architecture ${CMAKE_SIZEOF_VOID_P} in third_party/luajit/src. Build it first!")
    endif()

    message(STATUS "Using STATIC LuaJIT: ${LUA_LIBS}")
    include_directories(third_party/luajit/src)
    
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
