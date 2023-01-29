//
// Created by Rahul  Kushwaha on 1/20/23.
//

#include <gtest/gtest.h>
#include "OrderedMap.h"

namespace rk::projects::data_structures {

TEST(OrderedMapTests, addElement) {
  OrderedMap map;
  map.add("Hello", "World");

  ASSERT_TRUE(map.get("Hello").has_value());
  ASSERT_EQ(map.get("Hello"), "World");
}

TEST(OrderedMapTests, deleteElement) {
  OrderedMap map;
  std::string key{"Hello"};
  map.add(key, "World");
  map.erase(key);

  ASSERT_FALSE(map.get(key).has_value());
}

TEST(OrderedMapTests, getOnEmptyMap) {
  OrderedMap map;
  std::string key{"Hello"};

  ASSERT_FALSE(map.get(key).has_value());
}

TEST(OrderedMapTests, addRemoveAddElement) {
  OrderedMap map;
  std::string key{"Hello"};
  std::string value{"World1"};
  map.add(key, "World");
  map.erase(key);
  map.add(key, value);

  ASSERT_TRUE(map.get(key).has_value());
  ASSERT_EQ(map.get(key).value(), value);
}

TEST(OrderedMapTests, addSameElementMultipleTimes) {
  OrderedMap map;
  std::string key{"Hello"};
  std::string valuePrefix{"value"};

  std::int32_t index = 0;
  std::int32_t limit = 1000;
  for (; index <= limit; index++) {
    map.add(key, valuePrefix + std::to_string(index));
  }

  ASSERT_TRUE(map.get("Hello").has_value());
  ASSERT_EQ(map.get("Hello").value(), valuePrefix + std::to_string(limit));
}

}
