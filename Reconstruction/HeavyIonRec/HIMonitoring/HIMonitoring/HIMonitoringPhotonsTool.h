/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIMONITORINGPHOTONSTOOL_H
#define HIMONITORINGPHOTONSTOOL_H

#include <vector>
#include <string>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TRandom3.h"

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>

#include <xAODEgamma/Photon.h>
#include <xAODEgamma/PhotonContainer.h>

#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

class TH1;
class TGraph;
class TTree;
class TH1D;
//class TH1D_LW;
class TH2D_LW;
class TProfile_LW;

class HIMonitoringPhotonsTool : public ManagedMonitorToolBase
{
   public:

      HIMonitoringPhotonsTool( const std::string & type, const std::string & name,
            const IInterface* parent ); 

      virtual ~HIMonitoringPhotonsTool();

      virtual StatusCode bookHistogramsRecurrent();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();
  	
	  void book_hist();
	  
	  
   private:


	  AsgPhotonIsEMSelector* m_photonTightIsEMSelector;
	  AsgPhotonIsEMSelector* m_photonLooseIsEMSelector;
	  
	  double m_FCalEt; 

	  int m_FCalEt_nbins;
	  float m_FCalEt_low;
	  float m_FCalEt_high;

	  int m_PhotonPt_nbins;
	  float m_PhotonPt_low;
	  float m_PhotonPt_high;

	  int m_PhotonEta_nbins;
	  float m_PhotonEta_low;
	  float m_PhotonEta_high;

	  int m_PhotonPhi_nbins;
	  float m_PhotonPhi_low;
	  float m_PhotonPhi_high;

	  int m_reta_nbins;
	  float m_reta_low;
	  float m_reta_high;

	  int m_rphi_nbins;
	  float m_rphi_low;
	  float m_rphi_high;

	  int m_weta2_nbins;
	  float m_weta2_low;
	  float m_weta2_high;

	  int m_rhad_nbins;
	  float m_rhad_low;
	  float m_rhad_high;

	  int m_wtots1_nbins;
	  float m_wtots1_low;
	  float m_wtots1_high;

	  int m_fracs1_nbins;
	  float m_fracs1_low;
	  float m_fracs1_high;

	  int m_deltae_nbins;
	  float m_deltae_low;
	  float m_deltae_high;

	  int m_eratio_nbins;
	  float m_eratio_low;
	  float m_eratio_high;

	  int m_f1_nbins;
	  float m_f1_low;
	  float m_f1_high;

	  int m_IsoWide_nbins;
	  float m_IsoWide_low;
	  float m_IsoWide_high;

	  int m_IsoNarrow_nbins;
	  float m_IsoNarrow_low;
	  float m_IsoNarrow_high;

	  /// histograms 
	  
	  // basics 
	  TH2D_LW* m_h_photon_pt_eta;
	  TH2D_LW* m_h_photon_pt_phi;
	  TH2D_LW* m_h_photon_pt_fcal;
	  TH2D_LW* m_h_photon_eta_phi_ptCut;

	  TH2D_LW* m_h_photon_pt_eta_tight;
	  TH2D_LW* m_h_photon_pt_phi_tight;
	  TH2D_LW* m_h_photon_pt_fcal_tight;
	  TH2D_LW* m_h_photon_eta_phi_ptCut_tight;

	  // shower shapes
	  
	  
	  TH1D* m_h_photon_reta_ptCut_all;
	  TH1D* m_h_photon_rphi_ptCut_all;
	  TH1D* m_h_photon_weta2_ptCut_all;

	  TH1D* m_h_photon_rhad_ptCut_all;
	  TH1D* m_h_photon_wtots1_ptCut_all;
	  TH1D* m_h_photon_fracs1_ptCut_all;

	  TH1D* m_h_photon_deltae_ptCut_all;
	  TH1D* m_h_photon_eratio_ptCut_all;
	  TH1D* m_h_photon_f1_ptCut_all;

	  //
	  TH1D* m_h_photon_reta_ptCut_loose;
	  TH1D* m_h_photon_rphi_ptCut_loose;
	  TH1D* m_h_photon_weta2_ptCut_loose;

	  TH1D* m_h_photon_rhad_ptCut_loose;
	  TH1D* m_h_photon_wtots1_ptCut_loose;
	  TH1D* m_h_photon_fracs1_ptCut_loose;

