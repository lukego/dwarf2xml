/* 
 * Copyright (C) 2008-2009 by Emmanuel Azencot under the GNU LGPL
 * license version 2.0 or 2.1.  You should have received a copy of the
 * LGPL license along with this library if you did not you can find it
 * at http://www.gnu.org/.
 */
#ifndef _EXCEPTION_H_ /* [ */
#define _EXCEPTION_H_

#ifdef EXCP_INCLUDE
   #include EXCP_INCLUDE   
#else
   #ifdef EXCP_DEFAULT_INCLUDE
      #include EXCP_DEFAULT_INCLUDE
   #else
      #include "exception_stderr.h"
   #endif
#endif

#ifndef EXCP_MOD
   #define EXCP_MOD 0
#endif

#if defined(__FreeBSD__) || defined(__APPLE__)
/* ENOTNAM and ENOTUNIQ are not present on FreeBSD nor OSX */
#define ENOTNAM EFAULT
#define ENOTUNIQ EFAULT
#endif

#endif /* ] _EXCEPTION_H_ */
