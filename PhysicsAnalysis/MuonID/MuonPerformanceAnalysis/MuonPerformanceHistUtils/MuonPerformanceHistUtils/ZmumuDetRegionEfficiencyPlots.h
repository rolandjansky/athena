/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZMUMUDETREGIONTPEFFICIENCYPLOTS_H
#define ZMUMUDETREGIONTPEFFICIENCYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"

#include "MuonPerformanceHistUtils/Probe.h"
#include "MuonEfficiencyCorrections/DetRegionBinning.h"

///  This class provides the fine eta-phi binned efficiencies that are used for the reconstruction efficiency scale factors.

class ZmumuDetRegionEfficiencyPlots: public MuonTPEfficiencyPlotBase {
 public:
	ZmumuDetRegionEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF=false);


  void initializePlots();
  void fill(Probe& probe);

 private:

  bool m_isMatched;
  TH1* m_detregions;
  TH1* m_detregions_A;
  TH1* m_detregions_C;
  DetRegionBinning m_etaphi;

};

#endif		// ZMUMUDETREGIONTPEFFICIENCYPLOTS_H
