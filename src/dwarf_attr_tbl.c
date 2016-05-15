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
#include <assert.h>
#include <errno.h>
#include <dwarf.h>
#include <libdwarf.h>

#include "config.h"
#include "exception.h"
#include "dwarf2xml.h"
/*
 TODO : fixme m_libdwarf_assert return without desallocation 
*/

struct s_dw_attr_tbl_elem {
   Dwarf_Half attr;
   char *str;
   int (*fnct)(Dwarf_Debug debug, Dwarf_Attribute attribute, void *cookie, struct s_dwarf_cu_info *cu_info, char *indent);
   void *cookie;
};
char v_dward_unk_val[100];
const char *f_dw_val_2_str(int val, struct s_val_2_str *tbl) {
   while ( tbl->str ) {
// printf("look %d in %s\n", val, tbl->str);
      if ( tbl->val == val ) {
         return tbl->str;
      }
      tbl++;
   }
   sprintf(v_dward_unk_val, "unkown (%u)", val);
   return v_dward_unk_val;
}

int f_dw_form_udata_with_str(Dwarf_Debug debug, Dwarf_Attribute attribute, void *cookie, struct s_dwarf_cu_info *cu_info, char *indent) {
   Dwarf_Error error;
   Dwarf_Half attr;
   Dwarf_Unsigned uvalue;
   struct s_val_2_str *val_2_str = (struct s_val_2_str *)cookie;
   m_libdwarf_assert(dwarf_whatattr(attribute, &attr, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR );
   printf("%s<at_%s>",  indent, f_dw_attr_2_str(attr, 1));
   m_libdwarf_assert(dwarf_formudata(attribute, &uvalue, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   while ( val_2_str->str ) {
      if ( val_2_str->val == uvalue ) {
         printf("%s", val_2_str->str);
         printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));
         return DW_DLV_OK;
      }
      ++val_2_str;
   }
   printf("unkown (%llu)", uvalue);
   printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));
   return DW_DLV_OK;
}

int f_dw_print_loc_expr(char *indent, Dwarf_Locdesc *llbuf, Dwarf_Signed offset) {
   Dwarf_Half loc_e_i;
   if ( offset >= 0 ) {
      printf("%s<loc_expr id='loc:%llu'", indent, offset);
      printf(" low_pc='");
      printf(v_elf_arch.fmt_addr, v_elf_arch.mask & llbuf->ld_lopc);
      printf("' high_pc='");
      printf(v_elf_arch.fmt_addr, v_elf_arch.mask & llbuf->ld_hipc);
      printf("' >\n");
   } else {
      printf("%s<loc_expr>\n", indent);
   }
   indent -=2;
   for (loc_e_i = 0; loc_e_i < llbuf->ld_cents; ++loc_e_i) {
      switch ( llbuf->ld_s[loc_e_i].lr_atom ) {
      case DW_OP_deref:
      case DW_OP_dup ... DW_OP_over:
      case DW_OP_swap ... DW_OP_plus:
      case DW_OP_shl ... DW_OP_xor:
      case DW_OP_eq ... DW_OP_ne:
      case DW_OP_lit0 ... DW_OP_reg31:
      case DW_OP_nop:
      case DW_OP_push_object_address:
      case DW_OP_form_tls_address:
      case DW_OP_call_frame_cfa:
         printf("%s<loc_op pos='%llu' op='%s' />\n", 
                        indent, 
                        llbuf->ld_s[loc_e_i].lr_offset -1, 
                        f_dw_val_2_str(llbuf->ld_s[loc_e_i].lr_atom, c_dw_op_tbl));
         break;

      case DW_OP_addr:
      case DW_OP_const1u ... DW_OP_consts:
      case DW_OP_pick:
      case DW_OP_plus_uconst:
      case DW_OP_bra:
      case DW_OP_skip:
      case DW_OP_breg0 ... DW_OP_fbreg:
      case DW_OP_piece ... DW_OP_xderef_size:
      case DW_OP_call2 ... DW_OP_call_ref:
         printf("%s<loc_op pos='%llu' op='%s' p1='0x%llx'/>\n", 
                        indent, 
                        llbuf->ld_s[loc_e_i].lr_offset -1, 
                        f_dw_val_2_str(llbuf->ld_s[loc_e_i].lr_atom, c_dw_op_tbl), 
                        llbuf->ld_s[loc_e_i].lr_number);
         break;

      case DW_OP_bregx:
      case DW_OP_bit_piece:
      default:
         printf("%s<loc_op pos='%llu' op='%s' p1='0x%llx' p2='0x%llx' />\n", 
                        indent, 
                        llbuf->ld_s[loc_e_i].lr_offset -1, 
                        f_dw_val_2_str(llbuf->ld_s[loc_e_i].lr_atom, c_dw_op_tbl), 
                        llbuf->ld_s[loc_e_i].lr_number,
                        llbuf->ld_s[loc_e_i].lr_number2);
      }
   }
   indent +=2;
   printf("%s</loc_expr>\n", indent);
   if ( offset < 0 ) printf("%s", indent+2);
   return DW_DLV_OK;
}
// DW_AT_data_member_location : constant, exprloc, loclistptr
// error in file dwarf_attr_tbl.c at line 180 in function f_dw_location:Unexpected Attribute DW_AT_data_member_location form data1(11)
// error in file dwarf_forms.c at line 98 in function f_dw_form_glob_ref: dwarf_global_formref(attribute, &offset, &error) != DW_DLV_ERROR  returned message :DW_DLE_BAD_REF_FORM

