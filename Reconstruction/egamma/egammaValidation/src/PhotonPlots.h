/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_PHOTONPLOTS_H
#define EGAMMAVALIDATION_PHOTONPLOTS_H

#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"

#include "ParticlePlots.h"

#include "TH1.h"

namespace egammaMonitoring {
  
  class PhotonPlots : public ParticlePlots
  {
  public:
    
  PhotonPlots() : ParticlePlots () {}


    // Photons only Plots
    TH1D *convRadius = nullptr; //!    
    TH1D *mu         = nullptr; //!

    using ParticlePlots::initializePlots;
    void initializePlots();
    
    using ParticlePlots::fill;
    void fill(const xAOD::IParticle& phrec, float muValue);

  private:
    
    const xAOD::TruthParticle* m_tmp;

    float m_cR_bins[15] = {0, 50, 89, 123, 170, 210, 250, 299, 335, 371, 443, 514, 554, 800, 1085};
    float m_x, m_y, m_trueR;

  };

}

#endif
