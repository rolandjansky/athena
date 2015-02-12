/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JEMETSUMSAUXCONTAINER_H
#define XAODTRIGL1CALO_JEMETSUMSAUXCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/JEMEtSumsAuxContainer_v1.h"
#include "xAODTrigL1Calo/versions/JEMEtSumsAuxContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the JEMEtSums auxiliary container
   typedef JEMEtSumsAuxContainer_v2 JEMEtSumsAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JEMEtSumsAuxContainer, 1312707594 , 1 ) 

#endif // XAODTRIGL1CALO_JEMETSUMSAUXCONTAINER_H
