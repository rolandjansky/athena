/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEVALIDATION_RECONELECTRONSPLOTS_H
#define ZEEVALIDATION_RECONELECTRONSPLOTS_H
	
#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

namespace ZeeValidation{
  
  class ReconElectronsPlots:public PlotBase {
  public:
    ReconElectronsPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);

    void fill(const xAOD::Electron* electron, int level);
    void fillinAcc(const xAOD::Electron* electron, int level);
    void fill(const xAOD::EventInfo* eventInfo, const xAOD::VertexContainer* vertices);
    
    void fillShowerShape(const xAOD::Electron* electron);
    void fillTrackCaloMatch(const xAOD::Electron* electron, const xAOD::TrackParticle* track);
    void fillIsolation(const xAOD::Electron* electron);
    void fillHitInfo(const xAOD::Electron* electron);

    void makeEfficiencyPlot(TH1* hDenom, TH1* hNom, TProfile* hEff);
    
    //nLevels = 5 (LHLoose, LHMedium, LHTight, Loose, Medium, Tight, OQ, Reco)
    static const int nLevels = 8;
    static const std::string cLevelLabel[nLevels];

    // Reco only information
    std::string m_sParticleType;
    
    TH1* h_electron_n;
    TH1* h_photon_n;
    TH1* h_electron_author;
    TH1* h_nvtx;
    TH1* h_mu;
    TH1* h_bcid;
    TH1* h_track_n;
    TH1* h_pv_x;
    TH1* h_pv_y;
    TH1* h_pv_z;

    TH1* h_electron_pt[nLevels];
    TH1* h_electron_eta[nLevels];
    TH1* h_electron_phi[nLevels];

    TProfile* h_electron_eff_pt[nLevels-1];
    TProfile* h_electron_eff_eta[nLevels-1];
    TProfile* h_electron_eff_phi[nLevels-1];
    
    TH1* h_f1;
    TH1* h_f3;
    TH1* h_f1core;
    TH1* h_f3core;
    TH1* h_e233;
    TH1* h_e237;
    TH1* h_e277;
    TH1* h_reta;
    TH1* h_rphi; 
    TH1* h_weta1;
    TH1* h_weta2;
    TH1* h_wtots1;
    TH1* h_ethad;
    TH1* h_ethad1;
    TH1* h_fethad;
    TH1* h_fethad1;
    TH1* h_e2tsts1;
    TH1* h_fracs1;
    TH1* h_emins1;
    TH1* h_emaxs1;
    TH1* h_demm1;
    TH1* h_iso;
    TH1* h_eratio;

    TH1* h_deta1;
    TH1* h_deta2;
    TH1* h_dphi1;
    TH1* h_dphi2;    
    TH1* h_dphires2;   
           
    TH1* h_etcone20;
    TH1* h_etcone30;
    TH1* h_etcone40;
    TH1* h_fetcone20;
    TH1* h_fetcone30;
    TH1* h_fetcone40;
    TH1* h_ptcone20;
    TH1* h_ptcone30;
    TH1* h_ptcone40;

    TH1* h_n_blayer_hits;
    TH1* h_n_pixel_hits;
    TH1* h_n_si_hits;
    TH1* h_n_trt_hits;
    TH1* h_n_trt_hits_high;
    TH1* h_r_trt_hits;
 
    TH1* h_n_blayer_hits_outliers;
    TH1* h_n_pixel_hits_outliers;
    TH1* h_n_si_hits_outliers;
    TH1* h_n_trt_hits_outliers;
    TH1* h_n_trt_hits_high_outliers;
    TH1* h_r_trt_hits_outliers;
 
    TH1* h_d0;
    TH1* h_sigmad0;
    TH1* h_eoverp;
    TH1* h_eoverpq;

  private:
        
    virtual void initializePlots();
    virtual void finalizePlots();
    
  };
}

#endif
