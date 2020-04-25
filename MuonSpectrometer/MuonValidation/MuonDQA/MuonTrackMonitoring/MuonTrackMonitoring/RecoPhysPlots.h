/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_RECOPHYSPLOTS_H
#define MUONTRKPHYSMONITORING_RECOPHYSPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"


class RecoPhysPlots:public PlotBase {
  public:
    RecoPhysPlots(PlotBase *pParent, std::string sDir, std::string recObj);

    void fill(std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > mumucandidates);
    void fill(const float eta_mu_plus, const float eta_mu_minus, const float invariant_mass);
    
    float EtaRegionFine(double eta);
    float EtaRegionCoarse(double eta);
    bool  LabelLargeEtaRegions(TAxis * axis);
    bool  LabelCombinedEtaRegions(TAxis * axis);
    bool  LabelSectorAxis(TAxis * axis);

    TH1* m_Mass;
    TH1* m_occupancy;
    TH1* m_2occupancy;

    // TH2* m_Efficiency;
    // TH2* m_EffNumerator;
    // TH2* m_EffDenominator;
    
    // TH1* m_Efficiency_eta;
    // TH1* m_EffNumerator_eta;
    // TH1* m_EffDenominator_eta;
    
    TH1* m_M_Mean;
    TH1* m_M_Sigma;

    TH1* m_M_EA_EA;
    TH1* m_M_EA_BA;
    TH1* m_M_EA_BC;
    TH1* m_M_EA_EC;

    TH1* m_M_BA_EA;
    TH1* m_M_BA_BA;
    TH1* m_M_BA_BC;
    TH1* m_M_BA_EC;

    TH1* m_M_BC_EA;
    TH1* m_M_BC_BA;
    TH1* m_M_BC_BC;
    TH1* m_M_BC_EC;

    TH1* m_M_EC_EA;
    TH1* m_M_EC_BA;
    TH1* m_M_EC_BC;
    TH1* m_M_EC_EC;

    std::string type;

    void finalizeRecoPlots();
  private:
    void SetResultsBin(int iBin, TH1* pInputHist);
    void initializePlots();

};

#endif
