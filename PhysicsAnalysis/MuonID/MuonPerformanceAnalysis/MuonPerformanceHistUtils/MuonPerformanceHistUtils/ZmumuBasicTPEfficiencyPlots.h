/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZMUMUBASICTPEFFICIENCYPLOTS_H
#define ZMUMUBASICTPEFFICIENCYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"

#include "MuonPerformanceHistUtils/Probe.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"

class ZmumuBasicTPEfficiencyPlots: public MuonTPEfficiencyPlotBase {
 public:
  ZmumuBasicTPEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF=false);

  void initializePlots();
  void fill(Probe& probe);

 private:

  bool m_isMatched;
  TH1* m_pt;
  TH1* m_eta;
  TH1* m_phi;
  TH1* m_integrated;

};

#endif		// ZMUMUBASICTPEFFICIENCYPLOTS_H
