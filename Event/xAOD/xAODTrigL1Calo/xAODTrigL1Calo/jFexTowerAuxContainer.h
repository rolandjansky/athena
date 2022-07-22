/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JFEXTOWERAUXCONTAINER_H
#define XAODTRIGL1CALO_JFEXTOWERAUXCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/jFexTowerAuxContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the TriggerTower auxiliary container
   typedef jFexTowerAuxContainer_v1 jFexTowerAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexTowerAuxContainer , 1312000862 , 1 )

#endif // XAODTRIGL1CALO_JFEXTOWERAUXCONTAINER_H
