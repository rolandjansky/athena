/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONPARAMPLOTS_H
#define MUONHISTUTILS_MUONPARAMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"
#include <string>

namespace Muon{

class MuonParamPlots: public PlotBase
{
 public:
  MuonParamPlots(PlotBase *pParent, const std::string& sDir);
  void fill(const xAOD::Muon& mu, float weight=1.0);
  
  TH1* msInnerMatchChi2{nullptr};
  TH1* msOuterMatchChi2{nullptr};

  TH1* ELoss{nullptr};
  TH1* ELossSigma{nullptr};
  TH1* paramELoss{nullptr};
  TH1* measELoss{nullptr};

  
 private:
  void initializePlots();
  void FillPlot(TH1* hist, const xAOD::Muon& mu,const xAOD::Muon::ParamDef paramDef,float scale=1, float weight=1.0);
};

}

#endif
