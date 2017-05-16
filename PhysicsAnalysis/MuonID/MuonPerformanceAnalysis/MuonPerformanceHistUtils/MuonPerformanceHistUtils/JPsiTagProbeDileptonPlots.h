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
  TH1* m_opening_angle;
  TH1* m_jpsi_pt;

  TH3* m_pem_num;
  TH3* m_pem_num_etaq;

  TH3* m_fem_num; 
  TH3* m_fem_num_etaq;

  TH3* m_pfm_num; 

  long double m_pi = TMath::Pi();

};

#endif      // JPSITAGPROBEDILEPTONPLOTS_H
