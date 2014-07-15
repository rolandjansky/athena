/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigElectronAuxContainer_v1.cxx 602876 2014-06-20 11:21:26Z krasznaa $

// Local include(s):
#include "xAODTrigEgamma/versions/TrigElectronAuxContainer_v1.h"

namespace xAOD {

   TrigElectronAuxContainer_v1::TrigElectronAuxContainer_v1()
      : AuxContainerBase() {

      // Simple electron properties:
      AUX_VARIABLE( roiWord );
      AUX_VARIABLE( zvtx );
      AUX_VARIABLE( charge );
      AUX_VARIABLE( nTRTHits );
      AUX_VARIABLE( nTRTHiThresholdHits );
      AUX_VARIABLE( rcore );
      AUX_VARIABLE( eratio );
      AUX_VARIABLE( etHad );
      AUX_VARIABLE( etHad1 );
      AUX_VARIABLE( f0 );
      AUX_VARIABLE( f1 );
      AUX_VARIABLE( f2 );
      AUX_VARIABLE( f3 );

      // 4-momentum properties:
      AUX_VARIABLE( trkEtaAtCalo );
      AUX_VARIABLE( trkPhiAtCalo );
      AUX_VARIABLE( etOverPt );
      AUX_VARIABLE( pt );
      AUX_VARIABLE( caloEta );
      AUX_VARIABLE( caloPhi );

      // Links to the electron constituents:
      AUX_VARIABLE( emClusterLink );
      AUX_VARIABLE( trackParticleLink );
   }

} // namespace xAOD
