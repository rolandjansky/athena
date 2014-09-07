/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonAuxContainer_v2.cxx 614853 2014-09-03 11:40:46Z krasznaa $

// Local include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v2.h"

namespace xAOD {

   PhotonAuxContainer_v2::PhotonAuxContainer_v2()
      : AuxContainerBase() {

/// Include the egamma variables:
#define EGAMMA_AUX_VARIABLE( TYPE, NAME ) AUX_VARIABLE( NAME )
#include "xAODEgamma/versions/EgammaAuxVariables_v2.def"
#undef EGAMMA_AUX_VARIABLE

      /// Links to track particles
      AUX_VARIABLE(vertexLinks);

      /// @name Vertex Match details
      /// @{

      AUX_VARIABLE(convMatchDeltaEta1);
      AUX_VARIABLE(convMatchDeltaEta2);    
      AUX_VARIABLE(convMatchDeltaPhi1);    
      AUX_VARIABLE(convMatchDeltaPhi2);
      
      /// @}
   }

} // namespace xAOD
