/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEAPMON_HEAPMON_H
#define HEAPMON_HEAPMON_H

#if defined(__linux__)

#define _GNU_SOURCE 1

#include <stdio.h>

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _FILE_OFFSET_BITS
#undef _FILE_OFFSET_BITS
#endif

#endif

#include "Python.h"

/* control parameters for static data blocks */

/* maximum available number of check points */
#define mm_MAX_CHECKPOINTS          10

/* start size of hash tables for data */
#define mm_HASHTABLE_MINSIZE      4096

/* maximum total depth of tracebacks kept in memory */
#define mm_TRACEDEPTH       12
/* system offset: 0 == helper, 1 == hook, 2 == malloc/realloc */
#define mm_SYSTEMOFF         3
/* total backtrace size left: mm_TRACEDEPTH - mm_SYSTEMOFF */
#define mm_BACKTRACE_SIZE   mm_TRACEDEPTH - mm_SYSTEMOFF

/* max size for symbols, files, and other buffers */
#define mm_SYMBOL_BUFLEN          2048

#endif /* !HEAPMON_HEAPMON_H */
