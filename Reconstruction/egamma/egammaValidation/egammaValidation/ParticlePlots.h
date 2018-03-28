/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_PARTICLEPLOTS_H
#define EGAMMAVALIDATION_PARTICLEPLOTS_H

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/xAODTruthHelpers.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

#include "TH1.h"

namespace egammaMonitoring{
  
  class ParticlePlots{
  public:

    ParticlePlots();

    // Photon Histos

    TH1D *pT_prtcl   = nullptr; //!
    TH1D *eta_prtcl  = nullptr; //!
    TH1D *phi_prtcl  = nullptr; //!
    
    void initializePlots();
    void fill(const xAOD::IParticle& egamma);

  private:

  };
 
}

#endif

