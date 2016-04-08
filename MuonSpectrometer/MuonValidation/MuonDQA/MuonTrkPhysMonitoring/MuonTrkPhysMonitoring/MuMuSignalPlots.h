/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_MuMuSIGNALPLOTS_H
#define MUONTRKPHYSMONITORING_MuMuSIGNALPLOTS_H

// #include "TrkValHistUtils/PlotBase.h"
// #include "xAODMuon/Muon.h"

// class MuMuSignalPlots:public PlotBase {
//   public:
//     MuMuSignalPlots(PlotBase *pParent, std::string sDir, std::string recObj);

//     void fill(const float eta_mu_plus, const float eta_mu_minus, const float invariant_mass);
//     void fill(xAOD::Muon::Quality qual_plus, xAOD::Muon::Quality qual_minus);
//     void fill(xAOD::Muon::Author auth_plus, xAOD::Muon::Author auth_minus);

//     TH1* m_Mass;
//     TH1* m_N_quality;
//     TH1* m_N_author;
//     TH1* m_occupancy;

//     TH2* m_Efficiency;
//     TH2* m_EffNumerator;
//     TH2* m_EffDenominator;
    
//     TH1* m_Efficiency_eta;
//     TH1* m_EffNumerator_eta;
//     TH1* m_EffDenominator_eta;
    
//     TH1* m_M_Mean;
//     TH1* m_M_Sigma;
//     TH1* m_N_Lumi;

//     TH1* m_M_EA_EA;
//     TH1* m_M_EA_BA;
//     TH1* m_M_EA_BC;
//     TH1* m_M_EA_EC;

//     TH1* m_M_BA_EA;
//     TH1* m_M_BA_BA;
//     TH1* m_M_BA_BC;
//     TH1* m_M_BA_EC;

//     TH1* m_M_BC_EA;
//     TH1* m_M_BC_BA;
//     TH1* m_M_BC_BC;
//     TH1* m_M_BC_EC;

//     TH1* m_M_EC_EA;
//     TH1* m_M_EC_BA;
//     TH1* m_M_EC_BC;
//     TH1* m_M_EC_EC;

//     std::string type;

//   private:
//     void SetResultsBin(int iBin, TH1* pInputHist);
//     void initializePlots();
//     void finalizePlots();

// };

#endif
