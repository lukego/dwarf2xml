# Copyright (C) 2013 by Emmanuel Azencot under the GNU GPL license 2.
#
# This GNU Makefile tests dwarf2xml using dwarfdump2 and dwarflib nonregression tests
# TS_PATH variable hold path to dwarflib nonregression tests
# see test_xml.exp for dwarfdump2 path

 
CFLAGS =  -g3
D2XFLAGS = -all
DWARF_INC = /usr/include/libdwarf

.PRECIOUS : %.dw2.xml %.dw3.xml %.dw4.xml %.xml

TESTSUITE_ELF= \
ia32/libc.so.6 \
cristi2/libc-2.5.so \
test-eh/test-eh.386 \
cristi2/libpthread-2.4.so \
mucci/main.gcc \
enciso2/template.elf \
ia32/mytry.ia32 \
ia64/hxdump.ia64 \
ia64/mytry.ia64 \
kartashev/combined.o \
kartashev2/combined.o \
sparc/tcombined.o \
cristi3/cristibadobj \
enciso2/test_templates.o \
enciso2/template.elf \
frame1/frame1.exe.save \
frame1/frame1.orig \
ia32/libpfm.so.3 \
ia32/mytry.ia32 \
ia32/preloadable_libintl.so \
ia64/hxdump.ia64 \
ia64/mytry.ia64 \
kartashev/combined.o \
macinfo/a.out3.4 \
macinfo/a.out4.3 \
modula2/write-fixed \
mucci/main.o \
mucci/main.o.gcc \
mucci/main.o.pathcc \
mucci/main.pathcc \
sandnes2/cu_dir_added_to_complete_path.elf \
test-alex1/orig.a.out \
test-alex2/orig.a.out \
test-array/test-array \
testcase/testcase \
zero/zero \
test-eh/eh-frame.386 \
irixn32/dwarfdump \
mutatee/test1.mutatee_gcc.exe \
ppc2/powerpc-750-linux-gnu-hello-static \
ref_addr/ELF3.elf \
sandnes/Test1.elf \
shihhuangti/t1.o \
shihhuangti/t2.o \
shihhuangti/tcombined.o \
sparc/tcombined.o  \
sun/sparc1-a.out \
val_expr/libpthread-2.5.so \
x86/dwarfdumpv4.3 \
wynn/unoptimised.axf \
allen1/todd-allen-gcc-4.4.4-bin.exe \
arm/armcc-test-dwarf2 \
arm/armcc-test-dwarf3 \
louzon/ppcobj.o \
atefail/ig_server \
irix64/libc.so \
irixn32/libc.so \
ia32/libpt_linux_x86_r.so.1 \
cell/c_malloc.o


# More than 80 tag levels : sun/sunelf1 \
# broken, dwarfdump moore/djpeg.v850 : dwarfdump ERROR:  dwarf_linesrc:  DW_DLE_LINE_FILE_NUM_BAD (61)
# broken, only interesting for aranges, but dwarf2xml produce undefines ref with -a option. 
# lloyd/arange.elf 
# Did my best, but at_sibling, 1 st CU have a bizare encoding ref4 form that not fit in dwarf_global_formref
# linkonce/comdattest.o

TESTSUITE_SRC=\
test-eh/test-eh.c \
test-eh/eh-frame.cc \
zero/zero.cc \
testcase/testcase.c \
test-array/array.c \
moore/simplec.c \
macinfo/test.c \
linkonce/test.cpp \
enciso2/test_templates.cpp \
cristi3/foo.cpp

TS_PATH=../Ressources/regressiontests-20100930
TESTSUITE_ELF_HTML=$(subst /,--,$(TESTSUITE_ELF:%=%.html))
TESTSUITE_SRC_HTML=\
$(subst /,--,$(addsuffix .dw2.html, $(basename $(TESTSUITE_SRC)))) \
$(subst /,--,$(addsuffix .dw3.html, $(basename $(TESTSUITE_SRC)))) \
$(subst /,--,$(addsuffix .dw4.html, $(basename $(TESTSUITE_SRC))))

all:  testsuite_src_html $(TESTSUITE_ELF_HTML)

testsuite_elf_html: $(TESTSUITE_ELF_HTML)
	@echo $(TESTSUITE_ELF_HTML)
