#pragma once

#include <cstddef>
#include <list>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace cache {

template <typename K, typename V>
class LRUCache {
public:
    explicit LRUCache(std::size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("LRUCache capacity must be > 0");
        }
        index_.reserve(capacity_);
    }

    std::optional<V> get(const K& key) {
        auto it = index_.find(key);
        if (it == index_.end()) {
            ++misses_;
            return std::nullopt;
        }
        ++hits_;
        ListIt listIt = it->second;
        touch(listIt);
        return (*listIt)->value;
    }

    void put(const K& key, const V& value) {
        auto it = index_.find(key);
        if (it != index_.end()) {
            (*it->second)->value = value;
            touch(it->second);
            return;
        }

        if (items_.size() >= capacity_) {
            evictLRU();
        }

        items_.emplace_front(std::make_shared<Node>(key, value));
        index_[key] = items_.begin();
    }

    bool contains(const K& key) const { return index_.find(key) != index_.end(); }
    std::size_t size() const { return items_.size(); }
    std::size_t capacity() const { return capacity_; }
    std::size_t hits() const { return hits_; }
    std::size_t misses() const { return misses_; }

    void clear() {
        items_.clear();
        index_.clear();
        hits_ = misses_ = 0;
    }

private:
    struct Node {
        K key;
        V value;
        Node(K k, V v) : key(std::move(k)), value(std::move(v)) {}
    };

    using NodePtr = std::shared_ptr<Node>;
    using ListIt = typename std::list<NodePtr>::iterator;

    void touch(ListIt it) {
        items_.splice(items_.begin(), items_, it);
        index_[(*it)->key] = items_.begin();
    }

    void evictLRU() {
        auto lru = items_.back();
        index_.erase(lru->key);
        items_.pop_back();
    }

    std::size_t capacity_;
    std::list<NodePtr> items_;
    std::unordered_map<K, ListIt> index_;
    std::size_t hits_ = 0;
    std::size_t misses_ = 0;
};

}  // namespace cache
