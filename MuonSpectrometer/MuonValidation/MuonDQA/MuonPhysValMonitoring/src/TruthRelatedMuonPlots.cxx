/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthRelatedMuonPlots.h"

TruthRelatedMuonPlots::TruthRelatedMuonPlots(PlotBase* pParent, std::string sDir,bool doBinnedResolutionPlots):
  PlotBase(pParent, sDir),
  // truth related information
  m_oMatchedPlots(this, "/", "Matched Muons"),
  m_oMSHitDiffPlots(this,"/"),
  m_oMuonHitDiffSummaryPlots(this, "/"),
  
  m_oMuonResolutionPlots(this, "/resolution/",doBinnedResolutionPlots),
  m_oMuonMSResolutionPlots(this, "/resolutionMS/",doBinnedResolutionPlots),
  m_oMuonIDResolutionPlots(this, "/resolutionID/",doBinnedResolutionPlots)

{
}	

TruthRelatedMuonPlots::~TruthRelatedMuonPlots() {}

void TruthRelatedMuonPlots::fill(const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu){
  m_oMatchedPlots.fill( truthMu );
  m_oMuonHitDiffSummaryPlots.fill(mu, truthMu);

  // Tracking related plots
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (!primaryTrk) return;
  m_oMSHitDiffPlots.fill(*primaryTrk, truthMu);
  m_oMuonResolutionPlots.fill(*primaryTrk, truthMu);

  const xAOD::TrackParticle* inDetTrk = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
  if (inDetTrk) {
    m_oMuonIDResolutionPlots.fill(*inDetTrk,truthMu);
  }

  const xAOD::TrackParticle* msTrk = mu.trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
  if (msTrk) {
    m_oMuonMSResolutionPlots.fill(*msTrk,truthMu);
  }
}
