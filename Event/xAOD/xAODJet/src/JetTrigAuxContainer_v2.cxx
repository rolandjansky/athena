/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



// Local include(s):
#include "xAODJet/versions/JetTrigAuxContainer_v2.h"

namespace xAOD {

   JetTrigAuxContainer_v2::JetTrigAuxContainer_v2()
      : ByteStreamAuxContainer_v1() {

      AUX_VARIABLE( momentum );
      AUX_VARIABLE( constituentLinks );
      AUX_VARIABLE( GhostTrack );
      AUX_VARIABLE( constituentWeights );
   }

} // namespace xAOD
