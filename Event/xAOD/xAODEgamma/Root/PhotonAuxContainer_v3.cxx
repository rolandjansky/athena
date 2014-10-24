/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonAuxContainer_v3.cxx 619839 2014-10-03 13:38:44Z christos $

// Local include(s):
#include "xAODEgamma/versions/PhotonAuxContainer_v3.h"

namespace xAOD {

   PhotonAuxContainer_v3::PhotonAuxContainer_v3()
      : AuxContainerBase() {

/// Include the egamma variables:
#define EGAMMA_AUX_VARIABLE( TYPE, NAME ) AUX_VARIABLE( NAME )
#include "xAODEgamma/versions/EgammaAuxVariables_v3.def"
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
