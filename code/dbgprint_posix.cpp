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

#include <unistd.h>

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
#endif
