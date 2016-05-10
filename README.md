dwarf2xml
=========
DWARF to XML converter

Taken from Emmanuel Azencot's page (http://machinman.net/code/dwarf2xml/) with intention of getting it to build on non-RH systems (including BSD).

License is GPLv2 (according to the author), license file not included, since it was not in the original tarball.

## Prerequisites

- awk, sed, grep
- libdwarf

## Errors and todo

- Library search does not quite work yet (work in progress), that's why wrong
  libdwarf.h gets picked up on FreeBSD. Need to find the libraries and set the
  path.

