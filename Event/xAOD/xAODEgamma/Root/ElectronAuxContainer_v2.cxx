/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronAuxContainer_v2.cxx 614829 2014-09-03 09:39:36Z krasznaa $

// Local include(s):
#include "xAODEgamma/versions/ElectronAuxContainer_v2.h"

namespace xAOD {
  
   ElectronAuxContainer_v2::ElectronAuxContainer_v2()
      : AuxContainerBase() {

/// Include the egamma variables:
#define EGAMMA_AUX_VARIABLE( TYPE, NAME ) AUX_VARIABLE( NAME )
#include "xAODEgamma/versions/EgammaAuxVariables_v2.def"
#undef EGAMMA_AUX_VARIABLE

      ///@links to track Particles
      AUX_VARIABLE(trackParticleLinks);

      ///@name Electron Charge
      ///@{
      AUX_VARIABLE(charge);
      ///@}

      /// @name Track Match details
      /// @{
      AUX_VARIABLE(deltaEta0);
      AUX_VARIABLE(deltaEta1);
      AUX_VARIABLE(deltaEta2);
      AUX_VARIABLE(deltaEta3);
      AUX_VARIABLE(deltaPhi0);
      AUX_VARIABLE(deltaPhi1);
      AUX_VARIABLE(deltaPhi2);
      AUX_VARIABLE(deltaPhi3);
      AUX_VARIABLE(deltaPhiRescaled0);
      AUX_VARIABLE(deltaPhiRescaled1);
      AUX_VARIABLE(deltaPhiRescaled2);
      AUX_VARIABLE(deltaPhiRescaled3);
      AUX_VARIABLE(deltaPhiFromLastMeasurement);
      ///@}
   }
  
} // namespace xAOD
