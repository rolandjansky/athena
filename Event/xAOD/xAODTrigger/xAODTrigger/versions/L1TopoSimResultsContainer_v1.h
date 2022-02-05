/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoSimResultsContainer_v1.cxx  2021-10-06 @asonay $
#ifndef XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTSCONTAINER_V1_H
#define XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTSCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigger/versions/L1TopoSimResults_v1.h"

namespace xAOD {
   /// Declare the em/tau RoI container type
   typedef DataVector< xAOD::L1TopoSimResults_v1 > L1TopoSimResultsContainer_v1;
}

#endif // XAODTRIGGER_VERSIONS_L1TOPOSIMRESULTSCONTAINER_V1_H
