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
#ifndef _DWARF2XML_H_ /* [ */
#define _DWARF2XML_H_

#include <dwarf_val_2_str.h>

/* Stringification of VERSION (from gcc/cpp manual) */
#define xstr(s) str(s)
#define str(s) #s


#define m_error(fmt, args...) { \
   fprintf(stderr,"error in file %s at line %d in function %s:", __FILE__,__LINE__,__FUNCTION__); \
   fprintf(stderr,fmt "\n", ##args); \
}
#define m_libdwarf_assert(expression, action) { \
   if ( !(expression) ) { \
      fprintf(stderr,"error in file %s at line %d in function %s: %s", __FILE__,__LINE__,__FUNCTION__, #expression); \
      fprintf(stderr,"  returned message :%s\n", dwarf_errmsg(error)); \
      action; \
   } \
}
#define m_dwarf2xml_assert(expression, action) { \
   if ( !(expression) ) { \
      fprintf(stderr,"error in file %s at line %d in function %s: %s\n", __FILE__,__LINE__,__FUNCTION__, #expression); \
      action; \
   } \
}

#define m_dw_cat_snprintf(buff, len, fmt, args...) \
   m_dwarf2xml_assert(((len)-strlen(buff)) > snprintf(buff+strlen(buff), (len)-strlen(buff) ,fmt , ##args), return DW_DLV_ERROR ) 
      

#define strncat2(p, str, l) strncat(p+strlen(p), str, l-strlen(p))

/** @memo This structure contains CU infos
 * It will be passed to call-backs as argument
 */
struct s_dwarf_cu_info {
   int            cu_no;
   int            level;
   Dwarf_Die      die;
   Dwarf_Unsigned abbrev_offset;
   Dwarf_Half     version_stamp;
   Dwarf_Half     address_size;
   Dwarf_Half     offset_size;
};

int f_dw_attr_value_2_str(char *indent, Dwarf_Attribute attribute, Dwarf_Debug debug, struct s_dwarf_cu_info *cu_info);
int f_dw_attr_value_2_str_with_form(Dwarf_Attribute attribute, Dwarf_Debug debug);

const char *f_dw_attr_2_str(Dwarf_Half attr, int open);
const char *f_dw_val_2_str(int val, struct s_val_2_str *tbl);
const char *f_dw_tag_2_str(Dwarf_Half tag, int open);
const char *f_dw_form_2_str(Dwarf_Half form);


struct s_dw_form *f_dw_form(Dwarf_Half form);
int f_dw_form_glob_ref(Dwarf_Debug debug, Dwarf_Attribute attribute);

struct s_elf_arch {
   size_t w_size;
   unsigned long long mask;
   char *fmt_addr;
};
extern struct s_elf_arch v_elf_arch;

int f_dw_print_loc_expr(char *indent, Dwarf_Locdesc *llbuf, Dwarf_Signed offset);
char *escape_html(const char *in);

#ifndef DW_AT_GNU_template_name
#define DW_AT_GNU_template_name 0x2108
#endif

// extern char v_dw_attr_str_unk[30];

#endif /* ] _DWARF2XML_H_ */

