//
// Created by Rahul  Kushwaha on 1/30/23.
//

#ifndef KEYVALUESTOREWITHSTABLEITEROR_KEYVALUEITERATOR_H
#define KEYVALUESTOREWITHSTABLEITEROR_KEYVALUEITERATOR_H

#include <queue>
#include "KeyValueSegment.h"

namespace rk::projects::data_structures {

class KeyValueIterator {
 private:
  struct KeyValueSegmentIteratorWithValue;

 public:
  explicit KeyValueIterator(std::vector<KeyValueSegmentIterator> itrs) {
    for (auto &itr: itrs) {
      if (itr.hasNext()) {
        pq_.push(KeyValueSegmentIteratorWithValue{itr, itr.next()});
      }
    }
  }

 public:
  bool hasNext() {
    return !pq_.empty();
  }

  std::optional<std::string> next() {
    auto top = pq_.top();
    populateNext(top.valueItr->first);
    return {top.valueItr->second};
  }

 private:

  void populateNext(const InternalKey &key) {
    auto &currentKey = key.key_;
    while (!pq_.empty()) {
      if (pq_.top().valueItr->first.key_ < currentKey) {
        break;
      }

      auto top = pq_.top();
      pq_.pop();
      if (top.itr.hasNext()) {
        pq_.push({top.itr, top.itr.next()});
      }
    }
  }

  struct Comparator {
    bool operator()(const KeyValueSegmentIteratorWithValue &itr1,
                    const KeyValueSegmentIteratorWithValue &itr2) const noexcept {
      auto &key1 = itr1.valueItr->first;
      auto &key2 = itr2.valueItr->first;
      if (key1.key_ == key2.key_) {
        return key1.sequenceNum_ > key2.sequenceNum_;
      }

      return key1.key_ > key2.key_;
    }
  };

 private:

  std::priority_queue<KeyValueSegmentIteratorWithValue,
                      std::vector<KeyValueSegmentIteratorWithValue>,
                      Comparator> pq_;

 private:
  struct KeyValueSegmentIteratorWithValue {
    KeyValueSegmentIterator itr;
    ConstStoreTypeIterator valueItr;
  };
};

}


#endif //KEYVALUESTOREWITHSTABLEITEROR_KEYVALUEITERATOR_H
