/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_CBMSIDALIGNPLOTS_H
#define MUONTRKPHYSMONITORING_CBMSIDALIGNPLOTS_H

#include "PlotBase.h"
#include "ResidualPlots.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonTrkPhysMonUtils.h"
#include "ChargeDepCbMSIDAlignPlots.h"
#include "xAODMuon/Muon.h"


class CbMSIDAlignPlots: public PlotBase {
  public:
    CbMSIDAlignPlots(PlotBase *pParent);
    void fill(const xAOD::Muon* Muon, const Trk::Perigee* measPerigeeID, const Trk::Perigee* measPerigeeME);

    TH1F* m_oPt_Diff;
    ChargeDepCbMSIDAlignPlots m_oPosCbMSIDAlignPlots;
    ChargeDepCbMSIDAlignPlots m_oNegCbMSIDAlignPlots;

    // <1/pT_ms-1/pT_id> vs muon eta
    TH1F* m_oPt_ms_id_sum;
    TH1F* m_oPt_ms_id_sum_weights;
    TH1F* m_oPt_ms_id_sum_entries;
    TH1F* m_oPt_ms_id_sum_mean;

    // <chi2_cb trk/ndof> vs muon eta
    TH1F* m_chi2ndof_CB;
    TH1F* m_chi2ndof_CB_weights;
    TH1F* m_chi2ndof_CB_entries;
    TH1F* m_chi2ndof_CB_mean;
    
    // <DeltaZ0> vs muon eta region
    TH1F* m_deltaZ0_CB_Eta_AllSectors;
    TH2F* m_deltaZ0_CB_Eta_Phi;
    TH2F* m_deltaZ0_CB_Eta_Phi_Width;
    
    TH2F* m_deltaZ0_CB;
    TH2F* m_deltaZ0_CB_S1;
    TH2F* m_deltaZ0_CB_S2;
    TH2F* m_deltaZ0_CB_S3;
    TH2F* m_deltaZ0_CB_S4;
    TH2F* m_deltaZ0_CB_S5;
    TH2F* m_deltaZ0_CB_S6;
    TH2F* m_deltaZ0_CB_S7;
    TH2F* m_deltaZ0_CB_S8;
    TH2F* m_deltaZ0_CB_S9;
    TH2F* m_deltaZ0_CB_S10;
    TH2F* m_deltaZ0_CB_S11;
    TH2F* m_deltaZ0_CB_S12;
    TH2F* m_deltaZ0_CB_S13;
    TH2F* m_deltaZ0_CB_S14;
    TH2F* m_deltaZ0_CB_S15;
    TH2F* m_deltaZ0_CB_S16;

	
  private:
    void initializePlots();
		void finalizePlots();

};

#endif
