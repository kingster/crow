cmake_minimum_required(VERSION 3.12)
project(crow_all)

# Exports the "compile_commands.json" file when the generator is Ninja or Makefile.
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Tcmalloc)
find_package(Threads)
find_package(OpenSSL)

if (OPENSSL_FOUND)
  include_directories(${OPENSSL_INCLUDE_DIR})
endif()

find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
  message("Found ccache ${CCACHE_FOUND}")
  message("Using ccache to speed up compilation")
  set(ENV{CCACHE_CPP2} "yes")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if (MSVC)
  set(Boost_USE_STATIC_LIBS "On")
  find_package( Boost 1.52 COMPONENTS system thread regex REQUIRED )
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -pedantic -Wextra")
  find_package( Boost 1.52 COMPONENTS system thread REQUIRED )
endif()

set(INC_DIR "${CMAKE_SOURCE_DIR}/include")
set(BIN_DIR "${CMAKE_BINARY_DIR}")

include_directories("${INC_DIR}")
include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${Boost_INCLUDE_DIR}")

add_subdirectory(examples)

if (NOT MSVC)
  add_subdirectory(tests)

  enable_testing()
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
