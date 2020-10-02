/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTMETMONTOOL_H
#define HLTMETMONTOOL_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "MissingETEvent/MissingET.h"

#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

#include "TLorentzVector.h"

#include <string>
#include <vector>


class HLTMETMonTool: public IHLTMonTool {

public:
  
  HLTMETMonTool(const std::string & type, const std::string & name, const IInterface* parent);
  ~HLTMETMonTool();
  
  StatusCode init();
  StatusCode book();
  StatusCode fill();
  StatusCode proc();
  
private:
  
  StatusCode fillMETHist();
  
  /** methods to make booking easier */
  void addL1BasicHistograms();
  void addL1JetHistograms();
  void addL1ProfileHistograms(std::map<std::string, int> met_signatures_tolook);
  void addL1ProfileShifterHistograms();
  void addL1ProfileExpertHistograms();
  void addHLTBasicHistograms();
  void addHLTProfileHistograms(std::map<std::string, int> met_signatures_tolook);
  void addHLTStatusHistograms();
  void addHLTCompHistograms();
  void addOffMETHistograms();
  void addL1vsHLTHistograms();
  void addHLTvsOffHistograms();
  void addL1vsOffHistograms();
  void addElMuHistograms();
  void addClusterHistograms();
  // methods to make filling easier
  void fillL1BasicHistograms(float l1_mex,float l1_mex_log,float l1_mey,float l1_mey_log,float l1_met,float l1_met_log,float l1_sumet,float l1_sumet_log,float l1_phi,float saturated);
  void fillL1JetHistograms(float l1_jet_pt,float l1_jet_eta);
  void fillL1ProfileHistograms(float off_met,std::map<std::string, int> met_signatures_tolook);
  void fillHLTBasicHistograms(float hlt_ex,float hlt_ex_log,float hlt_ey,float hlt_ey_log,float hlt_ez,float hlt_ez_log,float hlt_met,float hlt_met_log,float hlt_sumet,float hlt_sumet_log,float hlt_sume,float hlt_sume_log,float hlt_phi,float hlt_eta);
  void fillHLTProfileHistograms(float off_met,std::map<std::string, int> met_signatures_tolook);
  void fillHLTProfileShifterHistograms(float off_met);
  void fillHLTProfileExpertHistograms(float off_met);
  void fillOffMETHistograms(float off_met,float off_ex,float off_ey,float off_sumet,float off_phi);
  void fillL1vsHLTHistograms(float l1_met,float l1_sumet,float l1_phi,float l1_mex,float l1_mey,float hlt_met,float hlt_sumet,float hlt_phi,float hlt_ex,float hlt_ey,float saturated);
  void fillHLTvsOffHistograms(float hlt_met,float hlt_ex,float hlt_ey,float hlt_sumet,float hlt_phi,float off_met,float off_ex,float off_ey,float off_sumet,float off_phi);
  void fillL1vsOffHistograms(float l1_met,float l1_mex,float l1_mey,float l1_sumet,float l1_phi,float off_met,float off_ex,float off_ey,float off_sumet,float off_phi,float saturated);
  
  void check_triggers(std::vector<std::string> &m_triggers,
                      std::map<std::string,int> &m_signatures);

  void trigger_decision();
  
  std::string get_trigger_level(std::string item);
  std::string get_trigger_algo(std::string item);

  void print_trigger_stats();
  
  // calculate phi1-phi2 keeping value between -pi and pi
  double signed_delta_phi(double ph11, double phi2);
  
  double signed_log(double, double);
  
private:
  
  double m_et_min, m_ec_min, m_sumet_min, m_sume_min;
  double m_phi_min, m_eta_min, m_det_min, m_eff_min, m_xs_min;
  double m_et_max, m_ec_max, m_sumet_max, m_sume_max;
  double m_phi_max, m_eta_max, m_det_max, m_eff_max, m_xs_max;
  int m_et_bins, m_ec_bins, m_sumet_bins, m_sume_bins;
  int m_phi_bins, m_eta_bins, m_det_bins, m_eff_bins, m_xs_bins;
  
  std::vector<std::string> m_primary_met;
  std::vector<std::string> m_monitoring_met_shifter;
  std::vector<std::string> m_monitoring_met_expert;
  std::vector<std::string> m_monitoring_alg;
  std::vector<std::string> m_monitoring_alg_shifter;
  std::vector<std::string> m_monitoring_alg_expert;
  std::vector<std::string> m_prescaled_met;
  
   
  std::vector<std::string> m_met_triggers_l1_expert, m_met_triggers_hlt_expert;
  std::vector<std::string> m_met_triggers_l1_shifter, m_met_triggers_hlt_shifter;
  std::map<std::string, int> m_l1_met_signatures_tolook_shifter;
  std::map<std::string, int> m_hlt_met_signatures_tolook_shifter;
  std::map<std::string, int> m_l1_met_signatures_tolook_expert;
  std::map<std::string, int> m_hlt_met_signatures_tolook_expert;
  
  bool m_print_met_trig_stats; 
  bool m_make_expert_histograms;
  
  std::string m_mon_path;
  std::string m_shifter_path;
  std::string m_expert_path;
  
  std::string m_lvl1_jet_roi_key;
  std::string m_lvl1_roi_key;
  std::string m_lvl1_rho_key;
  std::string m_lvl1_pufit_key;
  std::string m_lvl1_jwoj_key;
  std::string m_lvl1_noisecut_key;
  std::string m_lvl1_jnoisecut_key;
  std::string m_hlt_run3_met_key; 
  std::string m_hlt_cell_run3_met_key; 
  std::string m_hlt_mht_run3_met_key; 
  std::string m_hlt_topocl_run3_met_key; 
  std::string m_hlt_topocl_PUC_run3_met_key; 
  std::string m_hlt_trkmht_run3_met_key;
  std::string m_hlt_cell_met_key; 
  std::string m_hlt_mht_met_key; 
  std::string m_hlt_mhtem_met_key; 
  std::string m_hlt_trkmht_met_key; 
  std::string m_hlt_trkmhtFTK_met_key; 
  std::string m_hlt_trktc_met_key; 
  std::string m_hlt_trktcFTK_met_key; 
  std::string m_hlt_topocl_met_key; 
  std::string m_hlt_topocl_PS_met_key; 
  std::string m_hlt_topocl_PUC_met_key; 
  std::string m_hlt_FEB_met_key; 
  std::string m_hlt_Fex_met_key; 
  std::string m_off_met_key;
  std::string m_muon_key;
  std::string m_muon_run3_key;
  std::string m_muon_base_trigger;
  std::string m_electron_key;
  std::string m_electron_run3_key;
  std::string m_electron_base_trigger;
  std::string m_cluster_key;
  std::string m_cluster_run3_key;

  double m_muon_pt_thresh;
  double m_electron_pt_thresh;
  
  std::vector<std::string> m_compNames, m_bitNames;

  // for the efficiencies
  std::string m_denominator_trigger;
  std::string m_numerator_trigger[4];                                           

  // for MET significance
  float m_sigOffset;
  float m_sigSlope;
  float m_sigQuadr;
  float m_sigOffset_feb;
  float m_sigSlope_feb;
  float m_sigQuadr_feb;

};

#endif // HLTMETMONTOOL_H

