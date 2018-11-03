/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthPhotonHistograms.h"

using namespace egammaMonitoring;

#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AnaToolHandle.h"

StatusCode TruthPhotonHistograms::initializePlots() {

  ATH_CHECK(ParticleHistograms::initializePlots());

  histoMap["convRadius"] = new TH1D(Form("%s_%s",m_name.c_str(),"convRadius"), ";Conversion Radius [mm]; Conversion Radius Events", 14, m_cR_bins);

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"convRadius", histoMap["convRadius"]));

  return StatusCode::SUCCESS;

}

void TruthPhotonHistograms::fill(const xAOD::IParticle& phrec) {

  ParticleHistograms::fill(phrec);

  m_tmp = xAOD::TruthHelpers::getTruthParticle(phrec);
  m_trueR = -999;

  if (m_tmp) {
    if (m_tmp->pdgId() == 22 && m_tmp->hasDecayVtx()) {

      m_x     = m_tmp->decayVtx()->x();
      m_y     = m_tmp->decayVtx()->y();
      m_trueR = sqrt( m_x*m_x + m_y*m_y );

    }
  }

  histoMap["convRadius"]->Fill(m_trueR);


} // fill
