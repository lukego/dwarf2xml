find_package(LibDwarf REQUIRED)

# dwarf.h header to scrape
set(DWARF_H_PATH "${LIBDWARF_INCLUDE_DIRS}/dwarf.h")

##### Generate dwarf_val_to_str.c
set(DWARF_TABLE_SRC ${CMAKE_CURRENT_BINARY_DIR}/dwarf_val_to_str.c) # Generated file
# Prefixes to scrape from dwarf.h
set(DWARF_PREFIXES
  DW_AT_ DW_FORM_ DW_LANG_
  DW_OP_ DW_ATE_ DW_DS_ DW_END_ DW_ACCESS_
  DW_VIS_ DW_VIRTUALITY_ DW_ID_ DW_CC_ DW_INL_
  DW_ORD_ DW_DSC_ DW_LNS_ DW_LNE_ DW_MACINFO_
  DW_CFA_ DW_EH_PE_
)

# "Header" portion of the generated file
file(WRITE ${DWARF_TABLE_SRC} "/* This file is generaged from ${DWARF_H_PATH} */\n")
file(APPEND ${DWARF_TABLE_SRC} "#include <dwarf.h>\n")
file(APPEND ${DWARF_TABLE_SRC} "#include \"dwarf_val_to_str.h\"\n\n")

# Initializers
foreach(PREFIX ${DWARF_PREFIXES})
execute_process(
  COMMAND awk -v prefix=${PREFIX} -f ${CMAKE_CURRENT_SOURCE_DIR}/at_list.awk
  COMMAND sed -n -e "/DW_CFA_extended/ d; p"
  INPUT_FILE ${DWARF_H_PATH}
  OUTPUT_VARIABLE GEN_SECTION
)
file(APPEND ${DWARF_TABLE_SRC} "${GEN_SECTION}\n")
endforeach(PREFIX)

##### Generate dwarf_val_to_str.h
set(DWARF_TABLE_HEADER ${CMAKE_CURRENT_BINARY_DIR}/dwarf_val_to_str.h) # Generated file
file(WRITE ${DWARF_TABLE_HEADER} "/* This file is generaged from ${DWARF_H_PATH} */\n")
file(APPEND ${DWARF_TABLE_HEADER} "struct s_val_2_str {\n  int val;\n  char *str;\n};\n\n")
execute_process(
  COMMAND sed -e "/_tbl\\[\\] = {/ ! d; s/^/extern /; s/ *=.*$/;/"
  INPUT_FILE ${DWARF_TABLE_SRC}
  OUTPUT_VARIABLE DEFINITIONS
)
file(APPEND ${DWARF_TABLE_HEADER} "${DEFINITIONS}")

# Pull in generated header
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${LIDWARF_INCLUDE_DIRS})

add_executable(dwarf2xml
  dwarf_val_to_str.c
  dwarf_forms.c
  dwarf2xml.c
  dwarf_attr_tbl.c
  dwarf_tag_tbl.c
  ring.c
)

## Thirdparty libelf breaks the build on FreeBSD, but clearly there needs to be a way to get it to work
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
  target_include_directories(dwarf2xml
    PRIVATE
    ${LIBELF_INCLUDE_DIRS}
  )
endif()

target_include_directories(dwarf2xml
  PRIVATE
  ${LIBDWARF_INCLUDE_DIRS}
)

target_link_libraries(dwarf2xml
  ${LIBELF_LIBRARIES}
  ${LIBDWARF_LIBRARIES}
)
