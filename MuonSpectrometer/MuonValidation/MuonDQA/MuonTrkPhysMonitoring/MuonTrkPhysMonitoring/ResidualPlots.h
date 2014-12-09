/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_RESIDUALPLOTS_H
#define MUONTRKPHYSMONITORING_RESIDUALPLOTS_H

#include "PlotBase.h"

class ResidualPlots: public PlotBase {
  public:
    ResidualPlots(PlotBase *pParent, std::string sector):PlotBase(pParent),m_A_EE_Residuals(0),m_C_EE_Residuals(0),
                                                         m_A_BEE_Residuals(0),m_C_BEE_Residuals(0),m_A_CSC_Residuals(0),
                                                         m_C_CSC_Residuals(0),m_HR_Eta_I(0),m_HR_Eta_M(0),m_HR_Eta_O(0),m_sSector(sector){;}

    void fill(float eta, int imdtstationName, bool isMDT, bool isCSC, bool Fill_HR_Histograms, float residual);

    TH1F* m_A_EE_Residuals;
    TH1F* m_C_EE_Residuals;
    TH1F* m_A_BEE_Residuals;    
    TH1F* m_C_BEE_Residuals;
    TH1F* m_A_CSC_Residuals;
    TH1F* m_C_CSC_Residuals;
    TH2F* m_HR_Eta_I;
    TH2F* m_HR_Eta_M;
    TH2F* m_HR_Eta_O;
			
  private:
    void initializePlots();
    void finalizePlots(){;}
    std::string m_sSector; 

};

#endif





