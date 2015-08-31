# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -g3 -Wall -DEXCP_INCLUDE="\"exception_stderr.h\"" -I@DWARF_INC@
AM_LDFLAGS = -L@DWARF_LIB@

DWARF_ATT_PREF = 
DWARF_ATT_PREF += DW_AT_ DW_FORM_ DW_LANG_
DWARF_ATT_PREF += DW_OP_ DW_ATE_ DW_DS_ DW_END_ DW_ACCESS_
DWARF_ATT_PREF += DW_VIS_ DW_VIRTUALITY_ DW_ID_ DW_CC_ DW_INL_
DWARF_ATT_PREF += DW_ORD_ DW_DSC_ DW_LNS_ DW_LNE_ DW_MACINFO_
DWARF_ATT_PREF += DW_CFA_ DW_EH_PE_

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = dwarf2xml
dwarf2xml_SOURCES  = dwarf_val_2_str.c dwarf_forms.c dwarf2xml.c dwarf_attr_tbl.c dwarf_tag_tbl.c ring.c
dwarf2xml_SOURCES += dwarf2xml.h dwarf_val_2_str.h exception.h exception_stderr.h excp_mods.h ring.h

dwarf_val_2_str.c : at_list.awk @DWARF_INC@/dwarf.h Makefile
	rm -f $@;
	echo "/* DO NOT EDIT, This file is generateur from @DWARF_INC@/dwarf.h */" > $@
	echo "#include <dwarf.h>" >> $@
	echo "#include <dwarf_val_2_str.h>" >> $@

	echo "" >> $@
	for pref in $(DWARF_ATT_PREF); do { \
	   $(AWK) -v prefix=$$pref -f at_list.awk @DWARF_INC@/dwarf.h; \
	} done | sed -n -e "/DW_CFA_extended/ d; p" >> $@
dwarf_val_2_str.h : dwarf_val_2_str.c Makefile
	rm -f $@
	echo "/* DO NOT EDIT, This file is generateur from @DWARF_INC@/dwarf.h */" > $@
	echo -e "struct s_val_2_str {\n  int val;\n  char *str;\n};\n" >> $@
	sed -e "/_tbl\[\] = {/ ! d; s/^/extern /; s/ *=.*$$/;/" dwarf_val_2_str.c >> $@

dwarf_val_2_str.o : dwarf_val_2_str.h
