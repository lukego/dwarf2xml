exampledir = $(datarootdir)/doc/@PACKAGE@/examples
example_DATA = \
test_dwarf_enum.dw2.xml test_dwarf_fcnt.dw2.xml \
test_dwarf_enum.dw3.xml test_dwarf_fcnt.dw3.xml \
test_dwarf_enum.dw4.xml test_dwarf_fcnt.dw4.xml \
test_dwarf_enum.dw2.html test_dwarf_fcnt.dw2.html \
test_dwarf_enum.dw3.html test_dwarf_fcnt.dw3.html \
test_dwarf_enum.dw4.html test_dwarf_fcnt.dw4.html

CFLAGS =  -g3
D2XFLAGS = -all

.PRECIOUS : %.xml %.html
SUFFIXES =  .link .html .xml

EXEMPLES_ELF= \
arm/armcc-test-dwarf2 \
arm/armcc-test-dwarf3 \
cristi3/cristibadobj \
enciso2/template.elf \
ia32/mytry.ia32 \
ia64/hxdump.ia64 \
ia64/mytry.ia64 \
kartashev/combined.o \
kartashev2/combined.o \
sparc/tcombined.o 

TS_PATH=../Ressources/regressiontests-20100930
EXEMPLES_ELF_HTML=$(subst /,--,$(EXEMPLES_ELF:%=%.html))
EXEMPLES_ELF_XML=$(subst /,--,$(EXEMPLES_ELF:%=%.xml))

#TESTSUITE_XML=$(subst /,_,$(TESTSUITE_EXEMPLES:%=%.xml))
#TESTSUITE_ELF_HTML=$(patsubst /,_,$(TESTSUITE_EXEMPLES:%=%.html))

all: examples_src_html
examples_elf_html: $(EXEMPLES_ELF_XML) $(EXEMPLES_ELF_HTML)
examples_src_html:  $(example_DATA) 

clean: 
	rm -f *.html *.xml *.dw[234] *~

arm--armcc-test-dwarf2.exs : $(TS_PATH)/arm/armcc-test-dwarf2
arm--armcc-test-dwarf3.exs : $(TS_PATH)/arm/armcc-test-dwarf3
cristi3--cristibadobj.exs : $(TS_PATH)/cristi3/cristibadobj
enciso2--template.elf.exs : $(TS_PATH)/enciso2/template.elf
ia32--mytry.ia32.exs : $(TS_PATH)/ia32/mytry.ia32
ia64--hxdump.ia64.exs : $(TS_PATH)/ia64/hxdump.ia64
ia64--mytry.ia64.exs : $(TS_PATH)/ia64/mytry.ia64
kartashev--combined.o.exs : $(TS_PATH)/kartashev/combined.o
kartashev2--combined.o.exs : $(TS_PATH)/kartashev2/combined.o
sparc--tcombined.o.exs : $(TS_PATH)/sparc/tcombined.o 

.exs.xml : 
	../src/dwarf2xml $(D2XFLAGS) $(TS_PATH)/$(subst --,/,$(<:%.exs=%)) > $@
	xmllint --path "../" --valid $@ 1>/dev/null

.xml.html : 
	xsltproc --novalid -o $@ ../dwarf_basic.xslt  $<

www.clean: 
	@echo "-> cleaning $$WEB_INT/www/code/dwarf2xml/examples/"
	@[ x$$WEB_INT == "x" ] && { echo "!! Set environement variable WEB_INT to dest dir, please" 1>&2; exit 1; }; true
	find $$WEB_INT/www/code/dwarf2xml/examples -type l -exec rm {} \;
	rm -Rf $$WEB_INT/www/code/dwarf2xml/examples/*

%.html.link : %.html
	@echo "-> linking $$(pwd)/$(@:%.link=%.xml) and $$(pwd)/$(@:%.link=%.html) to $$WEB_INT/www/code/dwarf2xml/examples/"
	@[ x$$WEB_INT == "x" ] && { echo "!! Set environement variable WEB_INT to dest dir, please" 1>&2; exit 1; }; true
	ln -s $$(pwd)/$(@:%.link=%.xml) $$WEB_INT/www/code/dwarf2xml/examples/
	ln -s $$(pwd)/$(@:%.link=%.html) $$WEB_INT/www/code/dwarf2xml/examples/

www:  www.clean $(addsuffix .link,$(EXEMPLES_ELF_HTML) $(filter %.html,$(example_DATA))) 
	ln -s $(abs_top_srcdir)/dwarf.css $$WEB_INT/www/code/dwarf2xml/examples/


.c.dw4 : 
	$(CC) -gdwarf-4 $(CFLAGS) $^ $(LDFLAGS) -o $@
%.dw4.xml : %.dw4
	../src/dwarf2xml $(D2XFLAGS) $< > $@ || rm $@ not_found_file
	xmllint --path "../" --valid $@ 1>/dev/null || rm $@ not_found_file

.c.dw3 : 
	$(CC) -gdwarf-3 $(CFLAGS) $^ $(LDFLAGS) -o $@
%.dw3.xml : %.dw3
	../src/dwarf2xml $(D2XFLAGS) $< > $@ || rm $@ not_found_file
	xmllint --path "../" --valid $@ 1>/dev/null || rm $@ not_found_file

.c.dw2 : 
	$(CC) -gdwarf-2 $(CFLAGS) $^ $(LDFLAGS) -o $@
%.dw2.xml : %.dw2
	../src/dwarf2xml $(D2XFLAGS) $< > $@ || rm $@ not_found_file
	xmllint --path "../" --valid $@ 1>/dev/null || rm $@ not_found_file



