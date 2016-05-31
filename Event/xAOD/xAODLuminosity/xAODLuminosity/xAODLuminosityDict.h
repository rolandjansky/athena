// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODLuminosityDict.h 652111 2015-03-06 10:13:22Z krasznaa $
#ifndef XAODLUMINOSITY_XAODLUMINOSITYDICT_H
#define XAODLUMINOSITY_XAODLUMINOSITYDICT_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthLinks/DataLink.h"

// Local include(s):
#include "xAODLuminosity/versions/LumiBlockRange_v1.h"
#include "xAODLuminosity/versions/LumiBlockRangeContainer_v1.h"
#include "xAODLuminosity/versions/LumiBlockRangeAuxContainer_v1.h"
#include "xAODLuminosity/versions/BCMRawData_v1.h"
#include "xAODLuminosity/versions/BCMRawDataContainer_v1.h"
#include "xAODLuminosity/versions/BCMRawDataAuxContainer_v1.h"
#include "xAODLuminosity/BCMRawDataContainer.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODLUMINOSITY {
      /// Container(s):
      xAOD::LumiBlockRangeContainer_v1 c1;
      xAOD::BCMRawDataContainer_v1 c2;
      /// Element link(s):
      ElementLink< xAOD::BCMRawDataContainer_v1 > el1;
      std::vector< ElementLink< xAOD::BCMRawDataContainer_v1 > > el2;
      std::vector< std::vector< ElementLink< xAOD::BCMRawDataContainer_v1 > > > el3;
      /// Data link(s):
      DataLink< xAOD::BCMRawDataContainer_v1 > dl1;
      std::vector< DataLink< xAOD::BCMRawDataContainer_v1 > > dl2;
   };
}

#endif // XAODLUMINOSITY_XAODLUMINOSITYDICT_H
