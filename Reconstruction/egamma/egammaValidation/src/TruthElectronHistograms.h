/*
  Copyright- (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_TRUTHELECTRONHISTOGRAMS_H
#define EGAMMAVALIDATION_TRUTHELECTRONHISTOGRAMS_H

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODTracking/TrackParticle.h"

#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

#include "ParticleHistograms.h"
#include "IHistograms.h"
#include "TH1.h"

namespace egammaMonitoring {

  class TruthElectronHistograms : public ParticleHistograms
  {

  public:

    using ParticleHistograms::ParticleHistograms;
    using ParticleHistograms::initializePlots;
    StatusCode initializePlots();

    using ParticleHistograms::fill;
    
    void fill(const xAOD::Electron& elrec);

  private:    


  };

}

#endif
