/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_TRIGGERTOWERAUXCONTAINER_H
#define XAODTRIGL1CALO_TRIGGERTOWERAUXCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerAuxContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the TriggerTower auxiliary container
   typedef TriggerTowerAuxContainer_v2 TriggerTowerAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerTowerAuxContainer, 1086692909 , 1 ) 

#endif // XAODTRIGL1CALO_TRIGGERTOWERAUXCONTAINER_H
