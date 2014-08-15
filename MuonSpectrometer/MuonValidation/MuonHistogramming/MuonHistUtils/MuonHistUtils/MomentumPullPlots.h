/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MOMENTUMPULLPLOTS_H
#define MUONHISTUTILS_MOMENTUMPULLPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"

namespace Muon{

class MomentumPullPlots: public PlotBase {
  public:
    MomentumPullPlots(PlotBase *pParent, std::string sDir):PlotBase(pParent, sDir){;}
    void fill(const xAOD::Muon& mu);

    TH1* dpt_idme;
    TH1* ddpt_idme;
    TH1* dphi_idme;
    TH1* deta_idme;
    
    TH1* dpt_cbme;
    TH1* ddpt_cbme;
    TH1* dphi_cbme;
    TH1* deta_cbme;

  private:
    void initializePlots();

};

}

#endif

