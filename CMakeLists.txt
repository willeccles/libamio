# LibAMIO CMakeLists
# This is used to build the amio library for ARM targets.
# You can add this as a subdirectory of a project directly.
cmake_minimum_required (VERSION 3.13)

# this has to come after the CMAKE_TOOLCHAIN_FILE above
project (LibAMIO VERSION 0.0.1 DESCRIPTION "Library for Linux ARM-based SoC IO.")

# use -DCMAKE_BUILD_TYPE=Debug or =Release at the command line to change build types
# (this defaults to Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE PRIVATE Release)
endif()

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
