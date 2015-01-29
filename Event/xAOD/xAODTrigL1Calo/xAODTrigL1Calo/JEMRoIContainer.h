/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_JEMROICONTAINER_H
#define XAODTRIGL1CALO_JEMROICONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/JEMRoI.h"
#include "xAODTrigL1Calo/versions/JEMRoIContainer_v1.h"

namespace xAOD {
   /// Define the latest version of the JEMRoI container
   typedef JEMRoIContainer_v1 JEMRoIContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::JEMRoIContainer, 1198425825 , 1 )

#endif // XAODTRIGL1CALO_JEMROICONTAINER_H
