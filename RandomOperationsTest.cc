//
// Created by Rahul  Kushwaha on 1/28/23.
//

#include <gtest/gtest.h>
#include <string>
#include <random>
#include <vector>
#include "OrderedMap.h"

namespace rk::projects::data_structures {

namespace {
enum class Operation {
  Get,
  Put,
  Delete
};

struct Input {
  Operation operation;
  std::string key;
  std::string value;
};

struct Output {
  std::string key;
  std::string value;

  bool operator==(const Output &other) const noexcept {
    return key == other.key && value == other.value;
  }
};

std::vector<Input> getRandomInput(std::int32_t inputSize = 1000) {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type>
      opDist(1, 3);
  std::uniform_int_distribution<std::mt19937::result_type>
      keyDist(1, inputSize / 10);

  std::vector<Input> input;
  for (std::int32_t iteration = 0; iteration < inputSize; iteration++) {
    auto op = opDist(rng);
    std::string key = std::string{std::to_string(keyDist(rng))};
    switch (op) {
      case 1:
        input.emplace_back(Input{Operation::Get, key, key});
        break;

      case 2:
        input.emplace_back(Input{Operation::Delete, key, key});
        break;

      case 3:
        input.emplace_back(Input{Operation::Put, key, key});
        break;
      default:
        throw std::runtime_error{"Unknown Operation Type"};
    }
  }

  return input;
}

}

TEST(OrderedMapRandomOperationsTest, TestRandomInput) {
  auto randomInput = getRandomInput(10000);
  std::vector<Output> outputUsingSTDMap;

  std::map<std::string, std::string> stdMap;
  for (auto &input: randomInput) {
    switch (input.operation) {
      case Operation::Get:
        outputUsingSTDMap.emplace_back(Output{input.key, stdMap[input.key]});
        break;

      case Operation::Put:
        stdMap[input.key] = input.value;
        outputUsingSTDMap.emplace_back(Output{input.key, stdMap[input.key]});
        break;

      case Operation::Delete:
        stdMap.erase(input.key);
        outputUsingSTDMap.emplace_back(Output{input.key, stdMap[input.key]});
    }
  }

  std::vector<Output> outputUsingCustomMap;
  OrderedMap orderedMap;
  for (auto &input: randomInput) {
    switch (input.operation) {
      case Operation::Get:
        outputUsingCustomMap
            .emplace_back(
                Output{input.key, orderedMap.get(input.key).value_or("")});
        break;

      case Operation::Put:
        orderedMap.add(input.key, input.value);
        outputUsingCustomMap
            .emplace_back(
                Output{input.key, orderedMap.get(input.key).value_or("")});
        break;

      case Operation::Delete:
        orderedMap.erase(input.key);
        outputUsingCustomMap
            .emplace_back(
                Output{input.key, orderedMap.get(input.key).value_or("")});
    }
  }

  ASSERT_EQ(outputUsingCustomMap.size(), outputUsingSTDMap.size());

  for (std::size_t index = 0; index < outputUsingSTDMap.size(); index++) {
    ASSERT_EQ(outputUsingSTDMap[index].key, outputUsingCustomMap[index].key);
    ASSERT_EQ(outputUsingSTDMap[index].value,
              outputUsingCustomMap[index].value)
                  << outputUsingSTDMap[index].key;

  }

}

}