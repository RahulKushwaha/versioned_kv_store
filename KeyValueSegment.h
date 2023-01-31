//
// Created by Rahul  Kushwaha on 1/21/23.
//

#ifndef KEYVALUESTOREWITHSTABLEITEROR_KEYVALUESEGMENT_H
#define KEYVALUESTOREWITHSTABLEITEROR_KEYVALUESEGMENT_H

#include "Common.h"

namespace rk::projects::data_structures {

enum class ControlBits {
  ORDINARY_VALUE = 1,
  TOMBSTONE = 2,
};

struct InternalKey {
  std::string key_;
  SequenceType sequenceNum_;
  ControlBitsType controlBits_;
};

struct DataStoreKeyComparator {
  bool operator()(const InternalKey &x, const InternalKey &y) const {
    if (x.key_ == y.key_) {
      return x.sequenceNum_ < y.sequenceNum_;
    }

    return x.key_ < y.key_;
  }
};

using StoreType = std::map<InternalKey, Value, DataStoreKeyComparator>;

using ConstStoreTypeIterator = StoreType::const_iterator;

struct KeyValueSegment {
  Key minKey_;
  Key maxKey_;
  std::shared_ptr<StoreType> dataStore_;
  DeletionFilterType deletionFilter_;
};

class KeyValueSegmentIterator {
 public:
  explicit KeyValueSegmentIterator(const KeyValueSegment &keyValueSegment)
      : keyValueSegment_{std::ref(keyValueSegment)},
        itr_{keyValueSegment.dataStore_->cbegin()} {
  }

  bool hasNext() {
    if (!next_.has_value()) {
      populateNext({});
    }

    return *next_ != keyValueSegment_.get().dataStore_->cend();
  }

  ConstStoreTypeIterator next() {
    auto returnValue = itr_;
    populateNext(returnValue->first.key_);
    return returnValue;
  }

 private:
  void populateNext(std::string userKey) {
    auto key = std::move(userKey);

    while (itr_ != keyValueSegment_.get().dataStore_->cend()) {
      if (itr_->first.controlBits_
          == static_cast<ControlBitsType>(ControlBits::TOMBSTONE)) {
        key = itr_->first.key_;
        itr_++;
      } else if (key == itr_->first.key_) {
        itr_++;
      } else {
        break;
      }
    }

    next_ = {itr_};
  }

  std::reference_wrapper<const KeyValueSegment> keyValueSegment_;
  ConstStoreTypeIterator itr_;
  std::optional<ConstStoreTypeIterator> next_;
};

}

#endif //KEYVALUESTOREWITHSTABLEITEROR_KEYVALUESEGMENT_H
