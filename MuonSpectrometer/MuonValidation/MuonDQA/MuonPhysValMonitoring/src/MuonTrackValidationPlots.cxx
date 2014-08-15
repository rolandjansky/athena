/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackValidationPlots.h"

MuonTrackValidationPlots::MuonTrackValidationPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
m_oRecoMuonTrackPlots(this, "reco/MSTrackParticles/"),
m_oMatchedMuonTrackPlots(this,"matched/MSTrackParticles/") {}


void MuonTrackValidationPlots::fill(const xAOD::TrackParticle& muTP) {
  m_oRecoMuonTrackPlots.fill(muTP);
}

void MuonTrackValidationPlots::fill(const xAOD::TruthParticle* truthMu, const xAOD::TrackParticle* muTP){
  if (muTP) {
    m_oRecoMuonTrackPlots.fill(*muTP);
    if (truthMu) {
      m_oMatchedMuonTrackPlots.fill(*truthMu,*muTP);
    }
  }
 
}
