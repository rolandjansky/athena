/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPHAESTUS_MEMORYTRACE_H
#define HEPHAESTUS_MEMORYTRACE_H

/*
  Holder for memory trace information
*/

#include "Hephaestus/stackstash.h"

#ifdef __cplusplus
extern "C" {
#endif

struct hhh_MemoryTrace {
   unsigned long size;
   StackHandle* handle;
};

extern unsigned long hhh_gBacktraceSize;

/* allocate a fresh memory trace */
struct hhh_MemoryTrace *hhh_MemoryTrace_new();

/* deallocate an old memory trace */
void hhh_MemoryTrace_delete( void *trace );

/* initialize a newly created MemoryTrace */
void hhh_MemoryTrace_initialize( struct hhh_MemoryTrace *mt, long size,
                                 StackElement* addresses, long depth);

/* get the type that was newed, if available */
const char* hhh_MemoryTrace_getType( struct hhh_MemoryTrace *mt );

/* report on traceback */
void hhh_MemoryTrace_print( FILE*, struct hhh_MemoryTrace *mt, int hideMemAddr );

/* comparison function, returns like strcmp */
int hhh_MemoryTracePtr_compare( const void *pmt1, const void *pmt2 );

#ifdef __cplusplus
} /* extern "C" */
#endif

/* clarifying access to memory trace members */
#define hhh_MT_ORIGINATOR( b ) (*STACK_HANDLE_ELEMENT ((b).handle))

#endif /* !HEPHAESTUS_MEMORYTRACE_H */
