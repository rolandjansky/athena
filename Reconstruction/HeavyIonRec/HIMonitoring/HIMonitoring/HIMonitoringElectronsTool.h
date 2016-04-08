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

#include "ElectronPhotonSelectorTools/AsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"


class TH1D_LW;
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

	  AsgElectronIsEMSelector* m_electronLooseIsEMSelector;
	  AsgElectronIsEMSelector* m_electronMediumIsEMSelector;

	  AsgElectronLikelihoodTool* m_electronLooseLHTool;
	  AsgElectronLikelihoodTool* m_electronMediumLHTool;
      
	  double m_FCalEt;
	   
	  /// histograms 
	  TH1D_LW* h_FCalEt_looseCB;
	  TH1D_LW* h_FCalEt_looseLH;
	  TH1D_LW* h_FCalEt_mediumCB;
	  TH1D_LW* h_FCalEt_mediumLH;

	  TH2D_LW* h_electron_eta_phi;
	  TH1D_LW* h_electron_z0sintheta;

	  TH2D_LW* h_electron_fcal_etcone20_looseCB;
	  TH2D_LW* h_electron_fcal_etcone20_looseLH;
	  TH2D_LW* h_electron_fcal_etcone20_mediumCB;
	  TH2D_LW* h_electron_fcal_etcone20_mediumLH;

	  TH2D_LW* h_electron_fcal_reta_looseCB;
	  TH2D_LW* h_electron_fcal_reta_looseLH;
	  TH2D_LW* h_electron_fcal_reta_mediumCB;
	  TH2D_LW* h_electron_fcal_reta_mediumLH;

	  TH2D_LW* h_electron_fcal_rphi_looseCB;
	  TH2D_LW* h_electron_fcal_rphi_looseLH;
	  TH2D_LW* h_electron_fcal_rphi_mediumCB;
	  TH2D_LW* h_electron_fcal_rphi_mediumLH;

	  TH2D_LW* h_electron_fcal_weta2_looseCB;
	  TH2D_LW* h_electron_fcal_weta2_looseLH;
	  TH2D_LW* h_electron_fcal_weta2_mediumCB;
	  TH2D_LW* h_electron_fcal_weta2_mediumLH;

	  TH2D_LW* h_electron_fcal_rhad_looseCB;
	  TH2D_LW* h_electron_fcal_rhad_looseLH;
	  TH2D_LW* h_electron_fcal_rhad_mediumCB;
	  TH2D_LW* h_electron_fcal_rhad_mediumLH;

	  TH2D_LW* h_electron_fcal_wtots1_looseCB;
	  TH2D_LW* h_electron_fcal_wtots1_looseLH;
	  TH2D_LW* h_electron_fcal_wtots1_mediumCB;
	  TH2D_LW* h_electron_fcal_wtots1_mediumLH;

	  TH2D_LW* h_electron_fcal_eratio_looseCB;
	  TH2D_LW* h_electron_fcal_eratio_looseLH;
	  TH2D_LW* h_electron_fcal_eratio_mediumCB;
	  TH2D_LW* h_electron_fcal_eratio_mediumLH;

	  TH2D_LW* h_electron_fcal_f1_looseCB;
	  TH2D_LW* h_electron_fcal_f1_looseLH;
	  TH2D_LW* h_electron_fcal_f1_mediumCB;
	  TH2D_LW* h_electron_fcal_f1_mediumLH;

	  TH2D_LW* h_electron_fcal_f3_looseCB;
	  TH2D_LW* h_electron_fcal_f3_looseLH;
	  TH2D_LW* h_electron_fcal_f3_mediumCB;
	  TH2D_LW* h_electron_fcal_f3_mediumLH;

	  TH2D_LW* h_electron_fcal_deltaeta1_looseCB;
	  TH2D_LW* h_electron_fcal_deltaeta1_looseLH;
	  TH2D_LW* h_electron_fcal_deltaeta1_mediumCB;
	  TH2D_LW* h_electron_fcal_deltaeta1_mediumLH;

	  TH2D_LW* h_electron_fcal_deltaphires_looseCB;
	  TH2D_LW* h_electron_fcal_deltaphires_looseLH;
	  TH2D_LW* h_electron_fcal_deltaphires_mediumCB;
	  TH2D_LW* h_electron_fcal_deltaphires_mediumLH;

	  TH2D_LW* h_electron_fcal_eprobabilityht_looseCB;
	  TH2D_LW* h_electron_fcal_eprobabilityht_looseLH;
	  TH2D_LW* h_electron_fcal_eprobabilityht_mediumCB;
	  TH2D_LW* h_electron_fcal_eprobabilityht_mediumLH;
	  
	  /// histograms ranges and binning 
	  int m_FCalEt_nbins; 
	  double m_low_FCalEt; 
	  double m_high_FCalEt; 

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

	  int m_wtots1_nbins; 
	  double m_low_wtots1; 
	  double m_high_wtots1;

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
