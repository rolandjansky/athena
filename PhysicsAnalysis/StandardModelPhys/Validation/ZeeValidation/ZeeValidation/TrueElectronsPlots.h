/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEVALIDATION_TRUEELECTRONSPLOTS_H
#define ZEEVALIDATION_TRUEELECTRONSPLOTS_H
	
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticle.h"
#include "xAODEgamma/Electron.h"

namespace ZeeValidation{
  
  class TrueElectronsPlots:public PlotBase {
  public:
    TrueElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
    void fill(const xAOD::IParticle* part, int level);
    void fillinAcc(const xAOD::IParticle* part, int level);

    void fillResponse(const xAOD::IParticle* part, const xAOD::Electron* electron);  
    void fillResponseCluster(const xAOD::IParticle* part, const xAOD::CaloCluster* cluster);   

    void makeEfficiencyPlot(TH1* hDenom, TH1* hNom, TProfile* hEff);     

    // Reco only information
    std::string m_sParticleType;
    
   //nLevels = 10 (Loose, Medium, Tight, LHLoose, LHMedium, LHTight, OQ, MatchedEl, MatchedElPh, True)
    static const int nLevels = 10;
    static const std::string cLevelLabel[nLevels];

    TH1* h_electron_pt[nLevels];
    TH1* h_electron_eta[nLevels];
    TH1* h_electron_phi[nLevels]; 

    TProfile* h_electron_eff_pt[nLevels-1];
    TProfile* h_electron_eff_eta[nLevels-1];
    TProfile* h_electron_eff_phi[nLevels-1];  

    TProfile* h_e_response_vs_e;
    TProfile* h_e_cluster_response_vs_e;  
    TProfile* h_e_response_vs_eta;
    TProfile* h_e_cluster_response_vs_eta; 
    TProfile* h_e_response_vs_phi;
    TProfile* h_e_cluster_response_vs_phi; 

    TH1* h_dr_electron;
    TH1* h_dphi_electron;
    TH1* h_deta_electron;

    TH1* h_dr_photon;
   
  private:
 
    virtual void initializePlots();
    virtual void finalizePlots();

  };
}

#endif
