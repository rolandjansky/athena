/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JFEXTOWERCONTAINER_H
#define XAODTRIGL1CALO_JFEXTOWERCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/jFexTower.h"
#include "xAODTrigL1Calo/versions/jFexTowerContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the TriggerTower container
   typedef jFexTowerContainer_v1 jFexTowerContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexTowerContainer , 1151914207 , 1 )

#endif // XAODTRIGL1CALO_JFEXTOWERCONTAINER_H
