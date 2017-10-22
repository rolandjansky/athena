/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef SIMULATIONBASE
// Local include(s):
#include "xAODJet/versions/JetTrigAuxContainer_v2.h"

namespace xAOD {

   JetTrigAuxContainer_v2::JetTrigAuxContainer_v2()
      : ByteStreamAuxContainer_v1() {

      AUX_VARIABLE( constituentLinks );
      AUX_VARIABLE( GhostTrack );
      AUX_VARIABLE( constituentWeights );

      AUX_VARIABLE( pt );
      AUX_VARIABLE( eta );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( m );
      
   }

} // namespace xAOD
#endif //SIMULATIONBASE
