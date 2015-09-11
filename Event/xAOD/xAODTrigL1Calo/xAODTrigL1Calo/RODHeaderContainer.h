/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_RODHEADERCONTAINER_H
#define XAODTRIGL1CALO_RODHEADERCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/RODHeader.h"
#include "xAODTrigL1Calo/versions/RODHeaderContainer_v2.h"

namespace xAOD {
   /// Define the latest version of the RODHeader container
   typedef RODHeaderContainer_v2 RODHeaderContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::RODHeaderContainer, 1152973036 , 1 )

#endif // XAODTRIGL1CALO_RODHEADERCONTAINER_H
