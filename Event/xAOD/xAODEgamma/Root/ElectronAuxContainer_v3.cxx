/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronAuxContainer_v3.cxx 619839 2014-10-03 13:38:44Z christos $

// Local include(s):
#include "xAODEgamma/versions/ElectronAuxContainer_v3.h"

namespace xAOD {
  
   ElectronAuxContainer_v3::ElectronAuxContainer_v3()
      : AuxContainerBase() {

/// Include the egamma variables:
#define EGAMMA_AUX_VARIABLE( TYPE, NAME ) AUX_VARIABLE( NAME )
#include "xAODEgamma/versions/EgammaAuxVariables_v3.def"
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
