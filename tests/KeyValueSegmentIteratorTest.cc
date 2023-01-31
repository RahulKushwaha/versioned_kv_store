//
// Created by Rahul  Kushwaha on 1/21/23.
//
#include <gtest/gtest.h>
#include "../OrderedMap.h"

namespace rk::projects::data_structures {

TEST(KeyValueSegmentIteratorTests, addMultipleSameKeyValueAndIterate) {
  OrderedMap map;
  std::string key{"Hello"};
  std::string valuePrefix{"value"};

  std::int32_t index = 0;
  std::int32_t limit = 1000;
  for (; index <= limit; index++) {
    map.add(key, valuePrefix + std::to_string(index));
  }

  auto dataStore = std::make_shared<StoreType>(map.getCurrentStore());
  KeyValueSegment
      keyValueSegment
      {dataStore->cbegin()->first.key_, dataStore->crbegin()->first.key_,
       dataStore, DeletionFilterType{}};

  KeyValueSegmentIterator keyValueSegmentIterator{keyValueSegment};

  std::vector<Key> keys;
  while (keyValueSegmentIterator.hasNext()) {
    keys.emplace_back(keyValueSegmentIterator.next()->first.key_);
  }

  ASSERT_EQ(keys.size(), 1);
}

TEST(KeyValueSegmentIteratorTests, addMultipleDIffKeyValueAndIterate) {
  OrderedMap map;
  std::string key{"Hello"};
  std::string valuePrefix{"value"};

  std::int32_t index = 1;
  std::int32_t limit = 5;
  for (; index <= limit; index++) {
    map.add(key + std::to_string(index), valuePrefix + std::to_string(index));
  }

  auto dataStore = std::make_shared<StoreType>(map.getCurrentStore());
  KeyValueSegment
      keyValueSegment
      {dataStore->cbegin()->first.key_, dataStore->crbegin()->first.key_,
       dataStore, DeletionFilterType{}};

  KeyValueSegmentIterator keyValueSegmentIterator{keyValueSegment};

  std::vector<Key> keys;
  while (keyValueSegmentIterator.hasNext()) {
    keys.emplace_back(keyValueSegmentIterator.next()->first.key_);
  }

  ASSERT_EQ(keys.size(), limit);
}

}