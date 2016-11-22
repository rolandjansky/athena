/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPHAESTUS_DEBUGINFO_H
#define HEPHAESTUS_DEBUGINFO_H

#include <dlfcn.h>

#ifdef __cplusplus
extern "C" {
#endif

struct link_map;

/* address -> human-readable, file/line information */
const char* hhh_getDebugInfo( void *addr, Dl_info *info, struct link_map* lm, const char** );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !HEPHAESTUS_DEBUGINFO_H */
