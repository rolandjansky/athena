/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTau/versions/DiTauJetAuxContainer_v1.h"

namespace xAOD {

DiTauJetAuxContainer_v1::DiTauJetAuxContainer_v1()
   : AuxContainerBase() {

   AUX_VARIABLE( pt );
   AUX_VARIABLE( eta );
   AUX_VARIABLE( phi );
   AUX_VARIABLE( m );

   AUX_VARIABLE( jetLink );

   AUX_VARIABLE( R_jet );
   AUX_VARIABLE( R_subjet );
   AUX_VARIABLE( R_core );
   AUX_VARIABLE( BDT );
   
   AUX_VARIABLE( vertexLink );
   // AUX_VARIABLE( secVertexLinks );
   AUX_VARIABLE( TauJetVtxFraction );

   AUX_VARIABLE( trackLinks );
   AUX_VARIABLE( isoTrackLinks );
   AUX_VARIABLE( otherTrackLinks );

   AUX_VARIABLE( subjet_pt );
   AUX_VARIABLE( subjet_eta );
   AUX_VARIABLE( subjet_phi );
   AUX_VARIABLE( subjet_e );

   AUX_VARIABLE( subjet_f_core );
}
   
} // namespace xAOD