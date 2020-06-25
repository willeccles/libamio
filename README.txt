# LIBAMIO

This library is intended to provide a quick and easy way to do various comms on
ARM-based Linux devices. It has been tested and proven on the TI AM3358
platform, but should work equally well on others. It builds to a static library
and depends on libc, libm, librt, and libpthread. These should be provided by
your toolchain, which you should have already set up.

## BUILDING

This library should be built by adding it to a pre-existing CMake project. For
example, if your project source tree looks like this:

  myproject/
    include/
      myfile.h
    src/
      main.c
      CMakeLists.txt
    CMakeLists.txt

You would add this as a subdirectory (just like src), then in src/CMakeLists.txt
you would link with target 'amio'. Thus, your tree now looks like this
(abbreviated):
  
  myproject/
    ...
    libamio/
      include/
        ...
      src/
        ...
        CMakeLists.txt
      CMakeLists.txt
    src/
      ...
      CMakeLists.txt
    CMakeLists.txt

In myproject/CMakeLists.txt, simply add the following:
  
  add_subdirectory(libamio)

Then, in src/CMakeLists.txt, link your binary with the library:

  target_link_libraries(mycooltargetname ... amio ...)

You should be all set to build.

## USAGE

For more complete docs on the library, see https://eccles.dev/libamio.

## LICENSE

This library is released under the Apache-2.0 License.
See the LICENSE file distributed with this source for more details.

