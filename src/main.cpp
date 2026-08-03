#include <iostream>

#include "cache/lru_cache.hpp"
#include "cache/lfu_cache.hpp"

int main() {
    std::cout << "=== LRU Cache Demo ===\n";
    cache::LRUCache<int, std::string> lru(3);
    lru.put(1, "one");
    lru.put(2, "two");
    lru.put(3, "three");
    lru.get(1);
    lru.put(4, "four");

    for (int key : {1, 2, 3, 4}) {
        auto val = lru.get(key);
        std::cout << "get(" << key << ") = "
                  << (val ? *val : "<miss>") << '\n';
    }
    std::cout << "hits=" << lru.hits() << " misses=" << lru.misses() << "\n\n";

    std::cout << "=== LFU Cache Demo ===\n";
    cache::LFUCache<int, std::string> lfu(2);
    lfu.put(1, "one");
    lfu.put(2, "two");
    lfu.get(1);
    lfu.put(3, "three");

    for (int key : {1, 2, 3}) {
        auto val = lfu.get(key);
        std::cout << "get(" << key << ") = "
                  << (val ? *val : "<miss>") << '\n';
    }
    std::cout << "hits=" << lfu.hits() << " misses=" << lfu.misses() << '\n';

    return 0;
}
