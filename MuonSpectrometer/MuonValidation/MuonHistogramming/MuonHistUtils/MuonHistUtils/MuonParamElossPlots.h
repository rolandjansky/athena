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
  
  TH1* msInnerMatchChi2;
  TH1* msOuterMatchChi2;

  TH1* ELoss;
  TH1* ELossDiffTruth;
  TH1* ELossDiffTruthEta0_1p35;
  TH1* ELossDiffTruthEta1p35_1p55;
  TH1* ELossDiffTruthEta1p55_end;
  TH1* ELossSigma;
  TH1* paramELoss;
  TH1* paramELossDiff;
  TH1* paramELossDiffTruth;
  TH1* paramELossDiffTruthEta0_1p35;
  TH1* paramELossDiffTruthEta1p35_1p55;
  TH1* paramELossDiffTruthEta1p55_end;
  TH1* measELoss; 
  TH1* measELossDiff; 
  TH1* measELossDiffTruth; 
  TH1* measELossDiffTruthEta0_1p35;
  TH1* measELossDiffTruthEta1p35_1p55;
  TH1* measELossDiffTruthEta1p55_end;
  TH1* ELossType;
  TH1* ELossTypeParametrPt;
  TH1* ELossTypeNotIsoPt;
  TH1* ELossTypeTailPt;
  TH1* ELossTypeAllPt;
  
 private:
  void initializePlots();
  void FillPlot(TH1* hist, const xAOD::Muon& mu,const xAOD::Muon::ParamDef paramDef,float scale=1, float weight=1.0);
  void FillPlot(TH1* hist, TH1* hist_DiffTruth, TH1* hist_DiffTruthEta0_1p35, TH1* hist_DiffTruthEta1p35_1p55, TH1* hist_DiffTruthEta1p55_endd, const xAOD::TruthParticle& truthMu, const xAOD::Muon& mu, const xAOD::Muon::ParamDef paramDef,float scale=1, float weight=1.0);
  void FillPlotELossType(TH1* hist, const xAOD::Muon& mu,float scale=1, float weight=1.0);
  void FillPlotELossType(TH1* hist, const xAOD::Muon& mu, xAOD::Muon::EnergyLossType type, float scale=1,float weight=1.0);
};

}

#endif
