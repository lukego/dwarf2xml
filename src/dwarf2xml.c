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

/*
AZT mer 24 oct 2007 23:17:18 CEST 
gcc -g -I /usr/lib/libdwarf/include/ -L /usr/lib/libdwarf/lib -ldwarf -lelf -o dwarf2xml dwarf2xml.c  dwarf_attr_tbl.c  dwarf_forms.c  dwarf_tag_tbl.c
OK : XML Attribute Values Must be Quoted
OK : data_member_location
OK : sibling correct ?
OK : type ref incorrects
OK : desallocation sur le parcours des dies, ils ne sont pas a desallouer
OK : frame_eh (exceptions)
OK : dwarftest-20100930/cristi3/cristibadobj, <structure_type ref='4200'><name>._1</name> ? decl file is /usr/include/wchar.h
  Same with dwarfdump
TODO : <extension> ?
OK : DW_AT_ranges, 
TODO : DW_AT_default_value attribute. The value of this attribute is a reference to the debugging information entry for a variable or subroutine, 
       or the value may be a constant. If the value is 0, no default value has been specified. 
       If the value is of form constant, that constant is interpreted as a value of the type of the formal parameter.
TODO : list attributes that carry location and fix attrb tbl_fnct ?
OK  carry location DW_AT_return_addr DW_AT_static_link DW_AT_data_member_location DW_AT_vtable_elem_location DW_AT_string_length DW_AT_use_location 
    DW_AT_data_location DW_AT_segment 
OK  Version int dwarf_next_cu_header - version_stamp 
OK 
Static and Dynamic Properties of Types
Some attributes that apply to types specify a property (such as the lower bound of an array) that
is an integer value, where the value may be known during compilation or may be computed
dynamically during execution. The value of these attributes is determined based on the class as
follows:
•   For a constant, the value of the constant is the value of the attribute.
•   For a reference, the value is a reference to another entity whose value is the value of the
    attribute.
•   For a block, the value is interpreted as a DWARF expression; evaluation of the expression
    yields the value of the attribute.
Whether an attribute value can be dynamic depends on the rules of the applicable programming
language.
The applicable attributes include: DW_AT_allocated, DW_AT_associated, DW_AT_bit_offset,
DW_AT_bit_size, DW_AT_byte_size, DW_AT_count, DW_AT_lower_bound,
DW_AT_byte_stride, DW_AT_bit_stride, DW_AT_upper_bound (and possibly others).
TODO : option split by CU (for hudge files)
OK : test multiple op in location expr
OK : indentation des loc_expr et loc_ref
TODO : warning, Next cie record ref may not be correct
OK : tag_0x4106
OK : requires libdwarf dwarf-20071016 sinon 
   ./dwarf2xml: symbol lookup error: ./dwarf2xml: undefined symbol: dwarf_loclist_from_expr
   Installation std : --prefix=/usr --enable-shared
   commenter la ligne correspondante dans /etc/ld.so.conf
   recharger la config ld.so : /sbin/ldconfig
   verifier /sbin/ldconfig -p | grep dwarf
   chcon -u system_u /usr/lib/libdwarf.so
   chcon -t texrel_shlib_t /usr/lib/libdwarf.so
 */
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libelf.h>
#include <elf.h>
#include <gelf.h>
#include <dwarf.h>
#include <libdwarf.h>

#include "config.h"
#include "ring.h"
#include "dwarf2xml.h"

struct s_elf_arch v_elf_arch = { .w_size = 4, .mask = 0xFFFFFFFFLL, .fmt_addr = "0x%08llx" };

char c_spaces[] = 
"                                                                                                                        "
"                                                                                                                        ";

static Dwarf_Debug debug;
static Dwarf_Error error;

char *escape_html(const char *in) {
   int max, out_i = 0;
   char *out;

   assert( in );
   max = strlen(in) +20;
   assert( (out = malloc(max +1)) );

   *out = 0;
   for (; *in; ++in) {
      switch ( *in ) {
      case '<': strcpy(out +out_i,   "&lt;"); out_i += 4; break;
      case '>': strcpy(out +out_i,   "&gt;"); out_i += 4; break;
      case '&': strcpy(out +out_i,  "&amp;"); out_i += 5; break;
      case '"': strcpy(out +out_i, "&quot;"); out_i += 6; break;
      default:
         if ( *in >= 127 || *in < ' ') {
             char tmp = *in;
             strcpy(out +out_i, "&#");
             out[out_i +4] = tmp %10; tmp /= 10;
             out[out_i +3] = tmp %10; tmp /= 10;
             out[out_i +2] = tmp;
             out[out_i +5] = ';'; 
             out[out_i +6] = 0;
             out_i += 6;
             break;
         }
         out[out_i++] = *in; out[out_i] = 0;
         break;
      }
      assert( out_i < max );
      if ( max < 7 +out_i ) {
         max = strlen(in) +out_i +20;
         assert( (out = realloc(out, max +1)) );
      }
   }
   return out;
}

/** @name Provide general services for Die tree parsing
 * Walk throught debug_info dies, call three user call-back :
 * One at begin of die, one for each attribute and one at the end of die
 */
//@{
/** @memo Structure holds constant informations during recursion 
 * In order to save stack space, constant information are referenced with a pointer to this structure
 * It is not visible from call-backs
 */
struct s_dwarf_walk_dies_params {
   Dwarf_Debug debug;
   /** Begin die call back */
   int (*call_back_die_begin)(Dwarf_Debug debug, Dwarf_Die die, struct s_dwarf_cu_info *cu_info, void *cooky);
   /** Attribute call back */
   int (*call_back_attr)(Dwarf_Debug debug, Dwarf_Die die, Dwarf_Attribute Attribute, struct s_dwarf_cu_info *cu_info, void *cooky);
   /** End die call back */
   int (*call_back_die_end)(Dwarf_Debug debug, Dwarf_Die die, struct s_dwarf_cu_info *cu_info, void *cooky);
   /** User data */
   void *cooky;
   /** CU level data */
   struct s_dwarf_cu_info cu_info;
};
/** @memo Die recursion function
 * Walk throught Dies and their children. 
 * Call the begin call-back, then for each attributes call the attribute call back, 
 * Walk throught chidren with a recursive call,
 * then call the end die call-back
 * @param die (in) curent die
 * @param params (in) ptr to info structure
 * @return DW_DLV_ERROR, DW_DLV_OK
 */
static int f_dwarf_walk_recurse (Dwarf_Die die, struct s_dwarf_walk_dies_params *params) {
   int        ret, i;
   Dwarf_Die  child_die, sibling_child_die;


   ++params->cu_info.level;
   if ( params->cu_info.level > 80 ) {
      Dwarf_Off  offset;
      m_libdwarf_assert( dwarf_dieoffset(die, &offset, NULL) == DW_DLV_OK, return DW_DLV_ERROR);
      m_error("Recurssion limit exceded for dies offset %llu, give up at level %d", offset, params->cu_info.level);
      --params->cu_info.level;
      return DW_DLV_OK; 
   }
   /* Call begin call-back */
   if ( params->call_back_die_begin ) {
      m_libdwarf_assert( (ret = (*params->call_back_die_begin)(params->debug, die, &params->cu_info, params->cooky)) == DW_DLV_OK || 
                          ret == DW_DLV_NO_ENTRY, return ret);
      if ( ret == DW_DLV_NO_ENTRY ) goto end;
   }

   /* Call attribute call-back for each attribute */
   if ( params->call_back_attr ) {
      Dwarf_Attribute *atlist;
      Dwarf_Signed    attrcount;

      m_libdwarf_assert( ((ret = dwarf_attrlist(die, &atlist, &attrcount, &error)) == DW_DLV_OK || ret == DW_DLV_NO_ENTRY), return ret);
      if ( ret == DW_DLV_OK ) {
         for (i = 0; i < attrcount; ++i) {
            m_libdwarf_assert( (ret = (*params->call_back_attr)(params->debug, die, atlist[i], &params->cu_info, params->cooky)) == DW_DLV_OK || 
                                ret == DW_DLV_NO_ENTRY, return ret);
            if ( ret == DW_DLV_NO_ENTRY ) goto end;
            dwarf_dealloc(debug, atlist[i], DW_DLA_ATTR);
         }
         dwarf_dealloc(debug, atlist, DW_DLA_LIST);
      }
   }

   /* Visit all children */
   m_libdwarf_assert( (ret = dwarf_child (die, &child_die, &error)) == DW_DLV_OK || ret == DW_DLV_NO_ENTRY , return ret);
   if (ret == DW_DLV_OK) {
      sibling_child_die = child_die;
      do {
         child_die = sibling_child_die;
         m_libdwarf_assert( f_dwarf_walk_recurse (child_die, params) != DW_DLV_ERROR, return DW_DLV_ERROR);
      } while ( (ret = dwarf_siblingof (params->debug, child_die, &sibling_child_die, &error)) == DW_DLV_OK);
   }

end:
   /* Call end call-back */
   if ( params->call_back_die_end ) 
      m_libdwarf_assert( (ret = (*params->call_back_die_end)(params->debug, die, &params->cu_info, params->cooky)) == DW_DLV_OK || 
                          ret == DW_DLV_NO_ENTRY, return ret);
   
   ret = (ret == DW_DLV_NO_ENTRY)?DW_DLV_OK:ret;
   --params->cu_info.level;
   return DW_DLV_OK;
}
/** @memo Die tree parsing entry point 
 * This function setup recursion loop on the dies tree according to values provided by caller.
 * NOTE : users call-back can stop a die parse by returning DW_DLV_NO_ENTRY instead of DW_DLV_OK.
 *   When call_back_die_begin returns DW_DLV_NO_ENTRY, call_back_attr will not be called and call_back_die_end will be. Chidren will not be visited.
 *   When call_back_attr returns DW_DLV_NO_ENTRY, call_back_attr will no more be call and call_back_die_end will be. Chidren will not be visited.
 *   call_back_die_end return DW_DLV_NO_ENTRY has no effect.
 * @param debug (IN) valid libdwarf debug handle
 * @param call_back_die_begin (IN) function ptr, will be call, if not null, at begining of each die.
 * @param call_back_attr (IN) function ptr, will be call, if not null, for each attribute die.
 * @param call_back_die_end (IN) function ptr, will be call, if not null, at the end of each die.
 * @param cooky (IN) ptr to user data.
 * @return DW_DLV_ERROR, DW_DLV_OK
 */
