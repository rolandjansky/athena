// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMRoIContainer_v1.h 576243 2013-12-19 11:24:27Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMROICONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMROICONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMRoI_v1.h"

namespace xAOD {
   /// Define the CPMRoI as a simple DataVector
   typedef DataVector< xAOD::CPMRoI_v1 > CPMRoIContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::CPMRoIContainer_v1 , 1274067747 , 1 )
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_CPMROICONTAINER_V1_H
  