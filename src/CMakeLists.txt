# name our target in a variable for fewer typos and easier modification
set(TARGET_NAME amio)

# Include the sources that are in this directory.
# If more sources are added, or if more subdirectories are added,
# add sources here and include subdirectories' CMakeLists.txt here.
# This keeps every neat in the top level makefile.
# Keep this in alphabetical order or you'll be eaten by aliens.
list(APPEND LIBAMIO_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/libamio-gpio.c
    ${CMAKE_CURRENT_LIST_DIR}/libamio-i2c.c
    ${CMAKE_CURRENT_LIST_DIR}/libamio-spi.c
    ${CMAKE_CURRENT_LIST_DIR}/libamio-uart.c
)

add_library(${TARGET_NAME} STATIC ${LIBAMIO_SOURCES})

# include directories
target_include_directories(${TARGET_NAME} PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../include")

# link with libc and libm
find_library(LIBC c)
find_library(LIBM m)
find_library(LIBPTHREAD pthread)
find_library(LIBRT rt)

target_link_libraries(${TARGET_NAME} ${LIBC} ${LIBM} ${LIBRT} ${LIBPTHREAD})

# disable warnings for this target altogether with -w
target_compile_options(${TARGET_NAME} PRIVATE -w -O3)