int f_dwarf_walk_dies (Dwarf_Debug debug, 
          int (*call_back_die_begin)(Dwarf_Debug debug, Dwarf_Die die, struct s_dwarf_cu_info *cu_info, void *cooky), 
          int (*call_back_attr)(Dwarf_Debug debug, Dwarf_Die die, Dwarf_Attribute Attribute, struct s_dwarf_cu_info *cu_info, void *cooky), 
          int (*call_back_die_end)(Dwarf_Debug debug, Dwarf_Die die, struct s_dwarf_cu_info *cu_info, void *cooky), 
          void *cooky) {
   int             ret;
   Dwarf_Die       die;
   Dwarf_Unsigned  next_cu_header;
   struct s_dwarf_walk_dies_params params;

   params.debug = debug;
   params.call_back_die_begin = call_back_die_begin;
   params.call_back_attr = call_back_attr;
   params.call_back_die_end = call_back_die_end;
   params.cooky = cooky;
   params.cu_info.cu_no = 0;
   params.cu_info.level = 0;

   /* Bewarre that debug context must at start of CU, never stop this loop in normal flow */
#ifdef HAVE_NEXT_CU_HEADER_B
   while ( (ret = dwarf_next_cu_header_b (debug, NULL, &params.cu_info.version_stamp, &params.cu_info.abbrev_offset, 
                                         &params.cu_info.address_size, &params.cu_info.offset_size, NULL, &next_cu_header, &error)) == DW_DLV_OK ) {
#else
   while ( (ret = dwarf_next_cu_header (debug, NULL, &params.cu_info.version_stamp, &params.cu_info.abbrev_offset, 
                                         &params.cu_info.address_size, &next_cu_header, &error)) == DW_DLV_OK ) {
#endif
      assert ( params.cu_info.level == 0);
      m_libdwarf_assert((ret = dwarf_siblingof (debug, 0, &die, &error)) != DW_DLV_ERROR, break);
      params.cu_info.die = die;
      m_dwarf2xml_assert((ret = f_dwarf_walk_recurse (die, &params)) != DW_DLV_ERROR, break);
      ++params.cu_info.cu_no;
   }
   if (ret == DW_DLV_NO_ENTRY) return DW_DLV_OK;
   return ret;
}
/* Get CU from source line offset or from abbrev offset
 * Collect CU source line offset from DW_AT_stmt_list, if present
 * and abbrev offset (from dwarf_next_cu_header call)
 */
struct s_dw_lineoff_2_cu {
   struct s_ring link;
   Dwarf_Off lineoff;
   Dwarf_Die die;
   Dwarf_Off offset;
   Dwarf_Unsigned abbrev_offset;
} *v_dw_lineoff_2_cu;
/* Call back for f_dwarf_walk_dies
 * always return DW_DLV_NO_ENTRY to stop at CU level
 */
int f_dw_lineoff_add (Dwarf_Debug debug, Dwarf_Die die, struct s_dwarf_cu_info *cu_info, void *cooky) {
   int ret;
   Dwarf_Off lineoff = -1;
   Dwarf_Attribute Attribute;
   Dwarf_Off offset;
   struct s_dw_lineoff_2_cu *new;
   Dwarf_Half      form;

   m_libdwarf_assert( (ret = dwarf_attr (die, DW_AT_stmt_list, &Attribute, NULL)) != DW_DLV_ERROR, return ret);
   if ( ret == DW_DLV_OK ) {
      m_libdwarf_assert(dwarf_whatform(Attribute, &form, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
      switch ( form ) {
      case DW_FORM_data4:
      case DW_FORM_data8:
         m_libdwarf_assert( (ret = dwarf_formudata(Attribute, &lineoff, &error)) ==  DW_DLV_OK, return ret);
         break;
      case DW_FORM_sec_offset:
         m_libdwarf_assert( (ret = dwarf_global_formref(Attribute, &lineoff, &error)) ==  DW_DLV_OK, return ret);
         break;
      default:
         m_error("Unexpected Attribute type %s(%d)", f_dw_form_2_str(form), form);
         return DW_DLV_ERROR;
      }
      m_libdwarf_assert( (ret = dwarf_dieoffset(die, &offset, NULL)) == DW_DLV_OK, return ret);
   }
   assert ( (new = malloc(sizeof(*new))) );
   memset(new, 0, sizeof(*new));

   new->die = die;
   new->offset = offset;
   new->lineoff = lineoff;
   new->abbrev_offset = cu_info->abbrev_offset;

   assert ( (v_dw_lineoff_2_cu = m_ring_link(v_dw_lineoff_2_cu, link, new)) );
   return DW_DLV_NO_ENTRY;
}
/* launch CU parsing
 */
int f_dwarf_record_lineoff (void) {
   return f_dwarf_walk_dies (debug, &f_dw_lineoff_add, 0, 0 ,0); 
}
/* Free CU infos
 */
int f_dw_free_lineoff (void) {
   struct s_dw_lineoff_2_cu *node;
   while ( (node = v_dw_lineoff_2_cu) ) {
      v_dw_lineoff_2_cu = m_ring_unlink(node, link);
      free(node);
   }
   return DW_DLV_OK;
}
int f_dw_cu_2_lineoff(Dwarf_Die die, Dwarf_Off *lineoff) {
   Dwarf_Off offset;
   struct s_dw_lineoff_2_cu *node;

   m_libdwarf_assert( dwarf_dieoffset(die, &offset, NULL) == DW_DLV_OK, return DW_DLV_ERROR);
   for (node = v_dw_lineoff_2_cu; node; node = m_ring_list(v_dw_lineoff_2_cu, node, link) ) {
      if ( offset != node->offset ) continue;
      if ( node->lineoff == -1 ) break;
      *lineoff = node->lineoff;
      return DW_DLV_OK;
   }
   return DW_DLV_NO_ENTRY;
}
int f_dw_lineoff_2_cu(Dwarf_Off lineoff, Dwarf_Die *die) {
   struct s_dw_lineoff_2_cu *node;

   for (node = v_dw_lineoff_2_cu; node; node = m_ring_list(v_dw_lineoff_2_cu, node, link) ) {
      if ( lineoff != node->lineoff ) continue;
      *die = node->die;
      return DW_DLV_OK;
   }
   return DW_DLV_NO_ENTRY;
}
int f_dw_abbrevoff_2_cu(Dwarf_Unsigned abbrev_offset, Dwarf_Die *die) {
   struct s_dw_lineoff_2_cu *node;

   for (node = v_dw_lineoff_2_cu; node; node = m_ring_list(v_dw_lineoff_2_cu, node, link) ) {
      if ( abbrev_offset != node->abbrev_offset ) continue;
      *die = node->die;
      return DW_DLV_OK;
   }
   return DW_DLV_NO_ENTRY;
}
int f_dw_print_lineoff (void) {
   struct s_dw_lineoff_2_cu *node;

   for (node = v_dw_lineoff_2_cu; node; node = m_ring_list(v_dw_lineoff_2_cu, node, link) ) {
       printf("   die %llu, loneoff %lld, abbrev %llu\n", node->offset, node->lineoff, node->abbrev_offset);
   }
   return DW_DLV_OK;
   
}
//@}
/** @name Output .debug_aranges
 */
//@{
int f_dwarf_print_aranges (char *indent) {
   Dwarf_Signed cnt;
   Dwarf_Arange *arang;

   int res, i;
   m_libdwarf_assert((res = dwarf_get_aranges(debug, &arang, &cnt, &error)) != DW_DLV_ERROR, return DW_DLV_OK;);
   if (res != DW_DLV_OK) return DW_DLV_OK;
   printf("%s<debug_aranges>\n",indent);
   for (i = 0; i < cnt; ++i) {
      Dwarf_Addr Addr;
      Dwarf_Unsigned length;
      Dwarf_Off die_offset;

      m_libdwarf_assert(dwarf_get_arange_info(arang[i], &Addr, &length, &die_offset, &error) != DW_DLV_ERROR, break;);
      if ( Addr || length ) {
         printf("%s  <arange id='ar:%d' low_pc='",indent, i+1);
         printf(v_elf_arch.fmt_addr, v_elf_arch.mask & Addr);
         printf("' length='%llu' ref='i:%llu' />\n", length, die_offset);
      }
      dwarf_dealloc(debug, arang[i], DW_DLA_ARANGE);
   }
   printf("%s</debug_aranges>\n",indent);
   dwarf_dealloc(debug, arang, DW_DLA_LIST);
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_pubnames
 */
//@{
int f_dwarf_print_pubnames (char *indent) {
   Dwarf_Signed cnt;
   Dwarf_Global *globs;
   char *name, *hname;
   Dwarf_Off die_offset;
   Dwarf_Off cu_offset;
   int res, i;

   m_libdwarf_assert( (res = dwarf_get_globals(debug, &globs, &cnt, &error)) != DW_DLV_ERROR, return DW_DLV_OK);
   if (res != DW_DLV_OK) return DW_DLV_OK;
   printf("%s<debug_pubnames>\n",indent);
   for (i = 0; i < cnt; ++i) {
      /* use globs[i] */
      m_libdwarf_assert(dwarf_global_name_offsets(globs[i], &name, &die_offset, &cu_offset, &error) != DW_DLV_ERROR, break);
      assert ( (hname = escape_html(name)) );
      dwarf_dealloc(debug, name, DW_DLA_STRING);
      printf("%s  <pubname id='pub:%d' cu='i:%llu' ref='i:%llu'>%s</pubname>\n",indent, i+1, cu_offset, die_offset, hname);
   }
   dwarf_globals_dealloc(debug, globs, cnt);
   printf("%s</debug_pubnames>\n",indent);
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_pubtypes
 */
//@{
int f_dwarf_print_pubtypes (char *indent) {
   Dwarf_Signed cnt;
   Dwarf_Type *types;
   char *name, *hname;
   Dwarf_Off die_offset;
   Dwarf_Off cu_offset;
   int res, i;

   m_libdwarf_assert( (res = dwarf_get_pubtypes(debug, &types, &cnt, &error)) != DW_DLV_ERROR, return DW_DLV_OK);
   if (res != DW_DLV_OK) return DW_DLV_OK;
   printf("%s<debug_pubtypes>\n", indent);
   for (i = 0; i < cnt; ++i) {
      m_libdwarf_assert(dwarf_pubtype_name_offsets(types[i], &name, &die_offset, &cu_offset, &error) != DW_DLV_ERROR, break);
      assert ( (hname = escape_html(name)) );
      dwarf_dealloc(debug, name, DW_DLA_STRING);
      printf("%s  <pubtype id='pt:%d' cu='i:%llu' ref='i:%llu'>%s</pubtype>\n",indent, i+1, cu_offset, die_offset, hname);
   }
   dwarf_types_dealloc(debug, types, cnt);
   printf("%s</debug_pubtypes>\n",indent);
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_weaknames (SGI ext)
 */
//@{
int f_dwarf_print_weaknames (char *indent) {
   Dwarf_Signed cnt;
   Dwarf_Weak *weaks;
   char *name, *hname;
   Dwarf_Off die_offset;
   Dwarf_Off cu_offset;
   int res, i;

   m_libdwarf_assert( (res = dwarf_get_weaks(debug, &weaks, &cnt, &error)) != DW_DLV_ERROR, return DW_DLV_OK);
   if (res != DW_DLV_OK) return DW_DLV_OK;
   printf("%s<debug_weaknames>\n",indent);
   for (i = 0; i < cnt; ++i) {
      m_libdwarf_assert(dwarf_weak_name_offsets(weaks[i], &name, &die_offset, &cu_offset, &error) != DW_DLV_ERROR, break);
      assert ( (hname = escape_html(name)) );
      dwarf_dealloc(debug, name, DW_DLA_STRING);
      printf("%s  <weakname id='wn:%d' cu='i:%llu' ref='i:%llu'>%s</weakname>\n",indent, i+1, cu_offset, die_offset, hname);
   }
   dwarf_weaks_dealloc(debug, weaks, cnt);
   printf("%s</debug_weaknames>\n",indent);
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_funcnames (SGI ext)
 */
//@{
int f_dwarf_print_funcnames (char *indent) {
   Dwarf_Signed cnt;
   Dwarf_Func *funcs;
   char *name, *hname;
   Dwarf_Off die_offset;
   Dwarf_Off cu_offset;
   int res, i;

   m_libdwarf_assert( (res = dwarf_get_funcs(debug, &funcs, &cnt, &error)) != DW_DLV_ERROR, return DW_DLV_OK);
   if (res != DW_DLV_OK) return DW_DLV_OK;
   printf("%s<debug_funcnames>\n",indent);
   for (i = 0; i < cnt; ++i) {
      m_libdwarf_assert(dwarf_func_name_offsets(funcs[i], &name, &die_offset, &cu_offset, &error) != DW_DLV_ERROR, break);
      assert ( (hname = escape_html(name)) );
      dwarf_dealloc(debug, name, DW_DLA_STRING);
      printf("%s  <funcname id='fn:%d' cu='i:%llu' ref='i:%llu'>%s</funcname>\n",indent, i+1, cu_offset, die_offset, hname);
   }
   dwarf_funcs_dealloc(debug, funcs, cnt);
   printf("%s</debug_funcnames>\n",indent);
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_typenames (SGI ext)
 */
//@{
int f_dwarf_print_typenames (char *indent) {
   Dwarf_Signed cnt;
   Dwarf_Type *types;
   char *name, *hname;
   Dwarf_Off die_offset;
   Dwarf_Off cu_offset;
   int res, i;

   m_libdwarf_assert( (res = dwarf_get_types(debug, &types, &cnt, &error)) != DW_DLV_ERROR, return DW_DLV_OK);
   if (res != DW_DLV_OK) return DW_DLV_OK;
   printf("%s<debug_typenames>\n", indent);
   for (i = 0; i < cnt; ++i) {
      m_libdwarf_assert(dwarf_type_name_offsets(types[i], &name, &die_offset, &cu_offset, &error) != DW_DLV_ERROR, break);
      assert ( (hname = escape_html(name)) );
      dwarf_dealloc(debug, name, DW_DLA_STRING);
      printf("%s  <typename id='tn:%d' cu='i:%llu' ref='i:%llu'>%s</typename>\n",indent, i+1, cu_offset, die_offset, hname);
   }
   dwarf_types_dealloc(debug, types, cnt);
   printf("%s</debug_typenames>\n",indent);
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_varnames (SGI ext)
 */
//@{
int f_dwarf_print_varnames (char *indent) {
   Dwarf_Signed cnt;
   Dwarf_Var *vars;
   char *name, *hname;
   Dwarf_Off die_offset;
   Dwarf_Off cu_offset;
   int res, i;

   m_libdwarf_assert( (res = dwarf_get_vars(debug, &vars, &cnt, &error)) != DW_DLV_ERROR, return DW_DLV_OK);
   if (res != DW_DLV_OK) return DW_DLV_OK;
   printf("%s<debug_varnames>\n", indent);
   for (i = 0; i < cnt; ++i) {
      m_libdwarf_assert(dwarf_var_name_offsets(vars[i], &name, &die_offset, &cu_offset, &error) != DW_DLV_ERROR, break);
      assert ( (hname = escape_html(name)) );
      dwarf_dealloc(debug, name, DW_DLA_STRING);
      printf("%s  <varname id='vn:%d' cu='i:%llu' ref='i:%llu'>%s</varname>\n",indent, i+1, cu_offset, die_offset, hname);
   }
   dwarf_vars_dealloc(debug, vars, cnt);
   printf("%s</debug_varnames>\n",indent);
   return DW_DLV_OK;
}
//@}
/** @name Output debug_info
 */
//@{
int f_dwarf_print_attr(Dwarf_Debug debug, Dwarf_Die die, Dwarf_Attribute Attribute, struct s_dwarf_cu_info *cu_info, void *cooky) {
   Dwarf_Error error;
   int             ret;
   Dwarf_Half      attr;
   char *indent = (char *)cooky -2*cu_info->level -2;

   m_libdwarf_assert((ret = dwarf_whatattr(Attribute, &attr, &error)) != DW_DLV_ERROR, return DW_DLV_ERROR);

   // printf("%s<at_%s>",  indent, f_dw_attr_2_str(attr, 1));
   m_dwarf2xml_assert((ret = f_dw_attr_value_2_str(indent, Attribute, debug, cu_info)) != DW_DLV_ERROR, return DW_DLV_ERROR);
   // printf("</at_%s>\n", f_dw_attr_2_str(attr, 0));

   return DW_DLV_OK;
}
/* Search for srcline ref
 * Only CU have source lines
int f_dwarf_print_die_get_srcline_ref(Dwarf_Debug debug, Dwarf_Die die, Dwarf_Unsigned *fileno, Dwarf_Unsigned *lineno) {
   Dwarf_Signed cnt;
   Dwarf_Line *linebuf;
   int ret;

   m_libdwarf_assert((ret = dwarf_srclines(die, &linebuf,&cnt, &error)) != DW_DLV_ERROR, return ret);
   if ( ret != DW_DLV_OK || cnt == 0 ) return DW_DLV_NO_ENTRY;

   m_libdwarf_assert((ret = dwarf_lineno(linebuf[0], lineno, NULL)) != DW_DLV_ERROR, return ret);
   m_libdwarf_assert((ret = dwarf_line_srcfileno(linebuf[0], fileno, NULL)) != DW_DLV_ERROR, return ret);
   dwarf_srclines_dealloc(debug, linebuf, cnt);
   return DW_DLV_OK; 
}
 */
int f_dwarf_print_die_begin(Dwarf_Debug debug, Dwarf_Die die, struct s_dwarf_cu_info *cu_info, void *cooky) {
   Dwarf_Half tagval;
   Dwarf_Off  offset;
   Dwarf_Off  lineoff;
   char *indent = (char *)cooky -2*cu_info->level;
   int ret;


   m_libdwarf_assert( dwarf_tag (die, &tagval, NULL) == DW_DLV_OK, return DW_DLV_ERROR);
   m_libdwarf_assert( dwarf_dieoffset(die, &offset, NULL) == DW_DLV_OK, return DW_DLV_ERROR);
   /* Search for srcline ref */
   printf("%s<tag_%s id='i:%llu' ab='ab:%lluc%d'", indent, 
            f_dw_tag_2_str(tagval, 1), offset, cu_info->abbrev_offset, dwarf_die_abbrev_code(die));
   if ( die == cu_info->die ) {
      printf(" version='%hu' address_size='%hu'", cu_info->version_stamp, cu_info->address_size);
      m_libdwarf_assert( (ret = f_dw_cu_2_lineoff(die, &lineoff)) != DW_DLV_ERROR, return DW_DLV_ERROR);
      if ( ret == DW_DLV_OK )
         printf(" sl='sl:%llu'", lineoff);
   }
   printf(" >\n");
   return DW_DLV_OK; 
}
int f_dwarf_print_die_end(Dwarf_Debug debug, Dwarf_Die die, struct s_dwarf_cu_info *cu_info, void *cooky) {
   Dwarf_Half      tagval;
   char *indent = (char *)cooky -2*cu_info->level;

   m_libdwarf_assert( dwarf_tag (die, &tagval, NULL) == DW_DLV_OK, return DW_DLV_ERROR);
   printf("%s</tag_%s>\n", indent, f_dw_tag_2_str(tagval, 0));
   return DW_DLV_OK; 
}
int f_dwarf_print_cu_tree (char *indent) {
   int ret;

   printf("%s<debug_info>\n",indent);
   ret = f_dwarf_walk_dies (debug, &f_dwarf_print_die_begin, &f_dwarf_print_attr, &f_dwarf_print_die_end, indent); 
   printf("%s</debug_info>\n",indent);
   return ret; 
}
//@}

/** @name Output .debug_abbrevs
 */
//@{
int f_dwarf_print_abbrevs (char *indent) {
   Dwarf_Unsigned offset = 0;
   Dwarf_Abbrev abbrev;
   Dwarf_Unsigned length;
   Dwarf_Unsigned attr_count;

   Dwarf_Half tag;
   Dwarf_Unsigned code;
   Dwarf_Signed flag;
   Dwarf_Die die;

   Dwarf_Signed ab_index;
   Dwarf_Half ab_attr_num;
   Dwarf_Signed ab_form;
   Dwarf_Off ab_ent_offset = 0,  ab_cu_offset = 0, cu_offset;
   int start = 0, cu_no = 0;

   printf("%s<debug_abbrev>\n", indent);
   while ( dwarf_get_abbrev(debug, offset, &abbrev, &length, &attr_count, &error) == DW_DLV_OK) {
      m_libdwarf_assert(dwarf_get_abbrev_tag(abbrev, &tag, &error) == DW_DLV_OK, break );
      if ( f_dw_abbrevoff_2_cu(offset, &die) == DW_DLV_OK) {
         if ( start ) printf("%s  </ab_table>\n", indent);
         ab_cu_offset = offset;
         m_libdwarf_assert( dwarf_dieoffset(die, &cu_offset, &error) == DW_DLV_OK, break );
         printf("%s  <ab_table id='ab:%llu' ref='i:%llu' >\n", indent, ab_cu_offset, cu_offset);
         cu_no++;
         start = 1;
      }
      if ( !tag ) {
         offset += length;
         continue;
      }
      m_libdwarf_assert(dwarf_get_abbrev_code(abbrev, &code, &error) == DW_DLV_OK, break );
      m_libdwarf_assert(dwarf_get_abbrev_children_flag(abbrev, &flag, &error) == DW_DLV_OK, break );
      printf("%s    <ab_tag name='tag_%s' id='ab:%lluc%llu' children='%s' >\n", 
              indent, f_dw_tag_2_str(tag, 2), ab_cu_offset, code, (flag == DW_children_yes)?"true":"false");
      for ( ab_index = 0; ab_index < attr_count; ab_index++ ) {
         m_libdwarf_assert(dwarf_get_abbrev_entry(abbrev, ab_index, &ab_attr_num, &ab_form, &ab_ent_offset, &error) != DW_DLV_ERROR,
                           break);
         printf("%s      <ab_attr name='%s' form='%s' />\n", indent, 
                       f_dw_attr_2_str(ab_attr_num, 2), 
                       f_dw_val_2_str(ab_form, c_dw_form_tbl));
      }
      printf("%s    </ab_tag>\n", indent);
      offset += length;
   }
   if ( start ) 
      printf("%s  </ab_table>\n", indent);
   printf("%s</debug_abbrev>\n", indent);
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_line
 */
//@{
struct s_dw_pc_range_2_die {
   struct s_ring link;
   Dwarf_Addr lo_pc;
   size_t len;
   Dwarf_Die die;
   Dwarf_Off offset;
   int cu_no;
} *v_dw_pc_range_2_die;
int f_dw_pc_range_add (Dwarf_Debug debug, Dwarf_Die die, struct s_dwarf_cu_info *cu_info, void *cooky) {
   int ret, i;
   Dwarf_Addr lo_pc, hi_pc;
   Dwarf_Attribute Attribute;
   struct s_dw_pc_range_2_die *new;

   m_libdwarf_assert( (ret = dwarf_attr (die, DW_AT_ranges, &Attribute, NULL)) != DW_DLV_ERROR, return DW_DLV_ERROR);
   if ( ret == DW_DLV_OK ) {
      Dwarf_Half      form;
      Dwarf_Off rangesoffset = -1;
      Dwarf_Ranges *rangesbuf;
      Dwarf_Signed rangecount;
      Dwarf_Unsigned bytecount;
      /* get CU lo_pc */
      m_libdwarf_assert( (ret = dwarf_lowpc (cu_info->die, &lo_pc, &error)) != DW_DLV_ERROR, return DW_DLV_ERROR);
      /* get at_ranges value */
      m_libdwarf_assert(dwarf_whatform(Attribute, &form, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
      // m_libdwarf_assert ( form == DW_FORM_data4  || form == DW_FORM_data8, return DW_DLV_ERROR);
      switch ( form ) {
      case DW_FORM_data4:
      case DW_FORM_data8:
         break;
      case DW_FORM_sec_offset: {
         Dwarf_Off offset;
         m_libdwarf_assert((ret = dwarf_global_formref(Attribute, &offset, &error)) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
         goto no_range;
         break;
      }
      default:
         m_error("Unexpected Attribute DW_AT_%s form %s(%d)", 
                 f_dw_attr_2_str(DW_AT_ranges, 1), f_dw_form_2_str(form), form);
         return DW_DLV_ERROR;
      }
      m_libdwarf_assert(dwarf_formudata(Attribute, &rangesoffset, &error) !=  DW_DLV_ERROR,return DW_DLV_ERROR);

      m_libdwarf_assert(
         (ret = dwarf_get_ranges(debug, rangesoffset, &rangesbuf, &rangecount, &bytecount, &error)) != DW_DLV_ERROR, 
         return ret; );
      if ( ret == DW_DLV_OK ) {
         for ( i = 0; i < rangecount; ++i ) {
             assert ( (new = malloc(sizeof(*new))) );
             memset(new, 0, sizeof(*new));
             switch ( rangesbuf[i].dwr_type ) {
             case DW_RANGES_ENTRY: 
                new->lo_pc = rangesbuf[i].dwr_addr1 + lo_pc;
                new->len = rangesbuf[i].dwr_addr2 - rangesbuf[i].dwr_addr1;
                break;
             case DW_RANGES_ADDRESS_SELECTION:
                new->lo_pc = rangesbuf[i].dwr_addr2 + lo_pc;
                new->len = 1;
                break;
             case DW_RANGES_END: 
                free(new);
                continue;
             }
             new->die = die;
             new->cu_no = cu_info->cu_no;
             m_libdwarf_assert( dwarf_dieoffset(die, &new->offset, &error) == DW_DLV_OK, return DW_DLV_ERROR);
             assert ( (v_dw_pc_range_2_die = m_ring_link(v_dw_pc_range_2_die, link, new)) );
         }
         dwarf_ranges_dealloc(debug, rangesbuf, rangecount);
      }
      return DW_DLV_OK;
   }
no_range:
   m_libdwarf_assert( (ret = dwarf_lowpc (die, &lo_pc, NULL)) != DW_DLV_ERROR, return DW_DLV_ERROR);
   if ( ret == DW_DLV_NO_ENTRY ) return DW_DLV_OK;
   m_libdwarf_assert( (ret = dwarf_highpc(die, &hi_pc, NULL)) != DW_DLV_ERROR, return DW_DLV_ERROR);
   if ( ret == DW_DLV_NO_ENTRY ) return DW_DLV_OK;

   assert ( (new = malloc(sizeof(*new))) );
   memset(new, 0, sizeof(*new));

   new->die = die;
   m_libdwarf_assert( dwarf_dieoffset(die, &new->offset, &error) == DW_DLV_OK, return DW_DLV_ERROR);
   new->lo_pc = lo_pc;
   new->len   = hi_pc -lo_pc;
   new->cu_no = cu_info->cu_no;

   assert ( (v_dw_pc_range_2_die = m_ring_link(v_dw_pc_range_2_die, link, new)) );
   return DW_DLV_OK;
}
int f_dwarf_record_pc_range (void) {
   return f_dwarf_walk_dies (debug, &f_dw_pc_range_add, 0, 0 ,0); 
}
int f_dw_free_pc_range (void) {
   struct s_dw_pc_range_2_die *node;
   while ( (node = v_dw_pc_range_2_die) ) {
      v_dw_pc_range_2_die = m_ring_unlink(node, link);
      free(node);
   }
   return DW_DLV_OK;
}
int f_dw_pc_range_2_die(Dwarf_Debug dbg, Dwarf_Addr lo_pc, int cu_no, Dwarf_Off *offset, Dwarf_Die *die) {
   size_t min_size = -1;
   struct s_dw_pc_range_2_die *node, *min_node = 0;

   for (node = v_dw_pc_range_2_die; node; node = m_ring_list(v_dw_pc_range_2_die, node, link) ) {
      if ( cu_no >= 0 && cu_no != node->cu_no ) continue;
      if ( lo_pc != node->lo_pc ) continue;
      if ( min_size < node->len ) continue;
      min_node = node;
   }
   if ( !min_node ) 
      return DW_DLV_NO_ENTRY;

   if ( die ) *die = min_node->die;
   if ( !offset ) return DW_DLV_OK;
   m_libdwarf_assert( dwarf_dieoffset(min_node->die, offset, NULL) == DW_DLV_OK, return DW_DLV_ERROR);
   return DW_DLV_OK;
}
int f_dw_print_pc_range (void) {
   struct s_dw_pc_range_2_die *node;

   for (node = v_dw_pc_range_2_die; node; node = m_ring_list(v_dw_pc_range_2_die, node, link) ) {
       printf("   die %llu, lo_pc %llx, len %lu\n", node->offset, node->lo_pc, node->len);
   }
   return DW_DLV_OK;
   
}
#if 0
/* DOES NOT WORK ?
 * lines offset do not match dw_at_stmt_list value
 */
/* Converion of PC to line offsets */
struct s_dwarf_pc_to_lineoffset  {
   Dwarf_Die die;
   Dwarf_Unsigned offscnt;
   Dwarf_Off *offs;
   Dwarf_Addr *addrs;
} v_dwarf_pc_to_lineoffset;
/* die == 0 desalloc */
int _dwarf_line_address_offsets(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Addr ** addrs, Dwarf_Off ** offs, Dwarf_Unsigned * returncount, Dwarf_Error * err);
int f_dwarf_pc_to_lineoffset(Dwarf_Die die, Dwarf_Addr pc, Dwarf_Off *off) {
   Dwarf_Unsigned i;
   int ret;

   if ( v_dwarf_pc_to_lineoffset.die && die != v_dwarf_pc_to_lineoffset.die ) {
      dwarf_dealloc(debug, v_dwarf_pc_to_lineoffset.addrs, DW_DLA_ADDR);
      dwarf_dealloc(debug, v_dwarf_pc_to_lineoffset.offs, DW_DLA_ADDR);
      v_dwarf_pc_to_lineoffset.die = 0;
   }
   if ( !die ) return DW_DLV_OK;

   if ( die != v_dwarf_pc_to_lineoffset.die ) {
      m_libdwarf_assert((ret = _dwarf_line_address_offsets(debug, die, 
                                 &v_dwarf_pc_to_lineoffset.addrs, 
                                 &v_dwarf_pc_to_lineoffset.offs, 
                                 &v_dwarf_pc_to_lineoffset.offscnt, NULL)) == DW_DLV_OK, return ret);
      v_dwarf_pc_to_lineoffset.die = die;
   }
   for ( i = 0; i < v_dwarf_pc_to_lineoffset.offscnt; ++i) {
       if ( pc != v_dwarf_pc_to_lineoffset.addrs[i] ) continue;
       *off =  v_dwarf_pc_to_lineoffset.offs[i];
       return DW_DLV_OK;
   }
   return DW_DLV_NO_ENTRY;
}
#endif
int f_dwarf_print_srclines(char *indent, Dwarf_Die die, Dwarf_Off lineoff, int cu_no) {
   Dwarf_Signed cnt;
   Dwarf_Line *linebuf;
   Dwarf_Bool esbool, bsbool, bbbool;
   Dwarf_Unsigned lineno, fileno;
   Dwarf_Signed coloff;
   Dwarf_Addr lineaddr;
   Dwarf_Off funct_die_offset;
   int ret, i;

   m_libdwarf_assert((ret = dwarf_srclines(die, &linebuf,&cnt, &error)) != DW_DLV_ERROR, );
   printf("%s<sl_lines id='sl:%llul'>\n", indent, lineoff);
   if ( ret == DW_DLV_OK ) { 
      for (i = 0; i < cnt; ++i) {
         m_libdwarf_assert((ret = dwarf_linebeginstatement(linebuf[i], &bsbool, &error)) != DW_DLV_ERROR, break);
         m_libdwarf_assert((ret = dwarf_lineendsequence(linebuf[i], &esbool, &error)) != DW_DLV_ERROR, break);
         m_libdwarf_assert((ret = dwarf_lineblock(linebuf[i], &bbbool, &error)) != DW_DLV_ERROR, break);
         m_libdwarf_assert((ret = dwarf_lineno(linebuf[i], &lineno, &error)) != DW_DLV_ERROR, break);
         m_libdwarf_assert((ret = dwarf_line_srcfileno(linebuf[i], &fileno, &error)) != DW_DLV_ERROR, break);
         m_libdwarf_assert((ret = dwarf_lineoff(linebuf[i], &coloff, &error)) != DW_DLV_ERROR, break);
         m_libdwarf_assert((ret = dwarf_lineaddr(linebuf[i], &lineaddr, &error)) != DW_DLV_ERROR, break);
         m_libdwarf_assert((ret = f_dw_pc_range_2_die(debug, lineaddr, cu_no, &funct_die_offset, 0)) != DW_DLV_ERROR, return ret);
         if ( ret == DW_DLV_OK )
            printf("%s<sl_line id='sl:%llui%d' ref='i:%llu' >\n", indent-2, lineoff, i +1, funct_die_offset);
         else
            printf("%s<sl_line id='sl:%llui%d' >\n", indent-2, lineoff, i +1);
         printf("%s  <sl_low_pc>", indent-2); printf(v_elf_arch.fmt_addr, v_elf_arch.mask & lineaddr); printf("</sl_low_pc>\n");
         printf("%s  <sl_decl file='sl:%lluf%llu' line='%llu'", indent-2, lineoff, fileno, lineno); 
         if ( coloff != -1 ) printf(" col='%lld'", lineoff);
         printf("/>\n%s  <sl_properties stm_start='%s'", indent-2, bsbool?"true":"false");
         printf(" block_start='%s'", bbbool?"true":"false");
         printf(" seq_end='%s' />\n", esbool?"true":"false");
         printf("%s</sl_line>\n", indent-2);
      }
      dwarf_srclines_dealloc(debug, linebuf, cnt);
   }
   printf("%s</sl_lines>\n", indent);

   return ret;
} 
int f_dwarf_print_srcfiles(char *indent, Dwarf_Die die, Dwarf_Off lineoff) {
   Dwarf_Signed cnt;
   char **srcfiles;
   int res, i;

   m_libdwarf_assert((res = dwarf_srcfiles(die, &srcfiles, &cnt, &error)) != DW_DLV_ERROR, );
   printf("%s<sl_files id='sl:%lluf'>\n", indent, lineoff);
   if ( res == DW_DLV_OK ) { 
      for (i = 0; i < cnt; ++i) {
         char *html = escape_html(srcfiles[i]);
         printf("%s<sl_file id='sl:%lluf%d'>%s</sl_file>\n", indent-2, lineoff, i+1, html);
         free(html);
         dwarf_dealloc(debug, srcfiles[i], DW_DLA_STRING);
      }
      dwarf_dealloc(debug, srcfiles, DW_DLA_LIST);
   }
   printf("%s</sl_files>\n", indent);

   return DW_DLV_OK;
}
int f_dwarf_print_lines (char *indent) {
   int             ret;
   Dwarf_Die       die;
   Dwarf_Half      tagval;
   Dwarf_Unsigned  next_cu_header;
   Dwarf_Off offset;
   Dwarf_Off lineoff;
   int cu_no = 0, start = 0;

   
    while ( (ret = dwarf_next_cu_header (debug, NULL, NULL, NULL, NULL,&next_cu_header, NULL)) == DW_DLV_OK ) {
      m_libdwarf_assert((ret = dwarf_siblingof (debug, 0, &die, NULL)) != DW_DLV_ERROR, break);
      m_libdwarf_assert((ret = dwarf_tag (die, &tagval, NULL)) == DW_DLV_OK, break);
      m_dwarf2xml_assert(tagval == DW_TAG_compile_unit, ret = DW_DLV_ERROR; break; );

      m_libdwarf_assert( dwarf_dieoffset(die, &offset, NULL) == DW_DLV_OK, return DW_DLV_ERROR);
      m_libdwarf_assert((ret = f_dw_cu_2_lineoff(die, &lineoff)) != DW_DLV_ERROR, return DW_DLV_ERROR);
      if ( ret == DW_DLV_NO_ENTRY )
         continue;
      if ( !start ) { start = 1; printf("%s<debug_line>\n",indent); }
      printf("%s  <sl_compile_unit id='sl:%llu' ref='i:%llu'>\n", indent, lineoff, offset);
      f_dwarf_print_srcfiles(indent-4, die, lineoff);
      f_dwarf_print_srclines(indent-4, die, lineoff, cu_no);
      printf("%s  </sl_compile_unit>\n", indent);
      ++cu_no;
   }
   if ( start ) printf("%s</debug_line>\n",indent);
   if (ret == DW_DLV_NO_ENTRY) return DW_DLV_OK;
   return ret;
}
//@}
/** @name Output .debug_frame
 */
//@{
int f_dwarf_print_frame_op(char *indent, Dwarf_Cie Cie, Dwarf_Ptr instr_block, Dwarf_Unsigned instr_block_length) {
   Dwarf_Signed frameops_cnt = 0;
   Dwarf_Frame_Op *frameops;

   int ret, fop;
#ifdef EXPAND_FRAME_INSTR_USE_CIE
   m_libdwarf_assert((ret = dwarf_expand_frame_instructions(Cie, instr_block, instr_block_length,
                                &frameops, &frameops_cnt, &error))  != DW_DLV_ERROR, return DW_DLV_ERROR; );
#else
   m_libdwarf_assert((ret = dwarf_expand_frame_instructions(debug, instr_block, instr_block_length,
                                &frameops, &frameops_cnt, &error))  != DW_DLV_ERROR, return DW_DLV_ERROR; );
#endif
   if ( ret == DW_DLV_OK ) {
      for (fop = 0; fop < frameops_cnt; ++fop) {
         /* use frameops[fop] */
         printf("%s<fr_instr pc_off='+%llu' op='%s' p1='%hu' p2='%lld' />\n", indent, 
             frameops[fop].fp_instr_offset,
             f_dw_val_2_str((frameops[fop].fp_base_op<<6)+frameops[fop].fp_extended_op,c_dw_cfa_tbl),
             frameops[fop].fp_register, frameops[fop].fp_offset);
      }
   }
   dwarf_dealloc(debug, frameops, DW_DLA_FRAME_BLOCK);
   return DW_DLV_OK;
}
int f_dwarf_print_cie (char *indent, char *pfxid, Dwarf_Cie *Cie, Dwarf_Signed cie_count, Dwarf_Fde *Fde, Dwarf_Signed fde_count) {
   Dwarf_Unsigned bytes_in_cie;
   Dwarf_Small version;
   char *augmenter;
   Dwarf_Unsigned code_alignment_factor;
   Dwarf_Signed data_alignment_factor;
   Dwarf_Half return_address_register_rule;
   Dwarf_Ptr initial_instructions;
   Dwarf_Unsigned initial_instructions_length;

   Dwarf_Addr low_pc;
   Dwarf_Unsigned func_length;
   Dwarf_Ptr fde_bytes;
   Dwarf_Unsigned fde_byte_length;
   Dwarf_Off cie_offset;
   Dwarf_Signed cie_index;
   Dwarf_Off fde_offset;
   Dwarf_Off die_offset;

   int ret, cie, fde, cie_off_accu = 0; 

   for (cie = 0; cie < cie_count; ++cie) {
      /* use Cie[i] */
      m_libdwarf_assert(dwarf_get_cie_info(Cie[cie], &bytes_in_cie, &version, &augmenter, &code_alignment_factor,
                           &data_alignment_factor, &return_address_register_rule, &initial_instructions,
                           &initial_instructions_length, &error) != DW_DLV_ERROR, 
                        break; );
      printf("%s  <fr_cie id='%s:c%d' version='%hhu' augmenter='%s' >\n", indent, pfxid, cie, version, augmenter);
      cie_off_accu += bytes_in_cie +4;
      printf("%s    <fr_alignment_factor code='%llu' data='%lld' />\n", indent, code_alignment_factor, data_alignment_factor);      
      printf("%s    <fr_return_address register_rule='%hu' />\n", indent, return_address_register_rule);      

      printf("%s    <fr_initial_instructions>\n", indent);      
      m_dwarf2xml_assert(f_dwarf_print_frame_op(indent -6, Cie[cie], initial_instructions, initial_instructions_length) != DW_DLV_ERROR, return DW_DLV_ERROR)
      printf("%s    </fr_initial_instructions>\n", indent);      

      for (fde = 0; fde < fde_count; ++fde) {
         /* use Fde[fde] */
         m_libdwarf_assert((ret = dwarf_get_fde_range(Fde[fde], &low_pc, &func_length, &fde_bytes, &fde_byte_length, 
                              &cie_offset, &cie_index, &fde_offset, &error)) != DW_DLV_ERROR, 
                           break; );
         if ( cie_index != cie ) continue;
         m_dwarf2xml_assert( cie_off_accu == fde_offset,
            fprintf(stderr, "warning, Next cie record offset may not be correct (%x) == (%llx)\n", 
                    cie_off_accu, fde_offset);
            cie_off_accu = fde_offset; );
         cie_off_accu += fde_byte_length +4;
         if ( f_dw_pc_range_2_die(debug, low_pc, -1, &die_offset, 0) == DW_DLV_OK )
            printf("%s    <fr_fde id='%s:c%df%llu' ref='i:%llu' low_pc='", indent, pfxid, cie, fde_offset, die_offset);
         else
            printf("%s    <fr_fde id='%s:c%df%llu' low_pc='", indent, pfxid, cie, fde_offset);
         printf(v_elf_arch.fmt_addr, v_elf_arch.mask & low_pc); 
         printf("' func_length='%llu' >\n", func_length);
         /* why func_length, &fde_bytes not accepted by dwarf_expand_frame_instructions ? */
         m_libdwarf_assert((ret = dwarf_get_fde_instr_bytes(Fde[fde], &fde_bytes,  
                                     &fde_byte_length, &error))  != DW_DLV_ERROR, 
                               printf("%s    </fr_fde>\n", indent); break; );
         if (ret != DW_DLV_OK) continue;
         m_dwarf2xml_assert(f_dwarf_print_frame_op(indent -6, Cie[cie], fde_bytes, fde_byte_length) != DW_DLV_ERROR, 
                            printf("%s    </fr_fde>\n", indent); break;)
         printf("%s    </fr_fde>\n", indent);
      }
      printf("%s  </fr_cie>\n", indent);
   }
   return DW_DLV_OK;
}
int f_dwarf_print_frames (char *indent) {
   Dwarf_Cie *Cie;
   Dwarf_Signed cie_count;
   Dwarf_Fde *Fde;
   Dwarf_Signed fde_count;

   int ret; 

   m_libdwarf_assert((ret = dwarf_get_fde_list(debug, &Cie, &cie_count, &Fde, &fde_count, &error)) != DW_DLV_ERROR,
       return DW_DLV_ERROR; );
   if (ret != DW_DLV_OK) 
     return DW_DLV_OK;
   printf("%s<debug_frame>\n", indent);
   m_dwarf2xml_assert((ret = f_dwarf_print_cie (indent, "fr", Cie,  cie_count, Fde, fde_count)) == DW_DLV_OK, return DW_DLV_ERROR);
   dwarf_fde_cie_list_dealloc(debug, Cie, cie_count, Fde,fde_count);
   printf("%s</debug_frame>\n", indent);

   m_libdwarf_assert((ret = dwarf_get_fde_list_eh(debug, &Cie, &cie_count, &Fde, &fde_count, &error)) != DW_DLV_ERROR,
       return DW_DLV_ERROR; );
   if (ret != DW_DLV_OK) 
     return DW_DLV_OK;
   printf("%s<eh_frame>\n", indent);
   m_dwarf2xml_assert((ret = f_dwarf_print_cie (indent, "eh", Cie,  cie_count, Fde, fde_count)) == DW_DLV_OK, return DW_DLV_ERROR);
   dwarf_fde_cie_list_dealloc(debug, Cie, cie_count, Fde,fde_count);
   printf("%s</eh_frame>\n", indent);
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_loc
 */
//@{
int f_dwarf_print_locs (char *indent) {
   int ret, start = -1;
   Dwarf_Unsigned next_entry;
   Dwarf_Unsigned offset = 0;
   Dwarf_Addr hipc, lopc;
   Dwarf_Ptr data;
   Dwarf_Unsigned entry_len;

   Dwarf_Signed lcnt;
   Dwarf_Locdesc *llbuf;

   indent -= 2;
   while ( 1 ) {
      m_libdwarf_assert((ret = dwarf_get_loclist_entry(debug, offset, &hipc, &lopc, &data, &entry_len, &next_entry,&error)) != DW_DLV_ERROR,
                        break; );
      if (ret != DW_DLV_OK) break;
      m_libdwarf_assert((ret = dwarf_loclist_from_expr(debug, data, entry_len, &llbuf,&lcnt, &error)) != DW_DLV_ERROR,
                        break );
      if (ret != DW_DLV_OK) continue;
      if ( !lopc && !hipc && !start) {
         printf("%s</loc_list>\n", indent);
         start = 1;
         offset = next_entry;
         continue;
      }
      if ( start ) {
         if ( start == -1 ) printf("%s<debug_loc>\n", indent);
         printf("%s<loc_list id='loc:%llu'>\n", indent, offset);
         start = 0;
      }
      llbuf->ld_lopc = lopc;
      llbuf->ld_hipc = hipc;
      f_dw_print_loc_expr(indent -2, llbuf, offset);
      dwarf_dealloc(debug, llbuf->ld_s, DW_DLA_LOC_BLOCK);
      dwarf_dealloc(debug, llbuf, DW_DLA_LOCDESC);

      offset = next_entry;
   }
   if ( !start )
      printf("%s</loc_list>\n", indent);
   indent += 2;
   if ( start != -1 ) printf("%s</debug_loc>\n",indent);
   if (ret == DW_DLV_NO_ENTRY) return DW_DLV_OK;
   return ret;
}
//@}
/** @name Output .debug_ranges
 */
//@{
#if NOT
/* First appears in DWARF3.
   The dwr_addr1/addr2 data is either an offset (DW_RANGES_ENTRY)
   or an address (dwr_addr2 in DW_RANGES_ADDRESS_SELECTION) or
   both are zero (DW_RANGES_END).
*/
enum Dwarf_Ranges_Entry_Type { DW_RANGES_ENTRY,
    DW_RANGES_ADDRESS_SELECTION,
    DW_RANGES_END };
typedef struct {
    Dwarf_Addr dwr_addr1;
    Dwarf_Addr dwr_addr2;
    enum Dwarf_Ranges_Entry_Type  dwr_type;
} Dwarf_Ranges;
int dwarf_get_ranges(Dwarf_Debug /*dbg*/,
    Dwarf_Off /*rangesoffset*/,
    Dwarf_Ranges ** /*rangesbuf*/,
    Dwarf_Signed * /*listlen*/,
    Dwarf_Unsigned * /*bytecount*/,
    Dwarf_Error * /*error*/);
void dwarf_ranges_dealloc(Dwarf_Debug /*dbg*/,
    Dwarf_Ranges * /*rangesbuf*/,
    Dwarf_Signed /*rangecount*/);
#endif
int f_dwarf_print_ranges (char *indent) {
   int ret, i;
   Dwarf_Off rangesoffset = 0;
   Dwarf_Ranges *rangesbuf;
   Dwarf_Signed rangecount;
   Dwarf_Unsigned bytecount;

   m_libdwarf_assert((ret = dwarf_get_ranges(debug, 0, &rangesbuf, &rangecount, &bytecount, &error)) != DW_DLV_ERROR, return ret; );
   if ( ret != DW_DLV_OK ) return DW_DLV_OK;

   printf("%s<debug_ranges>\n", indent);
   while ( (ret = dwarf_get_ranges(debug, rangesoffset, &rangesbuf, &rangecount, &bytecount, &error)) == DW_DLV_OK ) {
      printf("%s<rg_range id='rg:%llu' >\n",indent -2, rangesoffset);
      for ( i = 0; i < rangecount; ++i ) {
          switch ( rangesbuf[i].dwr_type ) {
          case DW_RANGES_ENTRY: 
             printf("%s<rg_entry low_pc='",indent -4); 
             printf(v_elf_arch.fmt_addr, v_elf_arch.mask & rangesbuf[i].dwr_addr1);
             printf("' high_pc='"); 
             printf(v_elf_arch.fmt_addr, v_elf_arch.mask & rangesbuf[i].dwr_addr2);
             printf("' />\n"); 
             break;
          case DW_RANGES_ADDRESS_SELECTION: 
             printf("%s<rg_entry low_pc='",indent -4); 
             printf(v_elf_arch.fmt_addr, v_elf_arch.mask & rangesbuf[i].dwr_addr2);
             printf("' />\n"); 
             break;
          case DW_RANGES_END: 
             printf("%s<rg_entry />\n",indent -4);
             break;
          }
      }
      printf("%s</rg_range>\n",indent -2);
      dwarf_ranges_dealloc(debug, rangesbuf, rangecount);
      if ( bytecount == 0 ) return DW_DLV_OK;
      rangesoffset += bytecount;
   }
   printf("%s</debug_ranges>\n", indent);
   /* dwarf_get_ranges always finish with error ! */
   return DW_DLV_OK;
}
//@}
/** @name Output .debug_macinfo
 */
//@{
struct s_dw_mac_info_2_die {
   struct s_ring link;
   Dwarf_Off mac_off;
   Dwarf_Die die;
} *v_dw_mac_info_2_die;
int f_dw_mac_info_add (Dwarf_Debug debug, Dwarf_Die die, Dwarf_Attribute Attribute, struct s_dwarf_cu_info *cu_info, void *cooky) {
   int             ret;
   Dwarf_Half      attr;
   Dwarf_Half      form;
   Dwarf_Unsigned  mac_off;
   struct s_dw_mac_info_2_die *new;

   if ( cu_info->level > 1 ) return DW_DLV_NO_ENTRY;

   m_libdwarf_assert((ret = dwarf_whatattr(Attribute, &attr, NULL)) != DW_DLV_ERROR, return DW_DLV_ERROR);
   if ( attr != DW_AT_macro_info ) return DW_DLV_OK;

   m_libdwarf_assert(dwarf_whatform(Attribute, &form, &error) !=  DW_DLV_ERROR, return DW_DLV_ERROR);
   m_libdwarf_assert ( form == DW_FORM_data4  || form == DW_FORM_data8, return DW_DLV_ERROR);
   m_libdwarf_assert(dwarf_formudata(Attribute, &mac_off, NULL) !=  DW_DLV_ERROR,return DW_DLV_ERROR);

   assert ( (new = malloc(sizeof(*new))) );
   memset(new, 0, sizeof(*new));
   new->die = die;
   new->mac_off = mac_off;
   assert ( (v_dw_mac_info_2_die = m_ring_link(v_dw_mac_info_2_die, link, new)) );
   return DW_DLV_NO_ENTRY;
}
int f_dw_record_mac_info (void) {
   return f_dwarf_walk_dies (debug, 0, &f_dw_mac_info_add, 0 ,0); 
}
int f_dw_free_mac_info (void) {
   struct s_dw_mac_info_2_die *node;
   while ( (node = v_dw_mac_info_2_die) ) {
      v_dw_mac_info_2_die = m_ring_unlink(node, link);
      free(node);
   }
   return DW_DLV_OK;
}
int f_dw_mac_off_2_die(Dwarf_Debug dbg, Dwarf_Unsigned  mac_off, Dwarf_Off *offset, Dwarf_Die *die) {
   struct s_dw_mac_info_2_die *node;
   for (node = v_dw_mac_info_2_die; node; node = m_ring_list(v_dw_mac_info_2_die, node, link) ) {
       if ( mac_off != node->mac_off ) continue;
       if ( die ) *die = node->die;
       if ( !offset ) return DW_DLV_OK;
       m_libdwarf_assert( dwarf_dieoffset(node->die, offset, NULL) == DW_DLV_OK, return DW_DLV_ERROR);
       return DW_DLV_OK;
   }
   return DW_DLV_NO_ENTRY;
}
int f_dwarf_print_macros (char *indent) {
   Dwarf_Unsigned max = 0;
   Dwarf_Off cur_off = 0;
   Dwarf_Signed count = 0;
   Dwarf_Macro_Details *maclist;
   char *html_encoded = 0;
   int ret, i, cu_no = 0, start = 0;
   Dwarf_Off die_off;

   f_dw_record_mac_info();

   /* loop thru all the compilation units macro info */
   while((ret = dwarf_get_macro_details(debug, cur_off, max, &count, &maclist, &error)) == DW_DLV_OK) {
      if ( count == 1 && !maclist[0].dmd_type ) {
         cur_off = maclist[count-1].dmd_offset + 1;
         dwarf_dealloc(debug, maclist, DW_DLA_STRING);
         cu_no++;
         continue;
      }
      if ( !start ) {
         printf("%s<debug_macinfo>\n",indent);
         start = 1;
      }
      printf("%s  <mac_compile_unit id='mac:%llu'",indent, cur_off);
      if ( f_dw_mac_off_2_die(debug, cur_off, &die_off, 0) == DW_DLV_OK) printf(" ref='i:%llu'", die_off);
      printf(">\n");
      cu_no++;
      for (i = 0; i < count; ++i) {
         if ( !maclist[i].dmd_type ) break;
         /* use maclist[i] */
         printf("%s    <macro id='mac:%llum%d' file='%lld' line='%llu' type='%s' >", indent, 
                 cur_off, i+1, maclist[i].dmd_fileindex, maclist[i].dmd_lineno, 
                 f_dw_val_2_str(maclist[i].dmd_type, c_dw_macinfo_tbl));
         // printf("%s      <type>%s</type>\n",indent, f_dw_val_2_str(maclist[i].dmd_type,c_dw_macinfo_tbl));
         // printf("%s      <fileno>%llu</fileno>\n",indent, maclist[i].dmd_fileindex);
         // printf("%s      <lineno>%llu</lineno>\n",indent, maclist[i].dmd_lineno);
         if ( maclist[i].dmd_macro ) {
            m_dwarf2xml_assert((html_encoded = escape_html(maclist[i].dmd_macro)), break );
            printf("%s", html_encoded);
            free(html_encoded);
         }
         printf("</macro>\n");
      }
      printf("%s  </mac_compile_unit>\n",indent);
      cur_off = maclist[count-1].dmd_offset + 1;
      dwarf_dealloc(debug, maclist, DW_DLA_STRING);
   }
   if ( start ) printf("%s</debug_macinfo>\n",indent);
   f_dw_free_mac_info ();
   return DW_DLV_OK;
}
//@}

int main (int argc, char *argv []) {
   FILE *fp;
   char *file, *h_file;
   union {
      unsigned int ul;
      struct {
         int aranges:1;
         int pubnames:1;
         int info:1;
         int abbrev:1;
         int line:1;
         int frame:1;
         int str:1;
         int loc:1;
         int ranges:1;
         int macinfo:1;
         int pubtype:1;
         int weakname:1;
         int varname:1;
         int funcname:1;
         int typename:1;
      } b;
   } sections;
   char * indent = &c_spaces[sizeof(c_spaces) -1];
   if (argc <= 2) {
      if ( argc == 2 && !strcmp("-h", argv[1]) ) {
	      printf("Usage: dwarf2xml [-v] [-h] [-all] [-a] [-p] [-i] [-ab] [-l] [-f] [-s] [-lo] [-m] <filename>\n");
         printf("This software read debug information from ELF files and output them in XML format.\n"
                 "A XSLT style sheet is provided in the package to view debug information in HTML. eg :\n"
                 "   xsltproc -o <out.html> ./dwarf_basic.xslt <dwarf2xml_output.xml>\n");
         printf("Options summary\n");
         printf("   -v : print version\n");
         printf("   -h : print this text\n");
         printf(" -all : print all debug information found in ELF file\n");
         printf("   -a : print .debug_aranges section\n");
         printf("   -p : print .debug_pubnames section\n");
         printf("   -i : print .debug_info section\n");
         printf("  -ab : print .debug_abbrev section\n");
         printf("   -l : print .debug_loc section\n");
         printf("   -f : print .debug_frame and .eh_frame sections\n");
         printf("   -r : print .debug_ranges section\n");
         printf("   -m : print .debug_macinfo section\n");

         printf("  -pt : print .debug_pubtypes section\n");
         printf("  -wn : print .debug_weaknames section\n");
         printf("  -vn : print .debug_varnames section\n");
         printf("  -fn : print .debug_funcnames section\n");
         printf("  -tn : print .debug_typenames section\n");

         printf("Option a, p, i, ab, l, f, r, m are cumulatives. If some section is missing, reference to this section will be missing\n");
         exit (0);
      }
      if ( argc == 2 && !strcmp("-v", argv[1]) ) {
         printf("dwarf2xml version V%s, produced on %s\n", VERSION, __DATE__);
         printf("Copyrigth (C) 2010-2013 Emmanuel Azencot, All Rights Reserved.\n");
         printf("This program is free software; you can redistribute it and/or modify it\n");
         printf("under the terms of version 2. of the GNU General Public License\n");
         printf("as published by the Free Software Foundation (http://www.gnu.org).\n");
         exit (0);
      }
	   fprintf (stderr, "Usage: dwarf2xml [-v] [-h] [-all] [-a] [-p] [-i] [-ab] [-l] [-f] [-s] [-lo] [-m] filename\n");
	   exit (1);
   }
   file = argv[--argc];
   if ( argc == 1 ) { sections.b.info = 1; sections.b.line = 1; sections.b.loc = 1; }
   else sections.ul = 0;
   while ( --argc ) {
      if ( !strcmp("-all", argv[argc]) ) { sections.ul = 0xFFFFFFFF; continue; }
      if ( !strcmp("-a",   argv[argc]) ) { sections.b.aranges = 1; continue; }
      if ( !strcmp("-p",   argv[argc]) ) { sections.b.pubnames = 1; continue; }
      if ( !strcmp("-i",   argv[argc]) ) { sections.b.info = 1; continue; }
      if ( !strcmp("-ab",  argv[argc]) ) { sections.b.abbrev = 1; continue; }
      if ( !strcmp("-l",   argv[argc]) ) { sections.b.line = 1; continue; }
      if ( !strcmp("-f",   argv[argc]) ) { sections.b.frame = 1; continue; }
      if ( !strcmp("-s",   argv[argc]) ) { sections.b.str = 1; continue; }
      if ( !strcmp("-lo",  argv[argc]) ) { sections.b.loc = 1; continue; }
      if ( !strcmp("-r",   argv[argc]) ) { sections.b.ranges = 1; continue; }
      if ( !strcmp("-m",   argv[argc]) ) { sections.b.macinfo = 1; continue; }

      if ( !strcmp("-pt",  argv[argc]) ) { sections.b.pubtype = 1; continue; }
      if ( !strcmp("-wn",  argv[argc]) ) { sections.b.weakname = 1; continue; }
      if ( !strcmp("-vn",  argv[argc]) ) { sections.b.varname = 1; continue; }
      if ( !strcmp("-fn",  argv[argc]) ) { sections.b.funcname = 1; continue; }
      if ( !strcmp("-tn",  argv[argc]) ) { sections.b.typename = 1; continue; }

      fprintf (stderr, "%s: Illegal option %s\n", argv [0], argv[argc]);
      exit (1);
   }
   m_dwarf2xml_assert( (fp = fopen (file, "r")) != NULL, 
                       m_error("fopen(%s) : %s", file, strerror(errno)); return 1; );
   m_libdwarf_assert( dwarf_init (fileno(fp), DW_DLC_READ, NULL, NULL, &debug, &error) != DW_DLV_ERROR, 
                      fclose (fp); return 1;);
   {  /* Get Elf type for the machine word size */
      Elf *elf;
      GElf_Ehdr elf_ehdr;
      if ( dwarf_get_elf(debug, &elf, &error) == DW_DLV_ERROR || gelf_getehdr(elf, &elf_ehdr) == 0 ) {
         fclose (fp);
         fprintf(stderr, "%s: file %s does not seem to be a valid elf\n", argv [0], file);
         exit (1);
      }
      if ( elf_ehdr.e_ident[EI_CLASS] == ELFCLASS64 ) {
         v_elf_arch.w_size = 8;
         v_elf_arch.mask   = 0xFFFFFFFFFFFFFFFFLL;
         v_elf_arch.fmt_addr = "0x%016llx";
      }
      // fprintf(stderr, "w_size = %d\n", v_elf_arch.w_size);
   }
   m_dwarf2xml_assert( f_dwarf_record_pc_range () != DW_DLV_ERROR, goto error);
   m_dwarf2xml_assert( f_dwarf_record_lineoff () != DW_DLV_ERROR, goto error);
   printf ("<?xml version=\"1.0\"?>\n");
   printf ("<!DOCTYPE dwarf SYSTEM \"dwarf-v%s.dtd\">\n", VERSION);
   printf ("<dwarf file='%s' producer='dwarf2xml' >\n", (h_file = escape_html(file)) ); free(h_file);
   if ( sections.b.aranges )  m_dwarf2xml_assert(f_dwarf_print_aranges (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.pubnames ) m_dwarf2xml_assert(f_dwarf_print_pubnames(indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.info )     m_dwarf2xml_assert(f_dwarf_print_cu_tree (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.abbrev )   m_dwarf2xml_assert(f_dwarf_print_abbrevs (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.line )     m_dwarf2xml_assert(f_dwarf_print_lines   (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.frame )    m_dwarf2xml_assert(f_dwarf_print_frames  (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.loc )      m_dwarf2xml_assert(f_dwarf_print_locs    (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.ranges )   m_dwarf2xml_assert(f_dwarf_print_ranges  (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.macinfo )  m_dwarf2xml_assert(f_dwarf_print_macros  (indent-2) != DW_DLV_ERROR, goto error);

   if ( sections.b.pubtype )  m_dwarf2xml_assert(f_dwarf_print_pubtypes (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.weakname)  m_dwarf2xml_assert(f_dwarf_print_weaknames(indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.varname )  m_dwarf2xml_assert(f_dwarf_print_varnames (indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.funcname ) m_dwarf2xml_assert(f_dwarf_print_funcnames(indent-2) != DW_DLV_ERROR, goto error);
   if ( sections.b.typename ) m_dwarf2xml_assert(f_dwarf_print_typenames(indent-2) != DW_DLV_ERROR, goto error);

   printf ("</dwarf>\n");
   f_dw_free_pc_range();
   f_dw_free_lineoff();

   m_libdwarf_assert( dwarf_finish (debug, &error) != DW_DLV_ERROR, fclose (fp); return 1;);
   fclose (fp);
   return 0;
error:
   fclose (fp);
   return 4;

}
