/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODForward/versions/ForwardEventInfoAuxContainer_v1.h"

namespace xAOD {

   ForwardEventInfoAuxContainer_v1::ForwardEventInfoAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( timeDiff );
      AUX_VARIABLE( timeA  );
      AUX_VARIABLE( timeC );
      AUX_VARIABLE( countA );
      AUX_VARIABLE( countC );
   }

} // namespace xAOD
