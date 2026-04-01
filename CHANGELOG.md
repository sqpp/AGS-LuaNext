# Changelog

All notable changes to this project will be documented in this file.

## [1.2.1] - 2026-04-01
### Added
- Full 64-bit Linux support for Adventure Game Studio.
- New self-contained build system (No external LuaJIT dependency required for players).
- Automatic identification of 32-bit vs 64-bit Linux environments in the build process.
- Improved diagnostic logging to track plugin initialization in cross-platform environments.

### Fixed
- Fixed a 64-bit Segmentation Fault by hardening the Lua FFI memory bridge.
- Fixed a Linux loading error by enforcing lowercase plugin naming (`libagsluanext.so`).
- Corrected architecture mismatches where 64-bit builds were picking up 32-bit libraries.


## [1.2.0] - 2026-03-31
### Added
- Initial Cross-Platform support for Linux and macOS.
- Unified CMake build system for all platforms.
- Version tracking with VERSION file.

## [1.1.2] - 2026-03-30
### Added
- Integration of custom precompiled headers for Visual Studio builds.
- Refactored project structure for easier community contributions.

## [1.1.0] - 2026-03-29
### Added
- High-performance LuaJIT FFI bridge for raw engine access.
- Script functions for direct Lua execution within AGS scripts.
