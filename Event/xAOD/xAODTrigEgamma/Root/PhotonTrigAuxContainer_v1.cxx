/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PhotonTrigAuxContainer_v1.cxx 707252 2015-11-11 19:27:56Z rwhite $

// Local include(s):
#include "xAODTrigEgamma/versions/PhotonTrigAuxContainer_v1.h"

namespace xAOD {

   PhotonTrigAuxContainer_v1::PhotonTrigAuxContainer_v1()
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
      AUX_VARIABLE(ptcone20);
      AUX_VARIABLE(ptcone30);
      AUX_VARIABLE(ptcone40);
      AUX_VARIABLE(ptvarcone20);
      AUX_VARIABLE(ptvarcone30);
      AUX_VARIABLE(ptvarcone40);
      AUX_VARIABLE(etcone20);
      AUX_VARIABLE(etcone30);
      AUX_VARIABLE(etcone40);
      AUX_VARIABLE(topoetcone20);
      AUX_VARIABLE(topoetcone30);
      AUX_VARIABLE(topoetcone40);
      AUX_VARIABLE(Loose);
      AUX_VARIABLE(Medium);
      AUX_VARIABLE(Tight);
      AUX_VARIABLE(isEMLoose);
      AUX_VARIABLE(isEMMedium);
      AUX_VARIABLE(isEMTight);

      /// @}
   }

} // namespace xAOD
