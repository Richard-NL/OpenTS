/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

// The Win32 names the engine uses, for builds without <windows.h>. Each type has the
// size it has on Windows, because some of them are read from and written to disk.

#include <cstdint>

using BYTE = std::uint8_t;
using WORD = std::uint16_t;
using DWORD = std::uint32_t;
using BOOL = std::int32_t;
using LONG = std::int32_t;
using UINT = std::uint32_t;
using WPARAM = std::uintptr_t;
using LPARAM = std::intptr_t;
using LRESULT = std::intptr_t;
using COLORREF = std::uint32_t;
using HWND = struct HWND__ *;
using HINSTANCE = struct HINSTANCE__ *;
using HDC = struct HDC__ *;
using HBITMAP = struct HBITMAP__ *;
using HFONT = struct HFONT__ *;
using HKL = struct HKL__ *;
using HGDIOBJ = void *;
using HANDLE = void *;

#define FALSE 0
#define TRUE 1

// dialog results
#define IDOK 1
#define IDCANCEL 2
