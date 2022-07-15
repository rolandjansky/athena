/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_EFEXTOWERCONTAINER_H
#define XAODTRIGL1CALO_EFEXTOWERCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/eFexTower.h"
#include "xAODTrigL1Calo/versions/eFexTowerContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the TriggerTower container
   typedef eFexTowerContainer_v1 eFexTowerContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::eFexTowerContainer , 1159375134 , 1 )

#endif // XAODTRIGL1CALO_EFEXTOWERCONTAINER_H
