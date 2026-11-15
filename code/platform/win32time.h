/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

// Win32 timer and date functions the engine uses, for builds without <windows.h>.

#include "platform/win32types.h"

BOOL QueryPerformanceCounter(LARGE_INTEGER * lpPerformanceCount);
BOOL QueryPerformanceFrequency(LARGE_INTEGER * lpFrequency);
DWORD timeGetTime();
MMRESULT timeBeginPeriod(UINT uPeriod);
MMRESULT timeEndPeriod(UINT uPeriod);
ULONGLONG GetTickCount64();
DWORD GetTickCount();
void GetSystemTimeAsFileTime(FILETIME * lpSystemTimeAsFileTime);
void GetSystemTime(SYSTEMTIME * lpSystemTime);
void GetLocalTime(SYSTEMTIME * lpSystemTime);
BOOL SystemTimeToFileTime(const SYSTEMTIME * lpSystemTime, FILETIME * lpFileTime);
