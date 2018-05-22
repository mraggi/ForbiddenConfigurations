cmake_minimum_required(VERSION 3.2)

# Set project name here.
project(FConf)
enable_language(C CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_OWN_GOOGLETEST "Build own google's unit testing framework, even if found" OFF)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)
if (cmake_build_type_tolower STREQUAL "coverage")
    set(BUILD_TESTS ON)
endif()

# Set version number (change as needed). These definitions are available
# by including "exampleConfig.h" in the source. 
# See exampleConfig.h.in for some more details.
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)


# Include stuff. No change needed.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(ConfigSafeGuards)
include(Colors)


# --------------------------------------------------------------------------------
#                          Compile flags (change as needed).
# --------------------------------------------------------------------------------
# Set the C++ standard you wish to use (will apply to all files).
# If you do not use any features that limits the standard required,
# you could omit this line.
set(CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Build-type specific flags. Change as needed.
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")

message(STATUS "Building with the following extra flags: ${CMAKE_CXX_FLAGS}")

# --------------------------------------------------------------------------------
#                         Locate files (no change needed).
# --------------------------------------------------------------------------------
# We make sure that CMake sees all the files.
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests
    ${PROJECT_SOURCE_DIR}/external/googletest
    ${PROJECT_SOURCE_DIR}/external/googletest/include)

# Make variables referring to all the sources and test files.
file(GLOB SOURCES "src/*.cpp")


# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------
# Compile all sources into a library. Called forbiddenconfs here (change if you wish).
add_library( forbiddenconfs ${SOURCES} )

# Add an executable for the file main.cpp, here called FConf.
# If you add more executables, copy these three lines accordingly.
add_executable(FConf app/main.cpp)   # Name of exec. and location of file.
add_dependencies(FConf forbiddenconfs)       # main.cpp uses the 'forbiddenconfs', add dep.
target_link_libraries(FConf forbiddenconfs)  # Link the executable to the 'forbiddenconfs'.


if(${BUILD_TESTS})
    find_package(GTest)
    find_package(Threads REQUIRED)
    set(TEST_LIB googletest)
    file(GLOB TESTFILES "tests/*.cpp")
    set(TEST_MAIN unit_tests.x)
	if (${GTEST_FOUND} AND NOT ${BUILD_OWN_GOOGLETEST})
        include_directories(${GTEST_INCLUDE_DIRS})
        set(TEST_LIB ${GTEST_BOTH_LIBRARIES})
        add_executable(${TEST_MAIN} ${TESTFILES})
	else()
		message("Google testing framework not found. Building our own!")
        add_custom_target( git_update
        COMMAND git submodule init
        COMMAND git submodule update
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} )
        add_library(googletest
            ${PROJECT_SOURCE_DIR}/external/googletest/src/gtest-all.cc
            ${PROJECT_SOURCE_DIR}/external/googletest/src/gtest_main.cc)
        add_dependencies(googletest git_update)
        set_source_files_properties(${PROJECT_SOURCE_DIR}/external/googletest/src/gtest-all.cc  PROPERTIES GENERATED 1)
        set_source_files_properties(${PROJECT_SOURCE_DIR}/external/googletest/src/gtest_main.cc PROPERTIES GENERATED 1)
        add_executable(${TEST_MAIN} ${TESTFILES})
        add_dependencies(${TEST_MAIN} ${TEST_LIB})
    endif()
    
    target_link_libraries(${TEST_MAIN} ${TEST_LIB} Threads::Threads forbiddenconfs)
    add_custom_target(gtest COMMAND "${PROJECT_BINARY_DIR}/${TEST_MAIN}" DEPENDS ${TEST_MAIN})

    # Add a standard make target 'test' that runs the tests under CTest (only as an alt. to gtest).
    include(CTest)
    enable_testing()
    add_test(unit_tests ${PROJECT_BINARY_DIR}/${TEST_MAIN})
endif()

# --------------------------------------------------------------------------------
#                         Code Coverage (no change needed).
# --------------------------------------------------------------------------------
# If CMake aborts due to missing dependencies for code coverage 
# (gcov, lcov, genhtml and supported compiler), comment this line.
include(CodeCoverage)



# --------------------------------------------------------------------------------
#                            Misc (no change needed).
# --------------------------------------------------------------------------------
# Have CMake parse the config file, generating the config header, with
# correct definitions. Here only used to make version number available to
# the source code. Include "exampleConfig.h" (no .in suffix) in the source.
# configure_file ("${PROJECT_SOURCE_DIR}/include/exampleConfig.h.in" "${PROJECT_BINARY_DIR}/exampleConfig.h")
# add the binary tree to the search path for include files
# so that we will find exampleConfig.h
# include_directories("${PROJECT_BINARY_DIR}")

# Ask CMake to output a compile_commands.json file for use with things like Vim YCM.
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
