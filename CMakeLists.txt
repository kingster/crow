cmake_minimum_required(VERSION 3.12)
project(crow_all)

# Exports the "compile_commands.json" file when the generator is Ninja or Makefile.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_CCACHE "Use ccache (if found) to speed up compile and link" ON)
option(ADDRESS_SANITIZER "Use memory error detector (clang only and implies debug mode!)" OFF)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(compilation)
include(deps)

if (USE_CCACHE)
  find_program(CCACHE_FOUND ccache)
  if (CCACHE_FOUND)
    message("Found ccache ${CCACHE_FOUND}")
    message("Using ccache to speed up compilation")
    set(ENV{CCACHE_CPP2} "yes")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
  endif(CCACHE_FOUND)
endif()

if (ADDRESS_SANITIZER)
  if (NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(FATAL_ERROR "Clang is required to use address sanitizer to detect memory errors!")
  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -g -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")

  message(STATUS "Build type forced to Debug due to address sanitizer option")
  set(CMAKE_BUILD_TYPE Debug)
endif()

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(INC_DIR "${CMAKE_SOURCE_DIR}/include")
set(BIN_DIR "${CMAKE_BINARY_DIR}")

include_directories("${INC_DIR}")
include_directories("${CMAKE_SOURCE_DIR}")

add_subdirectory(examples)

if (NOT MSVC)
  enable_testing()
  add_subdirectory(tests)

  add_test(NAME crow_test COMMAND ${BIN_DIR}/tests/unittest)
  add_test(NAME template_test COMMAND ${BIN_DIR}/tests/template/test.py WORKING_DIRECTORY ${BIN_DIR}/tests/template)

  file(MAKE_DIRECTORY ${BIN_DIR}/amalgamate)

  file(GLOB inc_files
    "${INC_DIR}/*.h"
    "${INC_DIR}/crow/*.h")

  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  add_custom_command(
    OUTPUT ${BIN_DIR}/amalgamate/crow_all.h
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/amalgamate/merge_all.py ${INC_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${BIN_DIR}/amalgamate/crow_all.h ${CMAKE_SOURCE_DIR}/amalgamate
    WORKING_DIRECTORY ${BIN_DIR}/amalgamate
    DEPENDS ${inc_files}
    )

  add_custom_target(amalgamation ALL DEPENDS ${BIN_DIR}/amalgamate/crow_all.h)
endif()
