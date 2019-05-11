#include "gtest/gtest.h"

#include "crow/ci_map.h"
using namespace crow;

TEST(ci_map, instantiate) {
  ci_map map;
}

TEST(ci_map, multiItems) {
  ci_map map;
  EXPECT_EQ(map.size(), 0);

  map.emplace("a", "b");
  EXPECT_EQ(map.size(), 1);

  map.emplace("a", "c");
  EXPECT_EQ(map.size(), 2);
  EXPECT_EQ(map.count("a"), 2);

  // First item.
  auto it = map.find("a");
  ASSERT_NE(it, map.end());
  EXPECT_EQ(it->second, "b");

  // All items.
  const auto range = map.equal_range("a");
  int i = 0;
  for (auto it = range.first; it != range.second; ++it, ++i) {
    if (i == 0) {
      EXPECT_EQ(it->second, "b");
    }
    else if (i == 1) {
      EXPECT_EQ(it->second, "c");
    }
  }
  EXPECT_EQ(i, 2);
}

TEST(ci_map, clear) {
  ci_map map;
  map.emplace("a", "b");
  EXPECT_EQ(map.size(), 1);
  map.clear();
  EXPECT_EQ(map.size(), 0);
}
