/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_PLOTMUONS_H
#define MUONTRACKMONITORING_PLOTMUONS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "MuonHistUtils/RecoInfoPlots.h"
#include "MuonHistUtils/MomentumPullPlots.h"
#include "MuonHistUtils/MuonHitSummaryPlots.h"
#include "xAODMuon/Muon.h"

class PlotMuons:public PlotBase{

    public:

        PlotMuons(PlotBase* pParent, std::string sDir);

        void fill(const xAOD::Muon& muon);
        
        Trk::ParamPlots            m_plots_All;
        Muon::RecoInfoPlots        m_plots_RecoInfo;
        Muon::MomentumPullPlots    m_plots_MomentumPull;
        Muon::MuonHitSummaryPlots  m_plots_MuonHitSummary;
};

#endif
