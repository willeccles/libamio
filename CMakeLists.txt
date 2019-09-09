# LibAMIO CMakeLists
# This is used to build the amio library for ARM targets.
# Make sure you have the TI linux-rt SDK installed - this will be used to
# get the right compiler commands and includes for the ARM target.
cmake_minimum_required (VERSION 3.13)
# moved 'project' line to later in the file for toolchain reasons

# defines directories to install to
include(GNUInstallDirs)

# use -DCMAKE_BUILD_TYPE=Debug or =Release at the command line to change build types
# (this defaults to Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# various build flags
set(CMAKE_C_FLAGS_DEBUG "-O3 -D_DEBUG -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3")

# use -DTI_SDK_VERSION="06.00.00.07" at the command line if you want a specific
# version. otherwise, one is found automatically.
if(TI_SDK_VERSION)
    set(TI_SDK_DIR "$ENV{HOME}/ti-processor-sdk-linux-rt-am335x-evm-${TI_SDK_VERSION}")
else()
    # find the makefile (if it exists, which it definitely should) in
    # the TI SDK, then use that to get the directory
    # we get all makefiles found, then get the last one by reversing the list
    # this means if the user has many installed versions, the one at the bottom
    # (ostensibly the highest version number) is used.
    file(GLOB TI_MAKEFILES
        "$ENV{HOME}/ti-processor-sdk-linux-rt-am335x-evm-*/Makefile"
        )
    if(NOT TI_MAKEFILES)
        MESSAGE("Couldn't find TI SDK directory!")
        MESSAGE("Make sure you have the TI linux RT processor SDK for the am335x!")
        return()
    endif()
    list(REVERSE TI_MAKEFILES)
    list(GET TI_MAKEFILES 0 TI_MAKEFILE)
    # get the directory path from the makefile, which just gets us the TI SDK path
    get_filename_component(TI_SDK_DIR "${TI_MAKEFILE}" DIRECTORY)
endif()

# include the toolchain to tell cmake what to use to compile stuff
# this should *really* be set in -DCMAKE_TOOLCHAIN_FILE but hopefully this works
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-toolchain.cmake)

# this has to come after the CMAKE_TOOLCHAIN_FILE above
project (LibHWIO VERSION 0.0.1 DESCRIPTION "Library for AM335x SoC IO.")

# print info about our parameters
MESSAGE("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
MESSAGE("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
if(TI_SDK_VERSION)
    MESSAGE("TI_SDK_VERSION: ${TI_SDK_VERSION}")
else()
    MESSAGE("TI_SDK_VERSION: Not specified")
endif()
MESSAGE("TI_SDK_DIR: ${TI_SDK_DIR}")

# tell cmake to use a 'bin' directory to output binaries and libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(src)

# add doxygen if possible
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen" VERBATIM
    )
else()
    MESSAGE("Doxygen not found!")
    MESSAGE("Install Doxygen and Graphviz to enable documentation building.")
endif(DOXYGEN_FOUND)
