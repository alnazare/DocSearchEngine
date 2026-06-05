add_library(DocSearchEngineLib
    "InvertedIndex.cpp"
    "ConverterJson.cpp"
    "SearchServer.cpp" 
    "ThreadPool.cpp")
target_include_directories(DocSearchEngineLib PUBLIC 
    ${CMAKE_SOURCE_DIR}/include)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/archive/484483acad6d562306efc9b3c6d413404f1b1f8a.zip)
FetchContent_MakeAvailable(json)
target_link_libraries(DocSearchEngineLib PUBLIC nlohmann_json::nlohmann_json)

add_executable(DocSearchEngine "DocSearchEngine.cpp")
target_link_libraries(DocSearchEngine PRIVATE DocSearchEngineLib)

