/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

// dbgprint_win.cpp and dbgprint_posix.cpp each define these; only one is built.
unsigned long Debug_Process_ID(void);
// The thread's Win32 error code. Off Windows it is always 0, and setting it does nothing.
unsigned long Debug_Last_Error(void);
void Debug_Set_Last_Error(unsigned long error);
// Off Windows this does nothing.
void Debug_Write_To_Debugger(char const * text);
