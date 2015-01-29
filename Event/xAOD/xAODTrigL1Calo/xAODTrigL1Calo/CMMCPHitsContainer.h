/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CMMCPHITSCONTAINER_H
#define XAODTRIGL1CALO_CMMCPHITSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMMCPHits.h"
#include "xAODTrigL1Calo/versions/CMMCPHitsContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the CMMCPHits container
   typedef CMMCPHitsContainer_v1 CMMCPHitsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMMCPHitsContainer, 1296557234 , 1 )

#endif // XAODTRIGL1CALO_CMMCPHITSCONTAINER_H
