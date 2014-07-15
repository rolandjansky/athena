/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPhotonAuxContainer_v1.cxx 602896 2014-06-20 12:48:42Z krasznaa $

// Local include(s):
#include "xAODTrigEgamma/versions/TrigPhotonAuxContainer_v1.h"

namespace xAOD {

   TrigPhotonAuxContainer_v1::TrigPhotonAuxContainer_v1 ()
      : AuxContainerBase() {

      // Simple photon properties:
      AUX_VARIABLE( roiWord );
      AUX_VARIABLE( rcore );
      AUX_VARIABLE( eratio );
      AUX_VARIABLE( etHad );
      AUX_VARIABLE( etHad1 );
      AUX_VARIABLE( f0 );
      AUX_VARIABLE( f1 );
      AUX_VARIABLE( f2 );
      AUX_VARIABLE( f3 );
      AUX_VARIABLE( fside );
      AUX_VARIABLE( weta2 );

      // 4-momentum properties:
      AUX_VARIABLE( pt );
      AUX_VARIABLE( caloEta );
      AUX_VARIABLE( caloPhi );
      AUX_VARIABLE( dEta );
      AUX_VARIABLE( dPhi );

      // Link(s) to the constituents:
      AUX_VARIABLE( emClusterLink );
   }

} // namespace xAOD