testsuite_src_html: $(TESTSUITE_SRC_HTML)
	@echo $(TESTSUITE_SRC_HTML)

clean: 
	rm  -f *.html *.xml *.elf *.[234]-html *.[234]-xml *.dw[234] *~

cristi3--foo.dw2 : $(TS_PATH)/cristi3/foo.cpp
	$(CXX) -c -gdwarf-2 $(CFLAGS) $^ $(LDFLAGS) -o $@
cristi3--foo.dw3 : $(TS_PATH)/cristi3/foo.cpp
	$(CXX) -c -gdwarf-3 $(CFLAGS) $^ $(LDFLAGS) -o $@
cristi3--foo.dw4 : $(TS_PATH)/cristi3/foo.cpp
	$(CXX) -c -gdwarf-4 $(CFLAGS) $^ $(LDFLAGS) -o $@

enciso2--test_templates.dw2 : $(TS_PATH)/enciso2/test_templates.cpp
	$(CXX) -gdwarf-2 $(CFLAGS) $^ $(LDFLAGS) -o $@
enciso2--test_templates.dw3 : $(TS_PATH)/enciso2/test_templates.cpp
	$(CXX) -gdwarf-3 $(CFLAGS) $^ $(LDFLAGS) -o $@
enciso2--test_templates.dw4 : $(TS_PATH)/enciso2/test_templates.cpp
	$(CXX) -gdwarf-4 $(CFLAGS) $^ $(LDFLAGS) -o $@

linkonce--test.dw2 : $(TS_PATH)/linkonce/test.cpp
	$(CXX) -c -gdwarf-2 $(CFLAGS) $^ $(LDFLAGS) -o $@
linkonce--test.dw3 : $(TS_PATH)//linkonce/test.cpp
	$(CXX) -c -gdwarf-3 $(CFLAGS) $^ $(LDFLAGS) -o $@
linkonce--test.dw4 : $(TS_PATH)//linkonce/test.cpp
	$(CXX) -c -gdwarf-4 $(CFLAGS) $^ $(LDFLAGS) -o $@

macinfo--test.dw2 : $(TS_PATH)/macinfo/test.c
	$(CC) -gdwarf-2 $(CFLAGS) $^ $(LDFLAGS) -o $@
macinfo--test.dw3 : $(TS_PATH)/macinfo/test.c
	$(CC) -gdwarf-3 $(CFLAGS) $^ $(LDFLAGS) -o $@
macinfo--test.dw4 : $(TS_PATH)/macinfo/test.c
	$(CC) -gdwarf-4 $(CFLAGS) $^ $(LDFLAGS) -o $@

moore--simplec.dw2 : $(TS_PATH)/moore/simplec.c
	$(CC) -c -gdwarf-2 $(CFLAGS) $^ $(LDFLAGS) -o $@
moore--simplec.dw3 : $(TS_PATH)/moore/simplec.c
	$(CC) -c -gdwarf-3 $(CFLAGS) $^ $(LDFLAGS) -o $@
moore--simplec.dw4 : $(TS_PATH)/moore/simplec.c
	$(CC) -c -gdwarf-4 $(CFLAGS) $^ $(LDFLAGS) -o $@

test-array--array.dw2 : $(TS_PATH)/test-array/array.c
	$(CC) -gdwarf-2 $(CFLAGS) $^ $(LDFLAGS) -o $@
test-array--array.dw3 : $(TS_PATH)/test-array/array.c
	$(CC) -gdwarf-3 $(CFLAGS) $^ $(LDFLAGS) -o $@
test-array--array.dw4 : $(TS_PATH)/test-array/array.c
	$(CC) -gdwarf-4 $(CFLAGS) $^ $(LDFLAGS) -o $@

testcase--testcase.dw2 : $(TS_PATH)/testcase/testcase.c
	$(CC) -gdwarf-2 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf
testcase--testcase.dw3 : $(TS_PATH)/testcase/testcase.c
	$(CC) -gdwarf-3 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf
testcase--testcase.dw4 : $(TS_PATH)/testcase/testcase.c
	$(CC) -gdwarf-4 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf

zero--zero.dw2 : $(TS_PATH)/zero/zero.cc
	$(CXX) -gdwarf-2 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf 
