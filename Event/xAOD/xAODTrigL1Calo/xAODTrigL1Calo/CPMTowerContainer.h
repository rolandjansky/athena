/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CPMTOWERCONTAINER_H
#define XAODTRIGL1CALO_CPMTOWERCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CPMTower.h"
#include "xAODTrigL1Calo/versions/CPMTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMTowerContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the CPMHits container
   typedef CPMTowerContainer_v2 CPMTowerContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CPMTowerContainer, 1267839700 , 1 )

#endif // XAODTRIGL1CALO_CPMTOWERCONTAINER_H
