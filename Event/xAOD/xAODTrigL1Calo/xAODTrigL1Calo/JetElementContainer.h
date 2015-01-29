/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JETELEMENTCONTAINER_H
#define XAODTRIGL1CALO_JETELEMENTCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/JetElement.h"
#include "xAODTrigL1Calo/versions/JetElementContainer_v1.h"
#include "xAODTrigL1Calo/versions/JetElementContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the JetElement container
   typedef JetElementContainer_v2 JetElementContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JetElementContainer, 1256167516 , 1 )

#endif // XAODTRIGL1CALO_JETELEMENTCONTAINER_H
