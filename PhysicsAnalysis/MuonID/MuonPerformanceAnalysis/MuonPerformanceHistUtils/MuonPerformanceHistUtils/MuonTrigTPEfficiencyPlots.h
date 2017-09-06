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

    TH1* m_pt; //inclusive larger muon Pt
    TH1* m_eta;
    TH1* m_phi;
    TH2* m_eta_phi;


};

#endif		// MUONTRIGTPEFFICIENCYPLOTS_H
