# LIBAMIO

This library is intended to provide a quick and easy way to do various comms on
ARM-based Linux devices. It has been tested and proven on the TI AM3358
platform, but should work equally well on others. It builds to a static library
and depends on libc, libm, librt, and libpthread. These should be provided by
your toolchain, which you should have already set up.

## BUILDING

This library can be build with the included Makefile. To build both a static
library (.a) and a shared library (.so), you can run `make` with no arguments.
To build only one or the other, you can do either `make shared` or `make
static`. Note that by default, the shared library is statically linked with
libc. If you wish to dynamically link, you can edit the Makefile and remove
`-static` from `LDFLAGS`.

### Cross-compiling

If you are cross-compiling for another target, you can specify `CROSS_COMPILE`
as a prefix for the C compiler, linker, and `ar`. For example, if your target
compiler is `arm-linux-gnueabihf-gcc`, you would do `make
CROSS_COMPILE=arm-linux-gnueabihf-` (note the trailing `-`).

If your host system's `CC` environment variable is clang, you will also need to
specify `CC=gcc` in the previous example so that make doesn't look for
`arm-linux-gnueabihf-clang`.

### Building Documentation

If you need to build the documentation (which should only be true if you are
contributing), you can do `make docs` assuming you have Doxygen, GraphViz, and
some fonts installed.

## USAGE

For more complete docs on the library, see https://eccles.dev/libamio.

## LICENSE

This library is released under the Apache-2.0 License.
See the LICENSE file distributed with this source for more details.