int f_dw_location(Dwarf_Debug debug, Dwarf_Attribute attribute, void *cookie, struct s_dwarf_cu_info *cu_info, char *indent) {
   Dwarf_Error error;
   Dwarf_Signed lcnt;
   Dwarf_Locdesc *_llbuf;
   Dwarf_Locdesc **llbuf = &_llbuf;
   int ret, loc_d_i;

   Dwarf_Half form;
   Dwarf_Half attr;

   m_libdwarf_assert(dwarf_whatattr(attribute, &attr, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR );
   printf("%s<at_%s>",  indent, f_dw_attr_2_str(attr, 1));

   m_libdwarf_assert(dwarf_whatform(attribute, &form, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   switch ( form ) {
   case DW_FORM_data1: {
      Dwarf_Unsigned offset;
      printf("<loc_const>");
      m_dwarf2xml_assert((ret = dwarf_formudata(attribute, &offset, &error)) != DW_DLV_ERROR, return DW_DLV_ERROR);
      printf("%llu", offset);
      printf("</loc_const></at_%s>\n", f_dw_attr_2_str(attr, 0));
      return ret;
   }
   case DW_FORM_data4:
   case DW_FORM_data8:
      // fprintf(stderr, "External loc ref\n");
      printf("\n%s<loc_ref ref='loc:", indent -2);
      m_dwarf2xml_assert((ret = f_dw_form_glob_ref(debug, attribute)) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
      printf("' />");
      printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));
      return ret;

   case DW_FORM_block:
   case DW_FORM_block1:
   case DW_FORM_block2:
   case DW_FORM_block4:
      m_libdwarf_assert((ret = dwarf_loclist_n(attribute, &llbuf, &lcnt, &error)) != DW_DLV_ERROR, return DW_DLV_ERROR);
      if ( ret == DW_DLV_NO_ENTRY ) return DW_DLV_OK;
      break;

   case DW_FORM_exprloc: {
      Dwarf_Ptr exprloc;
      Dwarf_Unsigned exprloc_len;
      m_libdwarf_assert(
         (ret = dwarf_formexprloc(attribute, &exprloc_len, &exprloc, &error)) != DW_DLV_ERROR, 
         return DW_DLV_ERROR);
      if ( ret == DW_DLV_NO_ENTRY ) return DW_DLV_OK;
      m_libdwarf_assert(
         (ret = dwarf_loclist_from_expr(debug, exprloc, exprloc_len, llbuf, &lcnt, &error)) != DW_DLV_ERROR,
         return DW_DLV_ERROR);
      break;
   }
   default:
      m_error("Unexpected Attribute DW_AT_%s form %s(%d)", 
              f_dw_attr_2_str(attr, 2), f_dw_form_2_str(form), form);
      return DW_DLV_ERROR;
   }
   printf("\n");
   for (loc_d_i = 0; loc_d_i < lcnt; ++loc_d_i) {
      f_dw_print_loc_expr(indent-2, llbuf[loc_d_i], -1);
      dwarf_dealloc(debug, llbuf[loc_d_i]->ld_s, DW_DLA_LOC_BLOCK);
      dwarf_dealloc(debug,llbuf[loc_d_i], DW_DLA_LOCDESC);
   }
   if ( llbuf != &_llbuf ) dwarf_dealloc(debug, llbuf, DW_DLA_LIST);
   printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));
   return DW_DLV_OK;
}
int f_dw_encoding(Dwarf_Debug debug, Dwarf_Attribute attribute, void *cookie, struct s_dwarf_cu_info *cu_info, char *indent) {
   Dwarf_Error error;
   Dwarf_Unsigned uvalue;
   Dwarf_Half attr;

   m_libdwarf_assert(dwarf_whatattr(attribute, &attr, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR );
   printf("%s<at_%s>",  indent, f_dw_attr_2_str(attr, 1));
   m_libdwarf_assert(dwarf_formudata(attribute, &uvalue, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   printf("%s", f_dw_val_2_str(uvalue, c_dw_ate_tbl) );

   printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));
   return DW_DLV_OK;
}
int f_dw_skip(Dwarf_Debug debug, Dwarf_Attribute attribute, void *cookie, struct s_dwarf_cu_info *cu_info, char *indent) {
   return DW_DLV_OK;
}

/* dwarf3 2.19 Static and Dynamic Properties of Types 
 * Some attributes that apply to types specify a property (such as the lower bound of an array) that
 * is an integer value, where the value may be known during compilation or may be computed
 * dynamically during execution. The value of these attributes is determined based on the class as
 * follows:
 * •   For a constant, the value of the constant is the value of the attribute.
 * •   For a reference, the value is a reference to another entity whose value is the value of the
 *     attribute.
 * •   For a block, the value is interpreted as a DWARF expression; evaluation of the expression
 *     yields the value of the attribute.
 * Whether an attribute value can be dynamic depends on the rules of the applicable programming
 * language.
 * The applicable attributes include: DW_AT_allocated, DW_AT_associated, DW_AT_bit_offset,
 * DW_AT_bit_size, DW_AT_byte_size, DW_AT_count, DW_AT_lower_bound,
 * DW_AT_byte_stride, DW_AT_bit_stride, DW_AT_upper_bound (and possibly others).
 */
#ifndef HAVE_GET_FORM_CLASS
// Ooops already in dwarflib 201009 !
// enum e_dwarf_class {
enum Dwarf_Form_Class {
   DW_FORM_CLASS_REFERENCE = 0x7600,
   DW_CLASS_ADRESS,
   DW_CLASS_STRING,
   DW_FORM_CLASS_BLOCK,
   DW_FORM_CLASS_CONSTANT,
   DW_CLASS_FLAG,
   DW_CLASS_PTR,
   // DW_CLASS_ERROR = DW_DLV_ERROR,
   DW_FORM_CLASS_UNKNOWN,
};
//enum e_dwarf_class f_dw_get_attr_class(Dwarf_Attribute attribute) {
enum Dwarf_Form_Class dwarf_get_form_class(Dwarf_Half dwversion, Dwarf_Half attrnum, Dwarf_Half offset_size, Dwarf_Half form) {

   switch ( form ) {
   case DW_FORM_addr:
      return DW_CLASS_ADRESS;
   case DW_FORM_block2:
   case DW_FORM_block4:
   case DW_FORM_block:
   case DW_FORM_block1:
      return DW_FORM_CLASS_BLOCK;
   case DW_FORM_sdata:
   case DW_FORM_udata:
   case DW_FORM_data2:
   case DW_FORM_data1:
      return DW_FORM_CLASS_CONSTANT;
      /* May be wrong :
       Note that DW_FORM_data4 and DW_FORM_data8 are members of class constant only if the attribute
       in question does not allow one of the classes lineptr, loclistptr, macptr or rangelistptr */
   case DW_FORM_data4:
   case DW_FORM_data8:
      // return DW_CLASS_PTR;
      return DW_FORM_CLASS_REFERENCE;
   case DW_FORM_flag:
      return DW_CLASS_FLAG;
   case DW_FORM_string:
   case DW_FORM_strp:
      return DW_CLASS_STRING;
   case DW_FORM_ref_addr:
   case DW_FORM_ref1:
   case DW_FORM_ref2:
   case DW_FORM_ref4:
   case DW_FORM_ref8:
   case DW_FORM_ref_udata:
      return DW_FORM_CLASS_REFERENCE;
   case DW_FORM_indirect:
   case DW_FORM_exprloc:
   case DW_FORM_sec_offset:
      break;
   }
   return DW_DLV_ERROR;
}
#endif
int f_dw_dyn_prop_type(Dwarf_Debug debug, Dwarf_Attribute attribute, void *cookie, struct s_dwarf_cu_info *cu_info, char *indent) {
   Dwarf_Error error;
   Dwarf_Half attr;
   Dwarf_Half form;
   Dwarf_Off  offset;
   Dwarf_Signed lcnt;
   Dwarf_Locdesc **llbuf;
   int loc_d_i, ret;
   // enum e_dwarf_class a_class;
   enum Dwarf_Form_Class a_class;

   m_libdwarf_assert(dwarf_whatattr(attribute, &attr, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR );
   m_libdwarf_assert(dwarf_whatform(attribute, &form, &error) ==  DW_DLV_OK, return DW_DLV_ERROR);

   // m_libdwarf_assert( (a_class = f_dw_get_attr_class(attribute)) != DW_DLV_ERROR, return DW_DLV_ERROR);
   m_libdwarf_assert( (a_class = dwarf_get_form_class(cu_info->version_stamp, attr, cu_info->offset_size, form)) != DW_FORM_CLASS_UNKNOWN, return DW_DLV_ERROR);
   switch ( a_class ) {
   case DW_FORM_CLASS_REFERENCE:
      /* Note that DW_FORM_data4 and DW_FORM_data8 are members of class constant only if the attribute
       in question does not allow one of the classes lineptr, loclistptr, macptr or rangelistptr */
      /* Most attributes listed for dyn type do not allow ptr */
      if ( cookie ) {
         m_libdwarf_assert(dwarf_global_formref(attribute, &offset, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
         printf("%s<at_%s ref='i:%llu' />\n",  indent, f_dw_attr_2_str(attr, 1), offset);
         return DW_DLV_OK;
      }
   case DW_FORM_CLASS_CONSTANT:
      printf("%s<at_%s>",  indent, f_dw_attr_2_str(attr, 1));
      m_libdwarf_assert( f_dw_attr_value_2_str_with_form(attribute, debug) ==  DW_DLV_OK, return DW_DLV_ERROR);
      printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));
      return DW_DLV_OK;


   case DW_FORM_CLASS_BLOCK:
      printf("%s<at_%s>",  indent, f_dw_attr_2_str(attr, 1));
      m_libdwarf_assert( (ret = dwarf_loclist_n(attribute, &llbuf, &lcnt, &error)) != DW_DLV_ERROR, return DW_DLV_ERROR);
      if ( ret == DW_DLV_NO_ENTRY ) return DW_DLV_OK;
      printf("\n");
      for (loc_d_i = 0; loc_d_i < lcnt; ++loc_d_i) {
         f_dw_print_loc_expr(indent, llbuf[loc_d_i], -1);
         dwarf_dealloc(debug, llbuf[loc_d_i]->ld_s, DW_DLA_LOC_BLOCK);
         dwarf_dealloc(debug,llbuf[loc_d_i], DW_DLA_LOCDESC);
      }
      dwarf_dealloc(debug, llbuf, DW_DLA_LIST);
      printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));
      return DW_DLV_OK;
   case DW_FORM_CLASS_EXPRLOC:
      m_libdwarf_assert(f_dw_location(debug, attribute, cookie, cu_info, indent) != DW_DLV_ERROR, return DW_DLV_ERROR);
      return DW_DLV_OK;

   default:
      m_error("Unknow form class %d", a_class);
      return DW_DLV_ERROR;
      break;
   }
   return DW_DLV_ERROR;
}

