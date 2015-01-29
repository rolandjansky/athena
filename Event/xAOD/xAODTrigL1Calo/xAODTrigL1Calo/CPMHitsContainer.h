/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CPMHITSCONTAINER_H
#define XAODTRIGL1CALO_CPMHITSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CPMHits.h"
#include "xAODTrigL1Calo/versions/CPMHitsContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the CPMHits container
   typedef CPMHitsContainer_v1 CPMHitsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CPMHitsContainer, 1211912814 , 1 )

#endif // XAODTRIGL1CALO_CPMHITSCONTAINER_H
