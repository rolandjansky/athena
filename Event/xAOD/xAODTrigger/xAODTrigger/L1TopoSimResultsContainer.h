/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoSimResultsContainer.h 2021-10-06 @asonay $
#ifndef XAODTRIGGER_L1TOPOSIMRESULTSCONTAINER_H
#define XAODTRIGGER_L1TOPOSIMRESULTSCONTAINER_H

// Local include(s):
#include "xAODTrigger/L1TopoSimResults.h"
#include "xAODTrigger/versions/L1TopoSimResultsContainer_v1.h"

namespace xAOD{
   typedef L1TopoSimResultsContainer_v1 L1TopoSimResultsContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L1TopoSimResultsContainer , 983476512 , 1 )

#endif // XAODTRIGGER_L1TOPOSIMRESULTSCONTAINER_H
