cmake_minimum_required(VERSION 2.8)
project(a4ece650)

if (EXISTS "${CMAKE_SOURCE_DIR}/minisat")
  add_subdirectory(minisat)
else()
  message (FATAL_ERROR "MiniSat not found. Use `git clone https://git.uwaterloo.ca/ece650-1231/minisat.git` to check it out.")
endif()


## ECE650 Specific configuration start here
## Project specific configurations go here

# enable c++11 support
set (CMAKE_CXX_FLAGS "-std=c++11 -Wall -pthread ${CMAKE_CXX_FLAGS}")
set (CMAKE_C_FLAGS -pthread)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

# expose minisat header files
include_directories(${CMAKE_SOURCE_DIR}/minisat)

# create the main executable
## add additional .cpp files if needed
add_executable(prjece650 prjece650.cpp ${SOURCES})
# link MiniSAT libraries
target_link_libraries(prjece650 minisat-lib-static)

add_executable(test test.cpp)

# link MiniSAT libraries
target_link_libraries(test minisat-lib-static)
