/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HLTMETMONTOOL_H
#define HLTMETMONTOOL_H

#include "TrigHLTMonitoring/IHLTMonTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "MissingETEvent/MissingET.h"

#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"

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
  
  /** methods called by book() */
  //void bookExpertL1Histograms();
  //void bookExpertHLTHistograms();
  void bookExpertEfficHistograms();
  
  /** methods to make booking easier */
  void addHLTCompHistograms();
  void addHLTStatusHistogram();
  void addL1vsHLTHistograms();
  void addHLTvsOffHistograms();
  void addL1vsOffHistograms();
  
  StatusCode fillMETHist();
  
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
  
  double m_et_min, m_sumet_min, m_phi_min, m_det_min, m_eff_min;
  double m_et_max, m_sumet_max, m_phi_max, m_det_max, m_eff_max;
  int m_et_bins, m_sumet_bins, m_phi_bins, m_det_bins, m_eff_bins;
  
  bool m_debuglevel; //!< private member to control debug messages
  
  std::vector<std::string> m_primary_met;
  std::vector<std::string> m_monitoring_met_shifter;
  std::vector<std::string> m_monitoring_met_expert;
  std::vector<std::string> m_monitoring_alg;
  std::vector<std::string> m_prescaled_met;
  
  std::vector<std::string> m_met_triggers_l1_expert, m_met_triggers_hlt_expert;
  std::vector<std::string> m_met_triggers_l1_shifter, m_met_triggers_hlt_shifter;
  std::map<std::string, int> m_l1_met_signatures_tolook_shifter;
  std::map<std::string, int> m_hlt_met_signatures_tolook_shifter;
  std::map<std::string, int> m_l1_met_signatures_tolook_expert;
  std::map<std::string, int> m_hlt_met_signatures_tolook_expert;
  
  bool m_print_met_trig_stats; 
  bool m_make_expert_histograms;
  //bool m_doRecMET;
  
  std::string m_mon_path;
  std::string m_shifter_path;
  std::string m_expert_path;
  
  //bool m_is_do_trigger_effic;
  
  std::string m_lvl1_roi_key;
  std::string m_hlt_cell_met_key; 
  std::string m_hlt_mht_met_key; 
  std::string m_hlt_mhtem_met_key; 
  std::string m_hlt_trkmht_met_key; 
  std::string m_hlt_trkmhtFTK_met_key; 
  std::string m_hlt_topocl_met_key; 
  std::string m_hlt_topocl_PS_met_key; 
  std::string m_hlt_topocl_PUC_met_key; 
  std::string m_hlt_FEB_met_key; 
  std::string m_hlt_Fex_met_key; 
  std::string m_off_met_key;
  std::string m_muon_key;
  std::string m_muon_base_trigger;
  std::string m_electron_key;
  std::string m_electron_base_trigger;

  double m_muon_pt_thresh;
  double m_electron_pt_thresh;
  
  std::vector<std::string> m_compNames, m_bitNames;

  // for the efficiencies                                                                                                    
  std::string m_denominator_trigger;
  TH1F *h_denominator = 0;

  std::string m_numerator_trigger[4]; // = {"HLT_xe80", "HLT_xe100"};                                                        
  TH1F *trig_eff_num[4] = {0};

  // for MET significance
  float m_sigOffset;
  float m_sigSlope;
  float m_sigQuadr;
  float m_sigOffset_feb;
  float m_sigSlope_feb;
  float m_sigQuadr_feb;

};

#endif // HLTMETMONTOOL_H

