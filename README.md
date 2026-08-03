# LRU/LFU Cache Engine (C++)

A generic, header-only LRU (Least Recently Used) and LFU (Least Frequently
Used) cache implemented in modern C++17, with O(1) average-time `get`/`put`.

## Features

- Template-based: works with any hashable key type and any value type.
- Header-only: drop `include/cache/lru_cache.hpp` or `lfu_cache.hpp` into any
  project, no linking required.
- Smart-pointer-managed nodes (LRU) — no manual `new`/`delete`.
- Unit tested with GoogleTest (15 tests covering eviction order, tie-breaks,
  updates, and edge cases).
- Benchmarked against a naive `std::map`-based LRU cache.

## Project structure

```
lru-lfu-cache/
├── CMakeLists.txt
├── include/cache/
│   ├── lru_cache.hpp
│   └── lfu_cache.hpp
├── src/main.cpp          # usage demo
├── benchmark/benchmark.cpp
├── tests/
│   ├── test_lru_cache.cpp
│   └── test_lfu_cache.cpp
└── README.md
```

## Build & run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

./build/cache_demo        # usage demo
./build/cache_tests       # GoogleTest suite
./build/cache_benchmark   # perf comparison vs std::map
```

## Design notes

**LRUCache** uses a `std::unordered_map<K, iterator>` pointing into a
`std::list<std::shared_ptr<Node>>`. The most-recently-used item sits at the
front of the list; eviction pops from the back. Both `get` and `put` are
O(1) average case because list splicing avoids reallocation.

**LFUCache** uses the classic "frequency bucket" scheme: each frequency
count has its own `std::list` of entries, and a `minFreq` pointer tracks
which bucket to evict from. Ties within the same frequency are broken by
recency (the back of that bucket's list).

## Benchmark result

On this machine (200,000 mixed get/put operations, capacity 1000):

| Implementation                          | Time     |
|------------------------------------------|----------|
| `cache::LRUCache` (hash map + list)       | ~29 ms   |
| Naive `std::map` + linear eviction scan   | ~1045 ms |

~36x faster, because eviction in the naive version requires an O(n) scan
of the map to find the least-recently-used entry, while `cache::LRUCache`
evicts in O(1).

