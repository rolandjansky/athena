/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoSimResultsAuxContainer_v1.cxx 2021-10-06 @asonay $
// Local include(s):
#include "xAODTrigger/versions/L1TopoSimResultsAuxContainer_v1.h"

namespace xAOD {

  L1TopoSimResultsAuxContainer_v1::L1TopoSimResultsAuxContainer_v1()
    : AuxContainerBase() {
   
    AUX_VARIABLE( boardName );
    AUX_VARIABLE( word32 );
    AUX_VARIABLE( word64 );
    AUX_VARIABLE( wordOptical );
    AUX_VARIABLE( clock );

  }

} // namespace xAOD