struct s_dw_attr_tbl_elem c_dw_attr_tbl_fcnt[] = {
  { /* 0x01 */ DW_AT_sibling, "sibling", 0  },
  { /* 0x02 */ DW_AT_location, "location", &f_dw_location, 0  },
  { /* 0x03 */ DW_AT_name, "name", 0  },
  { /* 0x09 */ DW_AT_ordering, "ordering", 0  },
  { /* 0x0a */ DW_AT_subscr_data, "subscr_data", 0  },
  { /* 0x0b */ DW_AT_byte_size, "byte_size", &f_dw_dyn_prop_type, 0  },
  { /* 0x0c */ DW_AT_bit_offset, "bit_offset", &f_dw_dyn_prop_type, 0  },
  { /* 0x0d */ DW_AT_bit_size, "bit_size", &f_dw_dyn_prop_type, 0  },
  { /* 0x0f */ DW_AT_element_list, "element_list", 0  },
  { /* 0x10 */ DW_AT_stmt_list, "stmt_list", 0  },
  { /* 0x11 */ DW_AT_low_pc, "low_pc", 0  },
  { /* 0x12 */ DW_AT_high_pc, "high_pc", 0  },
  { /* 0x13 */ DW_AT_language, "language", &f_dw_form_udata_with_str, c_dw_lang_tbl },
  { /* 0x14 */ DW_AT_member, "member", 0  },
  { /* 0x15 */ DW_AT_discr, "discr", 0  },
  { /* 0x16 */ DW_AT_discr_value, "discr_value", 0  },
  { /* 0x17 */ DW_AT_visibility, "visibility", &f_dw_form_udata_with_str, c_dw_vis_tbl },
  { /* 0x18 */ DW_AT_import, "import", 0  },
  { /* 0x19 */ DW_AT_string_length, "string_length", &f_dw_location, 0  },
  { /* 0x1a */ DW_AT_common_reference, "common_reference", 0  },
  { /* 0x1b */ DW_AT_comp_dir, "comp_dir", 0  },
  { /* 0x1c */ DW_AT_const_value, "const_value", 0  },
  { /* 0x1d */ DW_AT_containing_type, "containing_type", 0  },
  { /* 0x1e */ DW_AT_default_value, "default_value", 0  },
  { /* 0x20 */ DW_AT_inline, "inline", &f_dw_form_udata_with_str, c_dw_inl_tbl },
  { /* 0x21 */ DW_AT_is_optional, "is_optional", 0  },
  { /* 0x22 */ DW_AT_lower_bound, "lower_bound", &f_dw_dyn_prop_type, 0  },
  { /* 0x25 */ DW_AT_producer, "producer", 0  },
  { /* 0x27 */ DW_AT_prototyped, "prototyped", 0  },
  { /* 0x2a */ DW_AT_return_addr, "return_addr", &f_dw_location, 0  },
  { /* 0x2c */ DW_AT_start_scope, "start_scope", 0  },
  { /* 0x2e */ DW_AT_bit_stride, "bit_stride", &f_dw_dyn_prop_type, 0  },
  { /* 0x2f */ DW_AT_upper_bound, "upper_bound", &f_dw_dyn_prop_type, 0  },
  { /* 0x31 */ DW_AT_abstract_origin, "abstract_origin", 0  },
  { /* 0x32 */ DW_AT_accessibility, "accessibility", &f_dw_form_udata_with_str, c_dw_access_tbl },
  { /* 0x33 */ DW_AT_address_class, "address_class", 0  },
  { /* 0x34 */ DW_AT_artificial, "artificial", 0  },
  { /* 0x35 */ DW_AT_base_types, "base_types", 0  },
  { /* 0x36 */ DW_AT_calling_convention, "calling_convention", &f_dw_form_udata_with_str, c_dw_cc_tbl },
  { /* 0x37 */ DW_AT_count, "count", &f_dw_dyn_prop_type, 0  },
  { /* 0x38 */ DW_AT_data_member_location, "data_member_location", &f_dw_location, 0  },
  { /* 0x39 */ DW_AT_decl_column, "decl_column", 0  },
  { /* 0x3a */ DW_AT_decl_file, "decl_file", 0  },
  { /* 0x3b */ DW_AT_decl_line, "decl_line", 0  },
  { /* 0x3c */ DW_AT_declaration, "declaration", 0  },
  { /* 0x3d */ DW_AT_discr_list, "discr_list", 0  },
  { /* 0x3e */ DW_AT_encoding, "encoding", &f_dw_encoding  },
  { /* 0x3f */ DW_AT_external, "external", 0  },
  { /* 0x40 */ DW_AT_frame_base, "frame_base", &f_dw_location, 0},
  { /* 0x41 */ DW_AT_friend, "friend", 0  },
  { /* 0x42 */ DW_AT_identifier_case, "identifier_case", &f_dw_form_udata_with_str, c_dw_id_tbl },
  { /* 0x43 */ DW_AT_macro_info, "macro_info", 0  },
  { /* 0x44 */ DW_AT_namelist_item, "namelist_item", 0  },
  { /* 0x45 */ DW_AT_priority, "priority", 0  },
  { /* 0x46 */ DW_AT_segment, "segment", &f_dw_location, 0  },
  { /* 0x47 */ DW_AT_specification, "specification", 0  },
  { /* 0x48 */ DW_AT_static_link, "static_link", &f_dw_location, 0  },
  { /* 0x49 */ DW_AT_type, "type", 0  },
  { /* 0x4a */ DW_AT_use_location, "use_location", &f_dw_location, 0  },
  { /* 0x4b */ DW_AT_variable_parameter, "variable_parameter", 0  },
  { /* 0x4c */ DW_AT_virtuality, "virtuality", &f_dw_form_udata_with_str, c_dw_virtuality_tbl },
  { /* 0x4d */ DW_AT_vtable_elem_location, "vtable_elem_location", &f_dw_location, 0  },
  { /* 0x4e */ DW_AT_allocated, "allocated", &f_dw_dyn_prop_type, 0  },
  { /* 0x4f */ DW_AT_associated, "associated", &f_dw_dyn_prop_type, 0  },
  { /* 0x50 */ DW_AT_data_location, "data_location", 0  },
  { /* 0x51 */ DW_AT_byte_stride, "byte_stride", &f_dw_dyn_prop_type, 0  },
  { /* 0x52 */ DW_AT_entry_pc, "entry_pc", 0  },
  { /* 0x53 */ DW_AT_use_UTF8, "use_UTF8", 0  },
  { /* 0x54 */ DW_AT_extension, "extension", 0  },
  { /* 0x55 */ DW_AT_ranges, "ranges", 0  },
  { /* 0x56 */ DW_AT_trampoline, "trampoline", 0  },
  { /* 0x57 */ DW_AT_call_column, "call_column", 0  },
  { /* 0x58 */ DW_AT_call_file, "call_file", 0  },
  { /* 0x59 */ DW_AT_call_line, "call_line", 0  },
  { /* 0x5a */ DW_AT_description, "description", 0  },
  { /* 0x5b */ DW_AT_binary_scale, "binary_scale", 0  },
  { /* 0x5c */ DW_AT_decimal_scale, "decimal_scale", 0  },
  { /* 0x5d */ DW_AT_small, "small", 0  },
  { /* 0x5e */ DW_AT_decimal_sign, "decimal_sign", 0  },
  { /* 0x5f */ DW_AT_digit_count, "digit_count", 0  },
  { /* 0x60 */ DW_AT_picture_string, "picture_string", 0  },
  { /* 0x61 */ DW_AT_mutable, "mutable", 0  },
  { /* 0x62 */ DW_AT_threads_scaled, "threads_scaled", 0  },
  { /* 0x63 */ DW_AT_explicit, "explicit", 0  },
  { /* 0x64 */ DW_AT_object_pointer, "object_pointer", 0  },
  { /* 0x65 */ DW_AT_endianity, "endianity", &f_dw_form_udata_with_str, c_dw_end_tbl },
  { /* 0x66 */ DW_AT_elemental, "elemental", 0  },
  { /* 0x67 */ DW_AT_pure, "pure", 0  },
  { /* 0x68 */ DW_AT_recursive, "recursive", 0  },
{ /* 0x2108 */ DW_AT_GNU_template_name, "GNU_template_name", 0  },     /* GNU */
  { 0 },
};

