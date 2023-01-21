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

namespace rk::projects::data_structures {

class OrderedMap {
 private:

  struct DataStoreKeyComparator;
  struct InternalKey;

  using Key = std::string;
  using Value = std::string;
  using SequenceType = std::uint64_t;
  using ControlBitsType = std::uint32_t;
  using StoreType = std::map<InternalKey, Value, DataStoreKeyComparator>;

  static constexpr std::size_t MAX_SEGMENT_SIZE = 100;

  struct MagicSequenceNumbers {
    static constexpr SequenceType
        MAX_SEQUENCE = std::numeric_limits<SequenceType>::max();
  };

  enum class ControlBits {
    ORDINARY_VALUE = 1,
    TOMBSTONE = 2,
  };

 public:
  class Iterator {
   public:
    explicit Iterator(OrderedMap &orderedMap)
        : orderedMap_{orderedMap},
          seq_{orderedMap.getCurrentSequenceNum()} {}

    bool hasNext();
    std::optional<Value> next() {
      return {};
    }

   private:
    const OrderedMap &orderedMap_;
    SequenceType seq_;
  };

 public:
  explicit OrderedMap() :
      seq_{0},
      currentDataStore_{std::make_shared<StoreType>()} {}

  void add(Key k, Value v) {
    add(std::move(k), std::move(v), ControlBits::ORDINARY_VALUE);
  }

  void erase(Key key) {
    add(std::move(key), {}, ControlBits::TOMBSTONE);
  }

  std::optional<Value> get(Key key) {
    std::vector<Metadata> metadataList;

    {
      std::shared_lock sharedLock{mtx_};
      if (key >= currentDataStore_->begin()->first.key_
          && key <= currentDataStore_->rbegin()->first.key_) {
        auto value = getValue(key, *currentDataStore_);
        return value;
      }

      // Not able to find the element in the current map.
      // Find it in the previous blocks.
      // Create copy to avoid lock contention.
      // Find easier way to copy(instead of copying objects).
      metadataList = metadataStore_;
    }

    // Fix this use reverse ranges.
    for (int index = metadataList.size() - 1; index >= 0; index--) {
      const auto &metadata = metadataList[index];

      if (key >= metadata.minKey_ && key <= metadata.maxKey_) {
        auto value = getValue(key, *metadata.dataStore_);
        return value;
      }
    }

    return {};
  }

  SequenceType getCurrentSequenceNum() {
    std::shared_lock sharedLock{mtx_};
    return seq_;
  }

 private:
  void add(Key k, Value v, ControlBits controlBits) {
    std::unique_lock uniqueLock(mtx_);

    SequenceType sequenceNum = seq_++;
    auto key =
        InternalKey{std::move(k), sequenceNum,
                    static_cast<std::uint32_t>(controlBits)};

    currentDataStore_->emplace(key, std::move(v));

    if (currentDataStore_->size() > MAX_SEGMENT_SIZE) {
      Metadata metadata{currentDataStore_->begin()->first.key_, // MinKey
                        currentDataStore_->crbegin()->first.key_, // MaxKey
                        currentDataStore_};
      metadataStore_.emplace_back(metadata);

      currentDataStore_ = std::make_shared<StoreType>();
    }
  }

  std::optional<Value> getValue(const Key &key, const StoreType &store) {
    if (auto itr = store.upper_bound({key, MagicSequenceNumbers::MAX_SEQUENCE});
        itr != currentDataStore_->end()) {

      if (store.empty()) {
        return {};
      }

      if (itr == store.end()) {
        itr--;
      }

      if (itr->first.key_ != key
          || itr->first.controlBits_
              == static_cast<std::uint32_t>(ControlBits::TOMBSTONE)) {
        return {};
      }

      return itr->second;
    }

    return {};
  }

 private:

  struct InternalKey {
    std::string key_;
    SequenceType sequenceNum_;
    ControlBitsType controlBits_;
  };

  struct DataStoreKeyComparator {
    bool operator()(const InternalKey &x, const InternalKey &y) const {
      if (x.key_ == y.key_) {
        return x.sequenceNum_ > y.sequenceNum_;
      }

      return x.key_ < y.key_;
    }
  };

  struct Metadata {
    Key minKey_;
    Key maxKey_;
    std::shared_ptr<StoreType> dataStore_;
  };

  SequenceType seq_;
  std::vector<Metadata> metadataStore_;
  std::shared_ptr<StoreType> currentDataStore_;

  std::shared_mutex mtx_;
};

}

#endif //KEYVALUESTOREWITHSTABLEITEROR_ORDEREDMAP_H
