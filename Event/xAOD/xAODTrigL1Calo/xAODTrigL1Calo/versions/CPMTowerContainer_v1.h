// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTowerContainer_v1.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOWERCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOWERCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTower_v1.h"

namespace xAOD {
   /// Define the CPMHits as a simple DataVector
   typedef DataVector< xAOD::CPMTower_v1 > CPMTowerContainer_v1;
}

#endif // XAODTRIGL1CALO_VERSIONS_CPMTOWERCONTAINER_V1_H
  
