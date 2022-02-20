/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/RecoMuonPlotOrganizer.h"

namespace Muon{
  
  RecoMuonPlotOrganizer::RecoMuonPlotOrganizer(PlotBase* pParent, const std::string& sDir, std::vector<int> selPlots):
  PlotBase(pParent, sDir) {
  
  if (selPlots.empty()) {  
    for (unsigned int i=0; i<MAX_RECOPLOTCLASS; i++) m_selPlots.push_back(i);
  }
  else m_selPlots = std::move(selPlots); 
  
  for (int p: m_selPlots) {
    switch (p) {
    case MUON_TRKPARAM:
      m_oTrkParamPlots = std::make_unique<Trk::ParamPlots>(this, "/kinematics/", "RecoMuon");
      break;
    case MUON_PARAM:
      m_oMuonParamPlots = std::make_unique<Muon::MuonParamPlots>(this,"/parameters/");
      break;
    case MUON_RECOINFO:
      m_oMuRecoInfoPlots = std::make_unique<Muon::RecoInfoPlots>(this, "/parameters/");
      break;
    case MUON_TRKIMPACT:
      m_oImpactPlots = std::make_unique<Trk::ImpactPlots>(this, "/parameters/");
      break;
    case MUON_MOMPULLS:
      m_oMomentumPullPlots = std::make_unique<Muon::MomentumPullPlots>(this, "/momentumPulls/");
      break;
    case MUON_HITS:    
      m_oMuonHitSummaryPlots = std::make_unique<Muon::MuonHitSummaryPlots>(this,"/hits/");
      break;
    case MUON_IDHITS:
      m_oIDHitPlots = std::make_unique<Muon::IDHitSummaryPlots>(this,"/hits/");
      break;
    case MUON_ISO:	
      m_oMuonIsolationPlots = std::make_unique<Muon::MuonIsolationPlots>(this,"/isolation/");
      break;
    case MUON_CHARGEPARAM:
      m_oChargeParamPlotsLowPt = std::make_unique<Muon::ChargeDepParamPlots>(this, "/kinematics/", "lowPt");
      m_oChargeParamPlotsHighPt = std::make_unique<Muon::ChargeDepParamPlots>(this, "/kinematics/", "highPt");
     break;
    } 
  }
}
  
RecoMuonPlotOrganizer::~RecoMuonPlotOrganizer() = default;

void RecoMuonPlotOrganizer::fill(const xAOD::Muon& mu, float weight) {
  if (m_oIDHitPlots && (mu.inDetTrackParticleLink().isValid())) m_oIDHitPlots->fill(*mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle), weight);
  if (m_oTrkParamPlots) m_oTrkParamPlots->fill(mu,weight);
  if (m_oMuonParamPlots) m_oMuonParamPlots->fill(mu,weight);
  if (m_oMuRecoInfoPlots) m_oMuRecoInfoPlots->fill(mu,weight);
  if (m_oMomentumPullPlots) m_oMomentumPullPlots->fill(mu,weight);
  if (m_oMuonHitSummaryPlots) m_oMuonHitSummaryPlots->fill(mu,weight);
  if (m_oMuonIsolationPlots) m_oMuonIsolationPlots->fill(mu,weight);
  if (m_oChargeParamPlotsLowPt && m_oChargeParamPlotsHighPt) {
    if (mu.pt()<8000) m_oChargeParamPlotsLowPt->fill(mu,weight);
    else m_oChargeParamPlotsHighPt->fill(mu,weight);
  }
   
  // tracking related plots
  const xAOD::TrackParticle* primaryTrk = mu.trackParticle(xAOD::Muon::Primary);
  if (!primaryTrk) return;
  if (m_oImpactPlots) m_oImpactPlots->fill(*primaryTrk,weight);
}

}
