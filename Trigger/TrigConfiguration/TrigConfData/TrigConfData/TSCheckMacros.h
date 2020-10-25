/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_TSCHECKMACROS_H
#define TRIGCONFDATA_TSCHECKMACROS_H

#ifdef TRIGCONF_STANDALONE

#ifdef ATLAS_GCC_CHECKERS
#define ATLAS_THREAD_SAFE [[gnu::thread_safe]]
#else
#define ATLAS_THREAD_SAFE
#endif // ATLAS_GCC_CHECKERS

#else

#ifdef XAOD_STANDALONE
#define ATLAS_THREAD_SAFE
#else
#include "CxxUtils/checker_macros.h"
#endif // XAOD_STANDALONE

#endif // TRIGCONF_STANDALONE

#endif // TRIGCONFDATA_TSCHECKMACROS_H
