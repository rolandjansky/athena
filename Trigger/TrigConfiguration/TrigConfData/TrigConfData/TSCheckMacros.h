/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_TSCHECKMACROS_H
#define TRIGCONFDATA_TSCHECKMACROS_H

#ifdef TRIGCONF_STANDALONE

#ifdef ATLAS_GCC_CHECKERS
#define ATLAS_THREAD_SAFE [[gnu::thread_safe]]
#else
#define ATLAS_THREAD_SAFE
#endif

#else

#include "CxxUtils/checker_macros.h"

#endif

#endif