zero--zero.dw3 : $(TS_PATH)/zero/zero.cc
	$(CXX) -gdwarf-3 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf
zero--zero.dw4 : $(TS_PATH)/zero/zero.cc
	$(CXX) -gdwarf-4 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf

test-eh--eh-frame.dw2 : $(TS_PATH)/test-eh/eh-frame.cc
	$(CXX) -gdwarf-2 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf 
test-eh--eh-frame.dw3 : $(TS_PATH)/test-eh/eh-frame.cc
	$(CXX) -gdwarf-3 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf
test-eh--eh-frame.dw4 : $(TS_PATH)/test-eh/eh-frame.cc
	$(CXX) -gdwarf-4 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf

test-eh--test-eh.dw2 : $(TS_PATH)/test-eh/test-eh.c
	$(CC) -DUSE_ELF_POINTER -gdwarf-2 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf
test-eh--test-eh.dw3 : $(TS_PATH)/test-eh/test-eh.c
	$(CC) -DUSE_ELF_POINTER -gdwarf-3 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf
test-eh--test-eh.dw4 : $(TS_PATH)/test-eh/test-eh.c
	$(CC) -DUSE_ELF_POINTER -gdwarf-4 $(CFLAGS) -I$(DWARF_INC) $^ $(LDFLAGS) -o $@ -ldwarf -lelf

enciso2--test_templates.o.exs : $(TS_PATH)/enciso2/test_templates.o
frame1--frame1.exe.save.exs : $(TS_PATH)/frame1/frame1.exe.save
frame1--frame1.orig.exs : $(TS_PATH)/frame1/frame1.orig
ia32--libc.so.6.exs : $(TS_PATH)/ia32/libc.so.6
ia32--libpfm.so.3.exs : $(TS_PATH)/ia32/libpfm.so.3
ia32--libpt_linux_x86_r.so.1.exs : $(TS_PATH)/ia32/libpt_linux_x86_r.so.1
ia32--mytry.ia32.exs : $(TS_PATH)/ia32/mytry.ia32
ia32--preloadable_libintl.so.exs : $(TS_PATH)/ia32/preloadable_libintl.so
linkonce--comdattest.o.exs : $(TS_PATH)/linkonce/comdattest.o
macinfo--a.out3.4.exs : $(TS_PATH)/macinfo/a.out3.4
macinfo--a.out4.3.exs : $(TS_PATH)/macinfo/a.out4.3
modula2--write-fixed.exs : $(TS_PATH)/modula2/write-fixed
moore--djpeg.v850.exs : $(TS_PATH)/moore/djpeg.v850
mucci--main.gcc.exs : $(TS_PATH)/mucci/main.gcc
mucci--main.o.exs : $(TS_PATH)/mucci/main.o
mucci--main.o.gcc.exs : $(TS_PATH)/mucci/main.o.gcc
mucci--main.o.pathcc.exs : $(TS_PATH)/mucci/main.o.pathcc
mucci--main.pathcc.exs : $(TS_PATH)/mucci/main.pathcc
sandnes2--cu_dir_added_to_complete_path.elf.exs : $(TS_PATH)/sandnes2/cu_dir_added_to_complete_path.elf
test-alex1--orig.a.out.exs : $(TS_PATH)/test-alex1/orig.a.out
test-alex2--orig.a.out.exs : $(TS_PATH)/test-alex2/orig.a.out
test-array--test-array.exs : $(TS_PATH)/test-array/test-array
testcase--testcase.exs : $(TS_PATH)/testcase/testcase
zero--zero.exs : $(TS_PATH)/zero/zero
test-eh--test-eh.386.exs : $(TS_PATH)/test-eh/test-eh.386
test-eh--eh-frame.386.exs : $(TS_PATH)/test-eh/eh-frame.386
allen1--todd-allen-gcc-4.4.4-bin.exe.exs : $(TS_PATH)/allen1/todd-allen-gcc-4.4.4-bin.exe
atefail--ig_server.exs : $(TS_PATH)/atefail/ig_server
cell--c_malloc.o.exs : $(TS_PATH)/cell/c_malloc.o
cristi2--libc-2.5.so.exs : $(TS_PATH)/cristi2/libc-2.5.so
cristi2--libpthread-2.4.so.exs : $(TS_PATH)/cristi2/libpthread-2.4.so
irix64--libc.so.exs : $(TS_PATH)/irix64/libc.so
irixn32--libc.so.exs : $(TS_PATH)/irixn32/libc.so
irixn32--dwarfdump.exs : $(TS_PATH)/irixn32/dwarfdump
lloyd--arange.elf.xml : D2XFLAGS = -a
lloyd--arange.elf.exs : $(TS_PATH)/lloyd/arange.elf
louzon--ppcobj.o.exs : $(TS_PATH)/louzon/ppcobj.o
mutatee--test1.mutatee_gcc.exe.exs : $(TS_PATH)/mutatee/test1.mutatee_gcc.exe
ppc2--powerpc-750-linux-gnu-hello-static.exs : $(TS_PATH)/ppc2/powerpc-750-linux-gnu-hello-static
ref_addr--ELF3.elf.exs : $(TS_PATH)/ref_addr/ELF3.elf
sandnes--Test1.elf.exs : $(TS_PATH)/sandnes/Test1.elf
shihhuangti--t1.o.exs : $(TS_PATH)/shihhuangti/t1.o
shihhuangti--t2.o.exs : $(TS_PATH)/shihhuangti/t2.o
shihhuangti--tcombined.o.exs : $(TS_PATH)/shihhuangti/tcombined.o
sun--sparc1-a.out.exs : $(TS_PATH)/sun/sparc1-a.out
sun--sunelf1.exs : $(TS_PATH)/sun/sunelf1
val_expr--libpthread-2.5.so.exs : $(TS_PATH)/val_expr/libpthread-2.5.so
x86--dwarfdumpv4.3.exs : $(TS_PATH)/x86/dwarfdumpv4.3
wynn--unoptimised.axf.exs : $(TS_PATH)/wynn/unoptimised.axf

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

