# Need to load libelf package
if (NOT LIBELF_FOUND)
  find_package (LibElf REQUIRED)
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  # We only want to find thirdparty libdwarf from /usr/local
  find_path (LIBDWARF_INCLUDE_DIRS
    NAMES
      libdwarf.h
    NO_DEFAULT_PATH
    PATHS
      /usr/local/include
      /usr/local/include/libdwarf
      ENV CPATH
  )

  find_library (LIBDWARF_LIBRARIES
    NAMES
      dwarf
    NO_DEFAULT_PATH
    PATHS
      /usr/local/lib
      ENV LIBRARY_PATH
      ENV LD_LIBRARY_PATH
  )
else()
  find_path (LIBDWARF_INCLUDE_DIRS
    NAMES
      libdwarf.h
    PATHS
      /usr/include
      /usr/include/libdwarf
      /usr/local/include
      /usr/local/include/libdwarf
      /opt/local/include
      /opt/local/include/libdwarf
      /sw/include
      /sw/include/libdwarf
      ENV CPATH
  )

  find_library (LIBDWARF_LIBRARIES
    NAMES
      dwarf
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      ENV LIBRARY_PATH
      ENV LD_LIBRARY_PATH
  )
endif()

include (FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibDwarf DEFAULT_MSG
  LIBDWARF_LIBRARIES
  LIBDWARF_INCLUDE_DIRS
)

