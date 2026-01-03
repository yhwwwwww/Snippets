#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/core/LogLevel.h"
#include "quill/core/PatternFormatterOptions.h"
#include "quill/core/SinkManager.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/sinks/FileSink.h"

class Log {
public:
  static Log &instance() {
    static Log inst;
    return inst;
  }

  void set_log_level(std::string const &level) {
    root->set_log_level(quill::loglevel_from_string(level));
  }

  quill::Logger *getLogger() { return root; }

  void init(std::string const &filename, std::string const &format,
            std::string const &level) {
    auto sink = quill::Frontend::create_or_get_sink<quill::FileSink>(filename);
    root = quill::Frontend::create_or_get_logger(
        "root", sink, quill::PatternFormatterOptions{format});
    set_log_level(level);
  }

  void log_start() { quill::Backend::start(); }

private:
  quill::Logger *root = quill::Frontend::create_or_get_logger(
      "default",
      quill::Frontend::create_or_get_sink<quill::ConsoleSink>("AAA"));
};

#define MLOG_DEBUG(fmt, ...)                                                   \
  LOG_DEBUG(Log::instance().getLogger(), fmt, ##__VA_ARGS__)

#define MLOG_TRACE(fmt, ...)                                                   \
  LOG_TRACE_L1(Log::instance().getLogger(), fmt, ##__VA_ARGS__)

#define MLOG_INFO(fmt, ...)                                                    \
  LOG_INFO(Log::instance().getLogger(), fmt, ##__VA_ARGS__)

// Test:
int main() {
  Log::instance().set_log_level("debug");

  std::thread thread1([=]() {
    for (auto i = 0; i < 50; ++i) {
      MLOG_DEBUG("THRED1");
      MLOG_INFO("TTTREAD1 IS in {}", i);
    }
  });
  thread1.detach();

  std::thread thread2([=]() {
    for (auto i = 0; i < 50; ++i) {
      MLOG_DEBUG("THRED2");
      MLOG_INFO("TTTREAD2 IS in {}", i);
    }
  });
  thread2.detach();
  Log::instance().log_start();

  MLOG_INFO("info log2");
  Log::instance().init("folder/e.log", "%(message)", "debug");
  MLOG_DEBUG("debug log2");
  MLOG_DEBUG("debug 3log2");
  MLOG_INFO("info 3og3");

  return 0;
}
