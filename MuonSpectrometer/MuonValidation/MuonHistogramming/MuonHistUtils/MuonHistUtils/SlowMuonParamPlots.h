/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_SLOWMUONPARAMPLOTS_H
#define MUONHISTUTILS_SLOWMUONPARAMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODMuon/SlowMuon.h"

namespace Muon{

class BetaPlots: public PlotBase {
 public:
  BetaPlots(PlotBase *pParent, std::string sDir, std::string sDetType);

  void fill(const xAOD::SlowMuon& smu);
  
  TH1* nHits;
  TH1* avg;
  TH1* chi2;
  TH1* ndf;
  TH1* chi2ndf;
  TH1* chi2prob;
  TH1* rms;

 private:
  std::string m_sDetType;
  void initializePlots();

};
  
class SlowMuonParamPlots: public PlotBase {
  public:
    SlowMuonParamPlots(PlotBase *pParent, std::string sDir);

    void fill(const xAOD::SlowMuon& smu);

    TH1* beta;
    BetaPlots mdtBeta;
    BetaPlots rpcBeta;
    BetaPlots caloBeta;
  
  private:
    void initializePlots();

};

}

#endif
