# name our target in a variable for fewer typos and easier modification
set(TARGET_NAME amio)

# Include the sources that are in this directory.
# If more sources are added, or if more subdirectories are added,
# add sources here and include subdirectories' CMakeLists.txt here.
# This keeps every neat in the top level makefile.
# Keep this in alphabetical order or you'll be eaten by aliens.
list(APPEND LIB_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/libamio-gpio.c
    ${CMAKE_CURRENT_LIST_DIR}/libamio-i2c.c
    ${CMAKE_CURRENT_LIST_DIR}/libamio-spi.c
    ${CMAKE_CURRENT_LIST_DIR}/libamio-uart.c
)

# add our library target (name is a variable for consistency and
# easy modification)
# LIB_SOURCES is defined in the included other CMakeLists.txt files,
# which should be added to each subdirectory
add_library(${TARGET_NAME} STATIC ${LIB_SOURCES})

# include directories
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
set(CMAKE_LIBRARY_PATH ${CMAKE_FIND_ROOT_PATH}/../armv7at2hf-neon-linux-gnueabi/usr/lib)
# link with librt, libm (math), and libpthread
# also find libc since we are going to need that
file(GLOB LIBC ${CMAKE_SOURCE_DIR}/lib/libc-2.24.so)
file(GLOB LIBRT ${CMAKE_SOURCE_DIR}/lib/librt-2.24.so)
file(GLOB LIBM ${CMAKE_SOURCE_DIR}/lib/libm-2.24.so)
file(GLOB LIBPTHREAD ${CMAKE_SOURCE_DIR}/lib/libpthread-2.24.so)

MESSAGE("${CMAKE_FIND_ROOT_PATH}")
MESSAGE("LIBC: ${LIBC}")
MESSAGE("LIBRT: ${LIBRT}")
MESSAGE("LIBPTHREAD: ${LIBPTHREAD}")
MESSAGE("LIBM: ${LIBM}")

target_link_libraries(${TARGET_NAME} ${LIBC} ${LIBRT} ${LIBPTHREAD} ${LIBM})
