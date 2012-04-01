cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME_STR GameOfLife)
project(${PROJECT_NAME_STR})

set(GTEST_INCLUDE_DIRS /usr/include/gtest)
set(GMOCK_INCLUDE_DIRS /usr/include/gmock)

set(CMAKE_CXX_FLAGS --std=c++0x)

set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB HDR_FILES ${PROJECT_SOURCE_DIR}/*.h)
#file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/tests/*.cpp)

add_executable(${PROJECT_TEST_NAME} ${SRC_FILES} ${HDR_FILES}) #${TEST_SRC_FILES})
target_link_libraries(${PROJECT_TEST_NAME} /usr/lib/libgtest_main.a /usr/lib/libgmock.a /usr/lib/libgtest.a pthread)

add_test(test1 ${PROJECT_TEST_NAME})
