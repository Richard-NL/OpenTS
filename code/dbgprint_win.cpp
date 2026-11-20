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

#include <conio.h>
#include <cstdio>
#include <windows.h>
#include <shellapi.h>

#define CONSOLE_WINDOW_NAME		"Debug Console"

char const DEBUG_PATH_SEPARATOR = '\\';
bool const DEBUG_CONSOLE_CLOSES_WITH_PROCESS = true;

static HANDLE DebugFile = INVALID_HANDLE_VALUE;
static HANDLE DebugConsole = INVALID_HANDLE_VALUE;

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


std::vector<std::string> Debug_Command_Line_Arguments(void)
{
	std::vector<std::string> arguments;
	int argc = 0;
	LPWSTR * argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argv == NULL) {
		return(arguments);
	}

	// Index zero is the executable path, which may itself look like an option.
	for (int index = 1; index < argc; index++) {
		int const length = WideCharToMultiByte(CP_UTF8, 0, argv[index], -1, NULL, 0, NULL, NULL);
		if (length <= 0) {
			continue;
		}

		std::string argument(static_cast<size_t>(length - 1), '\0');
		WideCharToMultiByte(CP_UTF8, 0, argv[index], -1, argument.data(), length, NULL, NULL);
		arguments.push_back(argument);
	}

	LocalFree(argv);
	return(arguments);
}


bool Debug_Console_Open(void)
{
	if (!AllocConsole()) {
		return(false);
	}

	SetConsoleTitle(CONSOLE_WINDOW_NAME);

	// A new console decodes output in the OEM page, and the log lines it shows are UTF-8.
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	// Redirecting the standard streams is what lets ordinary stdio output, such as the
	// command line help, reach the console of a windowed application.
	FILE * stream = NULL;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);
	freopen_s(&stream, "CONIN$", "r", stdin);

	HANDLE output = CreateFile("CONOUT$", GENERIC_READ | GENERIC_WRITE,
										FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	HANDLE input = CreateFile("CONIN$", GENERIC_READ | GENERIC_WRITE,
										FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (output != INVALID_HANDLE_VALUE) {
		SetStdHandle(STD_OUTPUT_HANDLE, output);
		SetStdHandle(STD_ERROR_HANDLE, output);
	}

	if (input != INVALID_HANDLE_VALUE) {
		SetStdHandle(STD_INPUT_HANDLE, input);
	}

	DebugConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (DebugConsole == NULL) {
		DebugConsole = INVALID_HANDLE_VALUE;
	}

	if (DebugConsole != INVALID_HANDLE_VALUE) {
		CONSOLE_SCREEN_BUFFER_INFO info;
		if (GetConsoleScreenBufferInfo(DebugConsole, &info)) {
			COORD size;
			size.X = info.dwSize.X;
			size.Y = 4096;
			SetConsoleScreenBufferSize(DebugConsole, size);
		}
	}

	// Without this, closing the console window would take the game down with it.
	HWND console_window = GetConsoleWindow();
	if (console_window != NULL) {
		HMENU menu = GetSystemMenu(console_window, FALSE);
		if (menu != NULL) {
			DeleteMenu(menu, SC_CLOSE, MF_BYCOMMAND);
		}
	}

	return(true);
}


void Debug_Console_Write(char const * text, size_t length)
{
	DWORD actual;

	if (DebugConsole != INVALID_HANDLE_VALUE) {
		WriteConsole(DebugConsole, text, (DWORD)length, &actual, NULL);
	}
}


void Debug_Console_Wait_For_Key(void)
{
	_getch();
}
#endif
