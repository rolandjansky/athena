/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEVALIDATION_TRUEELECTRONSPLOTS_H
#define ZEEVALIDATION_TRUEELECTRONSPLOTS_H
	
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticle.h"

namespace ZeeValidation{
  
  class TrueElectronsPlots:public PlotBase {
  public:
    TrueElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
    void fill(const xAOD::IParticle* part, int level);
    void fillinAcc(const xAOD::IParticle* part, int level);
    void finalize();
     
    // Reco only information
    std::string m_sParticleType;
    
   //nLevels = 5 (Loose, Medium, Tight, OQ, MatchedEl, MatchedElPh, True)
    static const int nLevels = 7;
    static const std::string cLevelLabel[nLevels];

    TH1* electron_pt[nLevels];
    TH1* electron_eta[nLevels];
    TH1* electron_phi[nLevels]; 
    TH2* electron_etavsphi[nLevels];

    TH1* electron_eff_pt[nLevels-1];
    TH1* electron_eff_eta[nLevels-1];
    TH1* electron_eff_phi[nLevels-1];  
    
  private:
 
    virtual void initializePlots();
    
  };
}

#endif
