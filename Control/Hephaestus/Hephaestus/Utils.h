/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPHAESTUS_UTILS_H
#define HEPHAESTUS_UTILS_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* address -> human-readable, demangled symbol */
const char* hhh_getSymbol( void *addr );
const char* hhh_getSymbolE( void *addr );
const char* hhh_addrToLine( void *addr, const char** symbol );
int hhh_writeSymbols( FILE *out );

/* allocator address -> ctor address, if object and not inlined */
void* hhh_getConstructor( void *addr );

/* internal, put referenced library at the end of the dl.so shlib list */
int hhh_setLinkLate( void *addr );

/* internal, for syncing cleanup */
void hhh_utils_cleanup();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !HEPHAESTUS_UTILS_H */
