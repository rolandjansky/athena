/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: mutable.h 659538 2015-04-08 17:29:18Z wsfreund $ 
#ifndef XAODCALORINGS_UTILS_CXX_MUTABLE_H
#define XAODCALORINGS_UTILS_CXX_MUTABLE_H

#include "xAODCaloRings/tools/cxx/RingerUseNewCppFeatures.h"

#if RINGER_USE_NEW_CPP_FEATURES
# define ATH_RINGER_MUTABLE mutable
#else
# define ATH_RINGER_MUTABLE
#endif

#endif // XAODCALORINGS_UTILS_CXX_MUTABLE_H
