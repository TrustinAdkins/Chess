#include "Log.h"

#include <Windows.h>

#include <iostream>

namespace g {
Logger* normalLogger = 0;
Logger* errorLogger = 0;

HANDLE conInHandle = INVALID_HANDLE_VALUE;
HANDLE conOutHandle = INVALID_HANDLE_VALUE;

FILE* c_con_in_f = 0;
FILE* c_con_out_f = 0;
FILE* c_con_err_f = 0;

void Logger::Init() {
  AllocConsole();
  AttachConsole(GetCurrentProcessId());

  conInHandle =
      CreateFileA("CONIN$", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
                  OPEN_EXISTING, 0, NULL);
  conOutHandle =
      CreateFileA("CONOUT$", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                  0, OPEN_EXISTING, 0, NULL);

  SetStdHandle(STD_INPUT_HANDLE, conInHandle);
  SetStdHandle(STD_OUTPUT_HANDLE, conOutHandle);
  SetStdHandle(STD_ERROR_HANDLE, conOutHandle);

  freopen_s(&c_con_in_f, "CONIN$", "r", stdin);
  freopen_s(&c_con_out_f, "CONOUT$", "w", stdout);
  freopen_s(&c_con_err_f, "CONOUT$", "w", stderr);

  normalLogger = new Logger(LT_NORMAL);
  errorLogger = new Logger(LT_ERROR);
}

void Logger::Deinit() {
  delete normalLogger;
  delete errorLogger;

  CloseHandle(conInHandle);
  CloseHandle(conOutHandle);
  FreeConsole();
}

Logger::Logger(LOG_TYPE t) {}

}  // namespace g
