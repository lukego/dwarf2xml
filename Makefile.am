AUTOMAKE_OPTIONS = foreign
# SUBDIRS = src examples tests
SUBDIRS = src examples

dist_doc_DATA = index.html dwarf_basic.xslt dwarf.css dwarf-v4.01.dtd

TAR_SET = \
config.h.in \
configure \
configure.ac \
dwarf_basic.xslt \
dwarf.css \
dwarf-v4.01.dtd \
examples/test_dwarf_enum.c \
examples/test_dwarf_fcnt.c \
examples/Makefile.am \
examples/Makefile.in \
tests/test_xml.exp \
tests/line_on_demand.sh \
tests/Makefile \
index.html \
install-sh \
missing \
depcomp \
Makefile.am \
Makefile.in \
src/dwarf2xml.c \
src/dwarf2xml.h \
src/dwarf_attr_tbl.c \
src/dwarf_forms.c \
src/dwarf_tag_tbl.c \
src/exception.h \
src/exception_stderr.h \
src/excp_mods.h \
src/ring.c \
src/ring.h \
src/Makefile.am \
src/Makefile.in \
src/at_list.awk 

# a rule to make snapshots
dwarf2xml-v$(VERSION).tar.gz: $(TAR_SET)
	@echo
	@echo "->Buildind dwarf2xml-v$(VERSION).tar.gz"
	@echo "->copying all release files to the directory package"
	rm -Rf dwarf2xml-v$(VERSION) dwarf2xml-v$(VERSION).tar.gz
	mkdir dwarf2xml-v$(VERSION)
	tar --dereference -cf dwarf2xml-v$(VERSION).tar $(TAR_SET)
	cd dwarf2xml-v$(VERSION); tar -xf ../dwarf2xml-v$(VERSION).tar
	rm dwarf2xml-v$(VERSION).tar
	@echo "->making the compressed tar file " dwarf2xml-v$(VERSION).tar.gz
	tar -czf dwarf2xml-v$(VERSION).tar.gz dwarf2xml-v$(VERSION)
	rm -Rf dwarf2xml-v$(VERSION)
	@echo "->done"

tar: dwarf2xml-v$(VERSION).tar.gz

www: dwarf2xml-v$(VERSION).tar.gz dwarf_basic.xslt dwarf.css dwarf-v4.01.dtd index.html
	@echo "WEB_INT: $$WEB_INT"
	@[ x$$WEB_INT == "x" ] && { echo "!! Set environement variable WEB_INT to dest dir, please" 1>&2; exit 1; }; true
	find $$WEB_INT/www/code/dwarf2xml -type l -exec rm {} \; || true
	rm -Rf $$WEB_INT/www/code/dwarf2xml
	mkdir $$WEB_INT/www/code/dwarf2xml $$WEB_INT/www/code/dwarf2xml/examples
	ln -s $$(pwd)/dwarf2xml.arc/dwarf2xml-v1.01.tar.gz $$WEB_INT/www/code/dwarf2xml
	ln -s $$(pwd)/dwarf2xml-v$(VERSION).tar.gz $$WEB_INT/www/code/dwarf2xml
	ln -s $$(pwd)/dwarf_basic.xslt $$WEB_INT/www/code/dwarf2xml
	ln -s $$(pwd)/dwarf.css $$WEB_INT/www/code/dwarf2xml
	ln -s $$(pwd)/dwarf-v4.01.dtd $$WEB_INT/www/code/dwarf2xml
	ln -s $$(pwd)/index.html $$WEB_INT/www/code/dwarf2xml
	make -C examples www

# tar -xvzf /home/manu/projets/dwarf2xml/dwarf2xml-v4.01.tar.gz  && cd dwarf2xml-v4.01 && ./configure && make install
# make uninstall && cd .. && rm -Rf dwarf2xml-v4.01


