#include "gtest/gtest.h"

#include "crow/logging.h"
using namespace crow;

#ifdef CROW_ENABLE_LOGGING

#include <string>
#include <vector>

class LogHandler : public ILogHandler {
public:
  void log(const std::string &message, LogLevel /*level*/) override {
    messages.push_back(message);
  }

  std::vector<std::string> messages;
};

class LoggerTest : public ::testing::Test {
protected:
  LoggerTest() {}

  void SetUp() {
    logger::setHandler(&handler);
  }

  void TearDown() {
    logger::setHandler(nullptr);
  }

public:
  void checkMessage(const std::string &value, int i = 0) {
    ASSERT_EQ(handler.messages.size(), i+1);

    // Ends with value + '\n'.
    const auto logText = handler.messages[i];
    EXPECT_EQ(logText.substr(logText.size() - value.size() - 1, value.size()), value);
  }

  LogHandler handler;
};

TEST(logger, instantiate) {
  logger l("testing", LogLevel::Debug);
}

TEST_F(LoggerTest, logValues) {
  const auto level = LogLevel::Debug;
  logger::setLogLevel(level);

  const std::string prefix("testing");
  const std::string value("test");
  {
    logger l(prefix, level);
    l << value;
  } // Log message written to handler here.

  checkMessage(value);

  const std::string value2("more testing");
  {
    logger l(prefix, level);
    l << value2;
  }

  checkMessage(value2, 1);
}

TEST_F(LoggerTest, logCritical) {
  const std::string value("critical");
  CROW_LOG_CRITICAL << value;
  checkMessage(value);
}

TEST_F(LoggerTest, logError) {
  const std::string value("error");

  logger::setLogLevel(LogLevel::Error);
  CROW_LOG_ERROR << value;
  checkMessage(value);
  ASSERT_EQ(handler.messages.size(), 1);

  // Make sure the log level is adhered to: ignoring error when critical is minimum level.
  logger::setLogLevel(LogLevel::Critical);
  CROW_LOG_ERROR << value;
  ASSERT_EQ(handler.messages.size(), 1);
}

TEST_F(LoggerTest, logWarning) {
  const std::string value("warning");

  logger::setLogLevel(LogLevel::Warning);
  CROW_LOG_WARNING << value;
  checkMessage(value);
  ASSERT_EQ(handler.messages.size(), 1);

  // Make sure the log level is adhered to.
  logger::setLogLevel(LogLevel::Error);
  CROW_LOG_WARNING << value;
  ASSERT_EQ(handler.messages.size(), 1);
}

TEST_F(LoggerTest, logInfo) {
  const std::string value("info");

  logger::setLogLevel(LogLevel::Info);
  CROW_LOG_INFO << value;
  checkMessage(value);
  ASSERT_EQ(handler.messages.size(), 1);

  // Make sure the log level is adhered to.
  logger::setLogLevel(LogLevel::Warning);
  CROW_LOG_INFO << value;
  ASSERT_EQ(handler.messages.size(), 1);
}

TEST_F(LoggerTest, logDebug) {
  const std::string value("debug");

  logger::setLogLevel(LogLevel::Debug);
  CROW_LOG_DEBUG << value;
  checkMessage(value);
  ASSERT_EQ(handler.messages.size(), 1);

  // Make sure the log level is adhered to.
  logger::setLogLevel(LogLevel::Info);
  CROW_LOG_DEBUG << value;
  ASSERT_EQ(handler.messages.size(), 1);
}

#endif // CROW_ENABLE_LOGGING
