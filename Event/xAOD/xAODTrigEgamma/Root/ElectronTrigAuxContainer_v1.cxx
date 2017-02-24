/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronTrigAuxContainer_v1.cxx 707252 2015-11-11 19:27:56Z rwhite $

// Local include(s):
#include "xAODTrigEgamma/versions/ElectronTrigAuxContainer_v1.h"

namespace xAOD {

   ElectronTrigAuxContainer_v1::ElectronTrigAuxContainer_v1()
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
      AUX_VARIABLE(VLoose);
      AUX_VARIABLE(Loose);
      AUX_VARIABLE(Medium);
      AUX_VARIABLE(Tight);
      AUX_VARIABLE(isEMVLoose);
      AUX_VARIABLE(isEMLoose);
      AUX_VARIABLE(isEMMedium);
      AUX_VARIABLE(isEMTight);
      AUX_VARIABLE(LHVLoose);
      AUX_VARIABLE(LHLoose);
      AUX_VARIABLE(LHMedium);
      AUX_VARIABLE(LHTight);
      AUX_VARIABLE(isEMLHVLoose);
      AUX_VARIABLE(isEMLHLoose);
      AUX_VARIABLE(isEMLHMedium);
      AUX_VARIABLE(isEMLHTight);
      AUX_VARIABLE(LHCaloVLoose);
      AUX_VARIABLE(LHCaloLoose);
      AUX_VARIABLE(LHCaloMedium);
      AUX_VARIABLE(LHCaloTight);
      AUX_VARIABLE(isEMLHCaloVLoose);
      AUX_VARIABLE(isEMLHCaloLoose);
      AUX_VARIABLE(isEMLHCaloMedium);
      AUX_VARIABLE(isEMLHCaloTight);
      AUX_VARIABLE(LHValue);
      AUX_VARIABLE(LHCaloValue);
      ///@}
   }

} // namespace xAOD
