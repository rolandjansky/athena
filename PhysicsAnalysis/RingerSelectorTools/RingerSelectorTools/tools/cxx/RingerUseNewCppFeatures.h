/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RingerUseNewCppFeatures.h 693573 2015-09-07 19:15:49Z wsfreund $
#ifndef RINGERSELECTORTOOLS_TOOLS_CXX_RINGERUSENEWCPPFEATURES_H
#define RINGERSELECTORTOOLS_TOOLS_CXX_RINGERUSENEWCPPFEATURES_H

#include "RVersion.h"

#ifndef RINGER_STANDALONE
#include "xAODCaloRings/tools/cxx/RingerUseNewCppFeatures.h"
#define FORCE_RINGER_PROCEDURE_TYPE_CONST_EXPR
#else
# define RINGER_USE_NEW_CPP_FEATURES ((__cplusplus >= 201100) && \
                                      (ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)) )
#endif

#endif // RINGERSELECTORTOOLS_TOOLS_CXX_RINGERUSENEWCPPFEATURES_H
