/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIMONITORINGELECTRONSTOOL_H
#define HIMONITORINGELECTRONSTOOL_H

#include <vector>
#include <string>

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TRandom3.h"

#include <xAODHIEvent/HIEventShape.h>
#include <xAODHIEvent/HIEventShapeContainer.h>

#include <xAODEgamma/Electron.h>
#include <xAODEgamma/ElectronContainer.h>

#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"


class TH1D;
class TH2D_LW;
class TProfile_LW;

class HIMonitoringElectronsTool : public ManagedMonitorToolBase
{
   public:

      HIMonitoringElectronsTool( const std::string & type, const std::string & name,
            const IInterface* parent ); 

      virtual ~HIMonitoringElectronsTool();

      virtual StatusCode bookHistogramsRecurrent();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();

      
	  void book_hist();
	  
	  
   private:

	  AsgElectronLikelihoodTool* m_electronLooseLHTool;
	  AsgElectronLikelihoodTool* m_electronMediumLHTool;
	  AsgElectronLikelihoodTool* m_electronTightLHTool;
      
	  double m_FCalEt;
	  double m_FCalEt_A;
	  double m_FCalEt_C;
	   
	  /// histograms 
	  TH1D* h_FCalEt_looseLH;
	  TH1D* h_FCalEt_mediumLH;
	  TH1D* h_FCalEt_tightLH;

	  TH1D* h_FCalEt_A_looseLH;
	  TH1D* h_FCalEt_A_mediumLH;
	  TH1D* h_FCalEt_A_tightLH;

	  TH1D* h_FCalEt_C_looseLH;
	  TH1D* h_FCalEt_C_mediumLH;
	  TH1D* h_FCalEt_C_tightLH;

	  TH2D_LW* h_electron_eta_phi;
	  TH1D* h_electron_z0sintheta;

	  //etcone20
	  TH2D_LW* h_electron_fcal_etcone20_looseLH;
	  TH2D_LW* h_electron_fcal_etcone20_mediumLH;
	  TH2D_LW* h_electron_fcal_etcone20_tightLH;

	  TH2D_LW* h_electron_fcalA_etcone20_looseLH;
	  TH2D_LW* h_electron_fcalA_etcone20_mediumLH;
	  TH2D_LW* h_electron_fcalA_etcone20_tightLH;

	  TH2D_LW* h_electron_fcalC_etcone20_looseLH;
	  TH2D_LW* h_electron_fcalC_etcone20_mediumLH;
	  TH2D_LW* h_electron_fcalC_etcone20_tightLH;

	  //R_eta
	  TH2D_LW* h_electron_fcal_reta_looseLH;
	  TH2D_LW* h_electron_fcal_reta_mediumLH;
	  TH2D_LW* h_electron_fcal_reta_tightLH;

	  TH2D_LW* h_electron_fcalA_reta_looseLH;
	  TH2D_LW* h_electron_fcalA_reta_mediumLH;
	  TH2D_LW* h_electron_fcalA_reta_tightLH;

	  TH2D_LW* h_electron_fcalC_reta_looseLH;
	  TH2D_LW* h_electron_fcalC_reta_mediumLH;
	  TH2D_LW* h_electron_fcalC_reta_tightLH;

	  //R_phi
	  TH2D_LW* h_electron_fcal_rphi_looseLH;
	  TH2D_LW* h_electron_fcal_rphi_mediumLH;
	  TH2D_LW* h_electron_fcal_rphi_tightLH;

	  TH2D_LW* h_electron_fcalA_rphi_looseLH;
	  TH2D_LW* h_electron_fcalA_rphi_mediumLH;
	  TH2D_LW* h_electron_fcalA_rphi_tightLH;

	  TH2D_LW* h_electron_fcalC_rphi_looseLH;
	  TH2D_LW* h_electron_fcalC_rphi_mediumLH;
	  TH2D_LW* h_electron_fcalC_rphi_tightLH;

	  //W_eta2
	  TH2D_LW* h_electron_fcal_weta2_looseLH;
	  TH2D_LW* h_electron_fcal_weta2_mediumLH;
	  TH2D_LW* h_electron_fcal_weta2_tightLH;

	  TH2D_LW* h_electron_fcalA_weta2_looseLH;
	  TH2D_LW* h_electron_fcalA_weta2_mediumLH;
	  TH2D_LW* h_electron_fcalA_weta2_tightLH;

	  TH2D_LW* h_electron_fcalC_weta2_looseLH;
	  TH2D_LW* h_electron_fcalC_weta2_mediumLH;
	  TH2D_LW* h_electron_fcalC_weta2_tightLH;

	  //R_had
	  TH2D_LW* h_electron_fcal_rhad_looseLH;
	  TH2D_LW* h_electron_fcal_rhad_mediumLH;
	  TH2D_LW* h_electron_fcal_rhad_tightLH;

	  TH2D_LW* h_electron_fcalA_rhad_looseLH;
	  TH2D_LW* h_electron_fcalA_rhad_mediumLH;
	  TH2D_LW* h_electron_fcalA_rhad_tightLH;

	  TH2D_LW* h_electron_fcalC_rhad_looseLH;
	  TH2D_LW* h_electron_fcalC_rhad_mediumLH;
	  TH2D_LW* h_electron_fcalC_rhad_tightLH;

	  //E_ratio
	  TH2D_LW* h_electron_fcal_eratio_looseLH;
	  TH2D_LW* h_electron_fcal_eratio_mediumLH;
	  TH2D_LW* h_electron_fcal_eratio_tightLH;

	  TH2D_LW* h_electron_fcalA_eratio_looseLH;
	  TH2D_LW* h_electron_fcalA_eratio_mediumLH;
	  TH2D_LW* h_electron_fcalA_eratio_tightLH;

