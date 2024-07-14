#include "Log.h"

#include <Windows.h>

#include <iostream>

namespace g {
Logger* normalLogger = 0;
Logger* errorLogger = 0;

HANDLE consoleInputHandle = INVALID_HANDLE_VALUE;
HANDLE outputHandle = INVALID_HANDLE_VALUE;

FILE* c_con_in_f = 0;
FILE* c_con_out_f = 0;
FILE* c_con_err_f = 0;

void Logger::init() {
  AllocConsole();
  AttachConsole(GetCurrentProcessId());

  consoleInputHandle =
      CreateFileA("CONIN$", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
                  OPEN_EXISTING, 0, NULL);
  outputHandle =
      CreateFileA("CONOUT$", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                  0, OPEN_EXISTING, 0, NULL);

  SetStdHandle(STD_INPUT_HANDLE, consoleInputHandle);
  SetStdHandle(STD_OUTPUT_HANDLE, outputHandle);
  SetStdHandle(STD_ERROR_HANDLE, outputHandle);

  freopen_s(&c_con_in_f, "CONIN$", "r", stdin);
  freopen_s(&c_con_out_f, "CONOUT$", "w", stdout);
  freopen_s(&c_con_err_f, "CONOUT$", "w", stderr);

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

Logger::Logger(LOG_TYPE t) {}

}  // namespace g
