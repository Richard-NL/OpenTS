/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

// Compiler intrinsics the engine uses. On Windows they come from <intrin.h>;
// elsewhere they map onto Clang builtins.

#ifdef _WIN32
#include <intrin.h>
#else
#define _ReturnAddress() __builtin_return_address(0)
#endif
