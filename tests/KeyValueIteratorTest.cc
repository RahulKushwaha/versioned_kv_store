//
// Created by Rahul  Kushwaha on 1/30/23.
//
#include <gtest/gtest.h>
#include <random>
#include <vector>
#include "../OrderedMap.h"

namespace rk::projects::data_structures {
namespace {
struct KeyValue {
  std::string key;
  std::string value;
};

std::vector<KeyValue> generateRandomInput(std::int32_t numberOfElements,
                                          std::int32_t minKey = 1,
                                          std::int32_t maxKey = 1000) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type>
      keyDist(minKey, maxKey);

  std::vector<KeyValue> result;

  for (auto iteration = 0; iteration < numberOfElements; iteration++) {
    auto nextKey = keyDist(rng);
    result.emplace_back(KeyValue{std::to_string(nextKey),
                                 std::to_string(nextKey)});
  }

  return result;
}

}

TEST(KeyValueIteratorTest, IterateItemsInOrder) {
  auto keyValues = generateRandomInput(1000, 1, 10);

  OrderedMap orderedMap;
  for (auto &[k, v]: keyValues) {
    orderedMap.add(k, v);
  }


}

}