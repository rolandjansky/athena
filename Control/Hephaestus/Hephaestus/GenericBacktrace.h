/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPHAESTUS_GENERICBACKTRACE_H
#define HEPHAESTUS_GENERICBACKTRACE_H

/*
  Generic backtrace; for Linux only
*/

#ifdef __cplusplus
extern "C" {
#endif

/* generic version of backtrace(), from glibc */
int hhh_GenericBacktrace( void **array, int size );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !HEPHAESTUS_GENERICBACKTRACE_H */
