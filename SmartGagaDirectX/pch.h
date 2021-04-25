// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define CURRENT_VERSION "1.0.1.6"

// Windows Header Files
#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <WinInet.h>
#include <random>
#include <vector>
#include "shellapi.h"
#include "shlwapi.h"
#include "../VMProtect/VMProtectSDK.h"
#include "../MinHook/MinHook.h"
#pragma comment(lib, "WinInet.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

#include "Helpers.h"
#include "Keymap.h"
#include "GameThreadSuspender.h"
#include "Hooks.h"
#endif //PCH_H
