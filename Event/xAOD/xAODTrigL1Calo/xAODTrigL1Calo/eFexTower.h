/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_EFEXTOWER_H
#define XAODTRIGL1CALO_EFEXTOWER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/eFexTower_v1.h"
#include <map>

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the TriggerTower class
   typedef eFexTower_v1 eFexTower;

}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::eFexTower , 143961138 , 1 )

#endif // XAODTRIGL1CALO_EFEXTOWER_H
