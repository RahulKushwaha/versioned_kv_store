//
// Created by Rahul  Kushwaha on 1/30/23.
//
#include "OrderedMap.h"

namespace rk::projects::data_structures {
OrderedMap::OrderedMap() :
    seq_{0},
    currentDataStore_{std::make_shared<StoreType>()} {}

std::optional<std::string> OrderedMap::get(Key key) {
  std::vector<KeyValueSegment> metadataList;
  ValueResult valueResult;

  {
    std::shared_lock sharedLock{mtx_};
    if (!currentDataStore_->empty()
        && key >= currentDataStore_->cbegin()->first.key_
        && key <= currentDataStore_->crbegin()->first.key_) {
      valueResult = getValue(key, *currentDataStore_);
      if (valueResult.valueResultType == ValueResultType::DELETED) {
        return {};
      }

      if (valueResult.valueResultType == ValueResultType::PRESENT) {
        return valueResult.rawValue;
      }
    }
  }

  // Not able to find the element in the current map.
  // Find it in the previous blocks.
  // Create copy to avoid lock contention.
  // Find easier way to copy(instead of copying objects).
  metadataList = keyValueSegmentStore_;

  for (auto itr = metadataList.crbegin(); itr != metadataList.crend();
       itr++) {
    const auto &metadata = *itr;

    if (key >= metadata.minKey_ && key <= metadata.maxKey_) {
      valueResult = getValue(key, *metadata.dataStore_);
      if (valueResult.valueResultType == ValueResultType::DELETED) {
        return {};
      }

      if (valueResult.valueResultType == ValueResultType::PRESENT) {
        return valueResult.rawValue;
      }
    }
  }

  return {};
}

void OrderedMap::add(Key k, Value v) {
  add(std::move(k), std::move(v), ControlBits::ORDINARY_VALUE);
}

void OrderedMap::erase(Key key) {
  add(std::move(key), {}, ControlBits::TOMBSTONE);
}

void OrderedMap::add(Key k, Value v, ControlBits controlBits) {
  std::unique_lock uniqueLock(mtx_);

  SequenceType sequenceNum = seq_++;
  auto key =
      InternalKey{std::move(k), sequenceNum,
                  static_cast<std::uint32_t>(controlBits)};

  currentDataStore_->emplace(key, std::move(v));

  if (currentDataStore_->size() > MAX_SEGMENT_SIZE) {
    KeyValueSegment metadata{currentDataStore_->begin()->first.key_, // MinKey
                             currentDataStore_->crbegin()->first.key_, // MaxKey
                             currentDataStore_,
                             std::move(deletionFilter_)};
    keyValueSegmentStore_.emplace_back(metadata);

    currentDataStore_ = std::make_shared<StoreType>();
    deletionFilter_ = DeletionFilterType{};
  }
}

ValueResult OrderedMap::getValue(const Key &key,
                                 const StoreType &store) {
  auto itr = store.upper_bound({key, MagicSequenceNumbers::MAX_SEQUENCE});
  itr--;

  if (itr->first.key_ == key
      && itr->first.controlBits_
          == static_cast<std::uint32_t>(ControlBits::TOMBSTONE)) {
    return ValueResult{"", ValueResultType::DELETED};
  }

  if (itr->first.key_ != key) {
    return ValueResult{"", ValueResultType::NOT_PRESENT};
  }

  return ValueResult{itr->second, ValueResultType::PRESENT};
}

void OrderedMap::flushCurrentDataStore() {
  std::unique_lock uniqueLock{mtx_};
  KeyValueSegment metadata{currentDataStore_->begin()->first.key_, // MinKey
                           currentDataStore_->crbegin()->first.key_, // MaxKey
                           currentDataStore_,
                           std::move(deletionFilter_)};
  keyValueSegmentStore_.emplace_back(metadata);

  currentDataStore_ = std::make_shared<StoreType>();
  deletionFilter_ = DeletionFilterType{};
}

SequenceType OrderedMap::getCurrentSequenceNum() {
  std::shared_lock sharedLock{mtx_};
  return seq_;
}

StoreType OrderedMap::getCurrentStore() {
  return *currentDataStore_;
}

}