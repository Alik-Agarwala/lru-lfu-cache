cmake_minimum_required(VERSION 3.14)
project(lru_lfu_cache CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(cache INTERFACE)
target_include_directories(cache INTERFACE ${CMAKE_SOURCE_DIR}/include)

add_executable(cache_demo src/main.cpp)
target_link_libraries(cache_demo PRIVATE cache)

add_executable(cache_benchmark benchmark/benchmark.cpp)
target_link_libraries(cache_benchmark PRIVATE cache)

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  add_executable(cache_tests
    tests/test_lru_cache.cpp
    tests/test_lfu_cache.cpp
  )
  target_link_libraries(cache_tests PRIVATE cache GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(cache_tests)
endif()
