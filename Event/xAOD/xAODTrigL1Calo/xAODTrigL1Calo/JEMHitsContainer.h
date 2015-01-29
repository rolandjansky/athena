/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JEMHITSCONTAINER_H
#define XAODTRIGL1CALO_JEMHITSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/JEMHits.h"
#include "xAODTrigL1Calo/versions/JEMHitsContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the JEMHits container
   typedef JEMHitsContainer_v1 JEMHitsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JEMHitsContainer, 1251848444 , 1 )

#endif // XAODTRIGL1CALO_JEMHITSCONTAINER_H
