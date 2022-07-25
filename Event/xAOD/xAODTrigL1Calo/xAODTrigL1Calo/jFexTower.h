/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JFEXTOWER_H
#define XAODTRIGL1CALO_JFEXTOWER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/jFexTower_v1.h"
#include <map>

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the TriggerTower class
   typedef jFexTower_v1 jFexTower;

}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::jFexTower , 256835581 , 1 )

#endif // XAODTRIGL1CALO_JFEXTOWER_H
