cmake_minimum_required(VERSION 3.14)
project(finding_clusters_in_dilute_magnets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES
    src/config.cpp
    src/lattice.cpp
)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG master
)
FetchContent_MakeAvailable(nlohmann_json argparse)

if(NOT nlohmann_json_POPULATED OR NOT argparse_POPULATED)
    message(FATAL_ERROR "Failed to populate dependencies")
endif()

add_executable(${PROJECT_NAME} src/main.cpp ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${nlohmann_json_SOURCE_DIR}/include
    ${argparse_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

message(STATUS "Include path: ${CMAKE_CURRENT_SOURCE_DIR}")