/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerUseNewCppFeatures.h 693570 2015-09-07 19:12:27Z wsfreund $ 
#ifndef XAODCALORINGS_UTILS_CXX_RINGERUSENEWCPPFEATURES_H
#define XAODCALORINGS_UTILS_CXX_RINGERUSENEWCPPFEATURES_H

#include "RVersion.h"

/**
 * We cannot use new cpp features due to RootCore dictionary generation, which
 * doesn't use cpp std 11 during Root 5 dict generation 
 **/
#define RINGER_USE_NEW_CPP_FEATURES ((__cplusplus >= 201100) && \
                                     (ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)) )

#endif // XAODCALORINGS_UTILS_CXX_RINGERUSENEWCPPFEATURES_H
