/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoSimResultsAuxContainer.h 2021-10-06 @asonay $
#ifndef XAODTRIGGER_L1TOPOSIMRESULTSAUXCONTAINER_H
#define XAODTRIGGER_L1TOPOSIMRESULTSAUXCONTAINER_H

// Local include(s):
#include "xAODTrigger/versions/L1TopoSimResultsAuxContainer_v1.h"

namespace xAOD{
   typedef L1TopoSimResultsAuxContainer_v1 L1TopoSimResultsAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::L1TopoSimResultsAuxContainer , 913551489 , 1 )

#endif // XAODTRIGGER_L1TOPOSIMRESULTSAUXCONTAINER_H