char v_dw_attr_str_unk[300] = "uninitalized";
struct s_dw_attr_tbl_elem v_dw_attr_unk = { 0, 0, 0 };

struct s_dw_attr_tbl_elem *f_dw_attr(Dwarf_Half attr, int open) {
   int ind;

   v_dw_attr_unk.attr = 0;
   v_dw_attr_unk.fnct = 0;
   v_dw_attr_unk.str = v_dw_attr_str_unk;

   if ( attr == DW_AT_lo_user || attr >= DW_AT_hi_user ) {
      switch ( open )  {
      case 0: 
         strcpy(v_dw_attr_unk.str, "out_range");
         return &v_dw_attr_unk;
      case 1:
         sprintf(v_dw_attr_unk.str, "out_range name='attr_0x%04x'", attr);
         return &v_dw_attr_unk;
      case 2:
         sprintf(v_dw_attr_unk.str, "attr_0x%04x", attr);
         return &v_dw_attr_unk;
      }
      return 0;
   }
   for ( ind = 0; ind < (sizeof(c_dw_attr_tbl_fcnt)/sizeof(c_dw_attr_tbl_fcnt[0])); ind++) {
      if ( attr == c_dw_attr_tbl_fcnt[ind].attr ) {
         if ( attr < DW_AT_lo_user ) {
            return &c_dw_attr_tbl_fcnt[ind];
         }
         v_dw_attr_unk.attr = c_dw_attr_tbl_fcnt[ind].attr;
         v_dw_attr_unk.fnct = c_dw_attr_tbl_fcnt[ind].fnct;
         switch ( open )  {
         case 0: 
            strcpy(v_dw_attr_unk.str, "user");
            return &v_dw_attr_unk;
         case 1:
            sprintf(v_dw_attr_unk.str, "user name='%s'", c_dw_attr_tbl_fcnt[ind].str);
            return &v_dw_attr_unk;
         case 2:
            sprintf(v_dw_attr_unk.str, "%s", c_dw_attr_tbl_fcnt[ind].str);
            return &v_dw_attr_unk;
         }
         return 0;
      }
   }
   for ( ind = 0; c_dw_at_tbl[ind].val; ind++) {
      if ( attr == c_dw_at_tbl[ind].val ) {
         if ( attr < DW_AT_lo_user ) {
            v_dw_attr_unk.str = c_dw_at_tbl[ind].str;
            return &v_dw_attr_unk;
         }
         switch ( open )  {
         case 0: 
            strcpy(v_dw_attr_unk.str, "user");
            return &v_dw_attr_unk;
         case 1:
            sprintf(v_dw_attr_unk.str, "user name='%s'", c_dw_at_tbl[ind].str);
            return &v_dw_attr_unk;
         case 2:
            sprintf(v_dw_attr_unk.str, "%s", c_dw_at_tbl[ind].str);
            return &v_dw_attr_unk;
         }
         return 0;
      }
   }
   switch ( open )  {
   case 0: 
      strcpy(v_dw_attr_unk.str, "unk");
      return &v_dw_attr_unk;
   case 1:
      sprintf(v_dw_attr_unk.str, "unk name='attr_0x%04x'", attr);
      return &v_dw_attr_unk;
   case 2:
      sprintf(v_dw_attr_unk.str, "attr_0x%04x", attr);
      return &v_dw_attr_unk;
   }
   return 0;
}
const char *f_dw_attr_2_str(Dwarf_Half attr, int open) {
   struct s_dw_attr_tbl_elem *tbl_elem;
   tbl_elem = f_dw_attr(attr, open);
   return tbl_elem->str;
}

