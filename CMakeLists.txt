cmake_minimum_required(VERSION 3.20)

project(miniser VERSION 0.1.0 LANGUAGES CXX)

option(MINISER_ENABLE_TESTS "Enable tests in miniser" OFF)
option(MINISER_ENABLE_EXAMPLES "Enable examples in miniser" OFF)

find_package(Boost REQUIRED)
find_package(yyjson REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

target_link_libraries(${PROJECT_NAME} INTERFACE Boost::headers yyjson::yyjson)

if(MINISER_ENABLE_TESTS)
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    find_package(GTest REQUIRED)
    include(GoogleTest)

    enable_testing()

    add_executable(${PROJECT_NAME}-test
        tests/ser.cpp
        tests/rename.cpp
        tests/deser.cpp
    )
    target_link_libraries(${PROJECT_NAME}-test PRIVATE ${PROJECT_NAME} GTest::gtest_main)
    set_target_properties(${PROJECT_NAME}-test PROPERTIES
        CXX_STANDARD 20
    )

    gtest_discover_tests(${PROJECT_NAME}-test)
endif()

if(MINISER_ENABLE_EXAMPLES)
    include(examples/CMakeLists.txt)
endif()
