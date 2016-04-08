/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RECOMUONPLOTS_H
#define MUONTRACKMONITORING_RECOMUONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "MuonHistUtils/RecoInfoPlots.h"
#include "MuonHistUtils/MomentumPullPlots.h"
#include "MuonHistUtils/MuonHitSummaryPlots.h"
#include "MuonHistUtils/MuonIsolationPlots.h"
#include "MuonHistUtils/MuonParamPlots.h"


#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

class RecoMuonPlots:public PlotBase {
    public:
      RecoMuonPlots(PlotBase* pParent, std::string sDir, bool detail=false);
      void fill(const xAOD::Muon& mu);
      void fill(const xAOD::Muon& mu, xAOD::Muon::Quality my_quality);
      
      // Reco only information
      Trk::ParamPlots                 m_oAllPlots;
      Muon::RecoInfoPlots             m_oMuRecoInfoPlots;
      Trk::ImpactPlots                m_oImpactPlots;
      Muon::MomentumPullPlots         m_oMomentumPullPlots;
      //Trk::MSHitPlots                 m_oMSHitPlots; 
      //Muon::MuonHitSummaryPlots       m_oMuonHitSummaryPlots;
      //Muon::MuonIsolationPlots        m_oMuonIsolationPlots;
      //Muon::MuonParamPlots            m_oMuonParamPlots;

      //extra monitoring plots
      //Trk::ParamPlots                 m_oAllPlots_tight;
      //Trk::ParamPlots                 m_oAllPlots_medium;
      //Trk::ParamPlots                 m_oAllPlots_loose;
      //Trk::ParamPlots                 m_oAllPlots_veryloose;

      //extra monitoring efficiency plots; filled in post processing
      TH2* m_origin_eta_phi;
      TH2* m_eff_tight;
      TH2* m_eff_medium;
      TH2* m_tight_eta_phi;
      TH2* m_medium_eta_phi;
      //TH2* m_eff_loose;
      //TH2* m_eff_veryloose;
      TH1* m_pt_broad;
      TH2* m_eta_phi_broad;

      //extra monitoring efficiency plots, normalized in post processing
      TH2* m_eff_nPrec;
      TH2* m_eff_nPhi;
      TH2* m_eff_nTrigEta;
      TH2* m_eff_ndof;
      TH2* m_eff_chi2;
      TH2* m_ID_eff_ndof;
      TH2* m_ID_eff_chi2;
      TH2* m_MS_eff_ndof;
      TH2* m_MS_eff_chi2;

      bool Detail;

    private:
      void initializePlots();
};

#endif
