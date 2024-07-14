#include "Log.h"

#include <iostream>
#include <Windows.h>

namespace g {
	Logger* normalLogger = 0;
	Logger* errorLogger = 0;

	HANDLE consoleInputHandle = INVALID_HANDLE_VALUE;
	HANDLE outputHandle = INVALID_HANDLE_VALUE;

	void Logger::init() {
		AllocConsole();
		AttachConsole(GetCurrentProcessId());

		consoleInputHandle = CreateFileA("CONIN$", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0 , NULL);
		outputHandle = CreateFileA("CONOUT$", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0 , NULL);

		SetStdHandle(STD_INPUT_HANDLE, consoleInputHandle);
		SetStdHandle(STD_OUTPUT_HANDLE, outputHandle);
		SetStdHandle(STD_ERROR_HANDLE, outputHandle);

		normalLogger = new Logger(LT_NORMAL);
		errorLogger = new Logger(LT_ERROR);
	}

	void Logger::deinit() {
		delete normalLogger;
		delete errorLogger;

		CloseHandle(consoleInputHandle);
		CloseHandle(outputHandle);
		FreeConsole();
	}

	Logger::Logger(LOG_TYPE t) {

	}

}
