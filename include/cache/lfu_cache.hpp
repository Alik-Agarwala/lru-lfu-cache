#pragma once

#include <cstddef>
#include <list>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace cache {

template <typename K, typename V>
class LFUCache {
public:
    explicit LFUCache(std::size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("LFUCache capacity must be > 0");
        }
    }

    std::optional<V> get(const K& key) {
        auto it = keyTable_.find(key);
        if (it == keyTable_.end()) {
            ++misses_;
            return std::nullopt;
        }
        ++hits_;
        Entry entry = *it->second;
        touch(key, it, entry.freq);
        return keyTable_[key]->value;
    }

    void put(const K& key, const V& value) {
        auto it = keyTable_.find(key);
        if (it != keyTable_.end()) {
            it->second->value = value;
            touch(key, it, it->second->freq);
            return;
        }

        if (keyTable_.size() >= capacity_) {
            evictLFU();
        }

        freqLists_[1].push_front(Entry{key, value, 1});
        keyTable_[key] = freqLists_[1].begin();
        minFreq_ = 1;
    }

    bool contains(const K& key) const { return keyTable_.find(key) != keyTable_.end(); }
    std::size_t size() const { return keyTable_.size(); }
    std::size_t capacity() const { return capacity_; }
    std::size_t hits() const { return hits_; }
    std::size_t misses() const { return misses_; }

    void clear() {
        keyTable_.clear();
        freqLists_.clear();
        minFreq_ = 0;
        hits_ = misses_ = 0;
    }

private:
    struct Entry {
        K key;
        V value;
        int freq;
    };

    using EntryList = std::list<Entry>;
    using EntryIt = typename EntryList::iterator;

    void touch(const K& key, typename std::unordered_map<K, EntryIt>::iterator mapIt, int oldFreq) {
        Entry entry = *mapIt->second;
        freqLists_[oldFreq].erase(mapIt->second);
        if (freqLists_[oldFreq].empty()) {
            freqLists_.erase(oldFreq);
            if (minFreq_ == oldFreq) ++minFreq_;
        }

        entry.freq = oldFreq + 1;
        freqLists_[entry.freq].push_front(entry);
        keyTable_[key] = freqLists_[entry.freq].begin();
    }

    void evictLFU() {
        auto& list = freqLists_[minFreq_];
        const K keyToEvict = list.back().key;
        list.pop_back();
        if (list.empty()) {
            freqLists_.erase(minFreq_);
        }
        keyTable_.erase(keyToEvict);
    }

    std::size_t capacity_;
    std::unordered_map<K, EntryIt> keyTable_;
    std::unordered_map<int, EntryList> freqLists_;
    int minFreq_ = 0;
    std::size_t hits_ = 0;
    std::size_t misses_ = 0;
};

}  // namespace cache
