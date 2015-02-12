/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JEMETSUMSCONTAINER_H
#define XAODTRIGL1CALO_JEMETSUMSCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/JEMEtSums.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the JEMEtSums container
   typedef JEMEtSumsContainer_v2 JEMEtSumsContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JEMEtSumsContainer, 1236231083 , 1 )

#endif // XAODTRIGL1CALO_JEMETSUMSCONTAINER_H
