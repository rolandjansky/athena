/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PhotonPlots.h"

using namespace egammaMonitoring;

void PhotonPlots::initializePlots() {
  
  ParticlePlots::initializePlots();

  convRadius = new TH1D("convRadius", ";Conversion Radius [mm]; Conversion Radius Events", 14, m_cR_bins);
  mu         = new TH1D("mu"        , ";<#mu>; Events of Avg num. of Interactions per Crossing"  , 15, 0, 60);
  
}

void PhotonPlots::fill(const xAOD::IParticle& phrec, float muValue) {
  
  ParticlePlots::fill(phrec);

  m_tmp = xAOD::TruthHelpers::getTruthParticle(phrec);
  m_trueR = -999;

  if (m_tmp) {
    if (m_tmp->pdgId() == 22 && m_tmp->hasDecayVtx()) {

      m_x     = m_tmp->decayVtx()->x();
      m_y     = m_tmp->decayVtx()->y();
      m_trueR = sqrt( m_x*m_x + m_y*m_y );

    }
  }

  convRadius->Fill(m_trueR);

  mu->Fill(muValue);
  
} // fill
