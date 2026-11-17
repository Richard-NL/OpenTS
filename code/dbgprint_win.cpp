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
#endif
