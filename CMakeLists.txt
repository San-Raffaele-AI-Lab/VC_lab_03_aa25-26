cmake_minimum_required(VERSION 3.16)

project(VC_lab_03_aa25-26 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework)
include_directories (${Boost_INCLUDE_DIRS})

# Add Eigen include path
set(EIGEN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(srimg++ SHARED
            src/utils.cpp
            src/utils.h
            src/image.h
            src/load_image.cpp
            src/stb_image.h
            src/stb_image_write.h
            src/process_image.cpp
            src/access_image.cpp
            src/filter_image.cpp
            src/edge_detection.cpp
            )

target_include_directories(srimg++ PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${EIGEN_INCLUDE_DIR}
)

link_libraries(srimg++ m stdc++)

add_executable (test_canny src/test/test3.cpp)
target_link_libraries (test_canny ${Boost_LIBRARIES})
