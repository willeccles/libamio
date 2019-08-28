# tell cmake we're cross compiling
set(CMAKE_CROSSCOMPILING TRUE CACHE INTERNAL "" FORCE)
set(CMAKE_SYSTEM_NAME Linux CACHE INTERNAL "" FORCE)
set(CMAKE_SYSTEM_PROCESSOR arm CACHE INTERNAL "" FORCE)

# set the compiler toolchains to use
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc CACHE INTERNAL "" FORCE)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++ CACHE INTERNAL "" FORCE)

set(CMAKE_FIND_ROOT_PATH ${TI_SDK_DIR}/linux-devkit/sysroots/x86_64-arago-linux CACHE INTERNAL "" FORCE)

# search for libraries on the host and the target fs
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH CACHE INTERNAL "" FORCE) 

# search for programs and includes *only* on the target fs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH CACHE INTERNAL "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE INTERNAL "" FORCE)
