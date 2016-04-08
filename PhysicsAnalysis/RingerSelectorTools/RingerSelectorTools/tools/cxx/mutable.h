/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: mutable.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_MUTABLE_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_MUTABLE_H

#include "RingerUseNewCppFeatures.h"

#ifndef RINGER_STANDALONE
#include "xAODCaloRings/tools/cxx/mutable.h"
#else
#if RINGER_USE_NEW_CPP_FEATURES
# define ATH_RINGER_MUTABLE mutable
#else
# define ATH_RINGER_MUTABLE
#endif
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_MUTABLE_H
