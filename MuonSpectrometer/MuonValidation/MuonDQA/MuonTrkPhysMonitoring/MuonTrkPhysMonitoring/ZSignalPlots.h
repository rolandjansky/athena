/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_ZSIGNALPLOTS_H
#define MUONTRKPHYSMONITORING_ZSIGNALPLOTS_H

#include "PlotBase.h"

class ZSignalPlots: public PlotBase {
  public:
    ZSignalPlots(PlotBase *pParent):PlotBase(pParent),m_Z_Mass(0),m_Z_occupancy(0),m_Z_Efficiency(0),m_Z_EffNumerator(0),
                                    m_Z_EffDenominator(0),m_Z_Efficiency_eta(0),m_Z_EffNumerator_eta(0),m_Z_EffDenominator_eta(0),
                                    m_M_Z_Mean(0),m_M_Z_Sigma(0),m_N_Z_Lumi(0),m_M_Z_EA_EA(0),m_M_Z_EA_BA(0),m_M_Z_EA_BC(0),
                                    m_M_Z_EA_EC(0),m_M_Z_BA_EA(0),m_M_Z_BA_BA(0),m_M_Z_BA_BC(0),m_M_Z_BA_EC(0),
                                    m_M_Z_BC_EA(0),m_M_Z_BC_BA(0),m_M_Z_BC_BC(0),m_M_Z_BC_EC(0),m_M_Z_EC_EA(0),
                                    m_M_Z_EC_BA(0),m_M_Z_EC_BC(0),m_M_Z_EC_EC(0){;}

    void fill(const float fEta1, const float fEta2, const float IVmass_record);

    TH1F* m_Z_Mass;
    TH1F* m_Z_occupancy;
	
    TH2F* m_Z_Efficiency;
    TH2F* m_Z_EffNumerator;
    TH2F* m_Z_EffDenominator;
    
    TH1F* m_Z_Efficiency_eta;
    TH1F* m_Z_EffNumerator_eta;
    TH1F* m_Z_EffDenominator_eta;
    
    TH1F* m_M_Z_Mean;
    TH1F* m_M_Z_Sigma;
    TH1F* m_N_Z_Lumi;
	
    TH1F* m_M_Z_EA_EA;
    TH1F* m_M_Z_EA_BA;
    TH1F* m_M_Z_EA_BC;
    TH1F* m_M_Z_EA_EC;
	
    TH1F* m_M_Z_BA_EA;
    TH1F* m_M_Z_BA_BA;
    TH1F* m_M_Z_BA_BC;
    TH1F* m_M_Z_BA_EC;
	
    TH1F* m_M_Z_BC_EA;
    TH1F* m_M_Z_BC_BA;
    TH1F* m_M_Z_BC_BC;
    TH1F* m_M_Z_BC_EC;
	
    TH1F* m_M_Z_EC_EA;
    TH1F* m_M_Z_EC_BA;
    TH1F* m_M_Z_EC_BC;
    TH1F* m_M_Z_EC_EC;
		    	
  private:
    void SetResultsBin(int iBin, TH1F* pInputHist);
    void initializePlots();
    void finalizePlots();

};

#endif





