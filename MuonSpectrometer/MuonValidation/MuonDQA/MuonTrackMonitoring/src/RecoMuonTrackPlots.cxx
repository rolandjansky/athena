/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RecoMuonTrackPlots.h"

RecoMuonTrackPlots::RecoMuonTrackPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
m_oAllPlots(this, "/", "Reco Muon"),
m_oImpactPlots(this, "/"),
m_oTrkRecoInfoPlots(this, "/"),
m_oMSHitPlots(this,"/"),

m_oResidualPlots(this,"/residuals/")
{}

void RecoMuonTrackPlots::fill(const xAOD::TrackParticle& muTP) {
  m_oAllPlots.fill(muTP);
  m_oImpactPlots.fill(muTP);
  m_oTrkRecoInfoPlots.fill(muTP);
  m_oMSHitPlots.fill(muTP);
}

void RecoMuonTrackPlots::fill(const Trk::ResidualPull& resPull, int stationPhi, Muon::MuonStationIndex::TechnologyIndex techid, bool measuresPhi)
{
  m_oResidualPlots.fill(resPull,stationPhi,techid,measuresPhi);
}
