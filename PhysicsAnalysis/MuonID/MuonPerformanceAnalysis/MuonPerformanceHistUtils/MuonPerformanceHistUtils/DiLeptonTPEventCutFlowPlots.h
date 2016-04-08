/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZMUMUTPEVENTCUTFLOWPLOTS_H
#define ZMUMUTPEVENTCUTFLOWPLOTS_H

#include "MuonPerformanceHistUtils/MuonTPCutFlowBase.h"
#include "TrkValHistUtils/PlotBase.h"

class DiLeptonTPEventCutFlowPlots: public MuonTPCutFlowBase {
 public:
  DiLeptonTPEventCutFlowPlots(PlotBase* pParent, std::string sDir);
  void initializePlots();

 private:

  TH1* tp_main_selection;
  TH1* tp_tag_selection;
  TH1* tp_probe_selection;

};

#endif		// ZMUMUTPEVENTCUTFLOWPLOTS_H
