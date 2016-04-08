/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlots.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/Muon.h"

MuonTPEfficiencyPlots::MuonTPEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched) 
  : MuonTPEfficiencyPlotBase (pParent, sDir, isMatched),
    m_isMatched(isMatched),
    m_probeTrkKinePlots(this, "/", "Probe"),
    m_probeTrkImpactPlots(this, "/"),
    m_probeTrkRecoInfoPlots(this, "/Trk_"),
    m_probeMuonRecoInfoPlots(this, "/Mu_"),
    m_probeMuonIsolationPlots(this, "/")
{}


//**********************************************************************


void MuonTPEfficiencyPlots::fill(Probe& probe) 
{
  if(m_isMatched && !probe.isMatched()) return;

  //Using IParticle for kinematics
  m_probeTrkKinePlots.fill(probe.probeTrack());

  // Probe is a xAOD::TrackParticle
  const xAOD::TrackParticle* probeTP = dynamic_cast<const xAOD::TrackParticle*>(&probe.probeTrack());
  if(probeTP) {
    m_probeTrkImpactPlots.fill(*probeTP);
    m_probeTrkRecoInfoPlots.fill(*probeTP);
  }

  // Probe is a xAOD::Muon
  const xAOD::Muon* probeMuon = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());
  if(probeMuon) { 
    const xAOD::TrackParticle* primaryTrk = probeMuon->trackParticle(xAOD::Muon::Primary);
    if (primaryTrk) {
      m_probeTrkImpactPlots.fill(*primaryTrk);
      m_probeTrkRecoInfoPlots.fill(*primaryTrk);
    }
    m_probeMuonRecoInfoPlots.fill(*probeMuon);
    m_probeMuonIsolationPlots.fill(*probeMuon);
  }
}
