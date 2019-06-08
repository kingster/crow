#include "gtest/gtest.h"

#include "crow/query_string.h"
using namespace crow;

#include <string>

TEST(query_string, instantiate) {
  query_string qs;
  query_string qs2("url");
}

TEST(query_string, get) {
  query_string qs("?x=1&y=hello");
  EXPECT_EQ(qs.get("x"), std::string("1"));
  EXPECT_EQ(qs.get("y"), std::string("hello"));
  EXPECT_EQ(qs.get("z"), nullptr);
}

TEST(query_string, clear) {
  query_string qs("?x=1");
  EXPECT_EQ(qs.get("x"), std::string("1"));
  qs.clear();
  EXPECT_EQ(qs.get("x"), nullptr);
}

TEST(query_string, get_list) {
  query_string qs("?x=1&y[]=1&y[]=2&y[]=3");
  EXPECT_EQ(qs.get_list("x").size(), 0);

  const auto y = qs.get_list("y");
  ASSERT_EQ(y.size(), 3);
  EXPECT_EQ(y[0], std::string("1"));
  EXPECT_EQ(y[1], std::string("2"));
  EXPECT_EQ(y[2], std::string("3"));
}

TEST(query_string, get_dict) {
  query_string qs("?x=1&y[1]=1&y[2]=2&y[3]=3");
  const auto x = qs.get_dict("x");
  ASSERT_EQ(x.size(), 1);
  EXPECT_EQ(x.begin()->second, std::string("1")); // Empty key so lookup first.

  const auto y = qs.get_dict("y");
  ASSERT_EQ(y.size(), 3);
  for (int i = 1; i <= 3; ++i) {
    const auto key = std::to_string(i);
    const auto it = y.find(key);
    ASSERT_NE(it, y.cend());
    EXPECT_EQ(it->second, key);
  }
}

TEST(query_string, copyCtor) {
  query_string qs("?x=1");
  query_string qs2(qs);
  ASSERT_EQ(qs2.get("x"), std::string("1"));
}

TEST(query_string, assignmentOperator) {
  query_string qs;
  ASSERT_EQ(qs.get("x"), nullptr);

  query_string qs2("?x=1");
  qs = qs2;
  ASSERT_EQ(qs.get("x"), std::string("1"));
  ASSERT_EQ(qs2.get("x"), std::string("1"));
}

TEST(query_string, assignmentOperatorMove) {
  query_string qs;
  ASSERT_EQ(qs.get("x"), nullptr);

  query_string qs2("?x=1");
  qs = std::move(qs2);
  ASSERT_EQ(qs.get("x"), std::string("1"));
  ASSERT_EQ(qs2.get("x"), nullptr);
}
