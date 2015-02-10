/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackValidationPlots.h"

MuonTrackValidationPlots::MuonTrackValidationPlots(PlotBase* pParent, std::string sDir, bool isData):PlotBase(pParent, sDir), m_oRecoMuonTrackPlots(NULL), m_oMatchedMuonTrackPlots(NULL)
{
  m_oRecoMuonTrackPlots = new RecoMuonTrackPlots(this, "reco/MSTrackParticles/");
  if (!isData) m_oMatchedMuonTrackPlots = new TruthRelatedMuonTrackPlots(this,"matched/MSTrackParticles/");
}

MuonTrackValidationPlots::~MuonTrackValidationPlots()
{
  delete m_oRecoMuonTrackPlots;
  if (m_oMatchedMuonTrackPlots) delete m_oMatchedMuonTrackPlots;
}

void MuonTrackValidationPlots::fill(const xAOD::TrackParticle& muTP) {
  m_oRecoMuonTrackPlots->fill(muTP);
}

void MuonTrackValidationPlots::fill(const xAOD::TruthParticle* truthMu, const xAOD::TrackParticle* muTP){
  if (muTP) {
    m_oRecoMuonTrackPlots->fill(*muTP);
    if (truthMu) {
      m_oMatchedMuonTrackPlots->fill(*truthMu,*muTP);
    }
  }
 
}
