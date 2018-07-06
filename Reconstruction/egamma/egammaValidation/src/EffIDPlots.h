/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_EFFIDPLOTS_H
#define EGAMMAVALIDATION_EFFIDPLOTS_H

#include "TH1.h"
#include "TMath.h"

#include <TROOT.h>
#include <TStyle.h>

namespace egammaMonitoring{
  
  class EffIDPlots{
  public:
    
    EffIDPlots();
    
    // Efficiency Histos
    
    TH1D *LLH_pT_Eff = nullptr; //!
    TH1D *MLH_pT_Eff = nullptr; //!
    TH1D *TLH_pT_Eff = nullptr; //!

    TH1D *LLH_eta_Eff = nullptr; //!
    TH1D *MLH_eta_Eff = nullptr; //!
    TH1D *TLH_eta_Eff = nullptr; //!

    TH1D *LLH_phi_Eff = nullptr; //!
    TH1D *MLH_phi_Eff = nullptr; //!
    TH1D *TLH_phi_Eff = nullptr; //!
    
    void initializePlots();
    void divide(const char* EffPlot, TH1* h1, TH1* h2);
      
  private:
    
  };
  
}

#endif


