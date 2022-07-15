/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_EFEXTOWERAUXCONTAINER_H
#define XAODTRIGL1CALO_EFEXTOWERAUXCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/eFexTowerAuxContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the TriggerTower auxiliary container
   typedef eFexTowerAuxContainer_v1 eFexTowerAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::eFexTowerAuxContainer , 1115941149 , 1 )

#endif // XAODTRIGL1CALO_EFEXTOWERAUXCONTAINER_H
