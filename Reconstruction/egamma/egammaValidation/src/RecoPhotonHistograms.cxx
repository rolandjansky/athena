/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RecoPhotonHistograms.h"

using namespace egammaMonitoring;

#include "AsgTools/AnaToolHandle.h" //just to get ATH_CHECK? should be a better way!
#include "GaudiKernel/ITHistSvc.h"
#include <cmath>

StatusCode RecoPhotonHistograms::initializePlots() {

  ATH_CHECK(ParticleHistograms::initializePlots());

  histoMap["convRadius"] = new TH1D(Form("%s_%s",m_name.c_str(),"convRadius"), ";Conversion Radius [mm]; Events", 14, m_cR_bins);

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"convRadius", histoMap["convRadius"]));

  return StatusCode::SUCCESS;
}

void RecoPhotonHistograms::fill(const xAOD::Photon& phrec) {

  double trueR(-999);
  
  ParticleHistograms::fill(phrec);
 
  const xAOD::TruthParticle *tmp = xAOD::TruthHelpers::getTruthParticle(phrec);

  if (tmp) {
    if (tmp->pdgId() == 22 && tmp->hasDecayVtx()) {

      float x = tmp->decayVtx()->x();
      float y = tmp->decayVtx()->y();
      trueR = std::sqrt( x*x + y*y );

    }
  }

  histoMap["convRadius"]->Fill(trueR);

  
} // fill
