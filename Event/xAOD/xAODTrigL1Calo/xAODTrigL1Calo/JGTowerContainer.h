/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JGTOWERCONTAINER_H
#define XAODTRIGL1CALO_JGTOWERCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/JGTower.h"
#include "xAODTrigL1Calo/versions/JGTowerContainer_v1.h"

namespace xAOD
{
  /// Define the latest version of the JGTower container
  typedef JGTowerContainer_v1 JGTowerContainer;
} // namespace xAOD

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF(xAOD::JGTowerContainer, 1174216567, 1)

#endif // XAODTRIGL1CALO_JGTOWERCONTAINER_H