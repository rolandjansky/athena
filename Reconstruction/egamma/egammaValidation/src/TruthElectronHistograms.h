/*
  Copyright- (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_TRUTHELECTRONHISTOGRAMS_H
#define EGAMMAVALIDATION_TRUTHELECTRONHISTOGRAMS_H

#include "xAODTracking/TrackParticle.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

#include "ParticleHistograms.h"
#include "TH1.h"

namespace egammaMonitoring {

  class TruthElectronHistograms : public ParticleHistograms
  {

  public:

    using ParticleHistograms::ParticleHistograms;
    using ParticleHistograms::initializePlots;
    
    StatusCode initializePlots();

    using ParticleHistograms::fill;
    
    void fill(const xAOD::TruthParticle* truth, const xAOD::Electron* el = nullptr) ;

  private:    


  };

}

#endif