int f_dw_attr_value_2_str(char *indent, Dwarf_Attribute attribute, Dwarf_Debug debug, struct s_dwarf_cu_info *cu_info) {
   Dwarf_Error error;
   Dwarf_Half attr;
   struct s_dw_attr_tbl_elem *p_attr_desc;

   m_libdwarf_assert(dwarf_whatattr(attribute, &attr, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR );
   if ( !(p_attr_desc = f_dw_attr(attr, 0)) && !p_attr_desc->attr ) {
      printf("%s<at_%s>",  indent, f_dw_attr_2_str(attr, 1));
      printf( "%s", p_attr_desc->str);
      m_dwarf2xml_assert(f_dw_attr_value_2_str_with_form(attribute, debug) != DW_DLV_ERROR, 
         return DW_DLV_ERROR );
      printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));
      return DW_DLV_OK;
   }
   if ( p_attr_desc->fnct ) {
      // if ( p_attr_desc->fnct(debug, attribute, p_attr_desc->cookie, cu_info, indent) ==  DW_DLV_ERROR )
      //    fprintf(stderr, "%s", "form_error\n" );
         
      m_dwarf2xml_assert(p_attr_desc->fnct(debug, attribute, p_attr_desc->cookie, cu_info, indent) !=  DW_DLV_ERROR,
         fprintf(stderr, "%s", "form_error\n" ); return DW_DLV_ERROR );
      return DW_DLV_OK;
   }
   printf("%s<at_%s>",  indent, f_dw_attr_2_str(attr, 1));
   m_dwarf2xml_assert(f_dw_attr_value_2_str_with_form(attribute, debug) != DW_DLV_ERROR,
      fprintf(stderr, "%s", "form_error" ); return DW_DLV_ERROR );
   printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));

   return DW_DLV_OK;
}



