/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEVALIDATION_TRUEFWDELECTRONSPLOTS_H
#define ZEEVALIDATION_TRUEFWDELECTRONSPLOTS_H
	
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticle.h"
#include "xAODEgamma/Electron.h"

namespace ZeeValidation{
  
  class TrueFwdElectronsPlots:public PlotBase {
  public:
    TrueFwdElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
    void fill(const xAOD::IParticle* part, int level);

    void fillResponse(const xAOD::IParticle* part, const xAOD::Electron* electron);   
  
    void makeEfficiencyPlot(TH1* hDenom, TH1* hNom, TProfile* hEff);     

    std::string m_sParticleType;
    
    //nLevels = 4 (Loose, Tight, MatchedEl, Truth)
    static const int nLevels = 4;
    static const std::string cLevelLabel[nLevels];

    TH1* h_true_fwd_electron_pt[nLevels];
    TH1* h_true_fwd_electron_eta[nLevels];
    TH1* h_true_fwd_electron_abseta[nLevels];
    TH1* h_true_fwd_electron_phi[nLevels];   

    TProfile* h_true_fwd_electron_eff_pt[nLevels-1];
    TProfile* h_true_fwd_electron_eff_eta[nLevels-1];
    TProfile* h_true_fwd_electron_eff_abseta[nLevels-1];
    TProfile* h_true_fwd_electron_eff_phi[nLevels-1]; 

    TProfile* h_fwd_e_response_vs_e;
    TProfile* h_fwd_e_response_vs_eta;
    TProfile* h_fwd_e_response_vs_abseta;
    TProfile* h_fwd_e_response_vs_phi;

    TH1* h_fwd_dr_electron;
    TH1* h_fwd_dphi_electron;
    TH1* h_fwd_deta_electron;

  private:
    
    virtual void initializePlots();
    virtual void finalizePlots();
    
  };
}

#endif
