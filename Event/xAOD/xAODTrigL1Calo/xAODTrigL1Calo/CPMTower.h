/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CPMTOWER_H
#define XAODTRIGL1CALO_CPMTOWER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/CPMTower_v1.h"
#include "xAODTrigL1Calo/versions/CPMTower_v2.h"
#include <map>

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CPMTower class
   typedef CPMTower_v2 CPMTower;

   typedef std::map<int, CPMTower*> CPMTowerMap_t;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CPMTower, 14223686 , 1 )

#endif // XAODTRIGL1CALO_CPMTOWER_H
