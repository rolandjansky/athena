/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_ZSIGNALPLOTS_H
#define MUONTRKPHYSMONITORING_ZSIGNALPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

class ZSignalPlots:public PlotBase {
  public:
    ZSignalPlots(PlotBase *pParent, std::string sDir);

    void fill(const float eta_mu_plus, const float eta_mu_minus, const float invariant_mass);

    TH1* m_Z_Mass;
    TH1* m_Z_occupancy;
	
    TH2* m_Z_Efficiency;
    TH2* m_Z_EffNumerator;
    TH2* m_Z_EffDenominator;
    
    TH1* m_Z_Efficiency_eta;
    TH1* m_Z_EffNumerator_eta;
    TH1* m_Z_EffDenominator_eta;
    
    TH1* m_M_Z_Mean;
    TH1* m_M_Z_Sigma;
    TH1* m_N_Z_Lumi;
	
    TH1* m_M_Z_EA_EA;
    TH1* m_M_Z_EA_BA;
    TH1* m_M_Z_EA_BC;
    TH1* m_M_Z_EA_EC;
	
    TH1* m_M_Z_BA_EA;
    TH1* m_M_Z_BA_BA;
    TH1* m_M_Z_BA_BC;
    TH1* m_M_Z_BA_EC;
	
    TH1* m_M_Z_BC_EA;
    TH1* m_M_Z_BC_BA;
    TH1* m_M_Z_BC_BC;
    TH1* m_M_Z_BC_EC;
	
    TH1* m_M_Z_EC_EA;
    TH1* m_M_Z_EC_BA;
    TH1* m_M_Z_EC_BC;
    TH1* m_M_Z_EC_EC;
		    	
  private:
    void SetResultsBin(int iBin, TH1* pInputHist);
    void initializePlots();
    void finalizePlots();

};

#endif





