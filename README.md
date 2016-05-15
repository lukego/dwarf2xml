dwarf2xml
=========
DWARF to XML converter

Taken from Emmanuel Azencot's page (http://machinman.net/code/dwarf2xml/) with intention of getting it to build on non-RH systems (including BSD).

License is GPLv2 (according to the author), license file not included, since it was not in the original tarball.

## Prerequisites

- awk, sed, grep (build)
- cmake (build)
- libdwarf
- libelf

## Building

```
mkdir build
cd build
cmake ..
make
```

## Errors and todo

- Generate config.h, check if any other macros need to be defined (use config.in.h in top-level directory as a reference)
- Execution error on FreeBSD, may be other issues on other platforms

