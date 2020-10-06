/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/PlotMuons.h"

PlotMuons::PlotMuons(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
    m_plots_All(this, "/", "Plot Muons"),
    m_plots_RecoInfo(this, "/"),
    m_plots_MomentumPull(this, "/"),
    m_plots_MuonHitSummary(this, "/")
{}

void PlotMuons::fill(const xAOD::Muon& muon){
  
    m_plots_All.fill(muon);
    m_plots_RecoInfo.fill(muon);
    m_plots_MomentumPull.fill(muon);
    m_plots_MuonHitSummary.fill(muon);
    
}
