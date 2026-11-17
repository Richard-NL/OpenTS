/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#ifndef _WIN32
#include "always.h"

#include "dbgprint_sys.h"

#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

char const DEBUG_PATH_SEPARATOR = '/';

static int DebugFile = -1;

unsigned long Debug_Process_ID(void)
{
	return(static_cast<unsigned long>(getpid()));
}


unsigned long Debug_Last_Error(void)
{
	return(0);
}


void Debug_Set_Last_Error(unsigned long)
{
}


void Debug_Write_To_Debugger(char const *)
{
}


bool Debug_Make_Directory(char const * path)
{
	return(mkdir(path, 0755) == 0 || errno == EEXIST);
}


void Debug_Log_Open(char const * path)
{
	DebugFile = open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0644);
}


bool Debug_Log_Is_Open(void)
{
	return(DebugFile != -1);
}


void Debug_Log_Write(char const * text, size_t length)
{
	write(DebugFile, text, length);
}


void Debug_Log_Close(void)
{
	close(DebugFile);
	DebugFile = -1;
}
#endif
