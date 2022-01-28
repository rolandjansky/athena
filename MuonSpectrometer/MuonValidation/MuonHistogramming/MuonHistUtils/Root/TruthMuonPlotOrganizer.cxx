/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/TruthMuonPlotOrganizer.h"

namespace Muon{
  
TruthMuonPlotOrganizer::TruthMuonPlotOrganizer(PlotBase* pParent, const std::string& sDir, std::vector<int> selPlots):
  PlotBase(pParent, sDir) {
  if (selPlots.empty()) {  
    for (unsigned int i=0; i<MAX_TRUTHPLOTCLASS; i++) m_selPlots.push_back(i);
  }
  else m_selPlots = std::move(selPlots);
    
  for (auto p: m_selPlots) {
    switch (p) {	
    case TRUTH_PARAM:
      m_oTruthPlots = std::make_unique<Trk::ParamPlots>(this, "/", "Truth Muons");
      break;
    case TRUTH_INFO:
      m_oTruthInfoPlots = std::make_unique<Trk::TruthInfoPlots>(this,"/");
      break;
    case TRUTH_TRKEXTRAP:
      m_oTruthTrkExtrapolationPlots = std::make_unique<Trk::TruthTrkExtrapolationPlots>(this, "/");
      break;
    case TRUTH_MSHITS:
      m_oTruthMSHitPlots = std::make_unique<Trk::MSHitPlots>(this, "/");
      break;
    }
  }
}
TruthMuonPlotOrganizer::~TruthMuonPlotOrganizer() = default;
  
  void TruthMuonPlotOrganizer::fill(const xAOD::TruthParticle& truthMu, float weight){
  //General Truth Plots
    if (m_oTruthPlots) m_oTruthPlots->fill(truthMu, weight);
    if (m_oTruthInfoPlots) m_oTruthInfoPlots->fill(truthMu, weight);
    if (m_oTruthTrkExtrapolationPlots) m_oTruthTrkExtrapolationPlots->fill(truthMu, weight);
    if (m_oTruthMSHitPlots) m_oTruthMSHitPlots->fill(truthMu, weight);
}


}
