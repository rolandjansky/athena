/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronAuxContainer_v1.cxx 597503 2014-05-16 16:34:05Z christos $

// Local include(s):
#include "xAODEgamma/versions/ElectronAuxContainer_v1.h"


namespace xAOD {
  
  ElectronAuxContainer_v1::ElectronAuxContainer_v1()
    : EgammaAuxContainer_v1() {


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
