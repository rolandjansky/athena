/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_PARTICLEHISTOGRAMS_H
#define EGAMMAVALIDATION_PARTICLEHISTOGRAMS_H

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/xAODTruthHelpers.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

#include "TH1D.h"

#include "IHistograms.h"

namespace egammaMonitoring{
  
  class ParticleHistograms: public IHistograms {
  public:


    using IHistograms::IHistograms;
    
    StatusCode initializePlots();

    void fill(const xAOD::IParticle& egamma);
    void fill(const xAOD::IParticle& egamma, float mu);


  private:

  };
 
}

#endif

