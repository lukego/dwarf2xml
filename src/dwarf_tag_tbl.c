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
#include <stdlib.h>

#include <dwarf.h>
#include <libdwarf.h>

#include "dwarf2xml.h"

#ifndef DW_TAG_GNU_template_parameter_pack 

/* GNU extension. http://gcc.gnu.org/wiki/TemplateParmsDwarf */
#define DW_TAG_GNU_template_template_parameter  0x4106 /* GNU */
#define DW_TAG_GNU_template_template_param      0x4106 /* GNU */
#define DW_TAG_GNU_template_parameter_pack      0x4107 /* GNU */
#define DW_TAG_GNU_formal_parameter_pack        0x4108 /* GNU */
#endif

struct s_dw_tag_2_str {
   Dwarf_Half tag;
   char *str;
};
const struct s_dw_tag_2_str c_dw_tag_2_str[] = {
  { /* 0x01   */ DW_TAG_array_type               ,"array_type" },
  { /* 0x02   */ DW_TAG_class_type               ,"class_type" },
  { /* 0x03   */ DW_TAG_entry_point              ,"entry_point" },
  { /* 0x04   */ DW_TAG_enumeration_type         ,"enumeration_type" },
  { /* 0x05   */ DW_TAG_formal_parameter         ,"formal_parameter" },
  { /* 0x08   */ DW_TAG_imported_declaration     ,"imported_declaration" },
  { /* 0x0a   */ DW_TAG_label                    ,"label" },
  { /* 0x0b   */ DW_TAG_lexical_block            ,"lexical_block" },
  { /* 0x0d   */ DW_TAG_member                   ,"member" },
  { /* 0x0f   */ DW_TAG_pointer_type             ,"pointer_type" },
  { /* 0x10   */ DW_TAG_reference_type           ,"reference_type" },
  { /* 0x11   */ DW_TAG_compile_unit             ,"compile_unit" },
  { /* 0x12   */ DW_TAG_string_type              ,"string_type" },
  { /* 0x13   */ DW_TAG_structure_type           ,"structure_type" },
  { /* 0x15   */ DW_TAG_subroutine_type          ,"subroutine_type" },
  { /* 0x16   */ DW_TAG_typedef                  ,"typedef" },
  { /* 0x17   */ DW_TAG_union_type               ,"union_type" },
  { /* 0x18   */ DW_TAG_unspecified_parameters   ,"unspecified_parameters" },
  { /* 0x19   */ DW_TAG_variant                  ,"variant" },
  { /* 0x1a   */ DW_TAG_common_block             ,"common_block" },
  { /* 0x1b   */ DW_TAG_common_inclusion         ,"common_inclusion" },
  { /* 0x1c   */ DW_TAG_inheritance              ,"inheritance" },
  { /* 0x1d   */ DW_TAG_inlined_subroutine       ,"inlined_subroutine" },
  { /* 0x1e   */ DW_TAG_module                   ,"module" },
  { /* 0x1f   */ DW_TAG_ptr_to_member_type       ,"ptr_to_member_type" },
  { /* 0x20   */ DW_TAG_set_type                 ,"set_type" },
  { /* 0x21   */ DW_TAG_subrange_type            ,"subrange_type" },
  { /* 0x22   */ DW_TAG_with_stmt                ,"with_stmt" },
  { /* 0x23   */ DW_TAG_access_declaration       ,"access_declaration" },
  { /* 0x24   */ DW_TAG_base_type                ,"base_type" },
  { /* 0x25   */ DW_TAG_catch_block              ,"catch_block" },
  { /* 0x26   */ DW_TAG_const_type               ,"const_type" },
  { /* 0x27   */ DW_TAG_constant                 ,"constant" },
  { /* 0x28   */ DW_TAG_enumerator               ,"enumerator" },
  { /* 0x29   */ DW_TAG_file_type                ,"file_type" },
  { /* 0x2a   */ DW_TAG_friend                   ,"friend" },
  { /* 0x2b   */ DW_TAG_namelist                 ,"namelist" },
  { /* 0x2c   */ DW_TAG_namelist_item            ,"namelist_item" },
  { /* 0x2c   */ DW_TAG_namelist_items           ,"namelist_items" },
  { /* 0x2d   */ DW_TAG_packed_type              ,"packed_type" },
  { /* 0x2e   */ DW_TAG_subprogram               ,"subprogram" },
  { /* 0x2f   */ DW_TAG_template_type_parameter  ,"template_type_parameter" },
  { /* 0x2f   */ DW_TAG_template_type_param      ,"template_type_param" },
  { /* 0x30   */ DW_TAG_template_value_parameter ,"template_value_parameter" },
  { /* 0x30   */ DW_TAG_template_value_param     ,"template_value_param" },
  { /* 0x31   */ DW_TAG_thrown_type              ,"thrown_type" },
  { /* 0x32   */ DW_TAG_try_block                ,"try_block" },
  { /* 0x33   */ DW_TAG_variant_part             ,"variant_part" },
  { /* 0x34   */ DW_TAG_variable                 ,"variable" },
  { /* 0x35   */ DW_TAG_volatile_type            ,"volatile_type" },
  { /* 0x36   */ DW_TAG_dwarf_procedure          ,"dwarf_procedure" },
  { /* 0x37   */ DW_TAG_restrict_type            ,"restrict_type" },
  { /* 0x38   */ DW_TAG_interface_type           ,"interface_type" },
  { /* 0x39   */ DW_TAG_namespace                ,"namespace" },
  { /* 0x3a   */ DW_TAG_imported_module          ,"imported_module" },
  { /* 0x3b   */ DW_TAG_unspecified_type         ,"unspecified_type" },
  { /* 0x3c   */ DW_TAG_partial_unit             ,"partial_unit" },
  { /* 0x3d   */ DW_TAG_imported_unit            ,"imported_unit" },
  { /* 0x3e   */ DW_TAG_mutable_type             ,"mutable_type" },
  { /* 0x3f   */ DW_TAG_condition                ,"condition" },
  { /* 0x40   */ DW_TAG_shared_type              ,"shared_type" },
  { /* 0x41   */ DW_TAG_type_unit                ,"type_unit" },
  { /* 0x42   */ DW_TAG_rvalue_reference_type    ,"rvalue_reference_type" },
  { /* 0x43   */ DW_TAG_template_alias           ,"template_alias" },
  { /* 0x4080 */ DW_TAG_lo_user                  ,"lo_user" },
  { /* 0x4081 */ DW_TAG_MIPS_loop                ,"MIPS_loop" },
  { /* 0x4090 */ DW_TAG_HP_array_descriptor      ,"HP_array_descriptor" },
  { /* 0x4101 */ DW_TAG_format_label             ,"format_label" },
  { /* 0x4102 */ DW_TAG_function_template        ,"function_template" },
  { /* 0x4103 */ DW_TAG_class_template           ,"class_template" },
  { /* 0x4104 */ DW_TAG_GNU_BINCL                ,"GNU_BINCL" },
  { /* 0x4105 */ DW_TAG_GNU_EINCL                ,"GNU_EINCL" },
/* GNU extension. http://gcc.gnu.org/wiki/TemplateParmsDwarf */
{ /* 0x4106 */ DW_TAG_GNU_template_template_parameter, "GNU_template_template_parameter"  },  /* GNU */
{ /* 0x4106 */ DW_TAG_GNU_template_template_param    , "GNU_template_template_param"  },  /* GNU */
{ /* 0x4107 */ DW_TAG_GNU_template_parameter_pack    , "GNU_template_parameter_pack"  },  /* GNU */
{ /* 0x4108 */ DW_TAG_GNU_formal_parameter_pack      , "GNU_formal_parameter_pack"  },   /* GNU */
/* SUN extensions */
  { /* 0x4201 */ DW_TAG_SUN_function_template      , "SUN_function_template" },
  { /* 0x4202 */ DW_TAG_SUN_class_template         , "SUN_class_template" },
  { /* 0x4203 */ DW_TAG_SUN_struct_template        , "SUN_struct_template" },
  { /* 0x4204 */ DW_TAG_SUN_union_template         , "SUN_union_template" },
  { /* 0x4205 */ DW_TAG_SUN_indirect_inheritance   , "SUN_indirect_inheritance" },
  { /* 0x4206 */ DW_TAG_SUN_codeflags              , "SUN_codeflags" },
  { /* 0x4207 */ DW_TAG_SUN_memop_info             , "SUN_memop_info" },
  { /* 0x4208 */ DW_TAG_SUN_omp_child_func         , "SUN_omp_child_func" },
  { /* 0x4209 */ DW_TAG_SUN_rtti_descriptor        , "SUN_rtti_descriptor" },
  { /* 0x420a */ DW_TAG_SUN_dtor_info              , "SUN_dtor_info" },
  { /* 0x420b */ DW_TAG_SUN_dtor                   , "SUN_dtor" },
  { /* 0x420c */ DW_TAG_SUN_f90_interface          , "SUN_f90_interface" },
  { /* 0x420d */ DW_TAG_SUN_fortran_vax_structure  , "SUN_fortran_vax_structure" },
  { /* 0x42ff */ DW_TAG_SUN_hi                     , "SUN_hi" },

  { /* 0x5101 */ DW_TAG_ALTIUM_circ_type         ,"ALTIUM_circ_type" },
  { /* 0x5102 */ DW_TAG_ALTIUM_mwa_circ_type     ,"ALTIUM_mwa_circ_type" },
  { /* 0x5103 */ DW_TAG_ALTIUM_rev_carry_type    ,"ALTIUM_rev_carry_type" },
  { /* 0x5111 */ DW_TAG_ALTIUM_rom               ,"ALTIUM_rom" },
  { /* 0x8765 */ DW_TAG_upc_shared_type          ,"upc_shared_type" },
  { /* 0x8766 */ DW_TAG_upc_strict_type          ,"upc_strict_type" },
  { /* 0x8767 */ DW_TAG_upc_relaxed_type         ,"upc_relaxed_type" },
  { /* 0xa000 */ DW_TAG_PGI_kanji_type           ,"PGI_kanji_type" },
  { /* 0xa020 */ DW_TAG_PGI_interface_block      ,"PGI_interface_block" },
  { /* 0xffff */ DW_TAG_hi_user                  ,"hi_user" },
};
char v_dw_tag_unk[300];
const char *f_dw_tag_2_str(Dwarf_Half tag, int open) {
   int ind;
   if ( tag == DW_TAG_lo_user || tag >= DW_TAG_hi_user ) {
      switch ( open ) {
      case 0 : return "out_range";
      case 1 : 
         sprintf(v_dw_tag_unk, "out_range name='0x%04x'", tag);
         return v_dw_tag_unk;
      case 2 : 
         sprintf(v_dw_tag_unk, "0x%04x", tag);
         return v_dw_tag_unk;
      }
   }
   for ( ind = 0; ind < (sizeof(c_dw_tag_2_str)/sizeof(c_dw_tag_2_str[0])); ind++) {
      if ( tag == c_dw_tag_2_str[ind].tag ) {
         if ( tag < DW_TAG_lo_user ) 
            return c_dw_tag_2_str[ind].str;
         switch ( open ) {
         case 0 : return "user";
         case 1 :
            sprintf(v_dw_tag_unk, "user name='%s'", c_dw_tag_2_str[ind].str);
            return v_dw_tag_unk;
         case 2 : 
            sprintf(v_dw_tag_unk, "%s", c_dw_tag_2_str[ind].str);
            return v_dw_tag_unk;
         }
      }
   }
   switch ( open ) {
   case 0 : return "unk";
   case 1 :
      sprintf(v_dw_tag_unk, "unk name='0x%04x'", tag);
      return v_dw_tag_unk;
   case 2 :
      sprintf(v_dw_tag_unk, "0x%04x", tag);
      return v_dw_tag_unk;
   }
   abort();
   return 0;
}


