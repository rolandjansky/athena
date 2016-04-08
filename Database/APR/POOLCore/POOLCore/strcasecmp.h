/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_KERNEL_STRCASECMP_H
#define POOL_KERNEL_STRCASECMP_H

#include <cstring>
#ifdef _WIN32
inline int strcasecmp(const char *s1, const char *s2) {
  return ::_stricmp(s1, s2);
}
inline int strncasecmp(const char *s1, const char *s2, size_t n) {
  return ::_strnicmp(s1, s2, n);
}
#endif

#endif // POOL_KERNEL_STRCASECMP_H
