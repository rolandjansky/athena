/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRKPHYSMONITORING_ONLINEPLOTS_H
#define MUONTRKPHYSMONITORING_ONLINEPLOTS_H

#include "PlotBase.h"

class OnlinePlots: public PlotBase {
  public:
    OnlinePlots(PlotBase *pParent):PlotBase(pParent){;}
    void fill(int region, float t0);

    TH1F* m_Good_t0;
    TH1F* m_Good_t0_EA;
    TH1F* m_Good_t0_BA;
    TH1F* m_Good_t0_BC;
    TH1F* m_Good_t0_EC;
    	
  private:
    void initializePlots();
    void finalizePlots();
    void CalculateResults(TH1F* hist, int iBin, std::string sLabel);
};

#endif





