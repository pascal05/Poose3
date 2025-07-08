cmake_minimum_required(VERSION 3.10)
project(Mercatori)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    src/Server.cpp
    src/User.cpp
    src/Product.cpp
)

# Pybind11 setup
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)

pybind11_add_module(server_module
    server/pybind.cpp
    ${SOURCES}
)

target_include_directories(server_module PRIVATE ${PROJECT_SOURCE_DIR}/include)

set_target_properties(server_module PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/server
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/server
)

# Executables for demos
add_executable(demo1 demo/demo1.cpp ${SOURCES})
add_executable(demo2 demo/demo2.cpp ${SOURCES})

# ----------------------
# Google Test Setup
# ----------------------
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

add_executable(unittests tests/unittests.cpp ${SOURCES})

target_link_libraries(unittests PRIVATE ${GTEST_LIBRARIES} pthread)

enable_testing()
add_test(NAME UnitTests COMMAND unittests)
