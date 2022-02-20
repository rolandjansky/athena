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
    RecoMuonPlotOrganizer(PlotBase* pParent, const std::string& sDir, std::vector<int> selPlots = {});
    ~RecoMuonPlotOrganizer();    
   
    std::vector<int> m_selPlots;  
    
    void fill(const xAOD::Muon& mu, float weight=1.0);
    
    // Reco only information
    std::unique_ptr<Muon::IDHitSummaryPlots>         m_oIDHitPlots{};
    std::unique_ptr<Trk::ParamPlots>                 m_oTrkParamPlots{};
    std::unique_ptr<Trk::ImpactPlots>                m_oImpactPlots{};
    std::unique_ptr<Muon::MuonParamPlots>            m_oMuonParamPlots{};
    std::unique_ptr<Muon::RecoInfoPlots>             m_oMuRecoInfoPlots{};
    std::unique_ptr<Muon::MomentumPullPlots>         m_oMomentumPullPlots{};
    std::unique_ptr<Muon::MuonHitSummaryPlots>       m_oMuonHitSummaryPlots{};
    std::unique_ptr<Muon::MuonIsolationPlots>        m_oMuonIsolationPlots{};
    std::unique_ptr<Muon::ChargeDepParamPlots>       m_oChargeParamPlotsLowPt{};
    std::unique_ptr<Muon::ChargeDepParamPlots>       m_oChargeParamPlotsHighPt{};
  
  };
 
}

#endif
