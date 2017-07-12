/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZMUMUTAGPROBEDILEPTONPLOTS_H
#define ZMUMUTAGPROBEDILEPTONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"

#include "MuonPerformanceHistUtils/Probe.h"

class ZmumuTagProbeDileptonPlots: public MuonTPEfficiencyPlotBase {
 public:
  ZmumuTagProbeDileptonPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF=false);

  void initializePlots();
  void fill(Probe& probe);

 private:

  bool m_isMatched;
  TH1* m_ll;
  TH1* m_opening_angle;
  TH1* m_z_pt;

};

#endif      // ZMUMUTAGPROBEDILEPTONPLOTS_H
