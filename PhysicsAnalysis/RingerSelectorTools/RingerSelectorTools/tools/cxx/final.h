/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: final.h 667905 2015-05-18 19:07:55Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_FINAL_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_FINAL_H

#include "RingerUseNewCppFeatures.h"

#undef ATH_RINGER_FINAL

//CxxUtils for final
#if RINGER_USE_NEW_CPP_FEATURES
# define ATH_RINGER_FINAL final
#else
# define ATH_RINGER_FINAL  
#endif


#endif  // RINGERSELECTORTOOLS_TOOLS_CXX_FINAL_H
