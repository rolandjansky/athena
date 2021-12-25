/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/TruthMuonPlotOrganizer.h"

namespace Muon{
  
TruthMuonPlotOrganizer::TruthMuonPlotOrganizer(PlotBase* pParent, const std::string& sDir, std::vector<int> *selPlots):
  PlotBase(pParent, sDir)
  // Truth plots
, m_oTruthPlots(nullptr)
, m_oTruthInfoPlots(nullptr)
, m_oTruthTrkExtrapolationPlots(nullptr)
, m_oTruthMSHitPlots(nullptr)
{
  if (!selPlots) {
    m_selPlots.clear();
    for (unsigned int i=0; i<MAX_TRUTHPLOTCLASS; i++) m_selPlots.push_back(i);
  }
  else m_selPlots = *selPlots;
    
  for (auto p: m_selPlots) {
    switch (p) {	
    case TRUTH_PARAM:
      m_oTruthPlots = new Trk::ParamPlots(this, "/", "Truth Muons");
      m_allPlots.push_back(m_oTruthPlots);
      break;
    case TRUTH_INFO:
      m_oTruthInfoPlots = new Trk::TruthInfoPlots(this,"/");
      m_allPlots.push_back(m_oTruthInfoPlots);
      break;
    case TRUTH_TRKEXTRAP:
      m_oTruthTrkExtrapolationPlots = new Trk::TruthTrkExtrapolationPlots(this, "/");
      m_allPlots.push_back(m_oTruthTrkExtrapolationPlots);
      break;
    case TRUTH_MSHITS:
      m_oTruthMSHitPlots = new Trk::MSHitPlots(this, "/");
      m_allPlots.push_back(m_oTruthMSHitPlots);
      break;
    }
  }
}
TruthMuonPlotOrganizer::~TruthMuonPlotOrganizer()
{
  for (auto plots: m_allPlots)  delete plots;
  m_allPlots.clear();
}
  
  void TruthMuonPlotOrganizer::fill(const xAOD::TruthParticle& truthMu, float weight){
  //General Truth Plots
    if (m_oTruthPlots) m_oTruthPlots->fill(truthMu, weight);
    if (m_oTruthInfoPlots) m_oTruthInfoPlots->fill(truthMu, weight);
    if (m_oTruthTrkExtrapolationPlots) m_oTruthTrkExtrapolationPlots->fill(truthMu, weight);
    if (m_oTruthMSHitPlots) m_oTruthMSHitPlots->fill(truthMu, weight);
}


}
