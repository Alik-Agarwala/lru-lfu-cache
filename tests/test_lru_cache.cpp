#include <gtest/gtest.h>

#include "cache/lru_cache.hpp"

using cache::LRUCache;

TEST(LRUCache, MissOnEmptyCache) {
    LRUCache<int, int> c(2);
    EXPECT_FALSE(c.get(1).has_value());
}

TEST(LRUCache, BasicPutGet) {
    LRUCache<int, std::string> c(2);
    c.put(1, "a");
    EXPECT_EQ(c.get(1).value(), "a");
}

TEST(LRUCache, UpdatesExistingKey) {
    LRUCache<int, std::string> c(2);
    c.put(1, "a");
    c.put(1, "b");
    EXPECT_EQ(c.size(), 1u);
    EXPECT_EQ(c.get(1).value(), "b");
}

TEST(LRUCache, EvictsLeastRecentlyUsed) {
    LRUCache<int, std::string> c(2);
    c.put(1, "a");
    c.put(2, "b");
    c.put(3, "c");
    EXPECT_FALSE(c.get(1).has_value());
    EXPECT_EQ(c.get(2).value(), "b");
    EXPECT_EQ(c.get(3).value(), "c");
}

TEST(LRUCache, GetRefreshesRecency) {
    LRUCache<int, std::string> c(2);
    c.put(1, "a");
    c.put(2, "b");
    c.get(1);
    c.put(3, "c");
    EXPECT_TRUE(c.get(1).has_value());
    EXPECT_FALSE(c.get(2).has_value());
    EXPECT_TRUE(c.get(3).has_value());
}

TEST(LRUCache, HitAndMissCountersTrackCorrectly) {
    LRUCache<int, int> c(1);
    c.put(1, 100);
    c.get(1);
    c.get(2);
    EXPECT_EQ(c.hits(), 1u);
    EXPECT_EQ(c.misses(), 1u);
}

TEST(LRUCache, ThrowsOnZeroCapacity) {
    using IntCache = LRUCache<int, int>;
    EXPECT_THROW(IntCache(0), std::invalid_argument);
}

TEST(LRUCache, ClearResetsState) {
    LRUCache<int, int> c(2);
    c.put(1, 1);
    c.put(2, 2);
    c.get(1);
    c.clear();
    EXPECT_EQ(c.size(), 0u);
    EXPECT_EQ(c.hits(), 0u);
    EXPECT_EQ(c.misses(), 0u);
    EXPECT_FALSE(c.get(1).has_value());
}
