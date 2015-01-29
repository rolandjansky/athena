/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_TRIGGERTOWERCONTAINER_H
#define XAODTRIGL1CALO_TRIGGERTOWERCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTowerContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the TriggerTower container
   typedef TriggerTowerContainer_v2 TriggerTowerContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerTowerContainer, 1174216566 , 1 )

#endif // XAODTRIGL1CALO_TRIGGERTOWERCONTAINER_H
