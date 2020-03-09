/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_RECOPHOTONHISTOGRAMS_H
#define EGAMMAVALIDATION_RECOPHOTONHISTOGRAMS_H

#include "xAODEgamma/Photon.h" //typedef
#include "ParticleHistograms.h"
class StatusCode;

namespace egammaMonitoring {

  class RecoPhotonHistograms : public ParticleHistograms {
  public:

    using ParticleHistograms::ParticleHistograms;
    using ParticleHistograms::initializePlots;

    StatusCode initializePlots();

    using ParticleHistograms::fill;

    void fill(const xAOD::Photon&);

  private:

    float m_cR_bins[15] = {0, 50, 89, 123, 170, 210, 250, 299, 335, 371, 443, 514, 554, 800, 1085};

  };

}

#endif
