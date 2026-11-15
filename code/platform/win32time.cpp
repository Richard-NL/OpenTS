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

#include <SDL3/SDL_time.h>

#include <chrono>

using Counter_Unit = std::chrono::nanoseconds;

BOOL QueryPerformanceCounter(LARGE_INTEGER * lpPerformanceCount)
{
	auto const now = std::chrono::steady_clock::now().time_since_epoch();
	lpPerformanceCount->QuadPart = std::chrono::duration_cast<Counter_Unit>(now).count();
	return TRUE;
}


BOOL QueryPerformanceFrequency(LARGE_INTEGER * lpFrequency)
{
	lpFrequency->QuadPart = Counter_Unit::period::den;
	return TRUE;
}


DWORD timeGetTime()
{
	return static_cast<DWORD>(GetTickCount64());
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


DWORD GetTickCount()
{
	return timeGetTime();
}


ULONGLONG GetTickCount64()
{
	auto const now = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}


static void Fill_System_Time(SYSTEMTIME * out, bool local)
{
	SDL_Time ticks = 0;
	SDL_DateTime dt = {};
	if (SDL_GetCurrentTime(&ticks)) {
		SDL_TimeToDateTime(ticks, &dt, local);
	}
	out->wYear = static_cast<WORD>(dt.year);
	out->wMonth = static_cast<WORD>(dt.month);
	out->wDayOfWeek = static_cast<WORD>(dt.day_of_week);
	out->wDay = static_cast<WORD>(dt.day);
	out->wHour = static_cast<WORD>(dt.hour);
	out->wMinute = static_cast<WORD>(dt.minute);
	out->wSecond = static_cast<WORD>(dt.second);
	out->wMilliseconds = static_cast<WORD>(dt.nanosecond / 1000000);
}


void GetSystemTimeAsFileTime(FILETIME * out)
{
	SDL_Time ticks = 0;
	SDL_GetCurrentTime(&ticks);
	SDL_TimeToWindows(ticks, &out->dwLowDateTime, &out->dwHighDateTime);
}


void GetSystemTime(SYSTEMTIME * out)
{
	Fill_System_Time(out, false);
}


void GetLocalTime(SYSTEMTIME * out)
{
	Fill_System_Time(out, true);
}


BOOL SystemTimeToFileTime(const SYSTEMTIME * in, FILETIME * out)
{
	SDL_DateTime dt = {};
	dt.year = in->wYear;
	dt.month = in->wMonth;
	dt.day = in->wDay;
	dt.day_of_week = in->wDayOfWeek;
	dt.hour = in->wHour;
	dt.minute = in->wMinute;
	dt.second = in->wSecond;
	dt.nanosecond = in->wMilliseconds * 1000000;
	SDL_Time ticks = 0;
	if (!SDL_DateTimeToTime(&dt, &ticks)) {
		return FALSE;
	}

	SDL_TimeToWindows(ticks, &out->dwLowDateTime, &out->dwHighDateTime);

	return TRUE;
}

#endif
