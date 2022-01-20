/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONPARAMELOSSPLOTS_H
#define MUONHISTUTILS_MUONPARAMELOSSPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"
#include "xAODTruth/TruthParticle.h"
#include <string>

namespace Muon{

class MuonParamElossPlots: public PlotBase
{
 public:
  MuonParamElossPlots(PlotBase *pParent, const std::string& sDir);
  void fill(const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu, float weight=1.0);
  
  TH1* msInnerMatchChi2{nullptr};
  TH1* msOuterMatchChi2{nullptr};

  TH1* ELoss{nullptr};
  TH1* ELossDiffTruth{nullptr};
  TH1* ELossDiffTruthEta0_1p35{nullptr};
  TH1* ELossDiffTruthEta1p35_1p55{nullptr};
  TH1* ELossDiffTruthEta1p55_end{nullptr};
  TH1* ELossSigma{nullptr};
  TH1* paramELoss{nullptr};
  TH1* paramELossDiff{nullptr};
  TH1* paramELossDiffTruth{nullptr};
  TH1* paramELossDiffTruthEta0_1p35{nullptr};
  TH1* paramELossDiffTruthEta1p35_1p55{nullptr};
  TH1* paramELossDiffTruthEta1p55_end{nullptr};
  TH1* measELoss{nullptr}; 
  TH1* measELossDiff{nullptr}; 
  TH1* measELossDiffTruth{nullptr}; 
  TH1* measELossDiffTruthEta0_1p35{nullptr};
  TH1* measELossDiffTruthEta1p35_1p55{nullptr};
  TH1* measELossDiffTruthEta1p55_end{nullptr};
  TH1* ELossType{nullptr};
  TH1* ELossTypeParametrPt{nullptr};
  TH1* ELossTypeNotIsoPt{nullptr};
  TH1* ELossTypeTailPt{nullptr};
  TH1* ELossTypeAllPt{nullptr};
  
 private:
  void initializePlots();
  void FillPlot(TH1* hist, const xAOD::Muon& mu,const xAOD::Muon::ParamDef paramDef,float scale=1, float weight=1.0);
  void FillPlot(TH1* hist, TH1* hist_DiffTruth, TH1* hist_DiffTruthEta0_1p35, TH1* hist_DiffTruthEta1p35_1p55, TH1* hist_DiffTruthEta1p55_endd, const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu, const xAOD::Muon::ParamDef paramDef,float scale=1, float weight=1.0);
  void FillPlotELossType(TH1* hist, const xAOD::Muon& mu,float scale=1, float weight=1.0);
  void FillPlotELossType(TH1* hist, const xAOD::Muon& mu, xAOD::Muon::EnergyLossType type, float scale=1,float weight=1.0);
};

}

#endif
