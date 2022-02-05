/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoSimResultsAuxContainer_v1.cxx 2021-10-06 @asonay $
// Local include(s):
#include "xAODTrigger/versions/L1TopoSimResultsAuxContainer_v1.h"

namespace xAOD {

  L1TopoSimResultsAuxContainer_v1::L1TopoSimResultsAuxContainer_v1()
    : AuxContainerBase() {
   
    AUX_VARIABLE( connectionId );
    AUX_VARIABLE( topoWord );
    AUX_VARIABLE( topoWordOverflow );
    AUX_VARIABLE( topoWord64 );
    AUX_VARIABLE( topoWord64Overflow );
    AUX_VARIABLE( bitWidth );
    AUX_VARIABLE( clock );

  }

} // namespace xAOD
