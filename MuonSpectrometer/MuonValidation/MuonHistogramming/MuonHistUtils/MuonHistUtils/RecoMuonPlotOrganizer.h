/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_RECOMUONPLOTORGANIZER_H
#define MUONHISTUTILS_RECOMUONPLOTORGANIZER_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "MuonHistUtils/RecoMuonTrackPlotOrganizer.h"
#include "MuonHistUtils/RecoInfoPlots.h"
#include "MuonHistUtils/MuonParamPlots.h"
#include "MuonHistUtils/ChargeDepParamPlots.h"
#include "MuonHistUtils/MomentumPullPlots.h"
#include "MuonHistUtils/MuonIsolationPlots.h"
#include "MuonHistUtils/MuonHitSummaryPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include <vector>

namespace Muon
{
  enum RECOPLOTCLASS {MUON_TRKPARAM=0,MUON_PARAM,MUON_TRKIMPACT,MUON_RECOINFO,MUON_MOMPULLS,MUON_HITS,MUON_IDHITS,MUON_ISO,MUON_CHARGEPARAM,MAX_RECOPLOTCLASS};
  
  class RecoMuonPlotOrganizer:public PlotBase {
  public:
    RecoMuonPlotOrganizer(PlotBase* pParent, const std::string& sDir, std::vector<int> *selPlots=0);
    ~RecoMuonPlotOrganizer();
    
    std::vector<PlotBase*> m_allPlots;
    std::vector<int> m_selPlots;  
    
    void fill(const xAOD::Muon& mu, float weight=1.0);
    
    // Reco only information
    Muon::IDHitSummaryPlots         *m_oIDHitPlots;
    Trk::ParamPlots                 *m_oTrkParamPlots;
    Trk::ImpactPlots                *m_oImpactPlots;
    Muon::MuonParamPlots            *m_oMuonParamPlots;
    Muon::RecoInfoPlots             *m_oMuRecoInfoPlots;
    Muon::MomentumPullPlots         *m_oMomentumPullPlots;
    Muon::MuonHitSummaryPlots       *m_oMuonHitSummaryPlots;
    Muon::MuonIsolationPlots        *m_oMuonIsolationPlots;
    Muon::ChargeDepParamPlots       *m_oChargeParamPlotsLowPt;
    Muon::ChargeDepParamPlots       *m_oChargeParamPlotsHighPt;
  
  };
 
}

#endif
