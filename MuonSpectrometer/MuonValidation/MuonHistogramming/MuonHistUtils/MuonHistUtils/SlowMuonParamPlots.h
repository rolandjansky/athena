/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_SLOWMUONPARAMPLOTS_H
#define MUONHISTUTILS_SLOWMUONPARAMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/SlowMuon.h"

namespace Muon{

class SlowMuonParamPlots: public PlotBase {
  public:
    SlowMuonParamPlots(PlotBase *pParent, std::string sDir);

    void fill(const xAOD::SlowMuon& smu);

    TH1* beta;
  
  private:
    void initializePlots();

};

}

#endif
