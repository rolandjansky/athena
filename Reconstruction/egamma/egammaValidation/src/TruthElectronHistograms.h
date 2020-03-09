/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
*/

#ifndef EGAMMAVALIDATION_TRUTHELECTRONHISTOGRAMS_H
#define EGAMMAVALIDATION_TRUTHELECTRONHISTOGRAMS_H

#include "xAODTruth/TruthParticle.h" //typedef
#include "xAODEgamma/Electron.h" //typedef
#include "ParticleHistograms.h"

class StatusCode;

namespace egammaMonitoring {

  class TruthElectronHistograms : public ParticleHistograms
  {

  public:

    using ParticleHistograms::ParticleHistograms;
    using ParticleHistograms::initializePlots;
    
    StatusCode initializePlots();

    using ParticleHistograms::fill;
    
    void fill(const xAOD::TruthParticle* truth, const xAOD::Electron* el = nullptr) ;
    virtual ~TruthElectronHistograms(){};

  private:    


  };

}

#endif
