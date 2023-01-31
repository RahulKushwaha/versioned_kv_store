//
// Created by Rahul  Kushwaha on 1/20/23.
//

#ifndef KEYVALUESTOREWITHSTABLEITEROR_ORDEREDMAP_H
#define KEYVALUESTOREWITHSTABLEITEROR_ORDEREDMAP_H

#include <atomic>
#include <cstdint>
#include <map>
#include <set>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "Common.h"
#include "KeyValueSegment.h"
#include "KeyValueIterator.h"

namespace rk::projects::data_structures {

class OrderedMap {
 private:
  static constexpr std::size_t MAX_SEGMENT_SIZE = 5;

  struct MagicSequenceNumbers {
    static constexpr SequenceType
        MAX_SEQUENCE = std::numeric_limits<SequenceType>::max();
  };

 public:
  explicit OrderedMap();

  // Extract the following methods as public interface.
  void add(Key k, Value v);
  void erase(Key key);
  std::optional<std::string> get(Key key);
  KeyValueIterator getIterator();

  void flushCurrentDataStore();
  SequenceType getCurrentSequenceNum();
  StoreType getCurrentStore();

 private:
  void add(Key k, Value v, ControlBits controlBits);
  ValueResult getValue(const Key &key, const StoreType &store);

 private:
  SequenceType seq_;
  std::vector<KeyValueSegment> keyValueSegmentStore_;
  std::shared_ptr<StoreType> currentDataStore_;
  DeletionFilterType deletionFilter_;

  std::shared_mutex mtx_;
};

}

#endif //KEYVALUESTOREWITHSTABLEITEROR_ORDEREDMAP_H