	  TH2D_LW* h_electron_fcalC_eratio_looseLH;
	  TH2D_LW* h_electron_fcalC_eratio_mediumLH;
	  TH2D_LW* h_electron_fcalC_eratio_tightLH;

	  //f_1
	  TH2D_LW* h_electron_fcal_f1_looseLH;
	  TH2D_LW* h_electron_fcal_f1_mediumLH;
	  TH2D_LW* h_electron_fcal_f1_tightLH;

	  TH2D_LW* h_electron_fcalA_f1_looseLH;
	  TH2D_LW* h_electron_fcalA_f1_mediumLH;
	  TH2D_LW* h_electron_fcalA_f1_tightLH;

	  TH2D_LW* h_electron_fcalC_f1_looseLH;
	  TH2D_LW* h_electron_fcalC_f1_mediumLH;
	  TH2D_LW* h_electron_fcalC_f1_tightLH;

	  //f_3
	  TH2D_LW* h_electron_fcal_f3_looseLH;
	  TH2D_LW* h_electron_fcal_f3_mediumLH;
	  TH2D_LW* h_electron_fcal_f3_tightLH;

	  TH2D_LW* h_electron_fcalA_f3_looseLH;
	  TH2D_LW* h_electron_fcalA_f3_mediumLH;
	  TH2D_LW* h_electron_fcalA_f3_tightLH;

	  TH2D_LW* h_electron_fcalC_f3_looseLH;
	  TH2D_LW* h_electron_fcalC_f3_mediumLH;
	  TH2D_LW* h_electron_fcalC_f3_tightLH;

	  //delta_eta1
	  TH2D_LW* h_electron_fcal_deltaeta1_looseLH;
	  TH2D_LW* h_electron_fcal_deltaeta1_mediumLH;
	  TH2D_LW* h_electron_fcal_deltaeta1_tightLH;

	  TH2D_LW* h_electron_fcalA_deltaeta1_looseLH;
	  TH2D_LW* h_electron_fcalA_deltaeta1_mediumLH;
	  TH2D_LW* h_electron_fcalA_deltaeta1_tightLH;

	  TH2D_LW* h_electron_fcalC_deltaeta1_looseLH;
	  TH2D_LW* h_electron_fcalC_deltaeta1_mediumLH;
	  TH2D_LW* h_electron_fcalC_deltaeta1_tightLH;

	  //delta_phi_rescaled
	  TH2D_LW* h_electron_fcal_deltaphires_looseLH;
	  TH2D_LW* h_electron_fcal_deltaphires_mediumLH;
	  TH2D_LW* h_electron_fcal_deltaphires_tightLH;

	  TH2D_LW* h_electron_fcalA_deltaphires_looseLH;
	  TH2D_LW* h_electron_fcalA_deltaphires_mediumLH;
	  TH2D_LW* h_electron_fcalA_deltaphires_tightLH;

	  TH2D_LW* h_electron_fcalC_deltaphires_looseLH;
	  TH2D_LW* h_electron_fcalC_deltaphires_mediumLH;
	  TH2D_LW* h_electron_fcalC_deltaphires_tightLH;

	  //e_probability_HT
	  TH2D_LW* h_electron_fcal_eprobabilityht_looseLH;
	  TH2D_LW* h_electron_fcal_eprobabilityht_mediumLH;
	  TH2D_LW* h_electron_fcal_eprobabilityht_tightLH;

	  TH2D_LW* h_electron_fcalA_eprobabilityht_looseLH;
	  TH2D_LW* h_electron_fcalA_eprobabilityht_mediumLH;
	  TH2D_LW* h_electron_fcalA_eprobabilityht_tightLH;

	  TH2D_LW* h_electron_fcalC_eprobabilityht_looseLH;
	  TH2D_LW* h_electron_fcalC_eprobabilityht_mediumLH;
	  TH2D_LW* h_electron_fcalC_eprobabilityht_tightLH;
	  
	  /// histograms ranges and binning 
	  int m_FCalEt_nbins; 
	  double m_low_FCalEt; 
	  double m_high_FCalEt; 

	  int m_FCalEt_oneSide_nbins; 
	  double m_low_FCalEt_oneSide; 
	  double m_high_FCalEt_oneSide;

	  int m_eta_nbins; 
	  double m_low_eta; 
	  double m_high_eta; 

	  int m_phi_nbins; 
	  double m_low_phi; 
	  double m_high_phi; 

	  int m_z0sintheta_nbins; 
	  double m_low_z0sintheta; 
	  double m_high_z0sintheta;

	  int m_iso_nbins; 
	  double m_low_iso; 
	  double m_high_iso; 

	  int m_reta_nbins; 
	  double m_low_reta; 
	  double m_high_reta; 

	  int m_rphi_nbins; 
	  double m_low_rphi; 
	  double m_high_rphi; 

	  int m_weta2_nbins; 
	  double m_low_weta2; 
	  double m_high_weta2;

	  int m_rhad_nbins; 
	  double m_low_rhad; 
	  double m_high_rhad; 

	  int m_eratio_nbins; 
	  double m_low_eratio; 
	  double m_high_eratio; 

	  int m_f1_nbins; 
	  double m_low_f1; 
	  double m_high_f1; 

	  int m_f3_nbins; 
	  double m_low_f3; 
	  double m_high_f3; 

	  int m_deltaeta1_nbins; 
	  double m_low_deltaeta1; 
	  double m_high_deltaeta1; 

	  int m_deltaphires_nbins; 
	  double m_low_deltaphires; 
	  double m_high_deltaphires;

	  int m_eprobabilityht_nbins; 
	  double m_low_eprobabilityht; 
	  double m_high_eprobabilityht; 
};

#endif