# e.g. : arm/armcc-test-dwarf2 -> arm_armcc-test-dwarf2.xml
# $(subst /,_,$(TESTSUITE_EXEMPLES:%=%.xml)) : $(TESTSUITE_EXEMPLES:%=$(TS_PATH)/%)
%.html : %.exs
	src=$$(echo "$(@:.html=)" | sed -e 's@--@/@; '); \
	../src/dwarf2xml $(D2XFLAGS) $(TS_PATH)/$$src > $(@:.html=.xml) || rm $(@:.html=.xml) 123
	xmllint --path "../" --valid $(@:.html=.xml) 1>/dev/null
	./test_xml.exp $(@:.html=.xml) $(TS_PATH)/$$(echo "$(@:.html=)" | sed -e 's@--@/@; ') 2>&1 1>/dev/null
	xsltproc --novalid -o $@ ../dwarf_basic.xslt $(@:.html=.xml)
	#rm -f $@ $(@:.html=.xml); touch $@

# Makefile
# .c.o :  
# 	$(CC) $(CFLAGS) -c -o $@ $<

%.dw2 : %.c
	$(CC) -gdwarf-2 $(CFLAGS) $^ $(LDFLAGS) -o $@

%.dw2.xml : %.dw2
	../src/dwarf2xml $(D2XFLAGS) $< > $@ || rm $@ not_found_file
	xmllint --path "../" --valid $@ 1>/dev/null || rm $@ not_found_file

%.dw3 : %.c
	$(CC) -gdwarf-3 $(CFLAGS) $^ $(LDFLAGS) -o $@

%.dw3.xml : %.dw3
	../src/dwarf2xml $(D2XFLAGS) $< > $@ || rm $@ not_found_file
	xmllint --path "../" --valid $@ 1>/dev/null || rm $@ not_found_file

%.dw4 : %.c
	$(CC) -gdwarf-4 $(CFLAGS) $^ $(LDFLAGS) -o $@

%.dw4.xml : %.dw4
	../src/dwarf2xml $(D2XFLAGS) $< > $@ || rm $@ not_found_file
	xmllint --path "../" --valid $@ 1>/dev/null || rm $@ not_found_file

%.html : %.xml
	./test_xml.exp $<  $(<:.xml=)  2>&1 1>/dev/null
	xsltproc --novalid -o $@ ../dwarf_basic.xslt  $<
	#rm -f $@ $< $(<:.xml=); touch $@

