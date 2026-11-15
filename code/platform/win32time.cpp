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

#include <chrono>
#include <ctime>

using Counter_Unit = std::chrono::nanoseconds;

// A FILETIME counts 100-nanosecond intervals from 1601-01-01 UTC, 11644473600 seconds before 1970.
using File_Time_Unit = std::chrono::duration<LONGLONG, std::ratio<1, 10000000>>;
constexpr std::chrono::seconds File_Time_Epoch_Offset{11644473600};

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
	auto const now = std::chrono::system_clock::now();
	std::time_t const seconds = std::chrono::system_clock::to_time_t(now);
	std::tm fields = {};
	if (local) {
		localtime_r(&seconds, &fields);
	} else {
		gmtime_r(&seconds, &fields);
	}
	auto const subsecond = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	out->wYear = static_cast<WORD>(fields.tm_year + 1900);
	out->wMonth = static_cast<WORD>(fields.tm_mon + 1);
	out->wDayOfWeek = static_cast<WORD>(fields.tm_wday);
	out->wDay = static_cast<WORD>(fields.tm_mday);
	out->wHour = static_cast<WORD>(fields.tm_hour);
	out->wMinute = static_cast<WORD>(fields.tm_min);
	out->wSecond = static_cast<WORD>(fields.tm_sec);
	out->wMilliseconds = static_cast<WORD>(subsecond.count());
}


static void Write_File_Time(std::chrono::system_clock::time_point time, FILETIME * out)
{
	auto const since_1601 = std::chrono::duration_cast<File_Time_Unit>(time.time_since_epoch()) + File_Time_Epoch_Offset;
	LONGLONG const ticks = since_1601.count();
	out->dwLowDateTime = static_cast<DWORD>(ticks);
	out->dwHighDateTime = static_cast<DWORD>(ticks >> 32);
}


void GetSystemTimeAsFileTime(FILETIME * out)
{
	Write_File_Time(std::chrono::system_clock::now(), out);
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
	if (in->wYear < 1601 || in->wYear > 30827 || in->wMonth > 12 || in->wDay > 31
		|| in->wHour > 23 || in->wMinute > 59 || in->wSecond > 59 || in->wMilliseconds > 999) {
		return FALSE;
	}

	std::chrono::year_month_day const date{std::chrono::year{in->wYear}, std::chrono::month{in->wMonth}, std::chrono::day{in->wDay}};
	if (!date.ok()) {
		return FALSE;
	}

	auto const time = std::chrono::sys_days{date} + std::chrono::hours{in->wHour} + std::chrono::minutes{in->wMinute}
		+ std::chrono::seconds{in->wSecond} + std::chrono::milliseconds{in->wMilliseconds};
	Write_File_Time(time, out);

	return TRUE;
}

#endif
