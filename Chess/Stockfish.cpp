#include "Stockfish.h"

#include <regex>
#include <sstream>

Stockfish::Stockfish() {
  SECURITY_ATTRIBUTES attrib;
  attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
  attrib.bInheritHandle = true;
  attrib.lpSecurityDescriptor = NULL;

  if (!CreatePipe(&inPipeR, &inPipeW, &attrib, 0)) {
    printf("CreatePipe failed for Stockfish input (%d).\n", GetLastError());
    exit(1);
  }

  if (!CreatePipe(&outPipeR, &outPipeW, &attrib, 0)) {
    printf("CreatePipe failed for Stockfish output (%d).\n", GetLastError());
    exit(1);
  }

  STARTUPINFO si = {0};
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdInput = inPipeR;
  si.hStdOutput = outPipeW;
  si.hStdError = outPipeW;

  if (!CreateProcess(L"stockfish-windows-x86-64.exe", NULL, NULL, NULL, TRUE, 0,
                     0, NULL, &si, &processInfo)) {
    printf("CreateProcess failed for Stockfish (%d).\n", GetLastError());
    exit(1);
  }

  // Wait for engine start-up message, start UCI and wait for ready
  Read(true);
  Write("uci\n");
  WaitForReady();
  Write("ucinewgame\n");
  WaitForReady();

  printf("Stockfish successfully initialized and connected.\n");
}

Stockfish::~Stockfish() {
  Write("quit\n");

  CloseHandle(outPipeW);
  CloseHandle(outPipeR);
  CloseHandle(inPipeW);
  CloseHandle(inPipeR);

  WaitForSingleObject(processInfo.hProcess, INFINITE);
  CloseHandle(processInfo.hProcess);
  CloseHandle(processInfo.hThread);
}

// Reads output from stockfish, guarantees non-partial output lines.
// Does not necessarily read all data in pipe.
// It is up to caller to continue reading until the desired output is found.
bool Stockfish::Read(bool waitForResponse) {

  constexpr unsigned MAX_BUFF = 1024;
  unsigned long bytesRead = 0;
  char buff[MAX_BUFF + 1] = {0};

  if (waitForResponse) {
    // waitForResponse means we must guarantee something is read.
    if (!ReadFile(outPipeR, buff, MAX_BUFF, &bytesRead, NULL)) {
      printf("ReadFile failed (%d).", GetLastError());
      exit(1);
    }
    outputStream << buff;
  }

  if (waitForResponse && buff[bytesRead - 1] == '\n') // stop read if newline.
    return true;

  // Check if there's data in pipe. If so, read the rest of it.
  unsigned long bytesLeft = 0;
  if (!PeekNamedPipe(outPipeR, NULL, 0, NULL, &bytesLeft, NULL)) {
    printf("PeekNamedPipe failed (%d).\n", GetLastError());
    exit(1);
  }

  if (!waitForResponse && bytesLeft == 0)
    return false;

  // There's data to read.
  do {
    if (!ReadFile(outPipeR, buff, MAX_BUFF, &bytesRead, NULL)) {
      printf("ReadFile failed (%d).", GetLastError());
      exit(1);
    }

    // Add null character to end so output stream knows where string stops.
    buff[bytesRead] = '\0';
    outputStream << buff;
  } while (buff[bytesRead - 1] != '\n');

  return true;
}

void Stockfish::WaitForReady() {
  Write("isready\n");

  // Blocks until stockfish engine is ready.
  std::string currLine;
  while (true) {
    Read(true);
    while (std::getline(outputStream, currLine)) {
      if (currLine.substr(0, 7) == "readyok")
        return;
    }
  }
}

std::string Stockfish::GetMove() {

  Write(position + "\n");
  Write("go movetime 10\n");
  std::string currLine;
  std::regex reg = std::regex("bestmove\\s(\\S+)\\s");
  std::smatch matches;

  while (true) {
    while (!outputStream.eof()) {

      std::getline(outputStream, currLine);
      std::regex_search(currLine, matches, reg);
      if (matches.size() > 0) {
        std::string move = *(++matches.begin());
        position += " " + move;
        return move;
      }
    }

    outputStream.clear();
    Read(true);
  }
}

void Stockfish::PlayMove(const std::string &move) { position += " " + move; }

unsigned long Stockfish::Write(const std::string &message) {
  unsigned long written = 0;
  WriteFile(inPipeW, message.c_str(), message.length(), &written, NULL);
  return written;
}

void Stockfish::PrintCurrentOutput() {
  while (Read(false)) {
  }

  std::string currLine;
  while (std::getline(outputStream, currLine)) {
    printf("%s\n", currLine.c_str());
  }

  outputStream.clear();
}