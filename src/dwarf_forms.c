/* 
 * Copyright (C) 2010-2013 by Emmanuel Azencot under the GNU GPL license 2.
 * You should have received a copy of the licence 
 * 
 *                          Version 2, June 1991
 *      Copyright (C) 1989, 1991 Free Software Foundation, Inc.
 *      675 Mass Ave, Cambridge, MA 02139, USA
 *      
 * GPL license along with this software os. if you did not you can find it
 * at http://www.gnu.org/.
 *
 * # 
 * # # project hosted at http://machinman.net/code/dwarf2xml
 * #
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dwarf.h>
#include <libdwarf.h>

#include "dwarf2xml.h"

/*
 TODO : fixme m_libdwarf_assert return without desallocation 
*/
int f_dw_form_addr(Dwarf_Debug debug, Dwarf_Attribute attribute) {
   Dwarf_Error error;
   Dwarf_Addr addr;
   m_libdwarf_assert(dwarf_formaddr(attribute, &addr, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   printf(v_elf_arch.fmt_addr, v_elf_arch.mask & addr);

   return DW_DLV_OK;
}
int f_dw_form_flag(Dwarf_Debug debug, Dwarf_Attribute attribute) {
   Dwarf_Error error;
   Dwarf_Bool bool;

   m_libdwarf_assert(dwarf_formflag(attribute, &bool, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   printf("%s", bool?"true":"false");

   return DW_DLV_OK;
}
int f_dw_form_string(Dwarf_Debug debug, Dwarf_Attribute attribute) {
   Dwarf_Error error;
   char *dw_str, *dw_str_h;

   m_libdwarf_assert(dwarf_formstring(attribute, &dw_str, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR );
   dw_str_h = escape_html(dw_str);
   printf("%s", dw_str_h);
   free(dw_str_h);
   dwarf_dealloc(debug, dw_str, DW_DLA_STRING);

   return DW_DLV_OK;
}
int f_dw_form_block(Dwarf_Debug debug, Dwarf_Attribute attribute) {
   Dwarf_Error error;
   Dwarf_Block *pblock;
   int i; char *p;

   m_libdwarf_assert(dwarf_formblock(attribute, &pblock, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   if ( pblock->bl_from_loclist ) {
      printf("loc list off=0x%02llx", pblock->bl_section_offset);
   } else {
      printf("%s", "[");
      for (p = (char *)pblock->bl_data, i = 0; i < pblock->bl_len; i++ ) {
         printf("%02x ", *p++ & 0xFF );
      }
      printf("%s", "]");
   }
   dwarf_dealloc(debug, pblock, DW_DLA_BLOCK);
   return DW_DLV_OK;
}
int f_dw_form_udata(Dwarf_Debug debug, Dwarf_Attribute attribute) {
   Dwarf_Error error;
   Dwarf_Unsigned uvalue;

   m_libdwarf_assert(dwarf_formudata(attribute, &uvalue, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   printf("%llu", uvalue);

   return DW_DLV_OK;
}
int f_dw_form_sdata(Dwarf_Debug debug, Dwarf_Attribute attribute) {
   Dwarf_Error error;
   Dwarf_Signed svalue;

   m_libdwarf_assert(dwarf_formsdata(attribute, &svalue, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   printf("%lld", svalue);

   return DW_DLV_OK;
}
int f_dw_form_glob_ref(Dwarf_Debug debug, Dwarf_Attribute attribute) {
   Dwarf_Error error;
   //Dwarf_Unsigned offset;
   Dwarf_Off offset;

   //m_libdwarf_assert(dwarf_formudata(attribute, &offset, &error) !=  DW_DLV_ERROR,return DW_DLV_ERROR);
   m_libdwarf_assert(dwarf_global_formref(attribute, &offset, &error) !=  DW_DLV_ERROR,return DW_DLV_ERROR);
   printf("%llu", offset);
//fprintf(stderr, "External global ref %llu\n", offset);

   return DW_DLV_OK;
}

int f_dw_form_loc_ref(Dwarf_Debug debug, Dwarf_Attribute attribute) {
   Dwarf_Error error;
   Dwarf_Off offset;
//fprintf(stderr, "External loc ref\n");

   if ( dwarf_global_formref(attribute, &offset, &error) ==  DW_DLV_ERROR ) 
      // break point design patern, lol
      m_libdwarf_assert(dwarf_global_formref(attribute, &offset, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);

   printf("%llu", offset);
//fprintf(stderr, "External local ref %llu\n", offset);

   return DW_DLV_OK;
}


struct s_dw_form {
   Dwarf_Half form;
   char *str;
   int (*fnct)(Dwarf_Debug debug, Dwarf_Attribute attribute);
};


static struct s_dw_form c_dw_form_fnctbl[] = {
  { /* 0x01 */ DW_FORM_addr, "addr", &f_dw_form_addr  },
  { /* 0x03 */ DW_FORM_block2, "block2", &f_dw_form_block  },
  { /* 0x04 */ DW_FORM_block4, "block4", &f_dw_form_block  },
  { /* 0x05 */ DW_FORM_data2, "data2", &f_dw_form_udata  },
  { /* 0x06 */ DW_FORM_data4, "data4", &f_dw_form_udata  },
  { /* 0x07 */ DW_FORM_data8, "data8", &f_dw_form_udata  },
  { /* 0x08 */ DW_FORM_string, "string", &f_dw_form_string  },
  { /* 0x09 */ DW_FORM_block, "block", &f_dw_form_block  },
  { /* 0x0a */ DW_FORM_block1, "block1", &f_dw_form_block  },
  { /* 0x0b */ DW_FORM_data1, "data1", &f_dw_form_udata  },
  { /* 0x0c */ DW_FORM_flag, "flag", &f_dw_form_flag  },
  { /* 0x0d */ DW_FORM_sdata, "sdata", &f_dw_form_sdata  },
  { /* 0x0e */ DW_FORM_strp, "strp", &f_dw_form_string  },
  { /* 0x0f */ DW_FORM_udata, "udata", &f_dw_form_udata  },
  { /* 0x10 */ DW_FORM_ref_addr, "ref_addr", &f_dw_form_loc_ref  },
  { /* 0x11 */ DW_FORM_ref1, "ref1", &f_dw_form_loc_ref  },
  { /* 0x12 */ DW_FORM_ref2, "ref2", &f_dw_form_loc_ref  },
  { /* 0x13 */ DW_FORM_ref4, "ref4", &f_dw_form_loc_ref  },
  { /* 0x14 */ DW_FORM_ref8, "ref8", &f_dw_form_loc_ref  },
  { /* 0x15 */ DW_FORM_ref_udata, "ref_udata", &f_dw_form_loc_ref  },
  { /* 0x16 */ DW_FORM_indirect, "indirect", 0  },
  { /* 0x17 */ DW_FORM_sec_offset, "sec_offset", &f_dw_form_glob_ref  },          /* DWARF4 */
#if 0
  { /* 0x18 */ DW_FORM_exprloc, "exprloc", &f_dw_form_glob_ref },                 /* DWARF4 */
#endif
  { /* 0x19 */ DW_FORM_flag_present, "flag_present", &f_dw_form_flag },           /* DWARF4 */
#if 0
  /* 0x1a thru 0x1f were left unused accidentally. Reserved for future use. */
  { /* 0x20 */ DW_FORM_ref_sig8, "ref_sig8", &f_dw_form_glob_ref },               /* DWARF4 */
#endif
  { 0 }, 
};
char v_dw_form_str_unk[30];
struct s_dw_form v_dw_form_unk = { 0, &v_dw_form_str_unk[30], 0 };
const char *f_dw_form_2_str(Dwarf_Half form) {
   int ind;
   for ( ind = 0; ind < (sizeof(c_dw_form_fnctbl)/sizeof(c_dw_form_fnctbl[0])); ind++) {
      if ( form == c_dw_form_fnctbl[ind].form ) 
         return c_dw_form_fnctbl[ind].str;
   }
   sprintf(v_dw_form_str_unk, "form_0x%02x", form);
   return v_dw_form_str_unk;
}
struct s_dw_form *f_dw_form(Dwarf_Half form) {
   int ind;
   for ( ind = 0; ind < (sizeof(c_dw_form_fnctbl)/sizeof(c_dw_form_fnctbl[0])); ind++) {
      if ( form == c_dw_form_fnctbl[ind].form ) 
         return &c_dw_form_fnctbl[ind];
   }
   // sprintf(v_dw_form_str_unk, "form_0x%02x", form);
   return 0;
}

int f_dw_attr_value_2_str_with_form(Dwarf_Attribute attribute, Dwarf_Debug debug) {
   Dwarf_Error error;
   Dwarf_Half form;
   struct s_dw_form *p_form;

   m_libdwarf_assert(dwarf_whatform(attribute, &form, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);

   m_dwarf2xml_assert( (p_form = f_dw_form(form)), 
      fprintf(stderr, "(unknow form : 0x%02x)\n" , form); return DW_DLV_ERROR; )

   m_dwarf2xml_assert( p_form->fnct,
      fprintf(stderr, "(form %s still not implemented)\n", p_form->str); 
      m_error("(form %s still not implemented)", p_form->str)
      return DW_DLV_ERROR; )

   m_dwarf2xml_assert( p_form->fnct(debug, attribute ) !=  DW_DLV_ERROR,
      fprintf(stderr, "form_print_error\n"); return DW_DLV_ERROR; )
   

   return DW_DLV_OK;
}
