/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRIGTPEFFICIENCYPLOTS_H
#define MUONTRIGTPEFFICIENCYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"
#include "MuonPerformanceHistUtils/Probe.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"
#include "TGraphAsymmErrors.h"
#include "TH3.h"

class MuonTrigTPEfficiencyPlots: public MuonTPEfficiencyPlotBase
{
 public:
    
    ///  use this to store the efficiency result for a single calculation. Storage scheme: < Eff,  < Err_up,  Err_down> >
    typedef std::pair < double,  std::pair <double,  double> > effResult; 

    MuonTrigTPEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched, bool ApplySF=false);

    void fill(Probe& probe);
    
    float GetTrigThreshold_L1(std::string SomeString);
    float GetTrigThreshold_HLT(std::string SomeString);

 private:

    bool m_isMatched;

    TH1* pt;
    TH1* eta;
    TH1* phi;

    TH2* eta_phi;

    TH1* dRL1;
    TH1* dRHLT;
    TH1* mll;

};

#endif		// MUONTRIGTPEFFICIENCYPLOTS_H
