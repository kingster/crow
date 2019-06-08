#include "gtest/gtest.h"
#include "testutil.h"

#include "crow/json.h"
using namespace crow;

template <typename I, typename K>
static void EXPECT_JSON_KEY_NOT_FOUND(const I &instance, const K &key) {
#ifndef CROW_JSON_NO_ERROR_CHECK
  EXPECT_RUNTIME_EXCEPT([&] { instance[key]; });
#else
  EXPECT_EQ(instance[key].t(), json::type::Null);
#endif
}

TEST(json, escape) {
  EXPECT_EQ(json::escape("\""), "\\\"");
  EXPECT_EQ(json::escape("\\"), "\\\\");
  EXPECT_EQ(json::escape("\n"), "\\n");
  EXPECT_EQ(json::escape("\b"), "\\b");
  EXPECT_EQ(json::escape("\f"), "\\f");
  EXPECT_EQ(json::escape("\r"), "\\r");
  EXPECT_EQ(json::escape("\t"), "\\t");
  EXPECT_EQ(json::escape("\x7"), "\\u0007");
  EXPECT_EQ(json::escape("\x19"), "\\u0019");
  EXPECT_EQ(json::escape("\"test\ting\"\n"), "\\\"test\\ting\\\"\\n");
}

TEST(json, get_type_str) {
  EXPECT_EQ(json::get_type_str(json::type::Number), "Number");
  EXPECT_EQ(json::get_type_str(json::type::False), "False");
  EXPECT_EQ(json::get_type_str(json::type::True), "True");
  EXPECT_EQ(json::get_type_str(json::type::List), "List");
  EXPECT_EQ(json::get_type_str(json::type::String), "String");
  EXPECT_EQ(json::get_type_str(json::type::Object), "Object");
  EXPECT_EQ(json::get_type_str(static_cast<json::type>(999)), "Unknown");
}

TEST(json, load) {
  const auto val = json::load("{\"one\": 2, \"three\": 4.2}");
  ASSERT_FALSE(val.error());

  const auto one = val["one"];
  ASSERT_FALSE(one.error());
  EXPECT_EQ(one.t(), json::type::Number);
  EXPECT_EQ(one.nt(), json::num_type::Unsigned_integer);
  EXPECT_EQ(one.i(), 2);

  const auto three = val["three"];
  ASSERT_FALSE(three.error());
  EXPECT_EQ(three.t(), json::type::Number);
  EXPECT_EQ(three.nt(), json::num_type::Floating_point);
  EXPECT_FLOAT_EQ(three.d(), 4.2);
}

TEST(json, keyNotFound) {
  const auto val = json::load("{}");
  ASSERT_FALSE(val.error());
  EXPECT_JSON_KEY_NOT_FOUND(val, "notfoundkey");
}

TEST(json, unsignedIntegerValue) {
  const auto val = json::load("{\"x\": 1}");
  ASSERT_FALSE(val.error());

  const auto x = val["x"];
  ASSERT_FALSE(x.error());
  EXPECT_EQ(x.t(), json::type::Number);
  EXPECT_EQ(x.nt(), json::num_type::Unsigned_integer);
  EXPECT_EQ(x.u(), 1);
  EXPECT_EQ(x.i(), 1);
}

TEST(json, signedIntegerValue) {
  const auto val = json::load("{\"x\": -1}");
  ASSERT_FALSE(val.error());

  const auto x = val["x"];
  ASSERT_FALSE(x.error());
  EXPECT_EQ(x.t(), json::type::Number);
  EXPECT_EQ(x.nt(), json::num_type::Signed_integer);
  EXPECT_EQ(x.i(), -1);
}

TEST(json, doubleValue) {
  const auto val = json::load("{\"x\": 4.2}");
  ASSERT_FALSE(val.error());

  const auto x = val["x"];
  ASSERT_FALSE(x.error());
  EXPECT_EQ(x.t(), json::type::Number);
  EXPECT_EQ(x.nt(), json::num_type::Floating_point);
  EXPECT_FLOAT_EQ(x.d(), 4.2);
}

