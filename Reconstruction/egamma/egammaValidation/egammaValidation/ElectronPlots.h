/*
  Copyright- (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_ELECTRONPLOTS_H
#define EGAMMAVALIDATION_ELECTRONPLOTS_H

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "xAODTracking/TrackParticle.h"

#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

#include "ParticlePlots.h"

#include "TH1.h"

namespace egammaMonitoring{

  class ElectronPlots : public ParticlePlots
  {

  public:

  ElectronPlots() : ParticlePlots() {}

    // Electron Plot(s)
    TH1D *TrkToEl = nullptr; //!
    
    using ParticlePlots::initializePlots;
    void initializePlots();

    using ParticlePlots::fill;
    void fill(const xAOD::Electron& elrec);

  private:    

    std::set<const xAOD::TrackParticle*> m_conversionTracks;

  };

}

#endif
