/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPHAESTUS_HEPHAESTUS_H
#define HEPHAESTUS_HEPHAESTUS_H

#if defined(linux)

#define _GNU_SOURCE 1

#include <stdio.h>

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _FILE_OFFSET_BITS
#undef _FILE_OFFSET_BITS
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#endif

#include "Python.h"

/* control parameters for static data blocks */

/* maximum available number of check points */
#define hhh_MAX_CHECKPOINTS          10

/* start size of hash tables for data */
#define hhh_HASHTABLE_MINSIZE      4096

/* the following numbers are defaults only as they can be modified at runtime,
   as long as no memory checking has been done yet; alternatively, they can
   of course still be modified here for compile time */

/* maximum total depth of tracebacks kept in memory */
#define hhh_TRACEDEPTH       20
/* system offset: 0 == helper, 1 == hook, 2 == malloc/realloc */
#define hhh_SYSTEMOFF         3
/* total backtrace size left: hhh_TRACEDEPTH - hhh_SYSTEMOFF */
#define hhh_BACKTRACE_SIZE   (hhh_TRACEDEPTH - hhh_SYSTEMOFF)

/* max size for symbols, files, and other buffers */
#define hhh_SYMBOL_BUFLEN          2048

#endif /* !HEPHAESTUS_HEPHAESTUS_H */
