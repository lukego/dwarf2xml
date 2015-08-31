#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([dwarf2xml], [4.01], [webmaster@machinman.net])
AM_INIT_AUTOMAKE([dwarf2xml], [4.01])
AC_CONFIG_SRCDIR([src/dwarf2xml.c])
AC_CONFIG_HEADERS([config.h])

# LIBPATH="$LIBPATH -L/usr/lib/libdwarf"
# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX

AC_ARG_WITH([dwarf],
[AS_HELP_STRING([--with-dwarf=<path to libdwarf>],
   [support fancy command line editing @<:@default=/usr/lib/libdwarf@:/usr>@])],
[],
[with_dwarf=/usr])

DWARF_LIB=/usr/lib/libdwarf
DWARF_INC=/usr/include/libdwarf
AS_IF([test "x$with_dwarf" != "x/usr" ],[
   AS_IF([test -e $with_dwarf/libdwarf.a -o -e $with_dwarf/libdwarf.so ], true,
      [AC_MSG_FAILURE([--with-dwarf was given, but could not find $with_dwarf/libdwarf.so or $with_dwarf/libdwarf.a])])
   AS_IF([test -e $with_dwarf/dwarf.h ], true,
      [AC_MSG_FAILURE([--with-dwarf was given, but could not find $with_dwarf/dwarf.h])])
   AS_IF([test -e $with_dwarf/libdwarf.h ], true,
      [AC_MSG_FAILURE([--with-dwarf was given, but could not find $with_dwarf/libdwarf.h])])
   AC_SUBST([DWARF_LIB], ["$with_dwarf"])
   AC_SUBST([DWARF_INC], ["$with_dwarf"])
   AC_MSG_RESULT([Taking $with_dwarf as dwarf library])
])
LDFLAGS="$LDFLAGS -L$DWARF_LIB"
CFLAGS="$CFLAGS -I$DWARF_INC"

# Checks for libraries.
AC_CHECK_LIB([elf], [gelf_getehdr], , [AC_MSG_ERROR([Can't find libelf. Try to install elfutils-libelf-devel])])
AC_CHECK_LIB([dwarf], [dwarf_init], , [AC_MSG_ERROR([Can't find libdwarf. Try to install libdwarf-devel])])

AC_CHECK_LIB(dwarf, dwarf_get_form_class,
  AC_DEFINE(HAVE_GET_FORM_CLASS,1, [Define to 1 if the dwarf_get_form_class function is in libdwarf.a.]))

AC_CHECK_LIB(dwarf, dwarf_next_cu_header_b,
  AC_DEFINE(HAVE_NEXT_CU_HEADER_B,1, [Define to 1 if the dwarf_next_cu_header_b function is in libdwarf.a.]))

CFLAGS_SAVE="$CFLAGS"
CFLAGS="$CFLAGS -Werror"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <dwarf.h>
   #include <libdwarf.h>],
   [Dwarf_Cie Cie; dwarf_expand_frame_instructions(Cie, 0, 0, 0, 0, 0);])] ,
	[AC_DEFINE(EXPAND_FRAME_INSTR_USE_CIE,1, [Define 1 if dwarf_expand_frame_instructions 1st param is type Dwarf_Cie])])
CFLAGS="$CFLAGS_SAVE"

AC_DEFINE(RING_ALLOC_BLK_CHK,1, [Define 1 to check rings integrity])

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h string.h assert.h errno.h], , [AC_MSG_ERROR([Can't find standart includes sorry])])
AC_CHECK_HEADERS([elf.h gelf.h libelf.h], , [AC_MSG_ERROR([Can't find elf headers. Try to install elfutils-libelf-devel])])
AC_CHECK_HEADERS([libdwarf/dwarf.h libdwarf/libdwarf.h], , [AC_MSG_ERROR([Can't find dwarf headers. Try to install libdwarf-devel])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strerror])

#AC_OUTPUT(Makefile src/Makefile examples/Makefile tests/Makefile)
AC_OUTPUT(Makefile src/Makefile examples/Makefile)

