#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <vector>

#include "cache/lru_cache.hpp"

class NaiveMapLRU {
public:
    explicit NaiveMapLRU(std::size_t capacity) : capacity_(capacity) {}

    void put(int key, int value) {
        store_[key] = {value, ++tick_};
        if (store_.size() > capacity_) {
            auto oldest = store_.begin();
            for (auto it = store_.begin(); it != store_.end(); ++it) {
                if (it->second.second < oldest->second.second) oldest = it;
            }
            store_.erase(oldest);
        }
    }

    bool get(int key, int& out) {
        auto it = store_.find(key);
        if (it == store_.end()) return false;
        it->second.second = ++tick_;
        out = it->second.first;
        return true;
    }

private:
    std::size_t capacity_;
    long long tick_ = 0;
    std::map<int, std::pair<int, long long>> store_;
};

template <typename Fn>
long long timeMs(Fn&& fn) {
    auto start = std::chrono::steady_clock::now();
    fn();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
    constexpr int kCapacity = 1000;
    constexpr int kOps = 200000;

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> keyDist(0, kCapacity * 5);

    std::vector<int> keys(kOps);
    for (auto& k : keys) k = keyDist(rng);

    cache::LRUCache<int, int> fastCache(kCapacity);
    long long fastMs = timeMs([&] {
        for (int i = 0; i < kOps; ++i) {
            int k = keys[i];
            if (!fastCache.get(k)) fastCache.put(k, k * 2);
        }
    });

    NaiveMapLRU naiveCache(kCapacity);
    long long naiveMs = timeMs([&] {
        for (int i = 0; i < kOps; ++i) {
            int k = keys[i];
            int out;
            if (!naiveCache.get(k, out)) naiveCache.put(k, k * 2);
        }
    });

    std::cout << kOps << " ops, capacity " << kCapacity << "\n";
    std::cout << "cache::LRUCache : " << fastMs << " ms\n";
    std::cout << "NaiveMapLRU     : " << naiveMs << " ms\n";
    if (fastMs > 0) {
        std::cout << "Speedup         : " << (double)naiveMs / (double)fastMs << "x\n";
    }
    return 0;
}
