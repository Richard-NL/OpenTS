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
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <fstream>
#include <istream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#if defined(__APPLE__)
#include <climits>
#include <crt_externs.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <mach-o/dyld.h>
#elif defined(__FreeBSD__)
#include <sstream>
#include <sys/sysctl.h>
#endif

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


bool Debug_Delete_Files_Older_Than(char const * directory, char const * pattern, unsigned days)
{
	time_t const now = time(nullptr);
	time_t const age = static_cast<time_t>(days) * 24 * 60 * 60;
	if (now < age) {
		return(false);
	}
	time_t const cutoff = now - age;

	DIR * listing = opendir(directory);
	if (listing == nullptr) {
		return(false);
	}

	while (dirent const * entry = readdir(listing)) {
		if (fnmatch(pattern, entry->d_name, 0) != 0) {
			continue;
		}

		std::string const path = std::string(directory) + DEBUG_PATH_SEPARATOR + entry->d_name;

		struct stat info;
		if (lstat(path.c_str(), &info) != 0 || !S_ISREG(info.st_mode)) {
			continue;
		}

		if (info.st_mtime == 0 || info.st_mtime >= cutoff) {
			continue;
		}

		unlink(path.c_str());
	}

	closedir(listing);
	return(true);
}


bool Debug_Executable_Path(char * path, size_t size)
{
#if defined(__linux__)
	ssize_t const length = readlink("/proc/self/exe", path, size);

	// A full buffer may hold a cut-off path, so it counts as a failure like -1 does.
	if (length < 0 || static_cast<size_t>(length) >= size) {
		return(false);
	}
	path[length] = '\0';
	return(true);
#elif defined(__APPLE__)
	char raw[PATH_MAX];
	uint32_t raw_size = sizeof(raw);
	char resolved[PATH_MAX];

	// Resolve symbolic links so the log folder sits beside the real file, as on Linux.
	if (_NSGetExecutablePath(raw, &raw_size) != 0 || realpath(raw, resolved) == nullptr) {
		return(false);
	}
	int const length = snprintf(path, size, "%s", resolved);
	return(length >= 0 && static_cast<size_t>(length) < size);
#elif defined(__FreeBSD__)
	int const request[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
	size_t length = size;
	return(sysctl(request, 4, path, &length, nullptr, 0) == 0);
#else
#error "Debug_Executable_Path has no implementation for this system."
#endif
}


#if defined(__linux__) || defined(__FreeBSD__)
// Reads arguments that each end with '\0', and drops the first one, the program name.
static std::vector<std::string> Read_Arguments(std::istream & input)
{
	std::vector<std::string> arguments;
	std::string argument;

	while (std::getline(input, argument, '\0')) {
		arguments.push_back(argument);
	}

	if (!arguments.empty()) {
		arguments.erase(arguments.begin());
	}
	return(arguments);
}
#endif


std::vector<std::string> Debug_Command_Line_Arguments(void)
{
#if defined(__linux__)
	std::ifstream file("/proc/self/cmdline", std::ios::binary);
	return(Read_Arguments(file));
#elif defined(__APPLE__)
	std::vector<std::string> arguments;
	int const argc = *_NSGetArgc();
	char ** const argv = *_NSGetArgv();

	// Index zero is the executable path, which may itself look like an option.
	for (int index = 1; index < argc; index++) {
		arguments.push_back(argv[index]);
	}
	return(arguments);
#elif defined(__FreeBSD__)
	int const request[] = { CTL_KERN, KERN_PROC, KERN_PROC_ARGS, static_cast<int>(getpid()) };
	size_t size = 0;

	// The first call asks for the size, the second fills the text.
	if (sysctl(request, 4, nullptr, &size, nullptr, 0) != 0) {
		return(std::vector<std::string>());
	}
	std::string block(size, '\0');
	if (sysctl(request, 4, block.data(), &size, nullptr, 0) != 0) {
		return(std::vector<std::string>());
	}
	block.resize(size);

	std::istringstream text(block);
	return(Read_Arguments(text));
#else
#error "Debug_Command_Line_Arguments has no implementation for this system."
#endif
}
#endif
