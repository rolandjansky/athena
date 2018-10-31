/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
*/

#ifndef EGAMMAVALIDATION_RECOELECTRONHISTOGRAMS_H
#define EGAMMAVALIDATION_RECOELECTRONHISTOGRAMS_H

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODTracking/TrackParticle.h"

#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

#include "ParticleHistograms.h"

#include "TH1.h"

namespace egammaMonitoring{

  class RecoElectronHistograms : public ParticleHistograms
  {

  public:

    // Electron Plot(s)
    TH1* TrkToEl; //!
    using ParticleHistograms::ParticleHistograms;   
    using ParticleHistograms::initializePlots;
    StatusCode initializePlots();

    using ParticleHistograms::fill;
    void fill(const xAOD::Electron& elrec);

  private:    

    std::set<const xAOD::TrackParticle*> m_conversionTracks;

  };

}

#endif