	  TH1D* m_h_photon_deltae_ptCut_loose;
	  TH1D* m_h_photon_eratio_ptCut_loose;
	  TH1D* m_h_photon_f1_ptCut_loose;

	  //
	  TH1D* m_h_photon_reta_ptCut_tight;
	  TH1D* m_h_photon_rphi_ptCut_tight;
	  TH1D* m_h_photon_weta2_ptCut_tight;

	  TH1D* m_h_photon_rhad_ptCut_tight;
	  TH1D* m_h_photon_wtots1_ptCut_tight;
	  TH1D* m_h_photon_fracs1_ptCut_tight;

	  TH1D* m_h_photon_deltae_ptCut_tight;
	  TH1D* m_h_photon_eratio_ptCut_tight;
	  TH1D* m_h_photon_f1_ptCut_tight;



	  TH2D_LW* m_h_photon_reta_eta_ptCut;
	  TH2D_LW* m_h_photon_rphi_eta_ptCut;
	  TH2D_LW* m_h_photon_weta2_eta_ptCut;
	  
	  TH2D_LW* m_h_photon_rhad_eta_ptCut;
	  TH2D_LW* m_h_photon_wtots1_eta_ptCut;
	  TH2D_LW* m_h_photon_fracs1_eta_ptCut;
	  
	  TH2D_LW* m_h_photon_deltae_eta_ptCut;
	  TH2D_LW* m_h_photon_eratio_eta_ptCut;
	  TH2D_LW* m_h_photon_f1_eta_ptCut;
	  
	  TH2D_LW* m_h_photon_reta_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_reta_fcal_ptCut_eta1;
	  TH2D_LW* m_h_photon_rphi_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_rphi_fcal_ptCut_eta1;
	  TH2D_LW* m_h_photon_weta2_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_weta2_fcal_ptCut_eta1;
	  TH2D_LW* m_h_photon_rhad_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_rhad_fcal_ptCut_eta1;
	  TH2D_LW* m_h_photon_wtots1_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_wtots1_fcal_ptCut_eta1;
	  TH2D_LW* m_h_photon_fracs1_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_fracs1_fcal_ptCut_eta1;
	  TH2D_LW* m_h_photon_deltae_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_deltae_fcal_ptCut_eta1;
	  TH2D_LW* m_h_photon_eratio_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_eratio_fcal_ptCut_eta1;
	  TH2D_LW* m_h_photon_f1_fcal_ptCut_eta0;
	  TH2D_LW* m_h_photon_f1_fcal_ptCut_eta1;

	  
	  // isolation
	  TH2D_LW* m_h_photon_fcal_etcone20_ptCut;
	  TH2D_LW* m_h_photon_fcal_etcone30_ptCut;
	  TH2D_LW* m_h_photon_fcal_etcone40_ptCut;
	  
	  TH2D_LW* m_h_photon_pt_etcone40_fcal0;
	  TH2D_LW* m_h_photon_pt_etcone40_fcal1;
	  TH2D_LW* m_h_photon_pt_etcone40_fcal2;
	  
	  TH1D* m_h_photon_etcone20_ptCut_fcal0;
	  TH1D* m_h_photon_etcone20_ptCut_fcal1;
	  TH1D* m_h_photon_etcone20_ptCut_fcal2;
	  
	  TH1D* m_h_photon_etcone30_ptCut_fcal0;
	  TH1D* m_h_photon_etcone30_ptCut_fcal1;
	  TH1D* m_h_photon_etcone30_ptCut_fcal2;
	  
	  TH1D* m_h_photon_etcone40_ptCut_fcal0;
	  TH1D* m_h_photon_etcone40_ptCut_fcal1;
	  TH1D* m_h_photon_etcone40_ptCut_fcal2;

	  TH1D* m_h_photon_etcone30_ptCut_fcal0_all;
	  TH1D* m_h_photon_etcone30_ptCut_fcal0_loose;
	  TH1D* m_h_photon_etcone30_ptCut_fcal0_tight;

	  TH1D* m_h_photon_etcone30_ptCut_fcal1_all;
	  TH1D* m_h_photon_etcone30_ptCut_fcal1_loose;
	  TH1D* m_h_photon_etcone30_ptCut_fcal1_tight;

	  TH1D* m_h_photon_etcone30_ptCut_fcal2_all;
	  TH1D* m_h_photon_etcone30_ptCut_fcal2_loose;
	  TH1D* m_h_photon_etcone30_ptCut_fcal2_tight;

};

#endif
