cmake_minimum_required(VERSION 3.20)
project(AgsLuaNext VERSION 1.1.2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Includes
include_directories(include)

# LuaJIT dependency
if(WIN32)
    # On Windows, we expect LuaJIT in third_party
    include_directories(third_party/luajit/src)
    link_directories(third_party/luajit/src)
    set(LUA_LIBS lua51.lib)
else()
    # On Linux and macOS, use pkg-config for LuaJIT
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LUAJIT REQUIRED luajit)
    include_directories(${LUAJIT_INCLUDE_DIRS})
    link_directories(${LUAJIT_LIBRARY_DIRS})
    set(LUA_LIBS ${LUAJIT_LIBRARIES})
    
    # Ensure -fPIC for shared libraries
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
endif()
