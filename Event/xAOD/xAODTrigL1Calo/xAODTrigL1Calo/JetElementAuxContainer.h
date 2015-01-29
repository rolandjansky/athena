/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JETELEMENTAUXCONTAINER_H
#define XAODTRIGL1CALO_JETELEMENTAUXCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/JetElementAuxContainer_v1.h"
#include "xAODTrigL1Calo/versions/JetElementAuxContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the JetElement auxiliary container
   typedef JetElementAuxContainer_v2 JetElementAuxContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetElementAuxContainer, 1134519579 , 1 ) 

#endif // XAODTRIGL1CALO_JETELEMENTAUXCONTAINER_H
