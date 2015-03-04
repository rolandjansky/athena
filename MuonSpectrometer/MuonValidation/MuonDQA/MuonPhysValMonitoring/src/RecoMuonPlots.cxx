/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecoMuonPlots.h"

typedef ElementLink< xAOD::TrackParticleContainer > TrackLink;
typedef ElementLink< xAOD::MuonContainer > MuonLink;

RecoMuonPlots::RecoMuonPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),

m_oAllPlots(this, "/kinematics/", "Reco Muon"),
m_oMuonParamPlots(this,"/parameters/"),
m_oMuRecoInfoPlots(this, "/parameters/"),
m_oImpactPlots(this, "/parameters/"),
m_oMomentumPullPlots(this, "/momentumPulls/"),
m_oMSHitPlots(this,"/hits/"),
m_oMuonHitSummaryPlots(this,"/hits/"),
m_oMuonIsolationPlots(this,"/isolation/")

{}

void RecoMuonPlots::fill(const xAOD::Muon& mu) {
  //General Plots
  m_oAllPlots.fill(mu);
  m_oMuonParamPlots.fill(mu);
  m_oMuRecoInfoPlots.fill(mu);
  m_oMomentumPullPlots.fill(mu);
  m_oMuonHitSummaryPlots.fill(mu);
  m_oMuonIsolationPlots.fill(mu);

  // tracking related plots
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (!primaryTrk) return;
  m_oImpactPlots.fill(*primaryTrk);
  m_oMSHitPlots.fill(*primaryTrk);
}
