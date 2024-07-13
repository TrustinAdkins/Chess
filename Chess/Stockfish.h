#pragma once

#include <Windows.h>

#include <sstream>
#include <string>

class Stockfish {
 public:
  Stockfish();
  ~Stockfish();

  std::string GetMove();
  void PlayMove(const std::string &move);
  void PrintCurrentOutput();
  std::string position = "position startpos moves";

 private:
  void WaitForReady();
  bool Read(bool waitForResponse);
  unsigned long Write(const std::string &message);

  HANDLE inPipeW = 0, inPipeR = 0;
  HANDLE outPipeW = 0, outPipeR = 0;
  PROCESS_INFORMATION processInfo = {0};
  std::stringstream outputStream;
};