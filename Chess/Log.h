#pragma once
// Create a temporary console
enum LOG_TYPE { LT_NORMAL = 0, LT_ERROR = 1 };

namespace g {
class Logger {
 public:
  static void init();
  static void deinit();
  // static Logger& GetNormalLogger();
  // static Logger& GetErrorLogger();

  Logger(LOG_TYPE);
};

}  // namespace g