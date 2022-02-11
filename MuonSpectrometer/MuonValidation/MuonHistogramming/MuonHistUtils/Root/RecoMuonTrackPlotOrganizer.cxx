/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/RecoMuonTrackPlotOrganizer.h"

namespace Muon{
  
RecoMuonTrackPlotOrganizer::RecoMuonTrackPlotOrganizer(PlotBase* pParent, const std::string& sDir, std::vector<int> selPlots):
  PlotBase(pParent, sDir) {
  
  if (selPlots.empty()) {
    m_selPlots.clear();
    for (unsigned int i=0; i<MAX_RECOTRKPLOTCLASS; i++) m_selPlots.push_back(i);
  }
  else m_selPlots = std::move(selPlots);
    
  for (int p: m_selPlots) {
    switch (p) {
    case TRK_PARAM:
      m_oTrkParamPlots = std::make_unique<Trk::ParamPlots>(this, "/kinematics/", "RecoMuon");     
      break;
    case TRK_RECOINFO:
      m_oTrkRecoInfoPlots = std::make_unique< Trk::RecoInfoPlots>(this, "/parameters/", "RecoMuon");      
      break;      
    case TRK_IMPACT:
      m_oImpactPlots = std::make_unique<Trk::ImpactPlots>(this, "/parameters/");
      break;
    case TRK_MSHITS:
      m_oMSHitPlots = std::make_unique<Trk::MSHitPlots>(this,"/hits/");
      break;    
    case TRK_IDHITS:
      m_oIDHitPlots = std::make_unique<Muon::IDHitSummaryPlots>(this,"/hits/");
      break;
    }
  }
}
  
RecoMuonTrackPlotOrganizer::~RecoMuonTrackPlotOrganizer() = default;
  
void RecoMuonTrackPlotOrganizer::fill(const xAOD::TrackParticle& tp, float weight) {
    if (m_oTrkParamPlots) m_oTrkParamPlots->fill(tp, weight);
    if (m_oTrkRecoInfoPlots) m_oTrkRecoInfoPlots->fill(tp,weight);
    if (m_oImpactPlots) m_oImpactPlots->fill(tp,weight);  
    if (m_oMSHitPlots) m_oMSHitPlots->fill(tp,weight);
    if (m_oIDHitPlots) m_oIDHitPlots->fill(tp,weight);
}

}
