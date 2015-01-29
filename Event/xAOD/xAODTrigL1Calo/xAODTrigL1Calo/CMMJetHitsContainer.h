/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_CMMJETHITSCONTAINER_H
#define XAODTRIGL1CALO_CMMJETHITSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/CMMJetHits.h"
#include "xAODTrigL1Calo/versions/CMMJetHitsContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the CMMJetHits container
   typedef CMMJetHitsContainer_v1 CMMJetHitsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CMMJetHitsContainer , 1257186099 , 1 )

#endif // XAODTRIGL1CALO_CMMJETHITSCONTAINER_H
