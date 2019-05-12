#include "gtest/gtest.h"
#include "testutil.h"

#include "crow/utility.h"
using namespace crow;
using namespace crow::black_magic;
using namespace crow::utility;

#include <string>
#include <tuple>

#ifndef CROW_MSVC_WORKAROUND

TEST(black_magic, find_closing_tag) {
  EXPECT_EQ(find_closing_tag("<hello>", 0), 6);
  EXPECT_EQ(find_closing_tag("<ok></ok>", 5), 8);
}

TEST(black_magic, is_valid) {
  EXPECT_FALSE(is_valid("<invalid"));
  EXPECT_FALSE(is_valid("invalid>"));
  EXPECT_TRUE(is_valid("<ok>"));
  EXPECT_FALSE(is_valid("<ok></invalid"));
  EXPECT_TRUE(is_valid("<ok></ok>"));
  EXPECT_TRUE(is_valid("<one><two><three>"));
}

TEST(black_magic, is_equ_p) {
  EXPECT_TRUE(is_equ_p("test", "test", 4));
  EXPECT_TRUE(is_equ_p("test", "test", 2));
  EXPECT_FALSE(is_equ_p("test", "teST", 4));
  EXPECT_TRUE(is_equ_p("", "", 0));
}

TEST(black_magic, is_equ_n) {
  EXPECT_TRUE(is_equ_n("test", 0, "test", 0, 4));
  EXPECT_TRUE(is_equ_n("test", 2, "TEst", 2, 2));
  EXPECT_FALSE(is_equ_n("test", 2, "test", 2, 3)); // 2+3=5>4
  EXPECT_FALSE(is_equ_n("TesT", 2, "TEst", 2, 2));
}

TEST(black_magic, is_int) {
  EXPECT_TRUE(is_int("<int>", 0));
  EXPECT_TRUE(is_int("  <int>", 2));
  EXPECT_FALSE(is_int("<INT>", 0));
}

TEST(black_magic, is_uint) {
  EXPECT_TRUE(is_uint("<uint>", 0));
  EXPECT_TRUE(is_uint("  <uint>", 2));
  EXPECT_FALSE(is_uint("<UINT>", 0));
}

TEST(black_magic, is_float) {
  EXPECT_TRUE(is_float("<float>", 0));
  EXPECT_TRUE(is_float("  <float>", 2));
  EXPECT_FALSE(is_float("<FLOAT>", 0));
  EXPECT_TRUE(is_float("<double>", 0));
  EXPECT_TRUE(is_float("  <double>", 2));
  EXPECT_FALSE(is_float("<DOUBLE>", 0));
}

TEST(black_magic, is_str) {
  EXPECT_TRUE(is_str("<str>", 0));
  EXPECT_TRUE(is_str("  <str>", 2));
  EXPECT_FALSE(is_str("<STR>", 0));
  EXPECT_TRUE(is_str("<string>", 0));
  EXPECT_TRUE(is_str("  <string>", 2));
  EXPECT_FALSE(is_str("<STRING>", 0));
}

TEST(black_magic, is_path) {
  EXPECT_TRUE(is_path("<path>", 0));
  EXPECT_TRUE(is_path("  <path>", 2));
  EXPECT_FALSE(is_path("<PATH>", 0));
}

#endif // CROW_MSVC_WORKAROUND

TEST(black_magic, is_parameter_tag_compatible) {
  EXPECT_TRUE(is_parameter_tag_compatible(0, 0));
  EXPECT_FALSE(is_parameter_tag_compatible(1, 0));
  EXPECT_FALSE(is_parameter_tag_compatible(0, 1));
  EXPECT_TRUE(is_parameter_tag_compatible(12, 6));
  EXPECT_TRUE(is_parameter_tag_compatible(6, 12));
}

TEST(black_magic, find_closing_tag_runtime) {
  EXPECT_EQ(find_closing_tag_runtime("<ok></ok>", 1), 3); // starting <
  EXPECT_EQ(find_closing_tag_runtime("<ok></ok>", 2), 3); // ..
  EXPECT_EQ(find_closing_tag_runtime("<ok></ok>", 3), 3); // ..
  EXPECT_EQ(find_closing_tag_runtime("<ok></ok>", 4), 8); // ending >
  EXPECT_RUNTIME_EXCEPT([] { find_closing_tag_runtime("<ok", 0); });
}

TEST(black_magic, get_parameter_tag_runtime) {
  EXPECT_EQ(get_parameter_tag_runtime(""), 0);
  EXPECT_EQ(get_parameter_tag_runtime("<int>"), 1);
  EXPECT_EQ(get_parameter_tag_runtime("<uint>"), 2);
  EXPECT_EQ(get_parameter_tag_runtime("<float>"), 3);
  EXPECT_EQ(get_parameter_tag_runtime("<double>"), 3);
  EXPECT_EQ(get_parameter_tag_runtime("<str>"), 4);
  EXPECT_EQ(get_parameter_tag_runtime("<string>"), 4);
  EXPECT_EQ(get_parameter_tag_runtime("<path>"), 5);
  EXPECT_RUNTIME_EXCEPT([] { get_parameter_tag_runtime("<unknown>", 0); });
}

#ifndef CROW_MSVC_WORKAROUND

TEST(black_magic, get_parameter_tag) {
  EXPECT_EQ(get_parameter_tag(""), 0);
  EXPECT_EQ(get_parameter_tag("<int>"), 1);
  EXPECT_EQ(get_parameter_tag("<uint>"), 2);
  EXPECT_EQ(get_parameter_tag("<float>"), 3);
  EXPECT_EQ(get_parameter_tag("<double>"), 3);
  EXPECT_EQ(get_parameter_tag("<str>"), 4);
  EXPECT_EQ(get_parameter_tag("<string>"), 4);
  EXPECT_EQ(get_parameter_tag("<path>"), 5);
  EXPECT_RUNTIME_EXCEPT([] { get_parameter_tag("<unknown>", 0); });
}

#endif // CROW_MSVC_WORKAROUND

TEST(utility, get_element_by_type) {
  auto tup = std::make_tuple<float, int, std::string>(1.2f, 42, "Hello, World!");
  EXPECT_EQ(get_element_by_type<int>(tup), 42);
  EXPECT_FLOAT_EQ(get_element_by_type<float>(tup), 1.2f);
  EXPECT_EQ(get_element_by_type<std::string>(tup), "Hello, World!");
}

TEST(utility, base64encode) {
  EXPECT_EQ(base64encode("hello", 5), "aGVsbG8=");
  EXPECT_EQ(base64encode("testing", 7), "dGVzdGluZw==");
  EXPECT_EQ(base64encode("Hello, World!", 13), "SGVsbG8sIFdvcmxkIQ==");
}

TEST(utility, base64encode_urlsafe) {
  EXPECT_EQ(base64encode_urlsafe("hello", 5), base64encode("hello", 5));
  EXPECT_EQ(base64encode_urlsafe("testing", 7), base64encode("testing", 7));
  EXPECT_EQ(base64encode_urlsafe("Hello, World!", 13), base64encode("Hello, World!", 13));

  // '/' is '_' in the URL-safe variant.
  const std::string s("æøåLL§§£¡±¡@~~¡?±");
  EXPECT_EQ(base64encode(s.c_str(), s.size()), "w6bDuMOlTEzCp8KnwqPCocKxwqFAfn7CoT/CsQ==");
  EXPECT_EQ(base64encode_urlsafe(s.c_str(), s.size()), "w6bDuMOlTEzCp8KnwqPCocKxwqFAfn7CoT_CsQ==");
}
