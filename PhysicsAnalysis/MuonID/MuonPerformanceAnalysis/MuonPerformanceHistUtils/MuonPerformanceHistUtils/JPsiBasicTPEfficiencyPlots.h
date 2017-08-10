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
  TH1* m_pt;
  TH1* m_eta;
  TH1* m_phi;
  TH1* m_d0;
  //   TH1* phi;
  TH1* m_fineEtaPhi;
  TH1* m_integrated;
  fineEtaPhiBinning m_fepb;

};

#endif		// JPSIBASICTPEFFICIENCYPLOTS_H
