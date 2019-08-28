# libhwio
Simple, efficient IO library for SoCs, including SPI, I2C, GPIO, and UART. This library is targeted directly at ARM Linux-RT targets.

This library has the goals of being:
- Fast
- Deterministic
- Efficient
- Easy

This library is particularly aimed at the AM335x-based SoCs, such as the
OSD-335x and the BeagleBone Black.

## Prebuilt Libraries

There may or may not be releases available on GitHub on the Releases tab. These
would be pre-compiled static libraries for ARM targets. These would be ready to
compile into executables. Their release pages would also include the header
files corresponding to their versions in order to keep consistency in case of
updates to the interfaces.

## Requirements

This library is probably not a particularly good one if you are a hobbyist - it
requires the Texas Instruments Linux RT SDK to be installed. This is so that
the library can be cross compiled.

This library also requires CMake >= 3.13, preferrably the latest release.

## Setup

For a standard Release build, you can simple do the following:

```
mkdir build && cd build
cmake ..
```

### Custom TI Linux RT Install Dir

The library will automatically attempt to find your TI Linux RT SDK in your
home directory, since this is the default location. If you wish to specify a
different *path*, you can do that by modifying CMakeLists.txt, since that was
created under the assumption that your path will always be default. However, if
you have more than one TI Linux RT SDK installed, or CMake didn't find it
somehow, you can specify a version using the `TI_SDK_VERSION` flag:

```
mkdir build && cd build    # assuming you haven't already made this directory
cmake -DTI_SDK_VERSION="06.00.00.07" ..
```

### Creating a Debug Release

This is as simple as specifying `CMAKE_BUILD_TYPE`, just like most other projects:

```
mkdir build && cd build    # assuming you haven't already made this directory
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## Building

Just like any other CMake project, after you've performed the setup steps
above, you can just run `make`. This will compile a *static* library, because
this avoids copying shared libs to an ARM target or having to compile it on
there as well.

The output from the build can be found in `libhwio/build/bin/lib`.

## Documentation

If you have Doxygen installed, you can generate documentation using `make docs`
in the build directory. These will be generated in the `libhwio/docs/html`
directory. Simply open `index.html` and you will be in business. If you don't
have Doxygen installed, use your preferred package manager to install Doxygen
and Graphviz.

## Library Credits

The libraries included in the `lib` directory are not made by me, nor owned by
me. They are copied from an ARM target in order to keep library versions
consistent and controlled. This is important for the project it was required
for. I do not own them, nor do I take any responsiblity for anything related to
them. All credit for those libraries goes to their respective owners.
