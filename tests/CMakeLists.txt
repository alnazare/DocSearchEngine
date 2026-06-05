include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/add971c7cb497d40985b48641891bf9b51423730.zip)
FetchContent_MakeAvailable(googletest)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(DocSearchEngineTests tests.cpp)
target_link_libraries(DocSearchEngineTests PRIVATE DocSearchEngineLib GTest::gtest_main)

include(googletest)
gtest_discover_tests(DocSearchEngineTests)