//
// Created by Rahul  Kushwaha on 1/21/23.
//

#ifndef KEYVALUESTOREWITHSTABLEITEROR_COMMON_H
#define KEYVALUESTOREWITHSTABLEITEROR_COMMON_H

#include <string>
#include <cstdint>
#include <map>
#include <vector>

namespace rk::projects::data_structures {

using Key = std::string;
using Value = std::string;
using SequenceType = std::uint64_t;
using ControlBitsType = std::uint32_t;
using DeletionFilterType = std::vector<std::pair<Key, Key>>;

enum class ValueResultType {
  NOT_PRESENT,
  PRESENT,
  DELETED
};

struct ValueResult {
  std::string rawValue;
  ValueResultType valueResultType;
};

}
#endif //KEYVALUESTOREWITHSTABLEITEROR_COMMON_H
