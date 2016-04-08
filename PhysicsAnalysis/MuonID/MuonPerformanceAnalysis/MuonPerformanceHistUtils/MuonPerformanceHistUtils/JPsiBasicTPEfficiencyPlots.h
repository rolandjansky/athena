/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JPSIBASICTPEFFICIENCYPLOTS_H
#define JPSIBASICTPEFFICIENCYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"

#include "MuonPerformanceHistUtils/Probe.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"

class JPsiBasicTPEfficiencyPlots: public MuonTPEfficiencyPlotBase {
 public:
  JPsiBasicTPEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF=false);


  void initializePlots();
  void fill(Probe& probe);

 private:

  bool m_isMatched;
  TH1* pt;
  TH1* eta;
  TH1* phi;
  TH1* d0;
  //   TH1* phi;
  TH1* fineEtaPhi;
  TH1* integrated;
  fineEtaPhiBinning m_fepb;

};

#endif		// JPSIBASICTPEFFICIENCYPLOTS_H
