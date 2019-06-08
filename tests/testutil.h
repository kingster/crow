#ifndef TESTS_TESTUTIL_H
#define TESTS_TESTUTIL_H

#define EXPECT_RUNTIME_EXCEPT(func)                                            \
  {                                                                            \
    bool ret = false;                                                          \
    try {                                                                      \
      func();                                                                  \
    }                                                                          \
    catch (std::runtime_error) {                                               \
      ret = true;                                                              \
    }                                                                          \
    EXPECT_TRUE(ret) << "Didn't yield runtime exception!";                     \
  }

#endif // TESTS_TESTUTIL_H
