// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODLUMINOSITY_XAODLUMINOSITYDICT_H
#define XAODLUMINOSITY_XAODLUMINOSITYDICT_H

// Local include(s).
#include "xAODLuminosity/LumiBlockRange.h"
#include "xAODLuminosity/LumiBlockRangeContainer.h"
#include "xAODLuminosity/LumiBlockRangeAuxContainer.h"
#include "xAODLuminosity/versions/LumiBlockRange_v1.h"
#include "xAODLuminosity/versions/LumiBlockRangeContainer_v1.h"
#include "xAODLuminosity/versions/LumiBlockRangeAuxContainer_v1.h"

#include "xAODLuminosity/BCMRawData.h"
#include "xAODLuminosity/BCMRawDataContainer.h"
#include "xAODLuminosity/BCMRawDataAuxContainer.h"
#include "xAODLuminosity/versions/BCMRawData_v1.h"
#include "xAODLuminosity/versions/BCMRawDataContainer_v1.h"
#include "xAODLuminosity/versions/BCMRawDataAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODLUMINOSITY {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, BCMRawDataContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, LumiBlockRangeContainer_v1 );
   };
}

#endif // XAODLUMINOSITY_XAODLUMINOSITYDICT_H
