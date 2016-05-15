if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  # We only want thirdparty libelf in /usr/local
  find_path (LIBELF_INCLUDE_DIRS
    NAMES
      libelf.h
    NO_DEFAULT_PATH
    PATHS
      /usr/local/include
      /usr/local/include/libelf
      ENV CPATH
  )

  find_library (LIBELF_LIBRARIES
    NAMES
      elf
    NO_DEFAULT_PATH
    PATHS
      /usr/local/lib
      ENV LIBRARY_PATH
      ENV LD_LIBRARY_PATH
  )
else()
  find_path (LIBELF_INCLUDE_DIRS
    NAMES
      libelf.h
    PATHS
      /usr/include
      /usr/include/libelf
      /usr/local/include
      /usr/local/include/libelf
      /opt/local/include
      /opt/local/include/libelf
      /sw/include
      /sw/include/libelf
      ENV CPATH
  )

  find_library (LIBELF_LIBRARIES
    NAMES
      elf
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

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibElf DEFAULT_MSG
  LIBELF_LIBRARIES
  LIBELF_INCLUDE_DIRS
)

