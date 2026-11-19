/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

#ifdef _WIN32
#include "always.h"

#include "dbgprint_sys.h"

#include <windows.h>

char const DEBUG_PATH_SEPARATOR = '\\';

static HANDLE DebugFile = INVALID_HANDLE_VALUE;

unsigned long Debug_Process_ID(void)
{
	return(GetCurrentProcessId());
}


unsigned long Debug_Last_Error(void)
{
	return(GetLastError());
}


void Debug_Set_Last_Error(unsigned long error)
{
	SetLastError(error);
}


void Debug_Write_To_Debugger(char const * text)
{
	// Reporting to a debugger that is not there costs an exception round trip per message,
	// which is far more than writing the line to the log.
	if (IsDebuggerPresent()) {
		OutputDebugString(text);
	}
}

bool Debug_Make_Directory(char const * path)
{
	return(CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS);
}


void Debug_Log_Open(char const * path)
{
	DebugFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
									CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
}


bool Debug_Log_Is_Open(void)
{
	return(DebugFile != INVALID_HANDLE_VALUE);
}


void Debug_Log_Write(char const * text, size_t length)
{
	DWORD actual;
	WriteFile(DebugFile, text, (DWORD)length, &actual, NULL);
}


void Debug_Log_Close(void)
{
	CloseHandle(DebugFile);
	DebugFile = INVALID_HANDLE_VALUE;
}


bool Debug_Delete_Files_Older_Than(char const * directory, char const * pattern, unsigned days)
{
	SYSTEMTIME now;
	FILETIME now_stamp;
	GetSystemTime(&now);
	if (!SystemTimeToFileTime(&now, &now_stamp)) {
		return(false);
	}

	ULARGE_INTEGER cutoff;
	cutoff.LowPart = now_stamp.dwLowDateTime;
	cutoff.HighPart = now_stamp.dwHighDateTime;

	unsigned __int64 const age = (unsigned __int64)days * 24ui64 * 60ui64 * 60ui64 * 10000000ui64;
	if (cutoff.QuadPart < age) {
		return(false);
	}
	cutoff.QuadPart -= age;

	char search[MAX_PATH];
	snprintf(search, sizeof(search), "%s\\%s", directory, pattern);

	WIN32_FIND_DATA found;
	HANDLE search_handle = FindFirstFile(search, &found);
	if (search_handle == INVALID_HANDLE_VALUE) {
		return(false);
	}

	do {
		if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
			continue;
		}

		ULARGE_INTEGER written;
		written.LowPart = found.ftLastWriteTime.dwLowDateTime;
		written.HighPart = found.ftLastWriteTime.dwHighDateTime;

		if (written.QuadPart == 0 || written.QuadPart >= cutoff.QuadPart) {
			continue;
		}

		char victim[MAX_PATH];
		snprintf(victim, sizeof(victim), "%s\\%s", directory, found.cFileName);
		DeleteFile(victim);

	} while (FindNextFile(search_handle, &found));

	FindClose(search_handle);
	return(true);
}


bool Debug_Executable_Path(char * path, size_t size)
{
	return(GetModuleFileName(GetModuleHandle(NULL), path, static_cast<DWORD>(size)) != 0);
}
#endif
