// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerContainer_v2.h 642658 2015-01-29 12:40:24Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOWERCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOWERCONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTower_v2.h"

namespace xAOD {
   /// Define the CPMHits as a simple DataVector
   typedef DataVector< xAOD::CPMTower_v2 > CPMTowerContainer_v2;
}

#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWERCONTAINER_V2_H
  
