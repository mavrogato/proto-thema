
cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS
  "-EHsc -W4 -WX -Wv:18 -std:c++20 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS")
set(CMAKE_CXX_FLAGS_DEBUG
  "-Od -Zi")

project(proto-thema)

file(GLOB HDR *.hpp)
file(GLOB SRC *.cc)
list(FILTER SRC EXCLUDE REGEX ".*-test\.cc")
file(GLOB TST *-test.cc)

add_executable(proto-thema ${HDR} ${SRC})
target_compile_options(proto-thema
  PRIVATE
  #-Zc:preprocessor -P -Fi:proto-thema.i
  -Fm -Fa)

################################################################################
# gtest
################################################################################
include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip)

if(WIN32)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(proto-thema-test ${HDR} ${TST})
target_compile_options(proto-thema-test
  PRIVATE
  #-Zc:preprocessor -P -Fi:proto-thema-test.i
  -Fm -Fa)

target_link_libraries(proto-thema-test PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(proto-thema-test)

################################################################################
# test and run
################################################################################
add_custom_target(run
  DEPENDS proto-thema proto-thema-test
  # COMMAND ctest
  # COMMAND start cmd.exe /K proto-thema
  COMMAND start proto-thema)
