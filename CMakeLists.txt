cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

project(Calculator)

include(cmake/vcpkg_util.cmake)

find_package(Boost 1.75 REQUIRED)

setup_vcpkg()

add_subdirectory(Calculator)

if(NOT DEFINED BUILD_WITHOUT_TESTS)
	enable_testing()
	find_package(Boost 1.75 REQUIRED COMPONENTS unit_test_framework)
	add_subdirectory(UnitTests)
else()
	message("skipping tests")
endif()