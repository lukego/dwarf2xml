#include <stdio.h>
#include <errno.h>

#ifdef raise
  #undef raise
#endif
#define raise(action, error_no, fmt, args...) { \
   if ( (error_no) ) { \
      errno = (EXCP_MOD) | (error_no); \
      fprintf(stderr,"error:%s:%d:%s:", __FILE__,__LINE__,__FUNCTION__); \
      fprintf(stderr, fmt, ##args); fprintf(stderr, "\n"); \
      action; \
   } }

#ifdef relay
  #undef relay
#endif
#define relay(action, fmt, args...) { \
   if ( errno ) { \
      fprintf(stderr,"error:%s:%d:%s:", __FILE__,__LINE__,__FUNCTION__); \
      fprintf(stderr, fmt, ##args); fprintf(stderr, "\n"); \
      action; \
   } }
#ifdef excp_assert
  #undef excp_assert
#endif
#define excp_assert(action, error_no, expression) { \
   if ( !(expression) ) { \
      (errno = (EXCP_MOD) | (error_no));\
      fprintf(stderr,"error:%s:%d:%s:assert failled for %s\n", __FILE__,__LINE__,__FUNCTION__, #expression); \
      action; \
   } \
}

