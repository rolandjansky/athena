/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEVALIDATION_TRUEFWDELECTRONSPLOTS_H
#define ZEEVALIDATION_TRUEFWDELECTRONSPLOTS_H
	
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticle.h"

namespace ZeeValidation{
  
  class TrueFwdElectronsPlots:public PlotBase {
  public:
    TrueFwdElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
    void fill(const xAOD::IParticle* part, int level);
    void finalize();

    // Reco only information
    std::string m_sParticleType;
    
   //nLevels = 5 (Loose, Tight, MatchedEl, Truth)
    static const int nLevels = 4;
    static const std::string cLevelLabel[nLevels];

    TH1* true_fwd_electron_pt[nLevels];
    TH1* true_fwd_electron_eta[nLevels];
    TH1* true_fwd_electron_phi[nLevels];   

    TH1* true_fwd_electron_eff_pt[nLevels-1];
    TH1* true_fwd_electron_eff_eta[nLevels-1];
    TH1* true_fwd_electron_eff_phi[nLevels-1]; 
    
  private:
    
    virtual void initializePlots();
    
  };
}

#endif
