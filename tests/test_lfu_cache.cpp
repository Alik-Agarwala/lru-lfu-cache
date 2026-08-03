#include <gtest/gtest.h>

#include "cache/lfu_cache.hpp"

using cache::LFUCache;

TEST(LFUCache, MissOnEmptyCache) {
    LFUCache<int, int> c(2);
    EXPECT_FALSE(c.get(1).has_value());
}

TEST(LFUCache, BasicPutGet) {
    LFUCache<int, std::string> c(2);
    c.put(1, "a");
    EXPECT_EQ(c.get(1).value(), "a");
}

TEST(LFUCache, EvictsLeastFrequentlyUsed) {
    LFUCache<int, std::string> c(2);
    c.put(1, "a");
    c.put(2, "b");
    c.get(1);
    c.put(3, "c");
    EXPECT_TRUE(c.get(1).has_value());
    EXPECT_FALSE(c.get(2).has_value());
    EXPECT_TRUE(c.get(3).has_value());
}

TEST(LFUCache, TieBreaksOnLeastRecentlyUsed) {
    LFUCache<int, std::string> c(2);
    c.put(1, "a");
    c.put(2, "b");
    c.put(3, "c");
    EXPECT_FALSE(c.get(1).has_value());
    EXPECT_TRUE(c.get(2).has_value());
    EXPECT_TRUE(c.get(3).has_value());
}

TEST(LFUCache, UpdatingExistingKeyBumpsFrequency) {
    LFUCache<int, int> c(2);
    c.put(1, 10);
    c.put(1, 20);
    c.put(2, 30);
    c.put(3, 40);
    EXPECT_TRUE(c.get(1).has_value());
    EXPECT_EQ(c.get(1).value(), 20);
    EXPECT_FALSE(c.get(2).has_value());
}

TEST(LFUCache, ThrowsOnZeroCapacity) {
    using IntCache = LFUCache<int, int>;
    EXPECT_THROW(IntCache(0), std::invalid_argument);
}

TEST(LFUCache, ClearResetsState) {
    LFUCache<int, int> c(2);
    c.put(1, 1);
    c.get(1);
    c.clear();
    EXPECT_EQ(c.size(), 0u);
    EXPECT_EQ(c.hits(), 0u);
    EXPECT_FALSE(c.get(1).has_value());
}
