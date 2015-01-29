/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CPMTOWERAUXCONTAINER_H
#define XAODTRIGL1CALO_CPMTOWERAUXCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTowerAuxContainer_v1.h"
#include "xAODTrigL1Calo/versions/CPMTowerAuxContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the CPMHits auxiliary container
   typedef CPMTowerAuxContainer_v2 CPMTowerAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CPMTowerAuxContainer, 1095086643 , 1 ) 

#endif // XAODTRIGL1CALO_CPMTOWERAUXCONTAINER_H