TEST(json, boolValue) {
  auto val = json::load("{\"x\": true}");
  ASSERT_FALSE(val.error());

  auto x = val["x"];
  ASSERT_FALSE(x.error());
  EXPECT_EQ(x.t(), json::type::True);
  EXPECT_TRUE(x.b());

  val = json::load("{\"x\": false}");
  ASSERT_FALSE(val.error());

  x = val["x"];
  ASSERT_FALSE(x.error());
  EXPECT_EQ(x.t(), json::type::False);
  EXPECT_FALSE(x.b());
}

TEST(json, stringValue) {
  const auto val = json::load("{\"x\": \"42\"}");
  ASSERT_FALSE(val.error());

  const auto x = val["x"];
  ASSERT_FALSE(x.error());
  EXPECT_EQ(x.t(), json::type::String);
  EXPECT_EQ(x.s(), "42");
}

TEST(json, listValue) {
  const auto val = json::load("{\"x\": [1, 2, 3]}");
  ASSERT_FALSE(val.error());

  const auto x = val["x"];
  ASSERT_FALSE(x.error());
  EXPECT_EQ(x.t(), json::type::List);
  ASSERT_EQ(x.size(), 3);
  EXPECT_EQ(x[0].i(), 1);
  EXPECT_EQ(x[1].i(), 2);
  EXPECT_EQ(x[2].i(), 3);
}

TEST(json, has) {
  const auto val = json::load("{\"x\": 1}");
  ASSERT_FALSE(val.error());
  EXPECT_TRUE(val.has("x"));
  EXPECT_FALSE(val.has("y"));
}

TEST(json, count) {
  auto val = json::load("{\"x\": 1}");
  ASSERT_FALSE(val.error());
  EXPECT_EQ(val.count("x"), 1);
  EXPECT_EQ(val.count("y"), 0);
}

TEST(json, key) {
  const auto val = json::load("{\"x\": 1}");
  ASSERT_FALSE(val.error());
  const auto x = val["x"];
  EXPECT_EQ(x.key(), "x");
}

TEST(json, size) {
  const auto val = json::load("{\"x\": 1, \"y\": 2, \"z\": 3}");
  ASSERT_FALSE(val.error());
  EXPECT_EQ(val.size(), 3);
}

TEST(json, objectEnumeration) {
  const auto instance = json::load("{\"x\": 1, \"y\": 2, \"z\": 3}");
  ASSERT_FALSE(instance.error());

  int v = 1;
  for (const auto &val : instance) {
    EXPECT_EQ(val, v++);
  }
  EXPECT_EQ(v, instance.size() + 1);
}

TEST(json, listEnumeration) {
  const auto instance = json::load("[1, 2, 3]");
  ASSERT_FALSE(instance.error());

  int v = 1;
  for (const auto &val : instance) {
    EXPECT_EQ(val, v++);
  }
  EXPECT_EQ(v, instance.size() + 1);
}

TEST(json, nestedValues) {
  const auto instance = json::load("{\"x\": [[1, 2]], \"y\": {\"z\": 42}}");
  ASSERT_FALSE(instance.error());

  const auto x = instance["x"];
  EXPECT_EQ(x.t(), json::type::List);
  const auto xList = x[0];
  EXPECT_EQ(xList.t(), json::type::List);
  EXPECT_EQ(xList.size(), 2);
  EXPECT_EQ(xList[0].i(), 1);
  EXPECT_EQ(xList[1].i(), 2);

  const auto y = instance["y"];
  EXPECT_EQ(y.t(), json::type::Object);
  const auto z = y["z"];
  EXPECT_EQ(z.t(), json::type::Number);
  EXPECT_EQ(z.i(), 42);
}
