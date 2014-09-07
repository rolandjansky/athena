/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonAuxContainer_v1.cxx 596434 2014-05-11 17:30:02Z krasznaa $

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v1.h"

namespace xAOD {

   PhotonAuxContainer_v1::PhotonAuxContainer_v1()
      : EgammaAuxContainer_v1() {

      ///@links to track Particles
      AUX_VARIABLE(vertexLinks);

      /// @name Vertex Match details
      /// @{

      AUX_VARIABLE(convMatchDeltaEta1);
      AUX_VARIABLE(convMatchDeltaEta2);    
      AUX_VARIABLE(convMatchDeltaPhi1);    
      AUX_VARIABLE(convMatchDeltaPhi2);
      
      ///@}


   }

} // namespace xAOD
