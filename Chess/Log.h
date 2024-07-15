#pragma once
// Create a temporary console
enum LOG_TYPE { LT_NORMAL = 0, LT_ERROR = 1 };

namespace g {
class Logger {
 public:
  static void Init();
  static void Deinit();

  Logger(LOG_TYPE);
};

}  // namespace g