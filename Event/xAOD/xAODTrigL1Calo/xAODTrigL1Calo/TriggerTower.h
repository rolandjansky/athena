/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_TRIGGERTOWER_H
#define XAODTRIGL1CALO_TRIGGERTOWER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/TriggerTower_v1.h"
#include "xAODTrigL1Calo/versions/TriggerTower_v2.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the TriggerTower class
   typedef TriggerTower_v2 TriggerTower;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TriggerTower, 264277036 , 1 )

#endif // XAODTRIGL1CALO_TRIGGERTOWER_H
