/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JPSITPEVENTCUTFLOWPLOTS_H
#define JPSITPEVENTCUTFLOWPLOTS_H

#include "MuonPerformanceHistUtils/MuonTPCutFlowBase.h"
#include "TrkValHistUtils/PlotBase.h"

class JPsiTPEventCutFlowPlots: public MuonTPCutFlowBase {
 public:
	JPsiTPEventCutFlowPlots(PlotBase* pParent, std::string sDir);

 private:

  TH1* m_tp_main_selection;
  TH1* m_tp_tag_selection;
  TH1* m_tp_probe_selection;

};

#endif		// JPSITPEVENTCUTFLOWPLOTS_H
