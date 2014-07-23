/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CXXUTILS_ATHUNLIKELYMACROS_H
#define CXXUTILS_ATHUNLIKELYMACROS_H 1

/* macros modeled after http://kernelnewbies.org/FAQ/LikelyUnlikely
 * to help the compiler into generating instructions to optimize the
 * branch prediction.
 */
#if __GNUC__ >= 4
# define ATH_LIKELY(x)       __builtin_expect(!!(x), 1)
# define ATH_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
# define ATH_LIKELY(x)   (x)
# define ATH_UNLIKELY(x) (x)
#endif

#endif /* !CXXUTILS_ATHUNLIKELYMACROS_H */
