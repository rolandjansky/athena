/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZMUMUFINEETAPHITPEFFICIENCYPLOTS_H
#define ZMUMUFINEETAPHITPEFFICIENCYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"

#include "MuonPerformanceHistUtils/Probe.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"

///  This class provides the fine eta-phi binned efficiencies that are used for the reconstruction efficiency scale factors.

class ZmumuFineEtaPhiEfficiencyPlots: public MuonTPEfficiencyPlotBase {
 public:
  ZmumuFineEtaPhiEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF=false);

  void initializePlots();
  void fill(Probe& probe);

 private:

  bool m_isMatched;
  TH1* m_fineEtaPhi_posq;
  TH1* m_fineEtaPhi_negq;
  TH1* m_fineEtaPhi;
  fineEtaPhiBinning m_fepb;

};

#endif		// ZMUMUFINEETAPHITPEFFICIENCYPLOTS_H
