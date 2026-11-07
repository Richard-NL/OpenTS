/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#ifndef _WIN32

#include "platform/win32time.h"

#include <SDL3/SDL_timer.h>

BOOL QueryPerformanceCounter(LARGE_INTEGER * lpPerformanceCount)
{
	lpPerformanceCount->QuadPart = static_cast<LONGLONG>(SDL_GetPerformanceCounter());
	return TRUE;
}


BOOL QueryPerformanceFrequency(LARGE_INTEGER * lpFrequency)
{
	lpFrequency->QuadPart = static_cast<LONGLONG>(SDL_GetPerformanceFrequency());
	return TRUE;
}


DWORD timeGetTime()
{
	return static_cast<DWORD>(SDL_GetTicks());
}


void Sleep(DWORD dwMilliseconds)
{
	SDL_Delay(dwMilliseconds);
}

// Linux has no timer resolution setting; sleeps are already accurate to the millisecond.
MMRESULT timeBeginPeriod(UINT)
{
	return 0;
}


MMRESULT timeEndPeriod(UINT)
{
	return 0;
}


ULONGLONG GetTickCount64()
{
	return SDL_GetTicks();
}

#endif
