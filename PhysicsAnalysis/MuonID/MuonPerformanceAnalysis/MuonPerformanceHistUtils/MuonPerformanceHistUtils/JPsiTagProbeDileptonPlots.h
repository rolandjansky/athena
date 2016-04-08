/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JPSITAGPROBEDILEPTONPLOTS_H
#define JPSITAGPROBEDILEPTONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"

#include "MuonPerformanceHistUtils/Probe.h"

class JPsiTagProbeDileptonPlots: public MuonTPEfficiencyPlotBase {

 public:

  JPsiTagProbeDileptonPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF=false);

  void initializePlots();
  void fill(Probe& probe);

 private:

  bool m_isMatched;

  TH1* m_ll;
  TH1* opening_angle;
  TH1* jpsi_pt;

  TH3* pem_num;
  TH3* pem_num_etaq;

  TH3* fem_num; 
  TH3* fem_num_etaq;

  TH3* pfm_num; 

  long double pi = TMath::Pi();

};

#endif      // JPSITAGPROBEDILEPTONPLOTS_H
