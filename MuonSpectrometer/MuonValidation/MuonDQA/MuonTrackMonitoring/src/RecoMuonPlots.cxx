/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/RecoMuonPlots.h"
#include "MuonHistUtils/MuonEnumDefs.h"
typedef ElementLink< xAOD::TrackParticleContainer > TrackLink;
typedef ElementLink< xAOD::MuonContainer > MuonLink;

RecoMuonPlots::RecoMuonPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir)

, m_oAllPlots(this, "/", "Reco Muon")
, m_oMuRecoInfoPlots(this, "/")
, m_oImpactPlots(this, "/")
, m_oMomentumPullPlots(this, "/")
, m_oMSHitPlots(this,"/")
, m_oMuonHitSummaryPlots(this,"/")
, m_oMuonIsolationPlots(this,"/")
, m_oMuonParamPlots(this,"/")
, m_oAllPlots_tight(this, "/Tight/", "Tight")
, m_oAllPlots_medium(this, "/Medium/", "Medium")
, m_oAllPlots_loose(this, "/Loose/", "Loose")
, m_oAllPlots_veryloose(this, "/Veryloose/", "Veryloose")
{}

void RecoMuonPlots::fill(const xAOD::Muon& mu){
  //General Plots
  m_oAllPlots.fill(mu);
  m_oMuRecoInfoPlots.fill(mu);
  m_oMomentumPullPlots.fill(mu);
  m_oMuonHitSummaryPlots.fill(mu);
  m_oMuonIsolationPlots.fill(mu);
  m_oMuonParamPlots.fill(mu);

  // tracking related plots
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (!primaryTrk) return;
  m_oImpactPlots.fill(*primaryTrk);
  m_oMSHitPlots.fill(*primaryTrk);
}

void RecoMuonPlots::fill(const xAOD::Muon& mu, xAOD::Muon::Quality my_quality){
  //General Plots
  if (my_quality == xAOD::Muon::Tight) m_oAllPlots_tight.fill(mu);
  else if (my_quality == xAOD::Muon::Medium) m_oAllPlots_medium.fill(mu);
  else if (my_quality == xAOD::Muon::Loose) m_oAllPlots_loose.fill(mu);
  else if (my_quality == xAOD::Muon::VeryLoose) m_oAllPlots_veryloose.fill(mu);
}