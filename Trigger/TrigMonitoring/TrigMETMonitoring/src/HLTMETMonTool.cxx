/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitoring/HLTMETMonTool.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>

#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

//___________________________________________________________________________________________________________
HLTMETMonTool::HLTMETMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : IHLTMonTool(type, name, parent),
    m_debuglevel(false) {

  ATH_MSG_DEBUG("in HLTMETMonTool::HLTMETMonTool()");

  declareProperty("MonPathBase", m_mon_path="/HLT/METMon");
  
  declareProperty("print_trigger_stats", m_print_met_trig_stats=true);
  declareProperty("make_expert_histograms", m_make_expert_histograms=true);

  // Triggers to monitor
  declareProperty("primary_met", m_primary_met);
  declareProperty("monitoring_met", m_monitoring_met);
  declareProperty("monitoring_alg", m_monitoring_alg);
  declareProperty("prescaled_met", m_prescaled_met);

  // Met keys
  declareProperty("l1_key", m_lvl1_roi_key="LVL1EnergySumRoI");
  declareProperty("hlt_key", m_hlt_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET");
  declareProperty("hlt_mht_key", m_hlt_mht_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht");
  declareProperty("hlt_topocl_key", m_hlt_topocl_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl");
  declareProperty("hlt_topocl_PS_key", m_hlt_topocl_PS_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS");
  declareProperty("hlt_topocl_PUC_key", m_hlt_topocl_PUC_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC");
  declareProperty("hlt_FEB_key", m_hlt_FEB_met_key="HLT_xAOD__TrigMissingETContainer_TrigL2MissingET_FEB");
  declareProperty("hlt_Fex_key", m_hlt_Fex_met_key="HLT_xAOD__TrigMissingETContainer_EFMissingET_Fex_2sidednoiseSupp_PUC");
  declareProperty("off_key", m_off_met_key="MET_Reference_AntiKt4LCTopo");  

  // Muons keys
  declareProperty("muon_key", m_muon_key="HLT_xAOD__MuonContainer_MuonEFInfo");
  declareProperty("muon_base", m_muon_base_trigger="HLT_mu8");
  declareProperty("muon_pt_thresh", m_muon_pt_thresh=18.0);

  // Electron keys
  declareProperty("electron_pt_thresh", m_electron_pt_thresh=20.0);

  declareProperty("comp_names", m_compNames);
  declareProperty("bit_names", m_bitNames);

  // histograms limits
  // et
  declareProperty("et_min",  m_et_min=-13.5, "Minimum Et");
  declareProperty("et_max",  m_et_max=601.5, "Maximum Et");
  declareProperty("et_bins", m_et_bins=205,  "Number of bins Et");
  // sumet
  declareProperty("sumet_min",  m_sumet_min=-27.,  "Minimum Sumet");
  declareProperty("sumet_max",  m_sumet_max=1803., "Maximum Sumet");
  declareProperty("sumet_bins", m_sumet_bins=305,  "Number of bins Sumet");
  // phi, dphi
  declareProperty("phi_min",  m_phi_min=-3.1416, "Minimum Phi");
  declareProperty("phi_max",  m_phi_max=3.1416,  "Maximum Phi");
  declareProperty("phi_bins", m_phi_bins=32,     "Number of bins Phi, dPhi");
  // dEt, dEx, dEy
  declareProperty("det_min",  m_det_min=-100., "Minimum dEt, dEx, dEy");
  declareProperty("det_max",  m_det_max=100.,  "Maximum dEt, dEx, dEy");
  declareProperty("det_bins", m_det_bins=100,  "Number of bins dEt, dEx, dEy");
  // efficiency
  declareProperty("eff_min",  m_eff_min=-13.5, "Minimum Eff");
  declareProperty("eff_max",  m_eff_max=406.5, "Maximum Eff");
  declareProperty("eff_bins", m_eff_bins=42,  "Number of bins Eff");
  // significance
  declareProperty("sig_offset", m_sigOffset=4.93182 , "Resolution offset");
  declareProperty("sig_slope",  m_sigSlope=0.442864 , "Resolution slope");
  declareProperty("sig_quadr",  m_sigQuadr=0.0 , "Resolution quad term");
  declareProperty("sig_offset_feb", m_sigOffset_feb=-1.886 , "Resolution offset");
  declareProperty("sig_slope_feb",  m_sigSlope_feb=1.15 , "Resolution slope");
  declareProperty("sig_quadr_feb",  m_sigQuadr_feb=0.0 , "Resolution quad term");
}


//___________________________________________________________________________________________________________
HLTMETMonTool::~HLTMETMonTool() {
  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool::~HLTMETMonTool()");
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::init() {

  // init message stream
  m_log->setLevel(outputLevel());
  m_debuglevel = (m_log->level() <= MSG::DEBUG);

  if (m_debuglevel) {
    ATH_MSG_DEBUG("in HLTMETMonTool::init()");
    ATH_MSG_DEBUG("HLTMETMonTool: L1Key  = "     << m_lvl1_roi_key);
    ATH_MSG_DEBUG("HLTMETMonTool: HLT_METKey = " << m_hlt_met_key);
    ATH_MSG_DEBUG("HLTMETMonTool: Off_METKey = " << m_off_met_key);
  }

  // put all trigger names into one arry
  for (std::vector<std::string>::iterator it = m_monitoring_met.begin(); it != m_monitoring_met.end(); ++it) {
    if (get_trigger_level(*it) == "L1")
      m_met_triggers_l1.push_back(*it);
    else if (get_trigger_level(*it) == "HLT")
      m_met_triggers_hlt.push_back(*it);
  }

  
  return StatusCode::SUCCESS;
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::book() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool::book()");

  // check if input met triggers are in trigger configuration and add them to signatures_to look vectors
  if (m_met_triggers_l1.size())
    check_triggers(m_met_triggers_l1, m_l1_met_signatures_tolook);
  if (m_met_triggers_hlt.size())
    check_triggers(m_met_triggers_hlt, m_hlt_met_signatures_tolook);
  
  // Create Shifter and Expert folders
  m_shifter_path = m_mon_path + "/Shifter";
  m_expert_path  = m_mon_path + "/Expert";

  addMonGroup(new MonGroup(this, m_shifter_path, run));
  addMonGroup(new MonGroup(this, m_expert_path, run));

  //-- Book "Shifter" histograms
  // L1 histograms
  std::string mongroupName = m_shifter_path + "/L1";
  addMonGroup(new MonGroup(this, mongroupName.c_str(), run));
  setCurrentMonGroup(mongroupName);

  addHistogram(new TH1F("L1_METx", "L1 Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("L1_METy", "L1 Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("L1_MET", "L1 MET (GeV);MET (GeV)", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("L1_SumEt", "L1 SumEt (GeV);SumEt (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("L1_MET_phi",  "L1 MET #phi (rad);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1_MET_phi_etweight",  "L1 MET #phi (|Missing E_{T}|);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1_MET_log", "L1 |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 40, -1.875, 4.125));
  addHistogram(new TH1F("L1_SumEt_log", "L1 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)", 40, -1.875, 4.125));


  /// Efficiencies for the L1 triggers 
  /// Use first item as denominator and compute the efficiency of the other with respect to this one.
  std::map<std::string,int>::const_iterator it;
  m_denominator_trigger = "";
  for (it=m_l1_met_signatures_tolook.begin(); it!=m_l1_met_signatures_tolook.end(); ++it) {
    if (m_denominator_trigger.empty()) {
      m_denominator_trigger = it->first;
      continue;
    }
    std::string hname = "Effh_" + it->first;
    std::string pname = "Eff_"+ it->first;
  }

  /* 
     pick up triggers for numerator efficiency histograms, these should be at pre-determined positions in the array:

    (0 - L1_prescaled used as denominator )

     1 - L1_unprescaled
     2 - HLT_unprescaled
     3 - HLT_physics
     4 - HLT-mu

  */
  /*
  for (int index = 0; index < 4; index++){
    m_numerator_trigger[index] = m_monitoring_met[index+1];
    ATH_MSG_INFO("In HLTMETMonTool::book() adding numerator histogram for  " << m_numerator_trigger[index]);
  }
  */

  //ATH_MSG_INFO("printing met_triggers_l1 0: " << m_met_triggers_l1[0]);
  //m_numerator_trigger[0] = m_met_triggers_l1[1];
  m_numerator_trigger[0] = m_met_triggers_l1[0];
  //ATH_MSG_INFO("printing met_triggers_l1 1: " << m_met_triggers_l1[1]);
  for (int index = 1; index < 4; index++){
    m_numerator_trigger[index] = m_met_triggers_hlt[index-1];
    ATH_MSG_DEBUG("adding m_numerator_trigger: " << index << " is: " << m_numerator_trigger[index]);
  }

  //  just one L1 efficiency to compute   
  std::string numerator_name = "Numerator_" + m_numerator_trigger[0];
  std::string numerator_title = numerator_name + "Efficiency Missing E_{T};ME_{T} (GeV)";
  std::string eff_name = "Effh_" + m_numerator_trigger[0];
  std::string prof_name = "Eff_" + m_numerator_trigger[0];
  std::string eff_title = eff_name + " Efficiency Missing E_{T};ME_{T} (GeV)";
  trig_eff_num[0] = new TH1F(numerator_name.c_str(), numerator_title.c_str(), m_eff_bins, m_eff_min, m_eff_max);
  //addHistogram(new TH1F(eff_name.c_str(), eff_title.c_str(), m_eff_bins, m_eff_min, m_eff_max));
  addProfile(new TProfile(prof_name.c_str(), eff_title.c_str(), m_eff_bins, m_eff_min, m_eff_max));


  h_denominator = new TH1F("efficiency_denominator", "efficiency denominator", m_eff_bins, m_eff_min, m_eff_max);

    
  // HLT histograms
  mongroupName = m_shifter_path + "/HLT";
  addMonGroup(new MonGroup(this, mongroupName.c_str(), run));
  setCurrentMonGroup(mongroupName);

  addHistogram(new TH1F("HLT_MEx", "HLT Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_MEy", "HLT Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_MET", "HLT |Missing E_{T}|;ME_{T} (GeV)", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("HLT_SumEt", "HLT Sum |E_{T}|;SumE_{T} (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("HLT_MET_phi", "HLT MET #phi (rad);#phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_phi_etweight",  "HLT MET #phi (|Missing E_{T}|);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_log",   "HLT |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_SumEt_log", "HLT Sum |E_{T}|;log_{10}(SumE_{T}/GeV)", 20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_MEz", "HLT Missing E_{z};E_{z} (GeV)", 100, -298.5,298.5));
  addHistogram(new TH1F("HLT_SumE", "HLT Sum |E|;SumE (GeV)", 153, -27., 18003.));
  addHistogram(new TH2F("HLT_MET_etaphi", "HLT MET #eta/#phi;#eta;#phi (rad)", 24, -4.8, 4.8, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH2F("HLT_MET_etaphi_etweight", "HLT MET #eta/#phi(|Missing E_{T}|);#eta;#phi (rad)", 24, -4.8, 4.8, m_phi_bins, m_phi_min, m_phi_max));

  // muon histograms                                                                                                                    
  int etabins = 14; double etamin = -2.8;   double etamax =  2.8;
  addHistogram(new TH1F("HLT_base_muon_pt", "HLT muon p_{T};p_{T} (GeV)", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("HLT_base_muon_phi", "HLT muon phi;#phi (rad)",  m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_base_muon_eta", "HLT muon eta;#eta", etabins, etamin, etamax));

  // numerators for HLT efficiency histograms  

  for (int i_eff = 1; i_eff < 4; i_eff++){
    std::string numerator_name = "Numerator_" + m_numerator_trigger[i_eff];
    std::string numerator_title = numerator_name + "Efficiency Missing E_{T};ME_{T} (GeV)";
    std::string eff_name = "Effh_" + m_numerator_trigger[i_eff];
    std::string prof_name = "Eff_" + m_numerator_trigger[i_eff];
    std::string eff_title = eff_name + " Efficiency Missing E_{T};ME_{T} (GeV)";
    trig_eff_num[i_eff] = new TH1F(numerator_name.c_str(), numerator_title.c_str(), m_eff_bins, m_eff_min, m_eff_max);
    //addHistogram(new TH1F(eff_name.c_str(), eff_title.c_str(), m_eff_bins, m_eff_min, m_eff_max));
    addProfile(new TProfile(prof_name.c_str(), eff_title.c_str(), m_eff_bins, m_eff_min, m_eff_max));
  }

  // add histograms for signal-like efficiency
  //h_muxe80_num = new TH1F("h_muxe80_num", "h_muxe80_num", m_eff_bins, m_eff_min, m_eff_max);
  //h_mu_denom = new TH1F("h_mu_denom", "h_mu_denom", m_eff_bins, m_eff_min, m_eff_max);
  //addHistogram(new TH1F("h_muxe80_eff", "Efficiency of xe80 on Wmunu-like events wrt Offline MET", m_eff_bins, m_eff_min, m_eff_max));
  addProfile(new TProfile("Eff_muxe80", "Efficiency of xe80 on Wmunu-like events wrt Offline MET", m_eff_bins, m_eff_min, m_eff_max));

  /*
  h_muxe100_num = new TH1F("h_muxe100_num", "h_muxe100_num", m_eff_bins, m_eff_min, m_eff_max);
  h_muXE35_denom = new TH1F("h_muXE35_denom", "h_muXE35_denom", m_eff_bins, m_eff_min, m_eff_max);
  addHistogram(new TH1F("h_muxe100XE35_eff", "Efficiency of xe100 on Wmunu-like events wrt Offline MET XE35", m_eff_bins, m_eff_min, m_eff_max));
  addProfile(new TProfile("Eff_muxe100XE35", "Efficiency of xe100 on Wmunu-like events wrt Offline MET XE35", m_eff_bins, m_eff_min, m_eff_max));
  */

  addProfile(new TProfile("Eff_elexe80", "Efficiency of xe80 on Wenu-like events wrt Offline MET", m_eff_bins, m_eff_min, m_eff_max));
  // to implement:
  // MET_SignalSelection

  addHistogram(new TH1F("HLT_electronpt", "HLT Electron Pt (GeV)", 50, 0, 100));
  //addHistogram(new TH1F("HLT_electroneta", "HLT Electron Eta", 25, -5, 5));
  addHistogram(new TH1F("HLT_electronmult", "HLT Electron Multiplicity", 10, 0, 10));

  addHistogram(new TH1F("HLT_muonpt", "HLT Muon Pt (GeV)", 200, 0, 100));
  //addHistogram(new TH1F("HLT_muoneta", "HLT Muon Eta", 25, -5, -5));
  addHistogram(new TH1F("HLT_muonmult", "HLT Muon Multiplicity", 10, 0, 10));

  addHistogram(new TH1F("HLT_MEx_mu", "HLT Missing E_{x};E_{x} Signal-like #mu (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_MEy_mu", "HLT Missing E_{y};E_{y} Signal-like #mu (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_MET_mu", "HLT |Missing E_{T}|;ME_{T} Signal-like #mu (GeV)", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("HLT_SumEt_mu", "HLT Sum |E_{T}|;SumE_{T} Signal-like #mu (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("HLT_MET_phi_mu", "HLT MET #phi (rad);#phi (rad) Signal-like #mu", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_phi_etweight_mu",  "HLT MET #phi (|Missing E_{T}|);MET #phi (rad) Signal-like #mu", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_log_mu",   "HLT |Missing E_{T}|;log_{10}(ME_{T}/GeV) Signal-like #mu", 20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_SumEt_log_mu", "HLT Sum |E_{T}|;log_{10}(SumE_{T}/GeV) Signal-like #mu", 20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_MEz_mu", "HLT Missing E_{z};E_{z} (GeV) Signal-like #mu", 100, -298.5,298.5));
  addHistogram(new TH1F("HLT_SumE_mu", "HLT Sum |E|;SumE (GeV) Signal-like #mu", 153, -27., 18003.));
  //addHistogram(new TH2F("HLT_MET_etaphi_etweight_mu", "HLT MET #eta/#phi(|Missing E_{T}|);#eta;#phi (rad) Signal-like #mu", 24, -4.8, 4.8, m_phi_bins, m_phi_min, m_phi_max));

  addHistogram(new TH1F("HLT_MEx_e", "HLT Missing E_{x};E_{x} Signal-like e (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_MEy_e", "HLT Missing E_{y};E_{y} Signal-like e (GeV)", 199, -298.5,  298.5));
  addHistogram(new TH1F("HLT_MET_e", "HLT |Missing E_{T}|;ME_{T} Signal-like e (GeV)", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("HLT_SumEt_e", "HLT Sum |E_{T}|;SumE_{T} Signal-like e (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("HLT_MET_phi_e", "HLT MET #phi (rad);#phi (rad) Signal-like e", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_phi_etweight_e",  "HLT MET #phi (|Missing E_{T}|);MET #phi (rad) Signal-like e", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_log_e",   "HLT |Missing E_{T}|;log_{10}(ME_{T}/GeV) Signal-like e", 20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_SumEt_log_e", "HLT Sum |E_{T}|;log_{10}(SumE_{T}/GeV) Signal-like e", 20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_MEz_e", "HLT Missing E_{z};E_{z} (GeV) Signal-like e", 100, -298.5,298.5));
  addHistogram(new TH1F("HLT_SumE_e", "HLT Sum |E|;SumE (GeV) Signal-like e", 153, -27., 18003.));
  //addHistogram(new TH2F("HLT_MET_etaphi_etweight_e", "HLT MET #eta/#phi(|Missing E_{T}|);#eta;#phi (rad) Signal-like e", 24, -4.8, 4.8, m_phi_bins, m_phi_min, m_phi_max));

  // Efficiency_xe80_SignalSelection
  // Efficiency_x005F_xe100_SignalSelection (Numerator = OfflineMET pass xe100, Denominator = OfflineMET pass XE35)

  if (!m_make_expert_histograms)
    return StatusCode::SUCCESS;  

  //-- Book expert histograms 
  /// L1
  /// No trigger requirement
  std::string monGroupName = m_expert_path + "/L1";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));

  addHistogram(new TH1F("L1_METx_log", "L1 Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("L1_METy_log", "L1 Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("L1_MET", "L1 MET (GeV);MET (GeV)", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("L1_MET_phi", "L1 MET #phi (rad);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  // to implement:
  // status
  // compEt
  // compETx
  // compETy
  // compETz
  // compSumEt
  // compHLT_usedChannel
  // compHLT_MET_status

  /// With trigger rquirement:  xE70
  for (it = m_l1_met_signatures_tolook.begin(); it != m_l1_met_signatures_tolook.end(); it++) {

    std::string m_trigger_path = monGroupName + "/" + it->first;

    //declare a group of histograms
    addMonGroup(new MonGroup(this, m_trigger_path, run));
    setCurrentMonGroup(m_trigger_path);

    addHistogram(new TH1F("L1_MET", "L1 MET (GeV);MET (GeV)", m_et_bins, m_et_min, m_et_max));
    addHistogram(new TH1F("L1_METx", "L1 Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
    addHistogram(new TH1F("L1_METy", "L1 Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));
    addHistogram(new TH1F("L1_SumEt", "L1 SumEt (GeV);SumEt (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
    addHistogram(new TH1F("L1_MET_phi", "L1 MET #phi (rad);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH1F("L1_MET_phi_etweight", "L1 MET #phi (|Missing E_{T}|);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max)); 
    addHistogram(new TH1F("L1_MET_log", "L1 |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 40, -1.875, 4.125));
    addHistogram(new TH1F("L1_METx_log", "L1 Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
    addHistogram(new TH1F("L1_METy_log", "L1 Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
    addHistogram(new TH1F("L1_SumEt_log", "L1 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)", 40, -1.875, 4.125));
    
    std::string effname = "Effh_" + it->first;
    std::string profname = "Eff_" + it->first;
    //addHistogram(new TH1F(effname.c_str(), "Efficiency;Offline MET (GeV);Efficiency", m_eff_bins, m_eff_min, m_eff_max));
    //addProfile(new TProfile(profname.c_str(), "Efficiency;Offline MET (GeV);Efficiency", m_eff_bins, m_eff_min, m_eff_max));

  }

  /// HLT_CELL. No specific trigger requirement
  monGroupName = m_expert_path + "/HLT";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));
  addHistogram(new TH1F("HLT_MEx_log", "HLT Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 27, -4.125, 4.125));
  addHistogram(new TH1F("HLT_MEy_log", "HLT Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 27, -4.125, 4.125));
  addHistogram(new TH1F("HLT_MET_lin1", "HLT |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)", 110, -0.5, 10.5));
  addHistogram(new TH1F("HLT_MET_phi_etweight", "HLT MET #phi(|Missing E_{T}|);#phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MEz_log", "HLT Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)",27, -4.125, 4.125));
  addHistogram(new TH1F("HLT_SumE_log",   "HLT Sum |E|;log_{10}(SumE/GeV)",40, -1.875, 6.125));
  addHistogram(new TH1F("HLT_XS", "HLT MET Significance;Significance (XS/GeV^{1/2})", 40, -0.025,  20.025));
  addHLTStatusHistogram();
  addHLTCompHistograms();
  
  /// With trigger requiriment: xe35, xe80, xe100
  for (it = m_hlt_met_signatures_tolook.begin(); it != m_hlt_met_signatures_tolook.end(); it++) {

    std::string m_expert_path_trigger = monGroupName + "/" + it->first;
    addMonGroup(new MonGroup(this, m_expert_path_trigger, run));
    setCurrentMonGroup(m_expert_path_trigger);

    addHistogram(new TH1F("HLT_MEx", "HLT Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
    addHistogram(new TH1F("HLT_MEy", "HLT Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));
    addHistogram(new TH1F("HLT_MET", "HLT |Missing E_{T}|;ME_{T} (GeV)", m_et_bins, m_et_min, m_et_max));
    addHistogram(new TH1F("HLT_MET_lin1", "HLT |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)", 110, -0.5, 10.5));
    addHistogram(new TH1F("HLT_SumEt", "HLT Sum |E_{T}|;SumE_{T} (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
    addHistogram(new TH1F("HLT_MET_phi", "HLT MET #phi (rad);#phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH1F("HLT_MET_phi_etweight", "HLT MET #phi(|Missing E_{T}|);#phi (rad)",    m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH1F("HLT_MEx_log", "HLT Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 27, -4.125, 4.125));
    addHistogram(new TH1F("HLT_MEy_log", "HLT Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 27, -4.125, 4.125));
    addHistogram(new TH1F("HLT_MET_log", "HLT |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 20, -1.875, 4.125));
    addHistogram(new TH1F("HLT_SumEt_log", "HLT Sum |E_{T}|;log_{10}(SumE_{T}/GeV)", 20, -1.875, 4.125));
    addHistogram(new TH1F("HLT_MEz", "HLT Missing E_{z};E_{z} (GeV)", 100, -298.5,298.5));
    addHistogram(new TH1F("HLT_SumE", "HLT Sum |E|;SumE (GeV)", 153, -27., 18003.));
    addHistogram(new TH1F("HLT_MEz_log", "HLT Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)",27, -4.125, 4.125));
    addHistogram(new TH1F("HLT_SumE_log", "HLT Sum |E|;log_{10}(SumE/GeV)",40, -1.875, 6.125));
    addHistogram(new TH2F("HLT_MET_etaphi", "HLT MET #eta/#phi;#eta;#phi (rad)", 24, -4.8, 4.8, m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH2F("HLT_MET_etaphi_etweight", "HLT MET #eta/#phi(|Missing E_{T}|);#eta;#phi (rad)", 24, -4.8, 4.8, m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH1F("HLT_XS", "HLT MET Significance;Significance (XS/GeV^{1/2})", 40, -0.025,  20.025));
    addHLTStatusHistogram();

    std::string effname = "Effh_"+it->first;
    std::string profname = "Eff_"+it->first;
    //addHistogram(new TH1F(effname.c_str(), "Efficiency;Offline MET (GeV);Efficiency", m_eff_bins, m_eff_min, m_eff_max));
    //addProfile(new TProfile(profname.c_str(), "Efficiency;Offline MET (GeV);Efficiency", m_eff_bins, m_eff_min, m_eff_max));
  }
  
  /// Alternative algorithms: tc_lcw, tc_em, pueta, pufit, mht, feb, fex
  for (std::vector<std::string>::iterator it2 = m_monitoring_alg.begin(); it2 != m_monitoring_alg.end(); it2++) {

    std::string m_expert_path_alg = monGroupName + "/" + *it2;
    addMonGroup(new MonGroup(this, m_expert_path_alg, run));
    setCurrentMonGroup(m_expert_path_alg);

    addHistogram(new TH1F("HLT_MEx", "HLT Missing E_{x};E_{x} (GeV)", 199, -298.5,  298.5));
    addHistogram(new TH1F("HLT_MEy", "HLT Missing E_{y};E_{y} (GeV)", 199, -298.5,  298.5));
    addHistogram(new TH1F("HLT_MET", "HLT |Missing E_{T}|;ME_{T} (GeV)", m_et_bins, m_et_min, m_et_max));
    addHistogram(new TH1F("HLT_MET_lin1", "HLT |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)", 110, -0.5, 10.5));
    addHistogram(new TH1F("HLT_SumEt", "HLT Sum |E_{T}|;SumE_{T} (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
    addHistogram(new TH1F("HLT_MET_phi", "HLT MET #phi (rad);#phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH1F("HLT_MET_phi_etweight", "HLT MET #phi(|Missing E_{T}|);#phi (rad)",    m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH1F("HLT_MEx_log", "HLT Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 27, -4.125, 4.125));
    addHistogram(new TH1F("HLT_MEy_log", "HLT Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 27, -4.125, 4.125));
    addHistogram(new TH1F("HLT_MET_log", "HLT |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 20, -1.875, 4.125));
    addHistogram(new TH1F("HLT_SumEt_log", "HLT Sum |E_{T}|;log_{10}(SumE_{T}/GeV)", 20, -1.875, 4.125));
    addHistogram(new TH1F("HLT_MEz", "HLT Missing E_{z};E_{z} (GeV)", 100, -298.5,298.5));
    addHistogram(new TH1F("HLT_SumE", "HLT Sum |E|;SumE (GeV)", 153, -27., 18003.));
    addHistogram(new TH1F("HLT_MEz_log", "HLT Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)",27, -4.125, 4.125));
    addHistogram(new TH1F("HLT_SumE_log", "HLT Sum |E|;log_{10}(SumE/GeV)",40, -1.875, 6.125));
    addHistogram(new TH2F("HLT_MET_etaphi", "HLT MET #eta/#phi;#eta;#phi (rad)", 24, -4.8, 4.8, m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH2F("HLT_MET_etaphi_etweight", "HLT MET #eta/#phi(|Missing E_{T}|);#eta;#phi (rad)", 24, -4.8, 4.8, m_phi_bins, m_phi_min, m_phi_max));
    addHistogram(new TH1F("HLT_XS", "HLT MET Significance;Significance (XS/GeV^{1/2})", 40, -0.025,  20.025));
    addHLTStatusHistogram();

    std::string effname = "Effh_"+ *it2;
    std::string profname = "Eff_"+ *it2;
    //addHistogram(new TH1F(effname.c_str(), "Efficiency;Offline MET (GeV);Efficiency", m_eff_bins, m_eff_min, m_eff_max));
    //addProfile(new TProfile(profname.c_str(), "Efficiency;Offline MET (GeV);Efficiency", m_eff_bins, m_eff_min, m_eff_max));
  }

  
  /// Offline MET
  monGroupName = m_expert_path + "/Offline";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));
  setCurrentMonGroup(monGroupName);
  addHistogram(new TH1F("Offline_MET", "Offline MET;Et", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("Offline_METx", "Offline METx;Et", 199, -298.5, 298.5));
  addHistogram(new TH1F("Offline_METy", "Offline METy;Et", 199, -298.5, 298.5));
  addHistogram(new TH1F("Offline_SumEt", "Offline SumEt;sumEt", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("Offline_MET_phi", "Offline MET phi;Phi", m_phi_bins, m_phi_min, m_phi_max));

  /// L1/HLT correlations and difference: XE35 vs xe35 and XE60 vs xe80
  monGroupName = m_expert_path + "/L1_vs_HLT";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));
  setCurrentMonGroup(monGroupName);
  addL1vsHLTHistograms();
  /*
  for (it = m_hlt_met_signatures_tolook.begin(); it != m_hlt_met_signatures_tolook.end(); it++) {
    std::string path_trigger = monGroupName + "/" + it->first;
    ATH_MSG_INFO("Adding " << path_trigger);
    addMonGroup(new MonGroup(this, path_trigger, run));
    setCurrentMonGroup(path_trigger);
    addL1vsHLTHistograms();
  }
  */

  /// HLT/Offline correlations and differences
  monGroupName = m_expert_path + "/HLT_vs_Offline";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));
  setCurrentMonGroup(monGroupName);
  addHLTvsOffHistograms();
  /*
  for (it = m_hlt_met_signatures_tolook.begin(); it != m_hlt_met_signatures_tolook.end(); it++) {
    std::string path_trigger = monGroupName + "/" + it->first;
    ATH_MSG_INFO("Adding " << path_trigger);
    addMonGroup(new MonGroup(this, path_trigger, run));
    setCurrentMonGroup(path_trigger);
    addHLTvsOffHistograms();
  }
  */

  /// HLT Muons
  // pt, eta. phi  

  return StatusCode::SUCCESS;
} 


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fill() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool:fill()");

  trigger_decision();

  StatusCode sc = fillMETHist();

  if (sc.isFailure()) {
    ATH_MSG_WARNING("The method fillMETHist(...) failed");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::proc() {

  std::string eff_name;
  std::string mongroupName;
  TH1 *h_numerator;

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HLTMETMonTool::proc()");

  if(m_print_met_trig_stats)
    print_trigger_stats();

  // efficiency histo shifter/L1     

  mongroupName = m_shifter_path + "/L1";
  setCurrentMonGroup(mongroupName);


  eff_name = "Effh_" + m_numerator_trigger[0];
  h_numerator = trig_eff_num[0];
  h_numerator->Sumw2();
  h_denominator->Sumw2();
  if(h_denominator->GetEntries() != 0) {
    ATH_MSG_DEBUG(" performing division ");
    //hist(eff_name)->Divide(h_numerator, h_denominator);
  }

  // efficiency histos shifter/HLT   

  mongroupName = m_shifter_path + "/HLT";
  setCurrentMonGroup(mongroupName);


  for (int i_eff = 1; i_eff < 4; i_eff++){
    eff_name = "Effh_" + m_numerator_trigger[i_eff];
    //hist(eff_name)->Divide(trig_eff_num[i_eff], h_denominator);
    ATH_MSG_DEBUG("in HLTMETMonTool::proc() filling efficiency for trigger " << m_numerator_trigger[i_eff]);
  }


  return StatusCode::SUCCESS;
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTCompHistograms() {

  // HLT MET all components in one 2d histogram
  float fMaxEFC = float(m_compNames.size());
  fMaxEFC -= 0.5;
  int fBinEFC = int(m_compNames.size());
  TH2F *h2f[10] = {0};
  h2f[0] = new TH2F("compN_compEx",      "HLT MissingE_{x} VS component;;sgn(ME_{x}) log_{10}(E_{x}/GeV)", fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[1] = new TH2F("compN_compEy",      "HLT Missing E_{y} VS component;;sgn(ME_{y}) log_{10}(E_{y}/GeV)", fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[2] = new TH2F("compN_compEz",      "HLT Missing E_{z} VS component;;sgn(ME_{z}) log_{10}(E_{z}/GeV)", fBinEFC, -0.5, fMaxEFC, 27, -4.125, 4.125);
  h2f[3] = new TH2F("compN_compEt",      "HLT MissingE_{T} VS component;;sgn(ME_{T}) log_{10}(ME_{T}/GeV)", fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[4] = new TH2F("compN_compSumEt",   "HLT Sum |E_{T}| VS component;;log_{10}(SumE_{T}/GeV)", fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[5] = new TH2F("compN_compSumE",    "HLT Sum |E| VS component;;log_{10}(SumE/GeV)", fBinEFC, -0.5, fMaxEFC, 20, -1.875, 4.125);
  h2f[6] = new TH2F("compN_compEt_lin",  "HLT MissingE_{T} VS component;;ME_{T} (GeV)", fBinEFC, -0.5, fMaxEFC, 102, -13.5, 601.5);
  h2f[7] = new TH2F("compN_compSumEt_lin",   "HLT Sum |E_{T}| VS component;;SumE_{T} (GeV)", fBinEFC, -0.5, fMaxEFC, 236, -30,   2802); 
  h2f[8] = new TH2F("compN_HLT_MET_status",   "HLT MET Status VS component;;", fBinEFC, -0.5, fMaxEFC,32, -0.5, 31.5);
  h2f[9] = new TH2F("compN_HLT_usedChannels", "HLT Used Channels VS component;;log_{10}(N > 0)", fBinEFC, -0.5, fMaxEFC,13,-0.5,6.0);

  for (size_t k = 0; k < 10; k++) { // 10 hists
    for (size_t cn = 0; cn < m_compNames.size(); cn++) { // 25 bins
      if(cn < m_compNames.size()) h2f[k]->GetXaxis()->SetBinLabel(cn+1, m_compNames[cn].c_str());
    }
    // for status v. component, set bin labels for Y axis
    if (k == 8) {
      for (size_t j = 0; j < m_bitNames.size(); j++) {
        if(j < m_bitNames.size()) h2f[k]->GetYaxis()->SetBinLabel(j+1, m_bitNames[j].c_str());
      }
      h2f[k]->GetYaxis()->SetLabelFont(42);
      h2f[k]->GetYaxis()->SetLabelOffset(0.002);
      h2f[k]->GetYaxis()->SetLabelSize(0.03);
    }
    if (k == 9) {
      // 10^x (x = 0.5, 1.0 1.5 ....)
      /*std::string binlb_nc[13] = { "none", "[0-3]", "[4-9]", "[10-31]", 
        "[32-99]", "[100-316]", "[317-999]", "[1-3.16]10^{3}", "[0.32 - 1]10^{4}", 
        "[1 - 3.2]10^{4}", "[0.32 - 1]10^{5}", "[1 - 3.2]10^{5}", "[0.32 - 1]10^{6}" };
        for(size_t l = 0; l < 13; l++) {*/
      h2f[k]->GetYaxis()->SetBinLabel(1,"none");
      //}
    }
    addHistogram(h2f[k]);
  }

  int phibins = 16; double phimin = m_phi_min; double phimax = m_phi_max;
  int etabins = 24; double etamin = -4.8;   double etamax =  4.8;

  for (unsigned int j = 0; j < m_compNames.size(); j++) {
    std::string name = Form("compN_EtaPhi_%02d", j);
    std::string title = m_compNames[j];
    title += ": N(#eta, #phi);#eta;#phi [rad];";
    TH2F* h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);

    name = Form("compEt_lin_EtaPhi_%02d", j);
    title = m_compNames[j];
    title += ": MissingE_{T}(#eta, #phi); #eta; #phi [rad];ME_{T}/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);
    
    name = Form("compSumEt_lin_EtaPhi_%02d", j);
    title = m_compNames[j];
    title += ": Sum |E_{T}|(#eta, #phi); #eta; #phi [rad];Sum|E_{T}|/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);
    
    name = Form("compSumE_lin_EtaPhi_%02d", j);
    title = m_compNames[j];
    title += ": Sum |E|(#eta, #phi); #eta; #phi [rad];Sum|E|/GeV";
    h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);
    
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTStatusHistogram() {
  TH1F *h1i = new TH1F("HLT_MET_status", "HLT MET Status", 32, -0.5, 31.5);
  for (size_t j = 0; j < m_bitNames.size(); j++) {
    if(j < m_bitNames.size()) h1i->GetXaxis()->SetBinLabel(j+1, m_bitNames[j].c_str());
  }
  addHistogram(h1i);
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1vsHLTHistograms() {

  // Correlation
  addHistogram(new TH2F("L1_HLT_EtCor", "L1 vs HLT Missing E_{T} ; HLTME_{T} (GeV); L1ME_{T} (GeV)", m_et_bins, m_et_min, m_et_max, m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH2F("L1_HLT_SumEtCor", "L1 vs HLT Sum |E_{T}| ; HLTSum|E_{T}| (GeV); L1Sum|E_{T}| (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max, m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH2F("L1_HLT_PhiCor", "L1 vs HLT #phi ; EF #phi (rad); L1 #phi (rad)", m_phi_bins, m_phi_min, m_phi_max, m_phi_bins, m_phi_min, m_phi_max));

  // Differences
  addHistogram(new TH1F("L1_HLT_dEt",  "#DeltaE_{T}(L1-HLT) ; #DeltaE_{T} (GeV)", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1_HLT_dPhi", "#Delta#phi(L1-HLT) ; #Delta#phi (GeV)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1_HLT_dEx",  "#DeltaE_{x}(L1-HLT) ; #DeltaE_{x} (GeV)", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1_HLT_dEy",  "#DeltaE_{y}(L1-HLT) ; #DeltaE_{y} (GeV)", m_det_bins, m_det_min, m_det_max));

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTvsOffHistograms() {

  addHistogram(new TH2F("HLT_Off_EtCor", "EtCor_HLTMET_OffMET; Offline MET ; HLT MET", m_et_bins, m_et_min, m_et_max, m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH2F("HLT_Off_SumEtCor", "SumEtCor_HLTMET_OffMET; Offline SumEt ; HLT SumEt", m_sumet_bins, m_sumet_min, m_sumet_max, m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH2F("HLT_Off_PhiCor", "PhiCor_HLTMET_OffMET; Offline #phi ; HLT #phi", m_phi_bins, m_phi_min, m_phi_max, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_Off_dEt", "dEt_HLTMET_OffMET; dEt", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("HLT_Off_dPhi", "dPhi_HLTMET_OffMET; dPhi", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_Off_dEx", "dEx_HLTMET_OffMET; dEx", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("HLT_Off_dEy", "dEy_HLTMET_OffMET; dEy", m_det_bins, m_det_min, m_det_max));   
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::trigger_decision() {

  std::vector<std::map<std::string,int>> m_met_signatures_tolook;
  m_met_signatures_tolook.push_back(m_l1_met_signatures_tolook);
  m_met_signatures_tolook.push_back(m_hlt_met_signatures_tolook);

  if (m_print_met_trig_stats) {
    std::map<std::string,int>::iterator iter;
    for (unsigned int i = 0; i<m_met_signatures_tolook.size(); i++) {
      for (iter = m_met_signatures_tolook[i].begin(); iter != m_met_signatures_tolook[i].end(); ++iter) {
        std::string name = iter->first;
        if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
          iter->second +=1;
        }
      }
    }
  }
}

//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillMETHist() {

  if (m_debuglevel) 
    ATH_MSG_DEBUG("in HTLMETMonTool::fillMETHistos()");

  StatusCode sc = StatusCode::SUCCESS;

  // retrieve xAOD L1 ROI 
  const xAOD::EnergySumRoI *m_l1_roi_cont = 0;
  sc = m_storeGate->retrieve(m_l1_roi_cont, m_lvl1_roi_key);
  if(sc.isFailure() || !m_l1_roi_cont) {
    ATH_MSG_WARNING("Could not retrieve LVL1_RoIs with key \"" << m_lvl1_roi_key << "\" from TDS"  );
  }

  // retrieve HLT containers
  // Get HLT (CELL) container
  const xAOD::TrigMissingETContainer *m_hlt_met_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_met_cont, m_hlt_met_key);
  if (sc.isFailure() || !m_hlt_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met with " << m_hlt_met_cont->size() << " elements");

  // Get HLT mht container
  const xAOD::TrigMissingETContainer *m_hlt_mht_met_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_mht_met_cont, m_hlt_mht_met_key);
  if (sc.isFailure() || !m_hlt_mht_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_mht_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(mht) with " << m_hlt_mht_met_cont->size() << " elements");

  // Get HLT topocl container
  const xAOD::TrigMissingETContainer *m_hlt_topocl_met_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_topocl_met_cont, m_hlt_topocl_met_key);
  if (sc.isFailure() || !m_hlt_topocl_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_topocl_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(topcl) with " << m_hlt_topocl_met_cont->size() << " elements");

  // Get HLT topocl_PS container
  const xAOD::TrigMissingETContainer *m_hlt_topocl_PS_met_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_topocl_PS_met_cont, m_hlt_topocl_PS_met_key);
  if (sc.isFailure() || !m_hlt_topocl_PS_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_topocl_PS_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(topocl_PS) with " << m_hlt_topocl_PS_met_cont->size() << " elements");

  // Get HLT topocl_PUC container
  const xAOD::TrigMissingETContainer *m_hlt_topocl_PUC_met_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_topocl_PUC_met_cont, m_hlt_topocl_PUC_met_key);
  if (sc.isFailure() || !m_hlt_topocl_PUC_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_topocl_PUC_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(topocl_PUC) with " << m_hlt_topocl_PUC_met_cont->size() << " elements");

  // Get HLT FEB container
  const xAOD::TrigMissingETContainer *m_hlt_FEB_met_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_FEB_met_cont, m_hlt_FEB_met_key);
  if (sc.isFailure() || !m_hlt_FEB_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_FEB_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(FEB) with " << m_hlt_FEB_met_cont->size() << " elements");

  // Get HLT Fex container
  const xAOD::TrigMissingETContainer *m_hlt_Fex_met_cont = 0;
  sc = m_storeGate->retrieve(m_hlt_Fex_met_cont, m_hlt_Fex_met_key);
  if (sc.isFailure() || !m_hlt_Fex_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_Fex_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(Fex) with " << m_hlt_Fex_met_cont->size() << " elements");

  // Get EF muon and EF egamma containers for signal-like selection
  // retrieve EF muon container
  const xAOD::MuonContainer *m_hlt_muonEFcontainer = 0;
  sc = m_storeGate->retrieve(m_hlt_muonEFcontainer, "HLT_xAOD__MuonContainer_MuonEFInfo"); // container name should be set as a key
  if (sc.isFailure() || !m_hlt_muonEFcontainer) {
    ATH_MSG_DEBUG("Could not retrieve HLT_xAOD__MuonContainer_MuonEFInfo from TDS"); // set to warning-level
  }
  else
    ATH_MSG_DEBUG("Accessing EF muons container with " << m_hlt_muonEFcontainer->size() << " elements"); // set to debug-level

  // retrieve EF electron container
  const xAOD::ElectronContainer *m_hlt_electronEFcontainer = 0;
  sc = m_storeGate->retrieve(m_hlt_electronEFcontainer, "HLT_xAOD__ElectronContainer_egamma_Electrons"); // container name should be set as a key
  if (sc.isFailure() || !m_hlt_electronEFcontainer) {
    ATH_MSG_DEBUG("Could not retrieve HLT_xAOD__ElectronContainer_egamma_Electrons from TDS"); // set to warning-level
  }
  else
    ATH_MSG_DEBUG("Accessing EF electrons container with " << m_hlt_electronEFcontainer->size() << " elements"); // set to debug-level

  // Get Offline MET container
  const xAOD::MissingETContainer *m_off_met_cont = 0;
  sc = m_storeGate->retrieve(m_off_met_cont, m_off_met_key);
  if (sc.isFailure() || !m_off_met_cont) {
    ATH_MSG_DEBUG("Could not retrieve Reconstructed MET term with Key " << m_off_met_key << " : m_off_met_cont = 0");
  }
  else if (m_debuglevel) {
    ATH_MSG_DEBUG("Got Reconstructed MET term with key " << m_off_met_key);
  }


  if (m_debuglevel) 
    ATH_MSG_DEBUG("Filling histograms...");

  //-- Get met values
  const xAOD::TrigMissingET *m_hlt_met = 0;
  const xAOD::MissingET     *m_off_met = 0;
 
  /// L1
  float epsilon = 1e-6;  // 1 keV
  float epsilon_l1met = 1.e-3; // 1 MeV
  
  float l1_mex = -9e9;
  float l1_mey = -9e9;
  float l1_met = -9e9;
  float l1_sumet = -9e9;
  float l1_phi = -9e9;
  float l1_mex_log = -9e9;
  float l1_mey_log = -9e9;
  float l1_met_log = -9e9;
  float l1_sumet_log = -9e9;

  if (m_l1_roi_cont) {
    if ((m_l1_roi_cont->energyX())>-9e12 && (m_l1_roi_cont->energyX())<9e12 && (m_l1_roi_cont->energyY())>-9e12 && (m_l1_roi_cont->energyY())<9e12) { 
      l1_mex = - (m_l1_roi_cont->energyX())/CLHEP::GeV;
      l1_mey = - (m_l1_roi_cont->energyY())/CLHEP::GeV;
      l1_met = sqrt(l1_mex*l1_mex + l1_mey*l1_mey);
      l1_phi = atan2f(l1_mey,l1_mex);
      l1_sumet = (m_l1_roi_cont->energyT())/CLHEP::GeV; 
      
      l1_mex_log = signed_log(l1_mex, epsilon);
      l1_mey_log = signed_log(l1_mey, epsilon);
      l1_met_log = signed_log(l1_met, epsilon);
      l1_sumet_log = signed_log(l1_sumet, epsilon);
    } else {
      ATH_MSG_WARNING("L1 POI enegy too large");
    }
  }

  /// HLT
  float hlt_ex = -9e9;
  float hlt_ey = -9e9;
  float hlt_ez = -9e9;
  float hlt_met = -9e9;
  float hlt_sumet = -9e9;
  float hlt_sume = -9e9;
  float hlt_phi = -9e9;
  float hlt_eta = -9e9;
  float hlt_ex_log = -9e9;
  float hlt_ey_log = -9e9;
  float hlt_ez_log = -9e9;
  float hlt_met_log = -9e9;
  float hlt_sume_log = -9e9;
  float hlt_sumet_log = -9e9;
  float hlt_significance = -9e9;

  if (m_hlt_met_cont && m_hlt_met_cont->size()) {  

    m_hlt_met = m_hlt_met_cont->at(0);
    
    hlt_ex = m_hlt_met->ex()/CLHEP::GeV; 
    hlt_ey = m_hlt_met->ey()/CLHEP::GeV; 
    hlt_ez = m_hlt_met->ez()/CLHEP::GeV;
    hlt_met = sqrt(hlt_ex*hlt_ex+hlt_ey*hlt_ey); 
    //hlt_me  = sqrt(hlt_ex*hlt_ex+hlt_ey*hlt_ey+hlt_ez*hlt_ez);
    hlt_sumet = m_hlt_met->sumEt()/CLHEP::GeV;
    hlt_sume  = m_hlt_met->sumE()/CLHEP::GeV; 

    CLHEP::Hep3Vector v(hlt_ex, hlt_ey, hlt_ez);
    hlt_eta = v.eta();
    hlt_phi = v.phi();
    
    float epsilon = 1e-6;  // 1 keV
    //if (hlt_me > epsilon)  hlt_me_log  = log10(fabsf(hlt_me)); // underflow otherwise

    epsilon = 1.189;
    hlt_ex_log = signed_log(hlt_ex, epsilon);
    hlt_ey_log = signed_log(hlt_ey, epsilon);
    hlt_ez_log = signed_log(hlt_ez, epsilon);
    hlt_met_log = signed_log(hlt_met, epsilon);
    hlt_sume_log = signed_log(hlt_sume, epsilon);
    hlt_sumet_log = signed_log(hlt_sumet, epsilon);

    float resolution = 0.0;
    if (hlt_sumet>0.0) resolution = m_sigOffset + m_sigSlope*sqrt(hlt_sumet) + m_sigQuadr*hlt_sumet;
    if (resolution>0.0) hlt_significance = hlt_met/resolution;
    ATH_MSG_DEBUG("MET = " << hlt_met);
    ATH_MSG_DEBUG("resolution = " << resolution);
    ATH_MSG_DEBUG("Significance = " << hlt_significance);
  }

  /// Offline MET
  float off_ex = -9e9;
  float off_ey = -9e9;
  float off_met = -9e9;
  float off_sumet = -9e9;
  float off_phi = -9e9;
  if (m_off_met_cont) {
    if (m_off_met_cont->size()) {
      m_off_met = (*m_off_met_cont)["FinalClus"];
      
      off_ex = ((*m_off_met_cont)["FinalClus"]->mpx())/CLHEP::GeV;
      off_ey = ((*m_off_met_cont)["FinalClus"]->mpy())/CLHEP::GeV;
      off_met = sqrt(off_ex*off_ex+off_ey*off_ey);
      off_sumet = ((*m_off_met_cont)["FinalClus"]->sumet())/CLHEP::GeV;
      off_phi = atan2(off_ey, off_ex);
    }
  }

  TH1 *h(0);
  TH2 *h2(0);

  //########################
  //-- Fill Shifter histograms
  //########################

  /////////////
  /// L1
  /////////////

  std::string mongroupName = m_shifter_path + "/L1";
  setCurrentMonGroup(mongroupName.c_str());

  if (l1_met > epsilon_l1met) {
    if ((h = hist("L1_METx")))      h->Fill(l1_mex);
    if ((h = hist("L1_METy")))      h->Fill(l1_mey);
    if ((h = hist("L1_MET")))       h->Fill(l1_met);
    if ((h = hist("L1_SumEt")))     h->Fill(l1_sumet);
    if ((h = hist("L1_MET_phi")))   h->Fill(l1_phi);
    if ((h = hist("L1_MET_phi_etweight")))  h->Fill(l1_phi, l1_met);
    if ((h = hist("L1_MET_log")))   h->Fill(l1_met_log);
    if ((h = hist("L1_SumEt_log"))) h->Fill(l1_sumet_log);
  }

  /*
  if (getTDT()->isPassed(m_denominator_trigger, TrigDefs::eventAccepted)) {
    //    hist("efficiency_denominator")->Fill(off_met);                                                                                                                                                      
    h_denominator->Fill(off_met);
    ATH_MSG_DEBUG("L1 signature " << m_denominator_trigger << " passed denominator histo has " << h_denominator->GetEntries());
  }
  else
    {
      ATH_MSG_DEBUG("L1 signature " << m_denominator_trigger << " NOT passed");
      }
  */
  // denominator hist for all efficiency histos
  h_denominator->Fill(off_met);

  TProfile *p(0);

  // numerator histo for L1 efficiency
  std::string profname = "Eff_"+m_numerator_trigger[0];
  if (getTDT()->isPassed(m_numerator_trigger[0], TrigDefs::eventAccepted)) {
    trig_eff_num[0]->Fill(off_met);
    if ((p = profile(profname))) p->Fill(off_met,1.0,1.0);
  } else {
    if ((p = profile(profname))) p->Fill(off_met,0.0,1.0);
  }
  
  

  /////////////
  // HLT
  /////////////

  setCurrentMonGroup(m_shifter_path + "/HLT");	

  if ((h = hist("HLT_MEx")))       h->Fill(hlt_ex);
  if ((h = hist("HLT_MEy")))       h->Fill(hlt_ey);
  if ((h = hist("HLT_MET")))       h->Fill(hlt_met);
  if ((h = hist("HLT_SumEt")))     h->Fill(hlt_sumet);
  if ((h = hist("HLT_MET_phi")))   h->Fill(hlt_phi);
  if ((h = hist("HLT_MET_phi_etweight")))  h->Fill(hlt_phi, hlt_met);
  if ((h = hist("HLT_MET_log")))   h->Fill(hlt_met_log);
  if ((h = hist("HLT_SumEt_log"))) h->Fill(hlt_sumet_log);
  if ((h = hist("HLT_MEz")))       h->Fill(hlt_ez);
  if ((h = hist("HLT_SumE")))      h->Fill(hlt_sume);
  if ((h2 = hist2("HLT_MET_etaphi"))) h2->Fill(hlt_eta, hlt_phi);
  if ((h2 = hist2("HLT_MET_etaphi_etweight"))) h2->Fill(hlt_eta, hlt_phi, hlt_met);

  // muons contributing to trigger chain defined by m_muon_base_trigger
  /*

  std::vector<float> baseMuonPt;
  std::vector<float> baseMuonEta;
  std::vector<float> baseMuonPhi;
  int numMuons = 0;

  ATH_MSG_DEBUG(" muon base chain is " << m_muon_base_trigger);

  auto chainGroup = getTDT()->getChainGroup(m_muon_base_trigger);
  auto featureContainer = chainGroup->features();
  auto vec_muons = featureContainer.get<xAOD::MuonContainer>();
  for( auto mufeat : vec_muons ) {
    for(auto muItr :  *(mufeat.cptr())) {
      if (muItr->author() != 1) continue;
      baseMuonPt.push_back(muItr->pt()/CLHEP::GeV);
      baseMuonEta.push_back(muItr->eta());
      baseMuonPhi.push_back(muItr->phi());
      ATH_MSG_DEBUG("Trigger chain " << m_muon_base_trigger << " has muon with pt, eta, phi = " << muItr->pt() << ", " << muItr->eta() << ", " << muItr->phi() << " auth " << muItr->author());
    }
  }

      //In the case where a muon is seeded by more than one RoI the trigger decision tool will deliver multiple copies of the same muon

  if ((numMuons = baseMuonPt.size()) >  0){
  std::vector<float> seenPt;
  std::vector<float> seenPhi;
  std::vector<float> seenEta;
  std::vector<int>   seenIx;

  seenPt.push_back(baseMuonPt[0]);
  seenPhi.push_back(baseMuonPhi[0]);
  seenEta.push_back(baseMuonEta[0]);
  seenIx.push_back(0);

  if ((numMuons = baseMuonPt.size()) >  1){
    ATH_MSG_DEBUG("More than one muone!! Stored " << numMuons << " muons ");

    bool seenBefore;
    unsigned int ix2;

    for (int ix1=1; ix1 < numMuons; ix1++){
      seenBefore = false;
      for (ix2=0; ix2 < seenIx.size(); ix2++){
	if(baseMuonPt[ix1] == seenPt[ix2]) seenBefore = true;
      }
      if(!seenBefore) {
	seenPt.push_back(baseMuonPt[ix2]);
	seenPhi.push_back(baseMuonPhi[ix2]);
	seenEta.push_back(baseMuonEta[ix2]);
	seenPt.push_back(baseMuonPt[ix2]);
        ATH_MSG_DEBUG(" pushing back ix2 is  " << ix2);
      } else {
        ATH_MSG_DEBUG(" Found before, not pushing back ix2 is  " << ix2);
      }
    }
  } 
  for (unsigned int nMu = 0; nMu < seenPt.size(); nMu++){
    if ((h = hist("HLT_base_muon_pt")))  h->Fill(seenPt[seenIx[nMu]]);
    if ((h = hist("HLT_base_muon_eta"))) h->Fill(seenEta[seenIx[nMu]]);
    if ((h = hist("HLT_base_muon_phi"))) h->Fill(seenPhi[seenIx[nMu]]);
  }
}
*/


  // fill numerator histos for HLT efficiency histos  
  for (int i_eff = 1; i_eff < 4; i_eff++){
    std::string profname = "Eff_"+m_numerator_trigger[i_eff];
    if (getTDT()->isPassed(m_numerator_trigger[i_eff], TrigDefs::eventAccepted)) {
      trig_eff_num[i_eff]->Fill(off_met);
      if ((p = profile(profname))) p->Fill(off_met,1.0,1.0);
    } else {
      if ((p = profile(profname))) p->Fill(off_met,0.0,1.0);
   }
  }

  ///////////////////////////////////////////
  //## fill in signal-like selection histograms ###############
  ///////////////////////////////////////////

  // Print out which trigger chains were fired in this event
  //auto chainGroup = getTDT()->getChainGroup("HLT_.*");
  //for(auto &trig : chainGroup->getListOfTriggers()) {
    //if(getTDT()->isPassed(trig)) ATH_MSG_INFO("Chain *" << trig << "* is passed: YES");
    //else ATH_MSG_INFO("Chain *" << trig << "* is passed: NO");
  //}

  // Muon signal-like selection
  //ATH_MSG_INFO("Pass state HLT_mu18.* = " << getTDT()->isPassed("HLT_mu18.*", TrigDefs::eventAccepted));

  ATH_MSG_DEBUG("Going to iterate through muon container");
  bool METMuonFilled = false; // flag is set to true if event satisfies signal-like requirement 
  int muonMult = 0;
  bool muonContEmpty = true;
  if (m_hlt_muonEFcontainer && m_hlt_muonEFcontainer->size()>0) {
    ATH_MSG_DEBUG("HLT EF Muon container exists and is not empty. Going to loop over container.");
    muonContEmpty = false;
    for (auto muon : *m_hlt_muonEFcontainer) {
      if(muon->muonType() == xAOD::Muon::Combined) {
        if((h = hist("HLT_muonpt") )) h->Fill(muon->pt()/CLHEP::GeV);
        muonMult += 1;
      }

      // If a muon in this event satifies the requirements, we will fill in MET for this event.
      if(muon->muonType() == xAOD::Muon::Combined && (muon->pt()/CLHEP::GeV > m_muon_pt_thresh)) {
	    METMuonFilled = true;
      }
    }
  } else {
    ATH_MSG_DEBUG("HLT EF Muon container DNE or is empty");
    //return StatusCode::SUCCESS;
  }

  // Fill in MET if we found a muon in this event that satifies the requirements
  if(METMuonFilled == true) {
    if((h = hist("HLT_MET_mu") )) h->Fill(hlt_met);
    if((h = hist("HLT_MEx_mu") )) h->Fill(hlt_ex);
    if((h = hist("HLT_MEy_mu") )) h->Fill(hlt_ey);
    if((h = hist("HLT_SumEt_mu") )) h->Fill(hlt_sumet);
    if((h = hist("HLT_MET_phi_mu") )) h->Fill(hlt_phi);
    if((h = hist("HLT_MET_phi_etweight_mu") )) h->Fill(hlt_phi,hlt_met);
    if((h = hist("HLT_MET_log_mu") )) h->Fill(hlt_met_log);
    if((h = hist("HLT_SumEt_log_mu") )) h->Fill(hlt_sumet_log);
    if((h = hist("HLT_MEz_mu") )) h->Fill(hlt_ez);
    if((h = hist("HLT_SumE_mu") )) h->Fill(hlt_sume);

    TProfile *pmu(0);
      if (getTDT()->isPassed("HLT_xe80", TrigDefs::eventAccepted)) {
          if ((pmu = profile("Eff_muxe80"))) pmu->Fill(off_met, 1.0, 1.0);
      } else {
          if ((pmu = profile("Eff_muxe80"))) pmu->Fill(off_met, 0.0, 1.0);
      }

  }

  if (muonContEmpty == false) {
        ATH_MSG_DEBUG("Muon multiciplicity: " << muonMult); 
        if((h = hist("HLT_muonmult") )) h->Fill(muonMult);
  }

  ATH_MSG_DEBUG("Going to iterate through electron container");
  bool METElectronFilled = false;
  bool electronEtaReq = false;
  int electronMult = 0;
  bool electronContEmpty = true;
  if (m_hlt_electronEFcontainer && m_hlt_electronEFcontainer->size()>0) {
    ATH_MSG_DEBUG("HLT EF electron container exists and is not empty. Going to loop over container.");
    electronContEmpty = false;
    for (auto eg : *m_hlt_electronEFcontainer) {
	  //ATH_MSG_INFO("--------------");
	  //ATH_MSG_INFO("electron found in electronEFContainer");
	  
	  //ATH_MSG_INFO("Electron Pt: " << eg->pt()/CLHEP::GeV << " Eta: " << eg->eta() << " Phi: " << eg->phi());
      
      //unsigned int isEMbit = 0;
      //ATH_MSG_INFO("Electron is loose: " << eg->selectionisEM(isEMbit,"isEMLoose"));
      //ATH_MSG_INFO("Electron is medium: " << eg->selectionisEM(isEMbit,"isEMMedium"));
      //ATH_MSG_INFO("Electron is tight: " << eg->selectionisEM(isEMbit,"isEMTight"));
      
      electronEtaReq = ( fabsf(eg->eta()) < 1.37 || fabsf(eg->eta()) > 1.52 ) && fabsf(eg->eta()) < 2.47; 
      
      if (eg->trackParticle()) {
          if((h = hist("HLT_electronpt") )) h->Fill(eg->pt()/CLHEP::GeV);
          electronMult += 1;
      }

      if (eg->pt()/CLHEP::GeV > m_electron_pt_thresh && electronEtaReq && eg->trackParticle()) {
        METElectronFilled = true;
      }
    }
  } else {
    ATH_MSG_DEBUG("HLT EF electron container DNE or is empty");
    //return StatusCode::SUCCESS;
  }
  
  if(METElectronFilled == true) {
    if((h = hist("HLT_MET_e") )) h->Fill(hlt_met);
    if((h = hist("HLT_MEx_e") )) h->Fill(hlt_ex);
    if((h = hist("HLT_MEy_e") )) h->Fill(hlt_ey);
    if((h = hist("HLT_SumEt_e") )) h->Fill(hlt_sumet);
    if((h = hist("HLT_MET_phi_e") )) h->Fill(hlt_phi);
    if((h = hist("HLT_MET_phi_etweight_e") )) h->Fill(hlt_phi,hlt_met);
    if((h = hist("HLT_MET_log_e") )) h->Fill(hlt_met_log);
    if((h = hist("HLT_SumEt_log_e") )) h->Fill(hlt_sumet_log);
    if((h = hist("HLT_MEz_e") )) h->Fill(hlt_ez);
    if((h = hist("HLT_SumE_e") )) h->Fill(hlt_sume);

     TProfile *pele(0);
      if (getTDT()->isPassed("HLT_xe80", TrigDefs::eventAccepted)) {
          if ((pele = profile("Eff_elexe80"))) pele->Fill(off_met, 1.0, 1.0);
      } else {
          if ((pele = profile("Eff_elexe80"))) pele->Fill(off_met, 0.0, 1.0);
      }
  }
  
    if (electronContEmpty == false) {
        ATH_MSG_DEBUG("Electron multiciplicity: " << electronMult);
        if((h = hist("HLT_electronmult") )) h->Fill(electronMult);
    }

  // End of signal-like selection histograms #################
  //////////////////////////////////////////

  if (!m_make_expert_histograms)
    return sc;

  //########################
  //-- Fill Expert histograms  ###############################
  //########################


  if (m_debuglevel) 
    ATH_MSG_DEBUG("filling Expert histograms");

  /////////////
  /// L1
  /////////////
  setCurrentMonGroup(m_expert_path+"/L1");
  
  if (l1_met > epsilon_l1met) {
    if ((h = hist("L1_METx_log")))  h->Fill(l1_mex_log);
    if ((h = hist("L1_METy_log")))  h->Fill(l1_mey_log);
    if ((h = hist("L1_MET")))       h->Fill(l1_met);
    if ((h = hist("L1_MET_phi")))   h->Fill(l1_phi);
  }

  //////////////////////////
  /// L1 Triggers
  //////////////////////////
  std::string expert_partial_path = m_expert_path + "/L1/";
  std::map<std::string,int>::const_iterator it;
  for (it = m_l1_met_signatures_tolook.begin(); it != m_l1_met_signatures_tolook.end(); it++) {
    std::string name = it->first;
    ATH_MSG_DEBUG("TriggerLoop: name = " << name);
    //if (name == m_denominator_trigger) continue;

    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      ATH_MSG_DEBUG("Passed L1 Trigger : " << name);

      setCurrentMonGroup(expert_partial_path+name);
      if ((h = hist("L1_MET")))        h->Fill(l1_met);
      if ((h = hist("L1_METx")))       h->Fill(l1_mex);
      if ((h = hist("L1_METy")))       h->Fill(l1_mey);
      if ((h = hist("L1_SumEt")))      h->Fill(l1_sumet);
      if ((h = hist("L1_MET_phi")))    h->Fill(l1_phi);
      if ((h = hist("L1_MET_phi_etweight")))   h->Fill(l1_phi, l1_met);
      if ((h = hist("L1_MET_log")))    h->Fill(l1_met_log);
      if ((h = hist("L1_METx_log")))   h->Fill(l1_mex_log);
      if ((h = hist("L1_METy_log")))   h->Fill(l1_mey_log);
      if ((h = hist("L1_SumEt_log")))  h->Fill(l1_sumet_log);

    }
  }
      
  //////////////////////////
  // fill HLT histograms without trigger requirement
  //////////////////////////

  setCurrentMonGroup(m_expert_path+"/HLT");
  if (m_hlt_met) {
    if ((h = hist("HLT_MEx_log"))) h->Fill(hlt_ex_log);
    if ((h = hist("HLT_MEy_log"))) h->Fill(hlt_ey_log);
    if ((h = hist("HLT_MET_lin1"))) h->Fill(hlt_met);
    if ((h = hist("HLT_MET_phi_etweight"))) h->Fill(hlt_phi, hlt_met);
    if ((h = hist("HLT_MEz_log")))  h->Fill(hlt_ez_log);
    if ((h = hist("HLT_SumE_log"))) h->Fill(hlt_sume_log);
    if ((h = hist("HLT_XS"))) h->Fill(hlt_significance);
  }

  //////////////////////////
  // fill in HLT status and component histograms without trigger requirement
  //////////////////////////
  const xAOD::TrigMissingET *missETEF = 0;

  if (m_hlt_met_cont && m_hlt_met_cont->size()) {

      missETEF = *(m_hlt_met_cont->begin());

      //////////////////////////
      // status histogram
      TH1F *h1i(0);
      bool m_fill_stat = false;
      if((h1i = (TH1F *) hist("HLT_MET_status"))) m_fill_stat = true;
      for (int i=0; i<32; ++i) {
          unsigned mask = (1<<i);
          if (missETEF->flag() & mask) {
              if(m_fill_stat && h1i) h1i->Fill(i,1.);
              //m_status_flag[i] = 1;
          } else {
              if(m_fill_stat && h1i) h1i->Fill(i,0);
              //m_status_flag[i] = 0;
          }
      }

      //////////////////////////
      // component histograms
      unsigned int Nc = missETEF->getNumberOfComponents();
      unsigned int compNc = m_compNames.size();
      Nc = Nc > compNc ? compNc : Nc;

      if (Nc > 0) {
        for (unsigned int i = 0; i < Nc; ++i) { // loop over components
           float ex =                missETEF->exComponent(i)/CLHEP::GeV;
           float ey =                missETEF->eyComponent(i)/CLHEP::GeV;
           float ez =                missETEF->ezComponent(i)/CLHEP::GeV;
           float et =                sqrt(ex*ex+ey*ey);
           float sumE =              missETEF->sumEComponent(i)/CLHEP::GeV;
           float sumEt =             missETEF->sumEtComponent(i)/CLHEP::GeV;

           float m_comp_ex_log = -9e9;
           float m_comp_ey_log = -9e9;
           float m_comp_ez_log = -9e9;
           float m_comp_et_log = -9e9;
           float m_comp_sum_et_log = -9e9;
           float m_comp_sum_e_log = -9e9;
           float m_comp_sum_et_lin = -9e9;
           float m_comp_sum_e_lin = -9e9;
           float m_comp_et_lin = -9e9;

           if (fabsf(ex)>epsilon)
             m_comp_ex_log = copysign(log10f(fabsf(ex)), ex);
           else
             m_comp_ex_log = 0.01;

           if (fabsf(ey)>epsilon)
             m_comp_ey_log = copysign(log10f(fabsf(ey)), ey);
           else
             m_comp_ey_log = 0.01;

           if (fabsf(ez)>epsilon)
             m_comp_ez_log = copysign(log10f(fabsf(ez)), ez);
           else
             m_comp_ez_log = 0.01;

           m_comp_et_lin = et;
           if (et>1e-6) // i.e. et > 0
             m_comp_et_log = log10f(et);
           else
             m_comp_et_log = -100; // underflow

           m_comp_sum_et_lin = sumEt;
           m_comp_sum_e_lin = sumE;
          
           if (fabsf(sumEt)>epsilon)
             m_comp_sum_et_log = copysign(log10f(fabsf(sumEt)), sumEt);
           else
             m_comp_sum_et_log = 0.01;
           if (fabsf(sumE)>epsilon)
             m_comp_sum_e_log = copysign(log10f(fabsf(sumE)), sumE);
           else
             m_comp_sum_e_log = 0.01;

           if((h2 = hist2("compN_compEx")))      h2->Fill(i,m_comp_ex_log);
           if((h2 = hist2("compN_compEy")))      h2->Fill(i,m_comp_ey_log);
           if((h2 = hist2("compN_compEz")))      h2->Fill(i,m_comp_ez_log);
           if((h2 = hist2("compN_compEt")))      h2->Fill(i,m_comp_et_log);
           if((h2 = hist2("compN_compSumEt")))      h2->Fill(i,m_comp_sum_et_log);
           if((h2 = hist2("compN_compSumE")))      h2->Fill(i,m_comp_sum_e_log);
           if((h2 = hist2("compN_compEt_lin")))      h2->Fill(i,m_comp_et_lin);
           if((h2 = hist2("compN_compSumEt_lin")))      h2->Fill(i,m_comp_sum_et_lin);
          
           bool m_fill_stat = false;
           if((h2 = hist2("compN_HLT_MET_status"))) m_fill_stat = true;
           // loop over status flags
           for (int j=0; j<32; ++j) {
             unsigned mask = (1<<j);
             if (missETEF->statusComponent(i) & mask) {
               if(m_fill_stat && h2) h2->Fill(i,j);
             }
           }
         
           float nchan_used = fabsf(missETEF->usedChannelsComponent(i));
           if(nchan_used > epsilon) nchan_used = log10f(nchan_used);
           else nchan_used = -1.;
           if((h2 = hist2("compN_HLT_usedChannels"))) h2->Fill(i,nchan_used);

           CLHEP::Hep3Vector v(ex,ey,ez);
           double eta = v.eta();
           double phi = v.phi();

           if((h2 = hist2(Form("compN_EtaPhi_%02d", i))))      h2->Fill(eta, phi);
           if((h2 = hist2(Form("compEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_et_lin);
           if((h2 = hist2(Form("compSumEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_sum_et_lin);
           if((h2 = hist2(Form("compSumE_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, m_comp_sum_e_lin);
        }
      }
  }
  // End of compN histograms
  //////////////////////////


  //////////////////////////
  // fill HLT histograms passing monitored triggers
  //////////////////////////
  expert_partial_path = m_expert_path + "/HLT/";
  for (it = m_hlt_met_signatures_tolook.begin(); it != m_hlt_met_signatures_tolook.end(); it++) {
    
    std::string name = it->first;
    ATH_MSG_DEBUG("Alg : Cell, Trig : " << name);
    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      setCurrentMonGroup(expert_partial_path+name);

      m_hlt_met = 0;
      if (m_hlt_met_cont && m_hlt_met_cont->size()) {
	m_hlt_met = m_hlt_met_cont->at(0);
      }

      if (m_hlt_met) {  
	
        float tmp_hlt_ex = m_hlt_met->ex()/CLHEP::GeV; 
        float tmp_hlt_ey = m_hlt_met->ey()/CLHEP::GeV; 
        float tmp_hlt_ez = m_hlt_met->ez()/CLHEP::GeV;
        float tmp_hlt_met = sqrt(hlt_ex*hlt_ex+hlt_ey*hlt_ey); 
        //float tmp_hlt_me  = sqrt(hlt_ex*hlt_ex+hlt_ey*hlt_ey+hlt_ez*hlt_ez);
        float tmp_hlt_sumet = m_hlt_met->sumEt()/CLHEP::GeV;
        float tmp_hlt_sume  = m_hlt_met->sumE()/CLHEP::GeV; 

        CLHEP::Hep3Vector v(hlt_ex, hlt_ey, hlt_ez);
        //float tmp_hlt_eta = v.eta();
        float tmp_hlt_phi = v.phi();
    
        float tmp_hlt_ex_log = -9e9;
        float tmp_hlt_ey_log = -9e9;
        float tmp_hlt_ez_log = -9e9;
        float tmp_hlt_met_log = -9e9;
        float tmp_hlt_sume_log = -9e9;
        float tmp_hlt_sumet_log = -9e9;
        //float tmp_hlt_me_log = -9e9;

        float epsilon = 1e-6;  // 1 keV
        //if (tmp_hlt_me > epsilon)  tmp_hlt_me_log  = log10(fabsf(tmp_hlt_me)); // underflow otherwise

        epsilon = 1.189;
        tmp_hlt_ex_log = signed_log(tmp_hlt_ex, epsilon);
        tmp_hlt_ey_log = signed_log(tmp_hlt_ey, epsilon);
        tmp_hlt_ez_log = signed_log(tmp_hlt_ez, epsilon);
        tmp_hlt_met_log = signed_log(tmp_hlt_met, epsilon);
        tmp_hlt_sume_log = signed_log(tmp_hlt_sume, epsilon);
        tmp_hlt_sumet_log = signed_log(tmp_hlt_sumet, epsilon);
    
        if ((h = hist("HLT_MEx")))       h->Fill(tmp_hlt_ex);
        if ((h = hist("HLT_MEy")))       h->Fill(tmp_hlt_ey);
        if ((h = hist("HLT_MEz")))       h->Fill(tmp_hlt_ez);
        if ((h = hist("HLT_MET")))       h->Fill(tmp_hlt_met);
        if ((h = hist("HLT_MET_lin1")))  h->Fill(tmp_hlt_met);
        if ((h = hist("HLT_SumEt")))     h->Fill(tmp_hlt_sumet);
        if ((h = hist("HLT_MET_phi")))   h->Fill(tmp_hlt_phi);
        if ((h = hist("HLT_MET_phi_etweight")))  h->Fill(tmp_hlt_phi, hlt_met); 
        if ((h = hist("HLT_MEx_log")))   h->Fill(tmp_hlt_ex_log);
        if ((h = hist("HLT_MEy_log")))   h->Fill(tmp_hlt_ey_log);
        if ((h = hist("HLT_MEz_log")))   h->Fill(tmp_hlt_ez_log);
        if ((h = hist("HLT_MET_log")))   h->Fill(tmp_hlt_met_log);
        if ((h = hist("HLT_SumEt_log"))) h->Fill(tmp_hlt_sumet_log);
        if ((h = hist("HLT_SumE")))      h->Fill(tmp_hlt_sume);
        if ((h = hist("HLT_SumE_log")))  h->Fill(tmp_hlt_sume_log);
	if ((h2 = hist2("HLT_MET_etaphi"))) h2->Fill(hlt_eta, hlt_phi);
	if ((h2 = hist2("HLT_MET_etaphi_etweight"))) h2->Fill(hlt_eta, hlt_phi, hlt_met);
	if ((h = hist("HLT_XS"))) h->Fill(hlt_significance);

	// status histogram
	missETEF = *(m_hlt_met_cont->begin());
	//ATH_MSG_INFO("missETEF = " << missETEF);
	//ATH_MSG_INFO("missETEF flag = " << missETEF->flag());
        TH1F *h1i(0);
        bool m_fill_stat = false;
        if((h1i = (TH1F *) hist("HLT_MET_status"))) m_fill_stat = true;
        for (int i=0; i<32; ++i) {
          unsigned mask = (1<<i);
          if (missETEF->flag() & mask) {
              if(m_fill_stat && h1i) h1i->Fill(i,1.);
	      ATH_MSG_DEBUG("missETEF flag Trig = " << missETEF->flag());
              //m_status_flag[i] = 1;
          } else {
              if(m_fill_stat && h1i) h1i->Fill(i,0);
              //m_status_flag[i] = 0;
          }
        }

      }

    }
  }
  // End of trigger specific histos
  //////////////////////////


  //////////////////////////
  // fill HLT histograms with alternative alg
  //////////////////////////
  expert_partial_path = m_expert_path + "/HLT/";
  for (std::vector<std::string>::iterator it2 = m_monitoring_alg.begin(); it2 != m_monitoring_alg.end(); it2++) {
   
    std::string name = *it2;
    setCurrentMonGroup(expert_partial_path+name);
    ATH_MSG_DEBUG("Histogram Folder : " << expert_partial_path+name);

    std::string algo = get_trigger_algo(name);
    ATH_MSG_DEBUG("Alternative Selected Alg : " << algo);
    const xAOD::TrigMissingETContainer *m_hlt_tmp_met_cont = 0;
    if (algo.empty() && m_hlt_met_cont && m_hlt_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : Cell");
      m_hlt_tmp_met_cont = m_hlt_met_cont;
      m_hlt_met = m_hlt_met_cont->at(0);
    } else if (algo == "mht" && m_hlt_mht_met_cont && m_hlt_mht_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : MHT");
      m_hlt_tmp_met_cont = m_hlt_mht_met_cont;
      m_hlt_met = m_hlt_mht_met_cont->at(0);
    } else if (algo == "topocl" && m_hlt_topocl_met_cont && m_hlt_topocl_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL");
      m_hlt_tmp_met_cont = m_hlt_topocl_met_cont;
      m_hlt_met = m_hlt_topocl_met_cont->at(0);
    } else if (algo == "topocl_PS" && m_hlt_topocl_PS_met_cont && m_hlt_topocl_PS_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL_PS");
      m_hlt_tmp_met_cont = m_hlt_topocl_PS_met_cont;
      m_hlt_met = m_hlt_topocl_PS_met_cont->at(0);
    } else if (algo == "topocl_PUC" && m_hlt_topocl_PUC_met_cont && m_hlt_topocl_PUC_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL_PUC");
      m_hlt_tmp_met_cont = m_hlt_topocl_PUC_met_cont;
      m_hlt_met = m_hlt_topocl_PUC_met_cont->at(0);
    } else if (algo == "FEB" && m_hlt_FEB_met_cont && m_hlt_FEB_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : FEB");
      m_hlt_tmp_met_cont = m_hlt_FEB_met_cont;
      m_hlt_met = m_hlt_FEB_met_cont->at(0);
    } else if (algo == "Fex" && m_hlt_Fex_met_cont && m_hlt_Fex_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : FEX");
      m_hlt_tmp_met_cont = m_hlt_Fex_met_cont;
      m_hlt_met = m_hlt_Fex_met_cont->at(0);
    } else {
      ATH_MSG_DEBUG("Alternative Alg : NONE");
      m_hlt_tmp_met_cont = 0;
      m_hlt_met = 0;
    }
    ATH_MSG_DEBUG("m_hlt_met = " << m_hlt_met);
    if (m_hlt_met_cont && m_hlt_met_cont->size()) {
      ATH_MSG_DEBUG("m_hlt_met cell = " << m_hlt_met_cont->at(0));
      ATH_MSG_DEBUG("CELL MET Ex = " << m_hlt_met_cont->at(0)->ex()/CLHEP::GeV);
      ATH_MSG_DEBUG("MHT  MET Ex = " << m_hlt_mht_met_cont->at(0)->ex()/CLHEP::GeV);
    }
    if (m_hlt_topocl_met_cont && m_hlt_topocl_met_cont->size()) {
      ATH_MSG_DEBUG("CL   MET Ex = " << m_hlt_topocl_met_cont->at(0)->ex()/CLHEP::GeV);
    }
    if (m_hlt_topocl_PS_met_cont && m_hlt_topocl_PS_met_cont->size()) {
      ATH_MSG_DEBUG("PS   MET Ex = " << m_hlt_topocl_PS_met_cont->at(0)->ex()/CLHEP::GeV);
    }
    if (m_hlt_topocl_PUC_met_cont && m_hlt_topocl_PUC_met_cont->size()) {
      ATH_MSG_DEBUG("PUC  MET Ex = " << m_hlt_topocl_PUC_met_cont->at(0)->ex()/CLHEP::GeV);
    }
    if (m_hlt_FEB_met_cont && m_hlt_FEB_met_cont->size()) {
      ATH_MSG_DEBUG("FEB  MET Ex = " << m_hlt_FEB_met_cont->at(0)->ex()/CLHEP::GeV);
    }
    
    if (m_hlt_met) {  
      
      float tmp_hlt_ex = m_hlt_met->ex()/CLHEP::GeV; 
      float tmp_hlt_ey = m_hlt_met->ey()/CLHEP::GeV; 
      float tmp_hlt_ez = m_hlt_met->ez()/CLHEP::GeV;
      float tmp_hlt_met = sqrt(tmp_hlt_ex*tmp_hlt_ex+tmp_hlt_ey*tmp_hlt_ey); 
      float tmp_hlt_sumet = m_hlt_met->sumEt()/CLHEP::GeV;
      float tmp_hlt_sume  = m_hlt_met->sumE()/CLHEP::GeV; 
      ATH_MSG_DEBUG("Alternative Ex = " << tmp_hlt_ex);
      ATH_MSG_DEBUG("Alternative Ey = " << tmp_hlt_ey);
      ATH_MSG_DEBUG("Alternative MET = " << tmp_hlt_met);
      
      CLHEP::Hep3Vector v(hlt_ex, hlt_ey, hlt_ez);
      //float tmp_hlt_eta = v.eta();
      float tmp_hlt_phi = v.phi();
      
      float tmp_hlt_ex_log = -9e9;
      float tmp_hlt_ey_log = -9e9;
      float tmp_hlt_ez_log = -9e9;
      float tmp_hlt_met_log = -9e9;
      float tmp_hlt_sume_log = -9e9;
      float tmp_hlt_sumet_log = -9e9;
      //float tmp_hlt_me_log = -9e9;
      
      float epsilon = 1e-6;  // 1 keV
      //if (tmp_hlt_me > epsilon)  tmp_hlt_me_log  = log10(fabsf(tmp_hlt_me)); // underflow otherwise
      
      epsilon = 1.189;
      tmp_hlt_ex_log = signed_log(tmp_hlt_ex, epsilon);
      tmp_hlt_ey_log = signed_log(tmp_hlt_ey, epsilon);
      tmp_hlt_ez_log = signed_log(tmp_hlt_ez, epsilon);
      tmp_hlt_met_log = signed_log(tmp_hlt_met, epsilon);
      tmp_hlt_sume_log = signed_log(tmp_hlt_sume, epsilon);
      tmp_hlt_sumet_log = signed_log(tmp_hlt_sumet, epsilon);
      
      if ((h = hist("HLT_MEx")))       h->Fill(tmp_hlt_ex);
      if ((h = hist("HLT_MEy")))       h->Fill(tmp_hlt_ey);
      if ((h = hist("HLT_MEz")))       h->Fill(tmp_hlt_ez);
      if ((h = hist("HLT_MET")))       h->Fill(tmp_hlt_met);
      if ((h = hist("HLT_MET_lin1")))  h->Fill(tmp_hlt_met);
      if ((h = hist("HLT_SumEt")))     h->Fill(tmp_hlt_sumet);
      if ((h = hist("HLT_MET_phi")))   h->Fill(tmp_hlt_phi);
      if ((h = hist("HLT_MET_phi_etweight")))  h->Fill(tmp_hlt_phi, hlt_met); 
      if ((h = hist("HLT_MEx_log")))   h->Fill(tmp_hlt_ex_log);
      if ((h = hist("HLT_MEy_log")))   h->Fill(tmp_hlt_ey_log);
      if ((h = hist("HLT_MEz_log")))   h->Fill(tmp_hlt_ez_log);
      if ((h = hist("HLT_MET_log")))   h->Fill(tmp_hlt_met_log);
      if ((h = hist("HLT_SumEt_log"))) h->Fill(tmp_hlt_sumet_log);
      if ((h = hist("HLT_SumE")))      h->Fill(tmp_hlt_sume);
      if ((h = hist("HLT_SumE_log")))  h->Fill(tmp_hlt_sume_log);
      if ((h2 = hist2("HLT_MET_etaphi"))) h2->Fill(hlt_eta, hlt_phi);
      if ((h2 = hist2("HLT_MET_etaphi_etweight"))) h2->Fill(hlt_eta, hlt_phi, hlt_met);
      if ((h = hist("HLT_XS"))) h->Fill(hlt_significance);

      // status histogram
      missETEF = *(m_hlt_tmp_met_cont->begin());
      //ATH_MSG_INFO("missETEF = " << missETEF);
      //ATH_MSG_INFO("missETEF flag = " << missETEF->flag());
      TH1F *h1i(0);
      bool m_fill_stat = false;
      if((h1i = (TH1F *) hist("HLT_MET_status"))) m_fill_stat = true;
      for (int i=0; i<32; ++i) {
	unsigned mask = (1<<i);
	if (missETEF->flag() & mask) {
	  //std::cout << "missETEF flag Alg = " << missETEF->flag() << std::endl;
	  if(m_fill_stat && h1i) h1i->Fill(i,1.);
	  //m_status_flag[i] = 1;
	} else {
	  if(m_fill_stat && h1i) h1i->Fill(i,0);
	  //m_status_flag[i] = 0;
	}
      }
    }

  }
  // End of alternative alg histograms
  //////////////////////////

  
  //////////////////////////
  // Offline MET
  //////////////////////////
  setCurrentMonGroup(m_expert_path+"/Offline");
  if (m_off_met) {
    if ((h = hist("Offline_MET")))    h->Fill(off_met);
    if ((h = hist("Offline_METx")))   h->Fill(off_ex);
    if ((h = hist("Offline_METy")))   h->Fill(off_ey);
    if ((h = hist("Offline_SumEt")))   h->Fill(off_sumet);
    if ((h = hist("Offline_MET_phi")))   h->Fill(off_phi);
  }
  
  // fill correlation histograms -- L1 vs HLT
  //ATH_MSG_INFO("Aqui: " << name);
  setCurrentMonGroup(m_expert_path+"/L1_vs_HLT");
  if (m_l1_roi_cont) {
    if ((h2 = hist2("L1_HLT_EtCor")))     h2->Fill(hlt_met, l1_met);
    if ((h2 = hist2("L1_HLT_SumEtCor")))  h2->Fill(hlt_sumet, l1_sumet);
    if ((h2 = hist2("L1_HLT_PhiCor")))    h2->Fill(hlt_phi, l1_phi);
    
    if ((h = hist("L1_HLT_dEt")))         h->Fill(l1_met - hlt_met);
    double dphi = signed_delta_phi(l1_phi, hlt_phi);
    if ((h = hist("L1_HLT_dPhi")))  h->Fill(dphi);
    if ((h = hist("L1_HLT_dEx")))   h->Fill(l1_mex - hlt_ex);
    if ((h = hist("L1_HLT_dEy")))   h->Fill(l1_mey - hlt_ey); 
  }
  
  // fill correlation histograms -- HLT vs Off
  setCurrentMonGroup(m_expert_path+"/HLT_vs_Offline");
  if (m_off_met) {
    if ((h2 = hist2("HLT_Off_EtCor")))    h2->Fill(off_met, hlt_met);
    if ((h2 = hist2("HLT_Off_SumEtCor"))) h2->Fill(off_sumet, hlt_sumet);
    if ((h2 = hist2("HLT_Off_PhiCor")))   h2->Fill(off_phi, hlt_phi);
    
    if ((h = hist("HLT_Off_dEt")))        h->Fill(hlt_met - off_met);
    double dphi = signed_delta_phi(hlt_phi, off_phi);
    if ((h = hist("HLT_Off_dPhi")))  h->Fill(dphi);
    if ((h = hist("HLT_Off_dEx")))   h->Fill(hlt_ex - off_ex);
    if ((h = hist("HLT_Off_dEy")))   h->Fill(hlt_ey - off_ey);
  }

  return sc;
}

//___________________________________________________________________________________________________________
std::string HLTMETMonTool::get_trigger_level(std::string item)
{
  size_t n = item.find_first_of('_');
  return item.substr(0, n);
}

std::string HLTMETMonTool::get_trigger_algo(std::string item)
{
  std::string algo = "";
  if (item.find("mht") != std::string::npos)
    algo = "mht";
  else if (item.find("tc_lcw") != std::string::npos)
    algo = "topocl";
  else if (item.find("pueta") != std::string::npos)
    algo = "topocl_PS";
  else if (item.find("pufit") != std::string::npos)
    algo = "topocl_PUC";
  else if (item.find("feb") != std::string::npos)
    algo = "FEB";
  else if (item.find("fex") != std::string::npos)
    algo = "Fex";
  
  return algo;
}

void HLTMETMonTool::check_triggers(std::vector<std::string>& m_triggers, std::map<std::string,int> &m_signatures) {

  const std::vector<std::string> trig_L1_items  = getTDT()->getListOfTriggers("L1_.*");
  const std::vector<std::string> trig_EF_items  = getTDT()->getListOfTriggers("EF_.*");
  const std::vector<std::string> trig_HLT_items = getTDT()->getListOfTriggers("HLT_.*");

  std::vector<std::string>::const_iterator iter;

  for (unsigned int it = 0; it < m_triggers.size(); ++it) { // Loop over trigger items

    std::string item = m_triggers[it];
    std::string trig_level = get_trigger_level(item); //item.substr(0,3);

    if (m_debuglevel)
      ATH_MSG_DEBUG("signature " << item << " requested");

    // check your trigger is defined in the menu
    bool triggerFound = false;
    if (trig_level == "L1") {
      for (iter=trig_L1_items.begin(); iter!=trig_L1_items.end(); ++iter) {
        std::string L1name = *iter;
        if (L1name == item) {
          triggerFound = true;
          m_signatures.insert(std::map<std::string,int>::value_type(item, 0));
        }
      }
      if (!triggerFound) {
        ATH_MSG_DEBUG("L1 signature " << item << " requested but not found in the menu");
      }
    }
    else if (trig_level == "EF") {
      for (iter=trig_EF_items.begin(); iter!=trig_EF_items.end(); ++iter) {
        std::string EFname = *iter;
        if (EFname == item) {
          triggerFound = true;
          m_signatures.insert(std::map<std::string,int>::value_type(item, 0));
        }
      }
      if (!triggerFound)
        ATH_MSG_DEBUG("HLT signature " << item << " requested but not found in the menu");
    }
    else if (trig_level == "HLT") {
      for (iter=trig_HLT_items.begin(); iter!=trig_HLT_items.end(); ++iter) {
        std::string HLTname = *iter;
        if (HLTname == item) {
          triggerFound = true;
          m_signatures.insert(std::map<std::string,int>::value_type(item, 0));
        }
      }
      if (!triggerFound)
        ATH_MSG_DEBUG("HLT signature " << item << " requested but not found in the menu");
    }
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::print_trigger_stats() {

  // L1
  ATH_MSG_DEBUG("Level-1 Met Triggers to look: " << m_l1_met_signatures_tolook.size());
  std::map<std::string,int>::iterator iter;
  for (iter = m_l1_met_signatures_tolook.begin(); iter!= m_l1_met_signatures_tolook.end(); ++iter) {
    ATH_MSG_DEBUG("Events passing " << iter->first << ": " << iter->second);
  }

  // EF
  ATH_MSG_DEBUG("HLT Met Triggers to look: "  << m_hlt_met_signatures_tolook.size());
  for (iter = m_hlt_met_signatures_tolook.begin(); iter != m_hlt_met_signatures_tolook.end(); ++iter) {
    ATH_MSG_DEBUG("Events passing " << iter->first << ": " << iter->second);
  }
}


//___________________________________________________________________________________________________________
double HLTMETMonTool::signed_delta_phi(double phi1, double phi2) {


  double phia = phi1;
  double phib = phi2;
  if (phi1>M_PI) phia = phi1 - 2*M_PI;
  if (phi2>M_PI) phib = phi2 - 2*M_PI;

  double dphi = phia - phib;
  if (dphi>M_PI) dphi -= 2*M_PI;
  if (dphi<(-1.*M_PI)) dphi += 2*M_PI;
  return dphi;
}

//___________________________________________________________________________________________________________
 double HLTMETMonTool::signed_log(double e, double epsilon=1e-6) {


  double e_log = -9e9;
  if (fabsf(e) > epsilon)
    e_log = copysign(log10(fabsf(e)), e);
  else
    e_log = 0.01;

  return e_log;
}
