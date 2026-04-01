# Changelog
 
 All notable changes to this project will be documented in this file.
 
 Format: [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
 Project: [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
 
 ## [1.1.2] - 2026-04-01
 
+### Fixed
+- **macOS CI build**: Removed universal binary enforcement in GitHub Actions to fix build failures when Homebrew dependencies only provide native architectures.
+
 ## [1.1.0] - 2026-04-01
 
 ### Added
 - **Multi-platform support**: Added build support for **Linux** and Possibly? **macOS**.
 - **CMake build system**: Unified build process using `CMakeLists.txt`.
 - **Dynamic calling convention**: Lua bridge now automatically handles `__stdcall` only on Windows x86.

 
 ## [1.0.0] - 2026-03-31
 
 ### Added
 - Version tracking file (`VERSION`).
 - Changelog file (`CHANGELOG.md`).
 - GitHub Actions release packaging and tag-based release asset publishing.
 
 ### Changed
 - Clarified README usage/build notes for AGS x86 runtime and Lua folder behavior.
 - Improved runtime Lua module loading/logging behavior around `lib` and native DLL loading.
 
 ## [0.1.0] - 2026-03-30
 
 ### Added
 - Initial public release of AgsLuaNext (LuaJIT runtime plugin for AGS).
