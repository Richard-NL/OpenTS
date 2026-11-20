/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstddef>
#include <string>
#include <vector>

// dbgprint_win.cpp and dbgprint_posix.cpp each define these; only one is built.
unsigned long Debug_Process_ID(void);
// The thread's Win32 error code. Off Windows it is always 0, and setting it does nothing.
unsigned long Debug_Last_Error(void);
void Debug_Set_Last_Error(unsigned long error);
// Off Windows this does nothing.
void Debug_Write_To_Debugger(char const * text);

// The character between a folder and a file name in the paths below.
extern char const DEBUG_PATH_SEPARATOR;
// True when the folder exists afterwards, whether or not this call created it.
bool Debug_Make_Directory(char const * path);

// The log file. Call these with the logging lock held. Opening leaves the log closed when
// the file already exists or cannot be created; an existing file is never overwritten.
void Debug_Log_Open(char const * path);
bool Debug_Log_Is_Open(void);
void Debug_Log_Write(char const * text, size_t length);
void Debug_Log_Close(void);

// Deletes the files in the folder whose names match the pattern and that were last written
// more than the given number of days ago; folders are skipped. False when the folder cannot
// be searched, and on Windows also when no file matches.
bool Debug_Delete_Files_Older_Than(char const * directory, char const * pattern, unsigned days);

// Writes the executable's full path. False when it cannot be read. A path that does not fit is
// cut short on Windows and gives false elsewhere.
bool Debug_Executable_Path(char * path, size_t size);

// The command-line arguments after the program name, in UTF-8. Empty when they cannot be read.
std::vector<std::string> Debug_Command_Line_Arguments(void);
