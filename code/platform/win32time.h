/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

// Win32 timer functions the engine uses, for builds without <windows.h>.
// SDL3 provides them in win32time.cpp.

#include "platform/win32types.h"

BOOL QueryPerformanceCounter(LARGE_INTEGER * lpPerformanceCount);
BOOL QueryPerformanceFrequency(LARGE_INTEGER * lpFrequency);
DWORD timeGetTime();
void Sleep(DWORD dwMilliseconds);
MMRESULT timeBeginPeriod(UINT uPeriod);
MMRESULT timeEndPeriod(UINT uPeriod);
ULONGLONG GetTickCount64();
