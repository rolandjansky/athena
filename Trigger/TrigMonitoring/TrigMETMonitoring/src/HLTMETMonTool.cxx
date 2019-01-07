/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMETMonitoring/HLTMETMonTool.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODEventInfo/EventInfo.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>

#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

//___________________________________________________________________________________________________________
HLTMETMonTool::HLTMETMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : IHLTMonTool(type, name, parent)
{

  ATH_MSG_DEBUG("in HLTMETMonTool::HLTMETMonTool()");

  declareProperty("MonPathBase", m_mon_path="/HLT/METMon");
  
  declareProperty("print_trigger_stats", m_print_met_trig_stats=true);
  declareProperty("make_expert_histograms", m_make_expert_histograms=true);

  // Triggers to monitor
  declareProperty("primary_met", m_primary_met);
  declareProperty("monitoring_met_shifter", m_monitoring_met_shifter);
  declareProperty("monitoring_met_expert", m_monitoring_met_expert);
  declareProperty("monitoring_alg", m_monitoring_alg);
  declareProperty("monitoring_alg_shifter", m_monitoring_alg_shifter);
  declareProperty("monitoring_alg_expert", m_monitoring_alg_expert);
  declareProperty("prescaled_met", m_prescaled_met);

  // Jet keys
  declareProperty("l1_jet_key", m_lvl1_jet_roi_key="LVL1JetRoIs");

  // Met keys
  declareProperty("l1_key", m_lvl1_roi_key="LVL1EnergySumRoI");
  declareProperty("hlt_cell_key", m_hlt_cell_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET");
  declareProperty("hlt_mht_key", m_hlt_mht_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht");
  declareProperty("hlt_mhtem_key", m_hlt_mhtem_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht_em");
  declareProperty("hlt_trkmht_key", m_hlt_trkmht_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_trkmht");
  declareProperty("hlt_trkmhtFTK_key", m_hlt_trkmhtFTK_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_trkmhtFTK");
  declareProperty("hlt_topocl_key", m_hlt_topocl_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl");
  declareProperty("hlt_topocl_PS_key", m_hlt_topocl_PS_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS");
  declareProperty("hlt_topocl_PUC_key", m_hlt_topocl_PUC_met_key="HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC");
  declareProperty("hlt_FEB_key", m_hlt_FEB_met_key="HLT_xAOD__TrigMissingETContainer_TrigL2MissingET_FEB");
  declareProperty("hlt_Fex_key", m_hlt_Fex_met_key="HLT_xAOD__TrigMissingETContainer_EFMissingET_Fex_2sidednoiseSupp_PUC");
  declareProperty("off_key", m_off_met_key="MET_Reference_AntiKt4LCTopo");  

  // Muons keys
  declareProperty("muon_key", m_muon_key="HLT_xAOD__MuonContainer_MuonEFInfo");
  declareProperty("muon_base", m_muon_base_trigger="HLT_mu6");
  declareProperty("muon_pt_thresh", m_muon_pt_thresh=18.0);

  // Electron keys
  declareProperty("electron_key", m_electron_key="HLT_xAOD__ElectronContainer_egamma_Electrons");
  declareProperty("electron_base", m_electron_base_trigger="HLT_e5_lhloose");
  declareProperty("electron_pt_thresh", m_electron_pt_thresh=20.0);

  //
  declareProperty("comp_names", m_compNames);
  declareProperty("bit_names", m_bitNames);

  // histograms limits
  // et
  declareProperty("et_bins", m_et_bins=205,  "Number of bins Et");
  declareProperty("et_min",  m_et_min=-13.5, "Minimum Et");
  declareProperty("et_max",  m_et_max=401.5, "Maximum Et");
  // Ex, Ey, Ez
  declareProperty("ec_bins", m_ec_bins=199,  "Number of bins Ex, Ey, Ez");
  declareProperty("ec_min",  m_ec_min=-298.5, "Minimum Ex, Ey, Ez");
  declareProperty("ec_max",  m_ec_max=298.5, "Maximum Ex, Ey, Ez");
  // sumet
  declareProperty("sumet_bins", m_sumet_bins=305,  "Number of bins SumEt");
  declareProperty("sumet_min",  m_sumet_min=-27.,  "Minimum SumEt");
  declareProperty("sumet_max",  m_sumet_max=4203., "Maximum SumEt");
  // sume
  declareProperty("sume_bins", m_sume_bins=153,  "Number of bins SumE");
  declareProperty("sume_min",  m_sume_min=-27.,  "Minimum SumE");
  declareProperty("sume_max",  m_sume_max=24003., "Maximum SumE");
  // phi, dphi
  declareProperty("phi_bins", m_phi_bins=32,     "Number of bins Phi, dPhi");
  declareProperty("phi_min",  m_phi_min=-3.1416, "Minimum Phi");
  declareProperty("phi_max",  m_phi_max=3.1416,  "Maximum Phi");
  // eta
  declareProperty("eta_bins", m_eta_bins=24,     "Number of bins Eta");
  declareProperty("eta_min",  m_eta_min=-4.8, "Minimum Eta");
  declareProperty("eta_max",  m_eta_max=4.8,  "Maximum Eta");
  // dEt, dEx, dEy
  declareProperty("det_bins", m_det_bins=100,  "Number of bins dEt, dEx, dEy");
  declareProperty("det_min",  m_det_min=-100., "Minimum dEt, dEx, dEy");
  declareProperty("det_max",  m_det_max=100.,  "Maximum dEt, dEx, dEy");
  // efficiency
  declareProperty("eff_bins", m_eff_bins=42,  "Number of bins Eff");
  declareProperty("eff_min",  m_eff_min=-13.5, "Minimum Eff");
  declareProperty("eff_max",  m_eff_max=406.5, "Maximum Eff");
  // xs
  declareProperty("xs_bins", m_xs_bins=40,  "Number of bins Xs");
  declareProperty("xs_min",  m_xs_min=-0.025, "Minimum Xs");
  declareProperty("xs_max",  m_xs_max=20.025, "Maximum Xs");

  // significance parameters
  declareProperty("sig_offset", m_sigOffset=4.93182 , "Resolution offset");
  declareProperty("sig_slope",  m_sigSlope=0.442864 , "Resolution slope");
  declareProperty("sig_quadr",  m_sigQuadr=0.0 , "Resolution quad term");
  declareProperty("sig_offset_feb", m_sigOffset_feb=-1.886 , "Resolution offset");
  declareProperty("sig_slope_feb",  m_sigSlope_feb=1.15 , "Resolution slope");
  declareProperty("sig_quadr_feb",  m_sigQuadr_feb=0.0 , "Resolution quad term");
}


//___________________________________________________________________________________________________________
HLTMETMonTool::~HLTMETMonTool() {
  ATH_MSG_DEBUG("in HLTMETMonTool::~HLTMETMonTool()");
}


class High2LowByPt {
public:
  bool operator () (const TLorentzVector &a, const TLorentzVector &b) const {
    return (a.Perp()>b.Perp());
  }
};

//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::init() {

  ATH_MSG_DEBUG("in HLTMETMonTool::init()");
  ATH_MSG_DEBUG("HLTMETMonTool: L1Key  = "     << m_lvl1_roi_key);
  ATH_MSG_DEBUG("HLTMETMonTool: HLT_METKey = " << m_primary_met[0]);
  ATH_MSG_DEBUG("HLTMETMonTool: Off_METKey = " << m_off_met_key);

  // put all trigger names into one array for shifter
  for (std::vector<std::string>::iterator it = m_monitoring_met_shifter.begin(); it != m_monitoring_met_shifter.end(); ++it) {
    if (get_trigger_level(*it) == "L1")
      m_met_triggers_l1_shifter.push_back(*it);
    else if (get_trigger_level(*it) == "HLT")
      m_met_triggers_hlt_shifter.push_back(*it);
  }
  //put all trigger names into one array for expert 
  for (std::vector<std::string>::iterator it = m_monitoring_met_expert.begin(); it != m_monitoring_met_expert.end(); ++it) {
    if (get_trigger_level(*it) == "L1")
      m_met_triggers_l1_expert.push_back(*it);
    else if (get_trigger_level(*it) == "HLT")
      m_met_triggers_hlt_expert.push_back(*it);
  }


  
  return StatusCode::SUCCESS;
}


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::book() {

  ATH_MSG_DEBUG("in HLTMETMonTool::book()");
    
  // check if input met triggers are in trigger configuration and add them to signatures_to look vectors
  if (m_met_triggers_l1_shifter.size())
    check_triggers(m_met_triggers_l1_shifter, m_l1_met_signatures_tolook_shifter);
  if (m_met_triggers_hlt_shifter.size())
    check_triggers(m_met_triggers_hlt_shifter, m_hlt_met_signatures_tolook_shifter);
  //Do the same for expert
  if (m_met_triggers_l1_expert.size())
    check_triggers(m_met_triggers_l1_expert, m_l1_met_signatures_tolook_expert);
  if (m_met_triggers_hlt_expert.size())
    check_triggers(m_met_triggers_hlt_expert, m_hlt_met_signatures_tolook_expert);//
  
  // declare iterator etc.
  std::map<std::string,int>::const_iterator it;
  std::map<std::string, int> met_signatures_tolook;

  // Create Shifter and Expert folders
  m_shifter_path = m_mon_path + "/Shifter";
  m_expert_path  = m_mon_path + "/Expert";

  addMonGroup(new MonGroup(this, m_shifter_path, run));
  addMonGroup(new MonGroup(this, m_expert_path, run));

  std::string monGroupName = "";  //L1 or HLT
  std::string monFolderName = "";  //Primary, Efficiecny, mht, trigger name etc.


  // Book Shifter L1 histograms *****
  monGroupName = m_shifter_path + "/L1";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));

  // Basic L1 histograms
  monFolderName = monGroupName + "/Primary";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  addL1BasicHistograms();

  // Efficiencies Shifter L1 histograms
  monFolderName = monGroupName + "/Efficiency";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  met_signatures_tolook = m_l1_met_signatures_tolook_shifter;
  addL1ProfileHistograms(met_signatures_tolook);
    

  // Book Shifter HLT histograms *****
  monGroupName = m_shifter_path + "/HLT";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));

  // Basic HLT histograms
  monFolderName = monGroupName + "/Primary";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  addHLTBasicHistograms();

  // Efficiencies Shifter HLT histograms
  monFolderName = monGroupName + "/Efficiency";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  met_signatures_tolook = m_hlt_met_signatures_tolook_shifter;
  addHLTProfileHistograms(met_signatures_tolook);

  // mu50 histograms
  monFolderName = monGroupName + "/mu50";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  addHLTBasicHistograms();

  // Signal like electron histograms
  monFolderName = monGroupName + "/SignalEl";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  addHLTBasicHistograms();
  met_signatures_tolook = m_hlt_met_signatures_tolook_shifter;
  addHLTProfileHistograms(met_signatures_tolook);

  // Signal like muon histograms
  monFolderName = monGroupName + "/SignalMu";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  addHLTBasicHistograms();
  met_signatures_tolook = m_hlt_met_signatures_tolook_shifter;
  addHLTProfileHistograms(met_signatures_tolook);

  // HLT cell status histograms
  monFolderName = monGroupName + "/Status";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  addHLTStatusHistograms();

  /// Alternative algorithms: tc_lcw, tc_em, pueta, pufit, mht, etc.
  for (std::vector<std::string>::iterator it2 = m_monitoring_alg_shifter.begin(); it2 != m_monitoring_alg_shifter.end(); it2++) {

    monFolderName = monGroupName + "/" + *it2;
    addMonGroup(new MonGroup(this, monFolderName, run));
    setCurrentMonGroup(monFolderName);

    addHLTBasicHistograms();

  }

  
  if (!m_make_expert_histograms)
    return StatusCode::SUCCESS;  

  // Book Expert L1 histograms ***************
  monGroupName = m_expert_path + "/L1";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));

  // Jets Expert L1 histograms 
  monFolderName = monGroupName + "/Jets";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  addL1JetHistograms();

  // Efficiencies Expert L1 histograms 
  monFolderName = monGroupName + "/Efficiency";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  met_signatures_tolook = m_l1_met_signatures_tolook_expert;
  addL1ProfileHistograms(met_signatures_tolook);

  // With trigger rquirement:  L1_XE50 etc.
  for (it = m_l1_met_signatures_tolook_shifter.begin(); it != m_l1_met_signatures_tolook_shifter.end(); it++) {

    monFolderName = monGroupName + "/" + it->first;
    addMonGroup(new MonGroup(this, monFolderName, run));
    setCurrentMonGroup(monFolderName);

    addL1BasicHistograms();
    
  }

  // Book Expert HLT histpograms ***********
  monGroupName = m_expert_path + "/HLT";
  addMonGroup(new MonGroup(this, monGroupName.c_str(), run));

  // Efficiencies Expert HLT histograms 
  monFolderName = monGroupName + "/Efficiency";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  met_signatures_tolook = m_hlt_met_signatures_tolook_expert;
  addHLTProfileHistograms(met_signatures_tolook);

  /// HLT cell component histograms
  monFolderName = monGroupName + "/Component";
  addMonGroup(new MonGroup(this, monFolderName, run));
  setCurrentMonGroup(monFolderName);
  addHLTCompHistograms();

  // Alternative algorithms: tc_lcw, tc_em, pueta, pufit, mht, etc.
  for (std::vector<std::string>::iterator it2 = m_monitoring_alg_expert.begin(); it2 != m_monitoring_alg_expert.end(); it2++) {

    monFolderName = monGroupName + "/" + *it2;
    addMonGroup(new MonGroup(this, monFolderName, run));
    setCurrentMonGroup(monFolderName);

    addHLTBasicHistograms();

  }
 
  // With trigger requiriment: xe35, xe80, xe100 etc.
  for (it = m_hlt_met_signatures_tolook_shifter.begin(); it != m_hlt_met_signatures_tolook_shifter.end(); it++) {

    monFolderName = monGroupName + "/" + it->first;
    addMonGroup(new MonGroup(this, monFolderName, run));
    setCurrentMonGroup(monFolderName);

    addHLTBasicHistograms();

  }

  monGroupName = m_expert_path;

  // Book Offline MET histograms *********
  monFolderName = monGroupName + "/Offline";
  addMonGroup(new MonGroup(this, monFolderName.c_str(), run));
  setCurrentMonGroup(monFolderName);
  addOffMETHistograms();


  // Book L1 vs HLT histograms ***********
  monFolderName = monGroupName + "/L1_vs_HLT";
  addMonGroup(new MonGroup(this, monFolderName.c_str(), run));
  setCurrentMonGroup(monFolderName);
  addL1vsHLTHistograms();



  // Book HLT vs Offline histograms ********
  monFolderName = monGroupName + "/HLT_vs_Offline";
  addMonGroup(new MonGroup(this, monFolderName.c_str(), run));
  setCurrentMonGroup(monFolderName);
  addHLTvsOffHistograms();


  // Book L1 vs Offline histograms ********
  monFolderName = monGroupName + "/L1_vs_Offline";
  addMonGroup(new MonGroup(this, monFolderName.c_str(), run));
  setCurrentMonGroup(monFolderName);
  addL1vsOffHistograms();



  // Book HLT Muons and Electrons histograms ********
  monFolderName = monGroupName + "/ElMu";
  addMonGroup(new MonGroup(this, monFolderName.c_str(), run));
  setCurrentMonGroup(monFolderName);
  addElMuHistograms();

  return StatusCode::SUCCESS;
} 


//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fill() {

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
  std::string monGroupName;

  ATH_MSG_DEBUG("in HLTMETMonTool::proc()");

  if(m_print_met_trig_stats)
    print_trigger_stats();

  // efficiency histo shifter/L1     
  monGroupName = m_shifter_path + "/L1";
  setCurrentMonGroup(monGroupName);

  // efficiency histos shifter/HLT   
  monGroupName = m_shifter_path + "/HLT";
  setCurrentMonGroup(monGroupName);

  return StatusCode::SUCCESS;
}




//___________________________________________________________________________________________________________
StatusCode HLTMETMonTool::fillMETHist() {

  ATH_MSG_DEBUG("in HTLMETMonTool::fillMETHistos()");

  StatusCode sc = StatusCode::SUCCESS;

  // Check HLTResult
  if(getTDT()->ExperimentalAndExpertMethods()->isHLTTruncated()){
    ATH_MSG_WARNING("HLTResult truncated, skip event");
    return sc;
  }


  // retrieve EventInfo 
  const xAOD::EventInfo *eventInfo = nullptr;
  sc = evtStore()->retrieve(eventInfo, "EventInfo");
  if(sc.isFailure() || !eventInfo) {
    ATH_MSG_WARNING("Could not retrieve EventInfo with key \"EventInfo\" from TDS"  );
  }

  // retrieve L1 Jet ROI 
  const xAOD::JetRoIContainer *v_l1_jet_roi_cont = 0;
  sc = evtStore()->retrieve(v_l1_jet_roi_cont, m_lvl1_jet_roi_key);
  if(sc.isFailure() || !v_l1_jet_roi_cont) {
    ATH_MSG_WARNING("Could not retrieve LVL1_Jet_RoIs with key \"" << m_lvl1_jet_roi_key << "\" from TDS"  );
  }

  // retrieve xAOD L1 ROI 
  const xAOD::EnergySumRoI *v_l1_roi_cont = 0;
  sc = evtStore()->retrieve(v_l1_roi_cont, m_lvl1_roi_key);
  if(sc.isFailure() || !v_l1_roi_cont) {
    ATH_MSG_WARNING("Could not retrieve LVL1_RoIs with key \"" << m_lvl1_roi_key << "\" from TDS"  );
  }

  // retrieve HLT containers
  // Get HLT (CELL) container
  const xAOD::TrigMissingETContainer *v_hlt_cell_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_cell_met_cont, m_hlt_cell_met_key);
  if (sc.isFailure() || !v_hlt_cell_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_cell_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met (cell) with " << v_hlt_cell_met_cont->size() << " elements");

  // Get HLT mht container
  const xAOD::TrigMissingETContainer *v_hlt_mht_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_mht_met_cont, m_hlt_mht_met_key);
  if (sc.isFailure() || !v_hlt_mht_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_mht_met_key << " from TDS");
  }
  else {
    ATH_MSG_DEBUG("Accessing met(mht) with " << v_hlt_mht_met_cont->size() << " elements");
  }

  // Get HLT mhtem container
  const xAOD::TrigMissingETContainer *v_hlt_mhtem_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_mhtem_met_cont, m_hlt_mhtem_met_key);
  if (sc.isFailure() || !v_hlt_mhtem_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_mhtem_met_key << " from TDS");
  }
  else {
    ATH_MSG_DEBUG("Accessing met(mhtem) with " << v_hlt_mhtem_met_cont->size() << " elements");
  }

  // Get HLT trkmht container
  const xAOD::TrigMissingETContainer *v_hlt_trkmht_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_trkmht_met_cont, m_hlt_trkmht_met_key);
  if (sc.isFailure() || !v_hlt_trkmht_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_trkmht_met_key << " from TDS");
  }
  else {
    ATH_MSG_DEBUG("Accessing met(trkmht) with " << v_hlt_trkmht_met_cont->size() << " elements");
  }

  // Get HLT trkmhtFTK container
  const xAOD::TrigMissingETContainer *v_hlt_trkmhtFTK_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_trkmhtFTK_met_cont, m_hlt_trkmhtFTK_met_key);
  if (sc.isFailure() || !v_hlt_trkmhtFTK_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_trkmhtFTK_met_key << " from TDS");
  }
  else {
    ATH_MSG_DEBUG("Accessing met(trkmhtFTK) with " << v_hlt_trkmhtFTK_met_cont->size() << " elements");
  }

  // Get HLT topocl container
  const xAOD::TrigMissingETContainer *v_hlt_topocl_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_topocl_met_cont, m_hlt_topocl_met_key);
  if (sc.isFailure() || !v_hlt_topocl_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_topocl_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(topcl) with " << v_hlt_topocl_met_cont->size() << " elements");

  // Get HLT topocl_PS container
  const xAOD::TrigMissingETContainer *v_hlt_topocl_PS_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_topocl_PS_met_cont, m_hlt_topocl_PS_met_key);
  if (sc.isFailure() || !v_hlt_topocl_PS_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_topocl_PS_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(topocl_PS) with " << v_hlt_topocl_PS_met_cont->size() << " elements");

  // Get HLT topocl_PUC container
  const xAOD::TrigMissingETContainer *v_hlt_topocl_PUC_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_topocl_PUC_met_cont, m_hlt_topocl_PUC_met_key);
  if (sc.isFailure() || !v_hlt_topocl_PUC_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_topocl_PUC_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(topocl_PUC) with " << v_hlt_topocl_PUC_met_cont->size() << " elements");

  // Get HLT FEB container
  const xAOD::TrigMissingETContainer *v_hlt_FEB_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_FEB_met_cont, m_hlt_FEB_met_key);
  if (sc.isFailure() || !v_hlt_FEB_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_FEB_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(FEB) with " << v_hlt_FEB_met_cont->size() << " elements");

  // Get HLT Fex container
  const xAOD::TrigMissingETContainer *v_hlt_Fex_met_cont = 0;
  sc = evtStore()->retrieve(v_hlt_Fex_met_cont, m_hlt_Fex_met_key);
  if (sc.isFailure() || !v_hlt_Fex_met_cont) {
    ATH_MSG_WARNING("Could not retrieve TrigMissingETContainer with key " << m_hlt_Fex_met_key << " from TDS");
  }
  else 
    ATH_MSG_DEBUG("Accessing met(Fex) with " << v_hlt_Fex_met_cont->size() << " elements");

  // Get EF muon and EF egamma containers for signal-like selection
  // retrieve EF muon container
  const xAOD::MuonContainer *v_hlt_muonEFcontainer = 0;
  sc = evtStore()->retrieve(v_hlt_muonEFcontainer, m_muon_key);
  if (sc.isFailure() || !v_hlt_muonEFcontainer) {
    ATH_MSG_WARNING("Could not retrieve muon container with key " << m_muon_key << " from TDS"); 
  }
  else
    ATH_MSG_DEBUG("Accessing EF muons container with " << v_hlt_muonEFcontainer->size() << " elements");

  // retrieve EF electron container
  const xAOD::ElectronContainer *v_hlt_electronEFcontainer = 0;
  sc = evtStore()->retrieve(v_hlt_electronEFcontainer, m_electron_key);
  if (sc.isFailure() || !v_hlt_electronEFcontainer) {
    ATH_MSG_WARNING("Could not retrieve electron container with key " << m_electron_key << " from TDS");
  }
  else
    ATH_MSG_DEBUG("Accessing EF electrons container with " << v_hlt_electronEFcontainer->size() << " elements");

  // Get Offline MET container
  const xAOD::MissingETContainer *v_off_met_cont = 0;
  sc = evtStore()->retrieve(v_off_met_cont, m_off_met_key);
  if (sc.isFailure() || !v_off_met_cont) {
    ATH_MSG_DEBUG("Could not retrieve Reconstructed MET term with Key " << m_off_met_key << " : v_off_met_cont = 0");
  } else {
    ATH_MSG_DEBUG("Got Reconstructed MET term with key " << m_off_met_key);
  }
  

  // Check if signal-like muon exists
  ATH_MSG_DEBUG("Going to iterate through muon container");
  bool METMuonFilled = false; // flag is set to true if event satisfies signal-like requirement 
  float pT_mumu = 0; 
  bool muonContEmpty = true;
  if (v_hlt_muonEFcontainer && v_hlt_muonEFcontainer->size()>0) {
    ATH_MSG_DEBUG("HLT EF Muon container exists and is not empty. Going to loop over container.");
    muonContEmpty = false;
    std::vector<TLorentzVector> muonCollection; 
    for (auto muon : *v_hlt_muonEFcontainer) {
      // If a muon in this event satifies the requirements, we will fill in MET for this event.
      if(muon->muonType() == xAOD::Muon::Combined && (muon->pt()/CLHEP::GeV > m_muon_pt_thresh)) {
	    METMuonFilled = true;
	    muonCollection.push_back(muon->p4()); 
      }
    }
    std::sort(muonCollection.begin(),muonCollection.end(),High2LowByPt());
    if(muonCollection.size()>=2)pT_mumu = (muonCollection.at(0)+muonCollection.at(1)).Perp()/CLHEP::GeV;
    muonCollection.clear();
  } else {
    ATH_MSG_DEBUG("HLT EF Muon container DNE or is empty");
  }

  // Check if signal-like electron exists
  ATH_MSG_DEBUG("Going to iterate through electron container");
  bool METElectronFilled = false;
  bool electronEtaReq = false;
  bool electronContEmpty = true;
  if (v_hlt_electronEFcontainer && v_hlt_electronEFcontainer->size()>0) {
    ATH_MSG_DEBUG("HLT EF electron container exists and is not empty. Going to loop over container.");
    //ATH_MSG_INFO("HLT EF electron container exists and is not empty. Going to loop over container.");
    electronContEmpty = false;
    for (auto eg : *v_hlt_electronEFcontainer) {
      
      electronEtaReq = ( std::abs(eg->eta()) < 1.37 || std::abs(eg->eta()) > 1.52 ) && std::abs(eg->eta()) < 2.47; 

      if (eg->pt()/CLHEP::GeV > m_electron_pt_thresh && electronEtaReq) {
        METElectronFilled = true;
      }
    }
  } else {
    ATH_MSG_DEBUG("HLT EF electron container DNE or is empty");
  }



  ATH_MSG_DEBUG("Filling histograms...");

  //####################
  //-- Get met values
  //####################
  const xAOD::TrigMissingET *v_hlt_met = 0;
  const xAOD::MissingET     *v_off_met = 0;
 
  //***********
  /// L1 JET
  //***********
  float l1_jet_pt = -9e9;
  float l1_jet_eta = -9e9;

 
  //***********
  /// L1 MET
  //***********
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
  float saturated = false; 

  if (v_l1_roi_cont) {
    if ((v_l1_roi_cont->energyX())>-9e12 && (v_l1_roi_cont->energyX())<9e12 && (v_l1_roi_cont->energyY())>-9e12 && (v_l1_roi_cont->energyY())<9e12) { 
      l1_mex = - (v_l1_roi_cont->energyX())/CLHEP::GeV;
      l1_mey = - (v_l1_roi_cont->energyY())/CLHEP::GeV;
      l1_met = sqrt(l1_mex*l1_mex + l1_mey*l1_mey);
      l1_phi = atan2f(l1_mey,l1_mex);
      l1_sumet = (v_l1_roi_cont->energyT())/CLHEP::GeV; 
      
      l1_mex_log = signed_log(l1_mex, epsilon);
      l1_mey_log = signed_log(l1_mey, epsilon);
      l1_met_log = signed_log(l1_met, epsilon);
      l1_sumet_log = signed_log(l1_sumet, epsilon);
      saturated = (fabs(l1_mex)>16383 || fabs(l1_mey)>16383 ); //this is a hacked way to see if Ex/Ey saturated. There may be better. 
    } else {
      ATH_MSG_WARNING("L1 POI enegy too large");
    }
  }

  //**************
  /// HLT MET
  //**************
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


  // Set primary trigger Algorithm
  std::string primary_name = m_primary_met[0];
  std::string primary_algo = get_trigger_algo(primary_name);
  ATH_MSG_DEBUG("Primary Selected Alg : " << primary_algo);
  if (primary_algo == "cell" && v_hlt_cell_met_cont && v_hlt_cell_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : Cell");
    v_hlt_met = v_hlt_cell_met_cont->at(0);
  } else if (primary_algo == "mht" && v_hlt_mht_met_cont && v_hlt_mht_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : MHT");
    v_hlt_met = v_hlt_mht_met_cont->at(0);
  } else if (primary_algo == "mhtem" && v_hlt_mhtem_met_cont && v_hlt_mhtem_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : mht_em");
    v_hlt_met = v_hlt_mhtem_met_cont->at(0);
  } else if (primary_algo == "trkmht" && v_hlt_trkmht_met_cont && v_hlt_trkmht_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : trkmht");
    v_hlt_met = v_hlt_trkmht_met_cont->at(0);
  } else if (primary_algo == "trkmhtFTK" && v_hlt_trkmhtFTK_met_cont && v_hlt_trkmhtFTK_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : trkmhtFTK");
    v_hlt_met = v_hlt_trkmhtFTK_met_cont->at(0);
  } else if (primary_algo == "topocl" && v_hlt_topocl_met_cont && v_hlt_topocl_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : TopoCL");
    v_hlt_met = v_hlt_topocl_met_cont->at(0);
  } else if (primary_algo == "topocl_PS" && v_hlt_topocl_PS_met_cont && v_hlt_topocl_PS_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : TopoCL_PS");
    v_hlt_met = v_hlt_topocl_PS_met_cont->at(0);
  } else if (primary_algo == "topocl_PUC" && v_hlt_topocl_PUC_met_cont && v_hlt_topocl_PUC_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : TopoCL_PUC");
    v_hlt_met = v_hlt_topocl_PUC_met_cont->at(0);
  } else if (primary_algo == "FEB" && v_hlt_FEB_met_cont && v_hlt_FEB_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : FEB");
    v_hlt_met = v_hlt_FEB_met_cont->at(0);
  } else if (primary_algo == "Fex" && v_hlt_Fex_met_cont && v_hlt_Fex_met_cont->size()>0) {
    ATH_MSG_DEBUG("Primary Alg : FEX");
    v_hlt_met = v_hlt_Fex_met_cont->at(0);
  } else {
    ATH_MSG_DEBUG("Primary Alg : NONE");
    v_hlt_met = 0;
  }
  if (v_hlt_met) {
    hlt_ex = v_hlt_met->ex()/CLHEP::GeV; 
    hlt_ey = v_hlt_met->ey()/CLHEP::GeV; 
    hlt_ez = v_hlt_met->ez()/CLHEP::GeV;
    hlt_met = sqrt(hlt_ex*hlt_ex+hlt_ey*hlt_ey); 
    hlt_sumet = v_hlt_met->sumEt()/CLHEP::GeV;
    hlt_sume  = v_hlt_met->sumE()/CLHEP::GeV; 

    CLHEP::Hep3Vector v(hlt_ex, hlt_ey, hlt_ez);
    hlt_eta = v.eta();
    hlt_phi = v.phi();
    
    float epsilon = 1e-6;  // 1 keV

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
  if (v_off_met_cont) {
    if (v_off_met_cont->size()) {
      v_off_met = (*v_off_met_cont)["FinalClus"];
      
      off_ex = ((*v_off_met_cont)["FinalClus"]->mpx())/CLHEP::GeV;
      off_ey = ((*v_off_met_cont)["FinalClus"]->mpy())/CLHEP::GeV;
      off_met = sqrt(off_ex*off_ex+off_ey*off_ey);
      off_sumet = ((*v_off_met_cont)["FinalClus"]->sumet())/CLHEP::GeV;
      off_phi = atan2(off_ey, off_ex);
    }
  }


  //########################
  //-- Fill histograms
  //########################
  
  // declare iterator etc.
  std::map<std::string,int>::const_iterator it;
  std::map<std::string, int> met_signatures_tolook;
  TH1 *h(0);
  TH2 *h2(0);

  // Group and Folder names under Shifter and Expert folders
  std::string monGroupName = "";  //L1 or HLT
  std::string monFolderName = "";  //Primary, Efficiecny, mht, trigger name etc.


  /////////////
  /// Shifter L1
  /////////////
  monGroupName = m_shifter_path + "/L1";

  // L1 Basic
  monFolderName = monGroupName + "/Primary";
  setCurrentMonGroup(monFolderName.c_str());

  if (l1_met > epsilon_l1met) {
    fillL1BasicHistograms(l1_mex,l1_mex_log,l1_mey,l1_mey_log,l1_met,l1_met_log,l1_sumet,l1_sumet_log,l1_phi,saturated);
  }
  

  // L1 efficiency
  monFolderName = monGroupName + "/Efficiency";
  setCurrentMonGroup(monFolderName.c_str());

  met_signatures_tolook = m_l1_met_signatures_tolook_shifter;
  fillL1ProfileHistograms(off_met,pT_mumu,METMuonFilled,met_signatures_tolook);
 

  /////////////
  // Shifter HLT
  /////////////

  monGroupName = m_shifter_path + "/HLT";	

  // HLT Basic
  monFolderName = monGroupName + "/Primary";
  setCurrentMonGroup(monFolderName);
  if (hlt_met > 0) { // remove MET=0 events
    fillHLTBasicHistograms(hlt_ex,hlt_ex_log,hlt_ey,hlt_ey_log,hlt_ez,hlt_ez_log,hlt_met,hlt_met_log,hlt_sumet,hlt_sumet_log,hlt_sume,hlt_sume_log,hlt_phi,hlt_eta,hlt_significance);
 }

  // HLT efficiency  
  monFolderName = monGroupName + "/Efficiency";
  setCurrentMonGroup(monFolderName);

  met_signatures_tolook = m_hlt_met_signatures_tolook_shifter;
  fillHLTProfileHistograms(off_met,met_signatures_tolook);


  // HLT mu50
  monFolderName = monGroupName + "/mu50";
  setCurrentMonGroup(monFolderName);
  if (hlt_met > 0 && eventInfo) { // remove MET=0 events
    if (eventInfo->actualInteractionsPerCrossing()>47. && eventInfo->actualInteractionsPerCrossing()<53.) {
      fillHLTBasicHistograms(hlt_ex,hlt_ex_log,hlt_ey,hlt_ey_log,hlt_ez,hlt_ez_log,hlt_met,hlt_met_log,hlt_sumet,hlt_sumet_log,hlt_sume,hlt_sume_log,hlt_phi,hlt_eta,hlt_significance);
    }
  }


  // HLT signal-like electron
  monFolderName = monGroupName + "/SignalEl";
  setCurrentMonGroup(monFolderName);
  
  if(METElectronFilled == true) {
    if (hlt_met > 0) { // remove MET=0 events
      fillHLTBasicHistograms(hlt_ex,hlt_ex_log,hlt_ey,hlt_ey_log,hlt_ez,hlt_ez_log,hlt_met,hlt_met_log,hlt_sumet,hlt_sumet_log,hlt_sume,hlt_sume_log,hlt_phi,hlt_eta,hlt_significance);
    }
    met_signatures_tolook = m_hlt_met_signatures_tolook_shifter;
    fillHLTProfileHistograms(off_met,met_signatures_tolook);
  }

  
  // HLT signal-like muon
  monFolderName = monGroupName + "/SignalMu";
  setCurrentMonGroup(monFolderName);

  if(METMuonFilled == true) {
    if (hlt_met > 0) { // remove MET=0 events
      fillHLTBasicHistograms(hlt_ex,hlt_ex_log,hlt_ey,hlt_ey_log,hlt_ez,hlt_ez_log,hlt_met,hlt_met_log,hlt_sumet,hlt_sumet_log,hlt_sume,hlt_sume_log,hlt_phi,hlt_eta,hlt_significance);
    }
    met_signatures_tolook = m_hlt_met_signatures_tolook_shifter;
    fillHLTProfileHistograms(off_met,met_signatures_tolook);
  }

  // HLT cell status
  monFolderName = monGroupName + "/Status";
  setCurrentMonGroup(monFolderName);

  const xAOD::TrigMissingET *missETEF = 0;
  if (v_hlt_cell_met_cont && v_hlt_cell_met_cont->size()) {
    missETEF = *(v_hlt_cell_met_cont->begin());

    // Lumi Block histogram
    if ((h = hist("HLT_limiBlock")) && eventInfo)    h->Fill(eventInfo->lumiBlock());

    // <mju> histogram
    if ((h = hist("HLT_mu")) && eventInfo)    h->Fill(eventInfo->actualInteractionsPerCrossing());

    // status histogram
    TH1F *h1i(0);
    bool v_fill_stat = false;
    if((h1i = (TH1F *) hist("HLT_MET_status"))) v_fill_stat = true;
    for (int i=0; i<32; ++i) {
      unsigned mask = (1<<i);
      if (missETEF->flag() & mask) {
	if(v_fill_stat && h1i) h1i->Fill(i,1.);
      } else {
	if(v_fill_stat && h1i) h1i->Fill(i,0);
      }
    }
    
    // component histograms
    unsigned int Nc = missETEF->getNumberOfComponents();
    unsigned int compNc = m_compNames.size();
    Nc = Nc > compNc ? compNc : Nc;
    
    if (Nc > 0) {
      for (unsigned int i = 0; i < Nc; ++i) { // loop over components
	float ex =                missETEF->exComponent(i)/CLHEP::GeV;
	float ey =                missETEF->eyComponent(i)/CLHEP::GeV;
	float et =                sqrt(ex*ex+ey*ey);
	float v_comp_et_lin = et;
	if((h2 = hist2("compN_compEt_lin")))      h2->Fill(i,v_comp_et_lin);

	bool v_fill_stat = false;
	if((h2 = hist2("compN_HLT_MET_status"))) v_fill_stat = true;
	// loop over status flags
	for (int j=0; j<32; ++j) {
	  unsigned mask = (1<<j);
	  if (missETEF->statusComponent(i) & mask) {
	    if(v_fill_stat && h2) h2->Fill(i,j);
	  }
	}
      }
    }
    
  }


  /// Alternative algorithms: tc_lcw, tc_em, pueta, pufit, mht, etc.
  for (std::vector<std::string>::iterator it2 = m_monitoring_alg_shifter.begin(); it2 != m_monitoring_alg_shifter.end(); it2++) {
   
    monFolderName = monGroupName + "/" + *it2;
    setCurrentMonGroup(monFolderName);

    std::string name = *it2;
    std::string algo = get_trigger_algo(name);
    ATH_MSG_DEBUG("Alternative Selected Alg : " << algo);
    if (algo == "cell" && v_hlt_cell_met_cont && v_hlt_cell_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : Cell");
      v_hlt_met = v_hlt_cell_met_cont->at(0);
    } else if (algo == "mht" && v_hlt_mht_met_cont && v_hlt_mht_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : MHT");
      v_hlt_met = v_hlt_mht_met_cont->at(0);
    } else if (algo == "mhtem" && v_hlt_mhtem_met_cont && v_hlt_mhtem_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : mht_em");
      v_hlt_met = v_hlt_mhtem_met_cont->at(0);
    } else if (algo == "trkmht" && v_hlt_trkmht_met_cont && v_hlt_trkmht_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : trkmht");
      v_hlt_met = v_hlt_trkmht_met_cont->at(0);
    } else if (algo == "trkmhtFTK" && v_hlt_trkmhtFTK_met_cont && v_hlt_trkmhtFTK_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : trkmhtFTK");
      v_hlt_met = v_hlt_trkmhtFTK_met_cont->at(0);
    } else if (algo == "topocl" && v_hlt_topocl_met_cont && v_hlt_topocl_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL");
      v_hlt_met = v_hlt_topocl_met_cont->at(0);
    } else if (algo == "topocl_PS" && v_hlt_topocl_PS_met_cont && v_hlt_topocl_PS_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL_PS");
      v_hlt_met = v_hlt_topocl_PS_met_cont->at(0);
    } else if (algo == "topocl_PUC" && v_hlt_topocl_PUC_met_cont && v_hlt_topocl_PUC_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL_PUC");
      v_hlt_met = v_hlt_topocl_PUC_met_cont->at(0);
    } else if (algo == "FEB" && v_hlt_FEB_met_cont && v_hlt_FEB_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : FEB");
      v_hlt_met = v_hlt_FEB_met_cont->at(0);
    } else if (algo == "Fex" && v_hlt_Fex_met_cont && v_hlt_Fex_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : FEX");
      v_hlt_met = v_hlt_Fex_met_cont->at(0);
    } else {
      ATH_MSG_DEBUG("Alternative Alg : NONE");
      v_hlt_met = 0;
    }
    
    if (v_hlt_met) {  
      float tmp_hlt_ex = v_hlt_met->ex()/CLHEP::GeV; 
      float tmp_hlt_ey = v_hlt_met->ey()/CLHEP::GeV; 
      float tmp_hlt_ez = v_hlt_met->ez()/CLHEP::GeV;
      float tmp_hlt_met = sqrt(tmp_hlt_ex*tmp_hlt_ex+tmp_hlt_ey*tmp_hlt_ey); 
      float tmp_hlt_sumet = v_hlt_met->sumEt()/CLHEP::GeV;
      float tmp_hlt_sume  = v_hlt_met->sumE()/CLHEP::GeV; 
      ATH_MSG_DEBUG("Alternative Ex = " << tmp_hlt_ex);
      ATH_MSG_DEBUG("Alternative Ey = " << tmp_hlt_ey);
      ATH_MSG_DEBUG("Alternative MET = " << tmp_hlt_met);
      
      CLHEP::Hep3Vector v(tmp_hlt_ex, tmp_hlt_ey, tmp_hlt_ez);
      float tmp_hlt_eta = v.eta();
      float tmp_hlt_phi = v.phi();
      
      float tmp_hlt_ex_log = -9e9;
      float tmp_hlt_ey_log = -9e9;
      float tmp_hlt_ez_log = -9e9;
      float tmp_hlt_met_log = -9e9;
      float tmp_hlt_sume_log = -9e9;
      float tmp_hlt_sumet_log = -9e9;
      float tmp_hlt_significance = -9e9;
      
      float epsilon = 1e-6;  // 1 keV
      
      epsilon = 1.189;
      tmp_hlt_ex_log = signed_log(tmp_hlt_ex, epsilon);
      tmp_hlt_ey_log = signed_log(tmp_hlt_ey, epsilon);
      tmp_hlt_ez_log = signed_log(tmp_hlt_ez, epsilon);
      tmp_hlt_met_log = signed_log(tmp_hlt_met, epsilon);
      tmp_hlt_sume_log = signed_log(tmp_hlt_sume, epsilon);
      tmp_hlt_sumet_log = signed_log(tmp_hlt_sumet, epsilon);

      float tmp_resolution = 0.0;
      if (tmp_hlt_sumet>0.0) tmp_resolution = m_sigOffset + m_sigSlope*sqrt(tmp_hlt_sumet) + m_sigQuadr*tmp_hlt_sumet;
      if (tmp_resolution>0.0) tmp_hlt_significance = tmp_hlt_met/tmp_resolution;
      
      fillHLTBasicHistograms(tmp_hlt_ex,tmp_hlt_ex_log,tmp_hlt_ey,tmp_hlt_ey_log,tmp_hlt_ez,tmp_hlt_ez_log,tmp_hlt_met,tmp_hlt_met_log,tmp_hlt_sumet,tmp_hlt_sumet_log,tmp_hlt_sume,tmp_hlt_sume_log,tmp_hlt_phi,tmp_hlt_eta,tmp_hlt_significance);

    }

  }




  if (!m_make_expert_histograms) return sc;


  //########################
  //-- Fill Expert histograms  ###############################
  //########################

  ATH_MSG_DEBUG("filling Expert histograms");

  /////////////
  /// Expert L1
  /////////////
  monGroupName = m_expert_path + "/L1";

  // L1 Jets                                                           
  monFolderName = monGroupName + "/Jets";
  setCurrentMonGroup(monFolderName);
  if (l1_met > 50) {
    for (auto l1_jet : *v_l1_jet_roi_cont) {
      l1_jet_pt = l1_jet->et8x8()/CLHEP::GeV;
      l1_jet_eta = l1_jet->eta();
      fillL1JetHistograms(l1_jet_pt,l1_jet_eta);
    }
  }

  // L1 efficiency                                                           
  monFolderName = monGroupName + "/Efficiency";
  setCurrentMonGroup(monFolderName);

  met_signatures_tolook = m_l1_met_signatures_tolook_expert;
  fillL1ProfileHistograms(off_met,pT_mumu,METMuonFilled,met_signatures_tolook);

  // L1 Triggers
  for (it = m_l1_met_signatures_tolook_shifter.begin(); it != m_l1_met_signatures_tolook_shifter.end(); it++) {
    std::string name = it->first;
    ATH_MSG_DEBUG("TriggerLoop: name = " << name);

    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      ATH_MSG_DEBUG("Passed L1 Trigger : " << name);

      monFolderName = monGroupName + "/" + it->first;
      setCurrentMonGroup(monFolderName);

      fillL1BasicHistograms(l1_mex,l1_mex_log,l1_mey,l1_mey_log,l1_met,l1_met_log,l1_sumet,l1_sumet_log,l1_phi,saturated);

    }
  }
      

  /////////////
  /// Expert HLT
  /////////////

  monGroupName = m_expert_path + "/HLT";

  // HLT efficiency expert                      
  monFolderName = monGroupName + "/Efficiency";
  setCurrentMonGroup(monFolderName);

  met_signatures_tolook = m_hlt_met_signatures_tolook_expert;
  fillHLTProfileHistograms(off_met,met_signatures_tolook);


  // HLT trigger requiriment: xe35, xe80, xe100 etc.
  for (it = m_hlt_met_signatures_tolook_shifter.begin(); it != m_hlt_met_signatures_tolook_shifter.end(); it++) {
    monFolderName = monGroupName + "/" + it->first;
    setCurrentMonGroup(monFolderName);

    std::string name = it->first;
    ATH_MSG_DEBUG("Trig : " << name);
    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      fillHLTBasicHistograms(hlt_ex,hlt_ex_log,hlt_ey,hlt_ey_log,hlt_ez,hlt_ez_log,hlt_met,hlt_met_log,hlt_sumet,hlt_sumet_log,hlt_sume,hlt_sume_log,hlt_phi,hlt_eta,hlt_significance);
    }
  }


  // HLT cell component
  monFolderName = monGroupName + "/Component";
  setCurrentMonGroup(monFolderName);

  if (v_hlt_cell_met_cont && v_hlt_cell_met_cont->size()) {
    missETEF = *(v_hlt_cell_met_cont->begin()); // cell based
    
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
	
	float v_comp_ex_log = -9e9;
	float v_comp_ey_log = -9e9;
	float v_comp_ez_log = -9e9;
	float v_comp_et_log = -9e9;
	float v_comp_sum_et_log = -9e9;
	float v_comp_sum_e_log = -9e9;
	float v_comp_sum_et_lin = -9e9;
	//float m_comp_sum_e_lin = -9e9;
	float v_comp_et_lin = -9e9;
	
	if (fabsf(ex)>epsilon)
	  v_comp_ex_log = copysign(log10f(fabsf(ex)), ex);
	else
	  v_comp_ex_log = 0.01;
	
	if (fabsf(ey)>epsilon)
	  v_comp_ey_log = copysign(log10f(fabsf(ey)), ey);
	else
	  v_comp_ey_log = 0.01;
	
	if (fabsf(ez)>epsilon)
	  v_comp_ez_log = copysign(log10f(fabsf(ez)), ez);
	else
	  v_comp_ez_log = 0.01;
	
	v_comp_et_lin = et;
	if (et>1e-6) // i.e. et > 0
	  v_comp_et_log = log10f(et);
	else
	  v_comp_et_log = -100; // underflow
	
	v_comp_sum_et_lin = sumEt;
        
	if (fabsf(sumEt)>epsilon)
	  v_comp_sum_et_log = copysign(log10f(fabsf(sumEt)), sumEt);
	else
	  v_comp_sum_et_log = 0.01;
	if (fabsf(sumE)>epsilon)
	  v_comp_sum_e_log = copysign(log10f(fabsf(sumE)), sumE);
	else
	  v_comp_sum_e_log = 0.01;
	
	if((h2 = hist2("compN_compEx")))      h2->Fill(i,v_comp_ex_log);
	if((h2 = hist2("compN_compEy")))      h2->Fill(i,v_comp_ey_log);
	if((h2 = hist2("compN_compEz")))      h2->Fill(i,v_comp_ez_log);
	if((h2 = hist2("compN_compEt")))      h2->Fill(i,v_comp_et_log);
	if((h2 = hist2("compN_compSumEt")))      h2->Fill(i,v_comp_sum_et_log);
	if((h2 = hist2("compN_compSumE")))      h2->Fill(i,v_comp_sum_e_log);
	if((h2 = hist2("compN_compEt_lin")))      h2->Fill(i,v_comp_et_lin);
	if((h2 = hist2("compN_compSumEt_lin")))      h2->Fill(i,v_comp_sum_et_lin);
        
	bool v_fill_stat = false;
	if((h2 = hist2("compN_HLT_MET_status"))) v_fill_stat = true;
	// loop over status flags
	for (int j=0; j<32; ++j) {
	  unsigned mask = (1<<j);
	  if (missETEF->statusComponent(i) & mask) {
	    if(v_fill_stat && h2) h2->Fill(i,j);
	  }
	}
        
	float nchan_used = missETEF->usedChannelsComponent(i);
	if(nchan_used > epsilon) nchan_used = log10f(nchan_used);
	else nchan_used = -1.;
	if((h2 = hist2("compN_HLT_usedChannels"))) h2->Fill(i,nchan_used);
	
	CLHEP::Hep3Vector v(ex,ey,ez);
	double eta = v.eta();
	double phi = v.phi();
	
	if((h2 = hist2(Form("compEt_lin_EtaPhi_%02d", i))))      h2->Fill(eta, phi, v_comp_et_lin);
      }
    }
  }


  /// Alternative algorithms: tc_lcw, tc_em, pueta, pufit, mht, etc.
  for (std::vector<std::string>::iterator it2 = m_monitoring_alg_expert.begin(); it2 != m_monitoring_alg_expert.end(); it2++) {
   
    monFolderName = monGroupName + "/" + *it2;
    setCurrentMonGroup(monFolderName);

    std::string name = *it2;
    std::string algo = get_trigger_algo(name);
    ATH_MSG_DEBUG("Alternative Selected Alg : " << algo);
    if (algo == "cell" && v_hlt_cell_met_cont && v_hlt_cell_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : Cell");
      v_hlt_met = v_hlt_cell_met_cont->at(0);
    } else if (algo == "mht" && v_hlt_mht_met_cont && v_hlt_mht_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : MHT");
      v_hlt_met = v_hlt_mht_met_cont->at(0);
    } else if (algo == "mhtem" && v_hlt_mhtem_met_cont && v_hlt_mhtem_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : mht_em");
     v_hlt_met = v_hlt_mhtem_met_cont->at(0);
    } else if (algo == "trkmht" && v_hlt_trkmht_met_cont && v_hlt_trkmht_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : trkmht");
      v_hlt_met = v_hlt_trkmht_met_cont->at(0);
    } else if (algo == "trkmhtFTK" && v_hlt_trkmhtFTK_met_cont && v_hlt_trkmhtFTK_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : trkmhtFTK");
      v_hlt_met = v_hlt_trkmhtFTK_met_cont->at(0);
    } else if (algo == "topocl" && v_hlt_topocl_met_cont && v_hlt_topocl_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL");
      v_hlt_met = v_hlt_topocl_met_cont->at(0);
    } else if (algo == "topocl_PS" && v_hlt_topocl_PS_met_cont && v_hlt_topocl_PS_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL_PS");
      v_hlt_met = v_hlt_topocl_PS_met_cont->at(0);
    } else if (algo == "topocl_PUC" && v_hlt_topocl_PUC_met_cont && v_hlt_topocl_PUC_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : TopoCL_PUC");
      v_hlt_met = v_hlt_topocl_PUC_met_cont->at(0);
    } else if (algo == "FEB" && v_hlt_FEB_met_cont && v_hlt_FEB_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : FEB");
      v_hlt_met = v_hlt_FEB_met_cont->at(0);
    } else if (algo == "Fex" && v_hlt_Fex_met_cont && v_hlt_Fex_met_cont->size()>0) {
      ATH_MSG_DEBUG("Alternative Alg : FEX");
      v_hlt_met = v_hlt_Fex_met_cont->at(0);
    } else {
      ATH_MSG_DEBUG("Alternative Alg : NONE");
      v_hlt_met = 0;
    }
    
    if (v_hlt_met) {  
      float tmp_hlt_ex = v_hlt_met->ex()/CLHEP::GeV; 
      float tmp_hlt_ey = v_hlt_met->ey()/CLHEP::GeV; 
      float tmp_hlt_ez = v_hlt_met->ez()/CLHEP::GeV;
      float tmp_hlt_met = sqrt(tmp_hlt_ex*tmp_hlt_ex+tmp_hlt_ey*tmp_hlt_ey); 
      float tmp_hlt_sumet = v_hlt_met->sumEt()/CLHEP::GeV;
      float tmp_hlt_sume  = v_hlt_met->sumE()/CLHEP::GeV; 
      ATH_MSG_DEBUG("Alternative Ex = " << tmp_hlt_ex);
      ATH_MSG_DEBUG("Alternative Ey = " << tmp_hlt_ey);
      ATH_MSG_DEBUG("Alternative MET = " << tmp_hlt_met);
      
      CLHEP::Hep3Vector v(tmp_hlt_ex, tmp_hlt_ey, tmp_hlt_ez);
      float tmp_hlt_eta = v.eta();
      float tmp_hlt_phi = v.phi();
      
      float tmp_hlt_ex_log = -9e9;
      float tmp_hlt_ey_log = -9e9;
      float tmp_hlt_ez_log = -9e9;
      float tmp_hlt_met_log = -9e9;
      float tmp_hlt_sume_log = -9e9;
      float tmp_hlt_sumet_log = -9e9;
      float tmp_hlt_significance = -9e9;
      
      float epsilon = 1e-6;  // 1 keV
      
      epsilon = 1.189;
      tmp_hlt_ex_log = signed_log(tmp_hlt_ex, epsilon);
      tmp_hlt_ey_log = signed_log(tmp_hlt_ey, epsilon);
      tmp_hlt_ez_log = signed_log(tmp_hlt_ez, epsilon);
      tmp_hlt_met_log = signed_log(tmp_hlt_met, epsilon);
      tmp_hlt_sume_log = signed_log(tmp_hlt_sume, epsilon);
      tmp_hlt_sumet_log = signed_log(tmp_hlt_sumet, epsilon);

      float tmp_resolution = 0.0;
      if (tmp_hlt_sumet>0.0) tmp_resolution = m_sigOffset + m_sigSlope*sqrt(tmp_hlt_sumet) + m_sigQuadr*tmp_hlt_sumet;
      if (tmp_resolution>0.0) tmp_hlt_significance = tmp_hlt_met/tmp_resolution;
      
      fillHLTBasicHistograms(tmp_hlt_ex,tmp_hlt_ex_log,tmp_hlt_ey,tmp_hlt_ey_log,tmp_hlt_ez,tmp_hlt_ez_log,tmp_hlt_met,tmp_hlt_met_log,tmp_hlt_sumet,tmp_hlt_sumet_log,tmp_hlt_sume,tmp_hlt_sume_log,tmp_hlt_phi,tmp_hlt_eta,tmp_hlt_significance);

    }

  }



  monGroupName = m_expert_path;
  
  //////////////////////////
  // Offline MET
  monFolderName = monGroupName + "/Offline";
  setCurrentMonGroup(monFolderName);

  if (v_off_met) {
    fillOffMETHistograms(off_met,off_ex,off_ey,off_sumet,off_phi);
  }
  

  //////////////////////////
  // L1 vs HLT
  monFolderName = monGroupName + "/L1_vs_HLT";
  setCurrentMonGroup(monFolderName);

  if (v_l1_roi_cont) {
    fillL1vsHLTHistograms(l1_met,l1_mex,l1_mey,l1_sumet,l1_phi,hlt_met,hlt_ex,hlt_ey,hlt_sumet,hlt_phi,saturated);
  }
  

  //////////////////////////
  // HLT vs Offline
  monFolderName = monGroupName + "/HLT_vs_Offline";
  setCurrentMonGroup(monFolderName);

  if (v_off_met) {
    fillHLTvsOffHistograms(hlt_met,hlt_ex,hlt_ey,hlt_sumet,hlt_phi,off_met,off_ex,off_ey,off_sumet,off_phi);
  }  
  

  //////////////////////////
  // L1 vs Offline
  monFolderName = monGroupName + "/L1_vs_Offline";
  setCurrentMonGroup(monFolderName);

  if (v_off_met) {
    fillL1vsOffHistograms(l1_met,l1_mex,l1_mey,l1_sumet,l1_phi,off_met,off_ex,off_ey,off_sumet,off_phi,saturated);
  }


  //////////////////////////
  // Electron and Muon
  monFolderName = monGroupName + "/ElMu";
  setCurrentMonGroup(monFolderName);

  int muonMult = 0;
  if (muonContEmpty == false) {
    for (auto muon : *v_hlt_muonEFcontainer) {
      if(muon->muonType() == xAOD::Muon::Combined) {
        if((h = hist("HLT_muonpt") )) h->Fill(muon->pt()/CLHEP::GeV);
        muonMult += 1;
      }
    }
    ATH_MSG_DEBUG("Muon multiciplicity: " << muonMult); 
    if((h = hist("HLT_muonmult") )) h->Fill(muonMult);
  }
  int electronMult = 0;
  if (electronContEmpty == false) {
    for (auto eg : *v_hlt_electronEFcontainer) {
      if((h = hist("HLT_electronpt") )) h->Fill(eg->pt()/CLHEP::GeV);
      electronMult += 1;
    }
    ATH_MSG_DEBUG("Electron multiciplicity: " << electronMult);
    if((h = hist("HLT_electronmult") )) h->Fill(electronMult);
  }
  
  return sc;
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1BasicHistograms() {

  addHistogram(new TH1F("L1_METx", "L1 Missing E_{x};E_{x} (GeV)", m_ec_bins, m_ec_min,  m_ec_max));
  addHistogram(new TH1F("L1_METx_log", "L1 Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("L1_METy", "L1 Missing E_{y};E_{y} (GeV)", m_ec_bins, m_ec_min,  m_ec_max));
  addHistogram(new TH1F("L1_METy_log", "L1 Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 55, -4.125, 4.125));
  addHistogram(new TH1F("L1_MET", "L1 MET (GeV);MET (GeV)", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("L1_MET_log", "L1 |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 40, -1.875, 4.125));
  addHistogram(new TH1F("L1_SumEt", "L1 SumEt (GeV);SumEt (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("L1_SumEt_log", "L1 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)", 40, -1.875, 4.125));
  addHistogram(new TH1F("L1_MET_phi",  "L1 MET #phi (rad);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1_MET_phi_etweight",  "L1 MET #phi (|Missing E_{T}|);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::fillL1BasicHistograms(float l1_mex,float l1_mex_log,float l1_mey,float l1_mey_log,float l1_met,float l1_met_log,float l1_sumet,float l1_sumet_log,float l1_phi,float saturated) {

  TH1 *h(0);
  if ((h = hist("L1_METx")))      h->Fill(l1_mex);
  if ((h = hist("L1_METx_log")))  h->Fill(l1_mex_log);
  if ((h = hist("L1_METy")))      h->Fill(l1_mey);
  if ((h = hist("L1_METy_log")))  h->Fill(l1_mey_log);
  if ((h = hist("L1_MET")))       h->Fill(l1_met);
  if ((h = hist("L1_MET_log")))   h->Fill(l1_met_log);
  if ((h = hist("L1_SumEt")))     h->Fill(l1_sumet);
  if ((h = hist("L1_SumEt_log"))) h->Fill(l1_sumet_log);
  if ((h = hist("L1_MET_phi")) && l1_met>0 && !saturated)   h->Fill(l1_phi);
  if ((h = hist("L1_MET_phi_etweight")) && l1_met>0 && !saturated)  h->Fill(l1_phi, l1_met);
  
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1JetHistograms() {

  const int nBins = 13;
  double edges[nBins + 1] = {-4.9,-3.1,-2.5,-1.9,-1.3,-0.75,-0.25,0.25,0.75,1.3,1.9,2.5,3.1,4.9};
  addHistogram(new TH2F("L1_Jet_eta_pt", "L1 Jet #eta/p_{T};#eta;p_{T} [GeV]", nBins, edges, 80, 10, 90));
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::fillL1JetHistograms(float l1_jet_pt,float l1_jet_eta) {

  TH2 *h2(0);
  if ((h2 = hist2("L1_Jet_eta_pt")) && l1_jet_pt>0)  h2->Fill(l1_jet_eta, l1_jet_pt);
  
}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1ProfileHistograms(std::map<std::string, int> met_signatures_tolook) {

  std::map<std::string,int>::const_iterator it;
  for (it = met_signatures_tolook.begin(); it != met_signatures_tolook.end(); it++) {
    std::string prof_name = "Eff_" + it->first;
    std::string prof_title = prof_name + " Efficiency Missing E_{T};ME_{T} (GeV)";
    addProfile(new TProfile(prof_name.c_str(), prof_title.c_str(), m_eff_bins, m_eff_min, m_eff_max));
    
    prof_name = "Eff_mu_" + it->first;
    prof_title = prof_name + " Efficiency Missing E_{T};ME_{T} (GeV)";
    addProfile(new TProfile(prof_name.c_str(), prof_title.c_str(), m_eff_bins, m_eff_min, m_eff_max));

    prof_name = "Eff_mumu_" + it->first;
    prof_title = prof_name + " Efficiency Missing E_{T};ME_{T} (GeV)";
    addProfile(new TProfile(prof_name.c_str(), prof_title.c_str(), m_eff_bins, m_eff_min, m_eff_max));
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::fillL1ProfileHistograms(float off_met,float pT_mumu,bool METMuonFilled,std::map<std::string, int> met_signatures_tolook) {

  std::map<std::string,int>::const_iterator it;
  TProfile *p(0);
  TProfile *pmu(0);
  for (it = met_signatures_tolook.begin(); it != met_signatures_tolook.end(); it++) {
    std::string name = it->first;
    std::string profname = "Eff_"+ name;
    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      if ((p = profile(profname))) p->Fill(off_met,1.0,1.0);
    } else {
      if ((p = profile(profname))) p->Fill(off_met,0.0,1.0);
    }

    if(METMuonFilled == true) {
      std::string name = it->first;
      std::string mu_profname = "Eff_mu_"+name;
      if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
	if ((pmu = profile(mu_profname))) pmu->Fill(off_met, 1.0, 1.0);
      } else {
	if ((pmu = profile(mu_profname))) pmu->Fill(off_met, 0.0, 1.0);
      }
      if(pT_mumu>0){
	std::string mumu_name = it->first;
	std::string mumu_profname = "Eff_mumu_"+name;
	if (getTDT()->isPassed(mumu_name, TrigDefs::eventAccepted)) {
	  if ((pmu = profile(mumu_profname))) pmu->Fill(pT_mumu, 1.0, 1.0);
	} else {
	  if ((pmu = profile(mumu_profname))) pmu->Fill(pT_mumu, 0.0, 1.0);
	}
      }//pT_mumu > 0
    }//loop over signatures  
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTBasicHistograms() {

  addHistogram(new TH1F("HLT_MEx", "HLT Missing E_{x};E_{x} (GeV)", m_ec_bins, m_ec_min,  m_ec_max));
  addHistogram(new TH1F("HLT_MEx_log", "HLT Missing E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)", 27, -4.125, 4.125));
  addHistogram(new TH1F("HLT_MEy", "HLT Missing E_{y};E_{y} (GeV)", m_ec_bins, m_ec_min,  m_ec_max));
  addHistogram(new TH1F("HLT_MEy_log", "HLT Missing E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)", 27, -4.125, 4.125));
  addHistogram(new TH1F("HLT_MEz", "HLT Missing E_{z};E_{z} (GeV)", m_ec_bins, m_ec_min, m_ec_max));
  addHistogram(new TH1F("HLT_MEz_log", "HLT Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)",27, -4.125, 4.125));
  addHistogram(new TH1F("HLT_MET", "HLT |Missing E_{T}|;ME_{T} (GeV)", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("HLT_MET_log",   "HLT |Missing E_{T}|;log_{10}(ME_{T}/GeV)", 20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_MET_lin1", "HLT |Missing E_{T}| (0-10 GeV);ME_{T} (GeV)", 110, -0.5, 10.5));
  addHistogram(new TH1F("HLT_SumEt", "HLT Sum |E_{T}|;SumE_{T} (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("HLT_SumEt_log", "HLT Sum |E_{T}|;log_{10}(SumE_{T}/GeV)", 20, -1.875, 4.125));
  addHistogram(new TH1F("HLT_SumE", "HLT Sum |E|;SumE (GeV)", m_sume_bins, m_sume_min, m_sume_max));
  addHistogram(new TH1F("HLT_SumE_log",   "HLT Sum |E|;log_{10}(SumE/GeV)",40, -1.875, 6.125));
  addHistogram(new TH1F("HLT_MET_phi", "HLT MET #phi (rad);#phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_phi_etweight",  "HLT MET #phi (|Missing E_{T}|);MET #phi (rad)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_MET_eta", "HLT MET #eta (rad);#eta", m_eta_bins, m_eta_min, m_eta_max));
  addHistogram(new TH1F("HLT_MET_eta_etweight",  "HLT MET #eta (|Missing E_{T}|);MET #eta", m_eta_bins, m_eta_min, m_eta_max));
  addHistogram(new TH2F("HLT_MET_etaphi", "HLT MET #eta/#phi;#eta;#phi (rad)", m_eta_bins, m_eta_min, m_eta_max, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH2F("HLT_MET_etaphi_etweight", "HLT MET #eta/#phi(|Missing E_{T}|);#eta;#phi (rad)", m_eta_bins, m_eta_min, m_eta_max, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_XS", "HLT MET Significance;Significance (XS/GeV^{1/2})", m_xs_bins, m_xs_min,  m_xs_max));

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::fillHLTBasicHistograms(float hlt_ex,float hlt_ex_log,float hlt_ey,float hlt_ey_log,float hlt_ez,float hlt_ez_log,float hlt_met,float hlt_met_log,float hlt_sumet,float hlt_sumet_log,float hlt_sume,float hlt_sume_log,float hlt_phi,float hlt_eta,float hlt_significance) {

  TH1 *h(0);
  TH2 *h2(0);
  if ((h = hist("HLT_MEx")))       h->Fill(hlt_ex);
  if ((h = hist("HLT_MEx_log"))) h->Fill(hlt_ex_log);
  if ((h = hist("HLT_MEy")))       h->Fill(hlt_ey);
  if ((h = hist("HLT_MEy_log"))) h->Fill(hlt_ey_log);
  if ((h = hist("HLT_MEz")))       h->Fill(hlt_ez);
  if ((h = hist("HLT_MEz_log")))  h->Fill(hlt_ez_log);
  if ((h = hist("HLT_MET")))       h->Fill(hlt_met);
  if ((h = hist("HLT_MET_log")))   h->Fill(hlt_met_log);
  if ((h = hist("HLT_MET_lin1"))) h->Fill(hlt_met);
  if ((h = hist("HLT_SumEt")))     h->Fill(hlt_sumet);
  if ((h = hist("HLT_SumEt_log"))) h->Fill(hlt_sumet_log);
  if ((h = hist("HLT_SumE")))      h->Fill(hlt_sume);
  if ((h = hist("HLT_SumE_log"))) h->Fill(hlt_sume_log);
  if ((h = hist("HLT_MET_phi")) && hlt_met>0)   h->Fill(hlt_phi);
  if ((h = hist("HLT_MET_phi_etweight")) && hlt_met>0)  h->Fill(hlt_phi, hlt_met);
  if ((h = hist("HLT_MET_eta")) && hlt_met>0)   h->Fill(hlt_eta);
  if ((h = hist("HLT_MET_eta_etweight")) && hlt_met>0)  h->Fill(hlt_eta, hlt_met);
  if ((h2 = hist2("HLT_MET_etaphi")) && hlt_met>0) h2->Fill(hlt_eta, hlt_phi);
  if ((h2 = hist2("HLT_MET_etaphi_etweight")) && hlt_met>0) h2->Fill(hlt_eta, hlt_phi, hlt_met);
  if ((h = hist("HLT_XS"))) h->Fill(hlt_significance);

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTProfileHistograms(std::map<std::string, int> met_signatures_tolook) {

  std::map<std::string,int>::const_iterator it;
  for (it = met_signatures_tolook.begin(); it != met_signatures_tolook.end(); it++) {
    std::string prof_name = "Eff_" + it->first;
    std::string prof_title = prof_name + " Efficiency Missing E_{T};ME_{T} (GeV)";
    addProfile(new TProfile(prof_name.c_str(), prof_title.c_str(), m_eff_bins, m_eff_min, m_eff_max));
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::fillHLTProfileHistograms(float off_met,std::map<std::string, int> met_signatures_tolook) {

  std::map<std::string,int>::const_iterator it;
  TProfile *p(0);
  for (it = met_signatures_tolook.begin(); it != met_signatures_tolook.end(); it++) {
    std::string name = it->first;
    std::string profname = "Eff_"+name;
    if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
      if ((p = profile(profname))) p->Fill(off_met,1.0,1.0);
    } else {
      if ((p = profile(profname))) p->Fill(off_met,0.0,1.0);
   }
  }

}


//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTStatusHistograms() {

  // Lumiblock histogram
  addHistogram(new TH1F("HLT_limiBlock", "HLT Lumi Block", 1000, 0, 1000));  

  // <mu> histogram
  addHistogram(new TH1F("HLT_mu", "HLT mu", 100, 0, 100));  

  // status in 1d
  TH1F *h1i = new TH1F("HLT_MET_status", "HLT MET Status", 32, -0.5, 31.5);
  for (size_t j = 0; j < m_bitNames.size(); j++) {
    if(j < m_bitNames.size()) h1i->GetXaxis()->SetBinLabel(j+1, m_bitNames[j].c_str());
  }
  addHistogram(h1i);

  // HLT MET all components in one 2d histogram
  float fMaxEFC = float(m_compNames.size());
  fMaxEFC -= 0.5;
  int fBinEFC = int(m_compNames.size());
  TH2F *h2f[2] = {0};
  h2f[0] = new TH2F("compN_HLT_MET_status",   "HLT MET Status VS component;;", fBinEFC, -0.5, fMaxEFC,32, -0.5, 31.5);
  h2f[1] = new TH2F("compN_compEt_lin",  "HLT MissingE_{T} VS component;;ME_{T} (GeV)", fBinEFC, -0.5, fMaxEFC, 102, -13.5, 601.5);
  for (size_t k = 0; k < 2; k++) { // 2 hists
    for (size_t cn = 0; cn < m_compNames.size(); cn++) { // 25 bins
      if(cn < m_compNames.size()) h2f[k]->GetXaxis()->SetBinLabel(cn+1, m_compNames[cn].c_str());
    }
    addHistogram(h2f[k]);
  }

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
      h2f[k]->GetYaxis()->SetBinLabel(1,"none");
    }
    addHistogram(h2f[k]);
  }

  int phibins = 16; double phimin = m_phi_min; double phimax = m_phi_max;
  int etabins = 24; double etamin = -4.8;   double etamax =  4.8;

  for (unsigned int j = 0; j < m_compNames.size(); j++) {
    std::string name = Form("compEt_lin_EtaPhi_%02d", j);
    std::string title = m_compNames[j];
    title += ": MissingE_{T}(#eta, #phi); #eta; #phi [rad];ME_{T}/GeV";
    TH2F* h = new TH2F(name.c_str(), title.c_str(), etabins, etamin, etamax, phibins, phimin, phimax);
    addHistogram(h);
  }

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addOffMETHistograms() {

  addHistogram(new TH1F("Offline_MET", "Offline MET;Et", m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH1F("Offline_METx", "Offline METx;Ex", m_ec_bins, m_ec_min, m_ec_max));
  addHistogram(new TH1F("Offline_METy", "Offline METy;Ey", m_ec_bins, m_ec_min, m_ec_max));
  addHistogram(new TH1F("Offline_SumEt", "Offline SumEt;sumEt", m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH1F("Offline_MET_phi", "Offline MET phi;Phi", m_phi_bins, m_phi_min, m_phi_max));

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::fillOffMETHistograms(float off_met,float off_ex,float off_ey,float off_sumet,float off_phi) {

  TH1 *h(0);
  if ((h = hist("Offline_MET")))    h->Fill(off_met);
  if ((h = hist("Offline_METx")))   h->Fill(off_ex);
  if ((h = hist("Offline_METy")))   h->Fill(off_ey);
  if ((h = hist("Offline_SumEt")))   h->Fill(off_sumet);
  if ((h = hist("Offline_MET_phi")) && off_met>0)   h->Fill(off_phi);

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1vsHLTHistograms() {

  addHistogram(new TH2F("L1_HLT_EtCor", "L1 vs HLT Missing E_{T} ; HLTME_{T} (GeV); L1ME_{T} (GeV)", m_et_bins, m_et_min, m_et_max, m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH2F("L1_HLT_SumEtCor", "L1 vs HLT Sum |E_{T}| ; HLTSum|E_{T}| (GeV); L1Sum|E_{T}| (GeV)", m_sumet_bins, m_sumet_min, m_sumet_max, m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH2F("L1_HLT_PhiCor", "L1 vs HLT #phi ; EF #phi (rad); L1 #phi (rad)", m_phi_bins, m_phi_min, m_phi_max, m_phi_bins, m_phi_min, m_phi_max));

  addHistogram(new TH1F("L1_HLT_dEt",  "#DeltaE_{T}(L1-HLT) ; #DeltaE_{T} (GeV)", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1_HLT_dPhi", "#Delta#phi(L1-HLT) ; #Delta#phi (GeV)", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1_HLT_dEx",  "#DeltaE_{x}(L1-HLT) ; #DeltaE_{x} (GeV)", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1_HLT_dEy",  "#DeltaE_{y}(L1-HLT) ; #DeltaE_{y} (GeV)", m_det_bins, m_det_min, m_det_max));

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::fillL1vsHLTHistograms(float l1_met,float l1_sumet,float l1_phi,float l1_mex,float l1_mey,float hlt_met,float hlt_sumet,float hlt_phi,float hlt_ex,float hlt_ey,float saturated) {

  TH1 *h(0);
  TH2 *h2(0);
  if ((h2 = hist2("L1_HLT_EtCor")))     h2->Fill(hlt_met, l1_met);
  if ((h2 = hist2("L1_HLT_SumEtCor")))  h2->Fill(hlt_sumet, l1_sumet);
  if ((h2 = hist2("L1_HLT_PhiCor")) && hlt_met>0 && !saturated)    h2->Fill(hlt_phi, l1_phi);
  
  if ((h = hist("L1_HLT_dEt")))         h->Fill(l1_met - hlt_met);
  double dphi = signed_delta_phi(l1_phi, hlt_phi);
  if ((h = hist("L1_HLT_dPhi")) && hlt_met>0)  h->Fill(dphi);
  if ((h = hist("L1_HLT_dEx")))   h->Fill(l1_mex - hlt_ex);
  if ((h = hist("L1_HLT_dEy")))   h->Fill(l1_mey - hlt_ey); 

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addHLTvsOffHistograms() {

  addHistogram(new TH2F("HLT_Off_EtCor", "HLT vs Offline Missing E_{T} ; Offline MET ; HLT MET", m_et_bins, m_et_min, m_et_max, m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH2F("HLT_Off_SumEtCor", "HLT vs Offline Sum |E_{T}| ; Offline SumEt ; HLT SumEt", m_sumet_bins, m_sumet_min, m_sumet_max, m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH2F("HLT_Off_PhiCor", "HLT vs Offline #phi ; Offline #phi ; HLT #phi", m_phi_bins, m_phi_min, m_phi_max, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_Off_dEt", "#DeltaE_{T}(HLT-Offline) ; dEt", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("HLT_Off_dPhi", "#Delta#phi(HLT-Offline) ; dPhi", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("HLT_Off_dEx", "#DeltaE_{x}(HLT-Offline) ; dEx", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("HLT_Off_dEy", "#DeltaE_{y}(HLT-Offline) ; dEy", m_det_bins, m_det_min, m_det_max));   

}
//___________________________________________________________________________________________________________
void HLTMETMonTool::fillHLTvsOffHistograms(float hlt_met,float hlt_ex,float hlt_ey,float hlt_sumet,float hlt_phi,float off_met,float off_ex,float off_ey,float off_sumet,float off_phi) {

  TH1 *h(0);
  TH2 *h2(0);
  if ((h2 = hist2("HLT_Off_EtCor")))    h2->Fill(off_met, hlt_met);
  if ((h2 = hist2("HLT_Off_SumEtCor"))) h2->Fill(off_sumet, hlt_sumet);
  if ((h2 = hist2("HLT_Off_PhiCor")) && hlt_met>0)   h2->Fill(off_phi, hlt_phi);
  if ((h = hist("HLT_Off_dEt")))        h->Fill(hlt_met - off_met);
  double dphi = signed_delta_phi(hlt_phi, off_phi);
  if ((h = hist("HLT_Off_dPhi")) && hlt_met>0)  h->Fill(dphi);
  if ((h = hist("HLT_Off_dEx")))   h->Fill(hlt_ex - off_ex);
  if ((h = hist("HLT_Off_dEy")))   h->Fill(hlt_ey - off_ey);

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addL1vsOffHistograms() {

  addHistogram(new TH2F("L1_Off_EtCor", "L1 vs Offline Missing E_{T} ; Offline MET ; L1 MET", m_et_bins, m_et_min, m_et_max, m_et_bins, m_et_min, m_et_max));
  addHistogram(new TH2F("L1_Off_SumEtCor", "L1 vs Offline Sum |E_{T}| ; Offline SumEt ; L1 SumEt", m_sumet_bins, m_sumet_min, m_sumet_max, m_sumet_bins, m_sumet_min, m_sumet_max));
  addHistogram(new TH2F("L1_Off_PhiCor", "L1 vs Offline #phi ; Offline #phi ; L1 #phi", m_phi_bins, m_phi_min, m_phi_max, m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1_Off_dEt", "#DeltaE_{T}(L1-Offline) ; dEt", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1_Off_dPhi", "#Delta#phi(L1-Offline) ; dPhi", m_phi_bins, m_phi_min, m_phi_max));
  addHistogram(new TH1F("L1_Off_dEx", "#DeltaE_{x}(L1-Offline) ; dEx", m_det_bins, m_det_min, m_det_max));
  addHistogram(new TH1F("L1_Off_dEy", "#DeltaE_{y}(L1-Offline) ; dEy", m_det_bins, m_det_min, m_det_max));   

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::fillL1vsOffHistograms(float l1_met,float l1_mex,float l1_mey,float l1_sumet,float l1_phi,float off_met,float off_ex,float off_ey,float off_sumet,float off_phi,float saturated) {
  
  TH1 *h(0);
  TH2 *h2(0);
  if ((h2 = hist2("L1_Off_EtCor")))    h2->Fill(off_met, l1_met);
  if ((h2 = hist2("L1_Off_SumEtCor"))) h2->Fill(off_sumet, l1_sumet);
  if ((h2 = hist2("L1_Off_PhiCor")) && l1_met>0 && !saturated)   h2->Fill(off_phi, l1_phi);
  if ((h = hist("L1_Off_dEt")))        h->Fill(l1_met - off_met);
  double dphi = signed_delta_phi(l1_phi, off_phi);
  if ((h = hist("L1_Off_dPhi")) && l1_met>0)  h->Fill(dphi);
  if ((h = hist("L1_Off_dEx")))   h->Fill(l1_mex - off_ex);
  if ((h = hist("L1_Off_dEy")))   h->Fill(l1_mey - off_ey);

}

//___________________________________________________________________________________________________________
void HLTMETMonTool::addElMuHistograms() {

  // muon histograms
  addHistogram(new TH1F("HLT_electronpt", "HLT Electron Pt (GeV)", 50, 0, 100));
  addHistogram(new TH1F("HLT_electronmult", "HLT Electron Multiplicity", 10, 0, 10));

  // electron histograms
  addHistogram(new TH1F("HLT_muonpt", "HLT Muon Pt (GeV)", 200, 0, 100));
  addHistogram(new TH1F("HLT_muonmult", "HLT Muon Multiplicity", 10, 0, 10));

}


//___________________________________________________________________________________________________________
void HLTMETMonTool::trigger_decision() {

  std::vector<std::map<std::string,int>> v_met_signatures_tolook_shifter;
  v_met_signatures_tolook_shifter.push_back(m_l1_met_signatures_tolook_shifter);
  v_met_signatures_tolook_shifter.push_back(m_hlt_met_signatures_tolook_shifter);

  if (m_print_met_trig_stats) {
    std::map<std::string,int>::iterator iter;
    for (unsigned int i = 0; i<v_met_signatures_tolook_shifter.size(); i++) {
      for (iter = v_met_signatures_tolook_shifter[i].begin(); iter != v_met_signatures_tolook_shifter[i].end(); ++iter) {
        std::string name = iter->first;
        if (getTDT()->isPassed(name, TrigDefs::eventAccepted)) {
          iter->second +=1;
        }
      }
    }
  }
}

//___________________________________________________________________________________________________________
std::string HLTMETMonTool::get_trigger_level(std::string item)
{
  size_t n = item.find_first_of('_');
  return item.substr(0, n);
}


//___________________________________________________________________________________________________________
std::string HLTMETMonTool::get_trigger_algo(std::string item)
{
  std::string algo = "";
  if (item.find("mht_lcw") != std::string::npos)
    algo = "mht";
  else if (item.find("mht_em") != std::string::npos)
    algo = "mhtem";
  else if (item.find("trkmht_FS") != std::string::npos)
    algo = "trkmht";
  else if (item.find("trkmht_FTK") != std::string::npos)
    algo = "trkmhtFTK";
  else if (item.find("tc_lcw") != std::string::npos)
    algo = "topocl";
  else if (item.find("tc_em") != std::string::npos)
    algo = "tcem";
  else if (item.find("pueta") != std::string::npos)
    algo = "topocl_PS";
  else if (item.find("pufit") != std::string::npos)
    algo = "topocl_PUC";
  else if (item.find("feb") != std::string::npos)
    algo = "FEB";
  else if (item.find("fex") != std::string::npos)
    algo = "Fex";
  else if (item.find("cell") != std::string::npos)
    algo = "cell";
  
  return algo;
}


//___________________________________________________________________________________________________________
void HLTMETMonTool::check_triggers(std::vector<std::string>& v_triggers, std::map<std::string,int> &v_signatures) {

  const std::vector<std::string> trig_L1_items  = getTDT()->getListOfTriggers("L1_.*");
  const std::vector<std::string> trig_EF_items  = getTDT()->getListOfTriggers("EF_.*");
  const std::vector<std::string> trig_HLT_items = getTDT()->getListOfTriggers("HLT_.*");

  std::vector<std::string>::const_iterator iter;

  for (unsigned int it = 0; it < v_triggers.size(); ++it) { // Loop over trigger items

    std::string item = v_triggers[it];
    std::string trig_level = get_trigger_level(item); //item.substr(0,3);

    ATH_MSG_DEBUG("signature " << item << " requested");

    // check your trigger is defined in the menu
    bool triggerFound = false;
    if (trig_level == "L1") {
      for (iter=trig_L1_items.begin(); iter!=trig_L1_items.end(); ++iter) {
        std::string L1name = *iter;
        if (L1name == item) {
          triggerFound = true;
          v_signatures.insert(std::map<std::string,int>::value_type(item, 0));
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
          v_signatures.insert(std::map<std::string,int>::value_type(item, 0));
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
          v_signatures.insert(std::map<std::string,int>::value_type(item, 0));
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
  ATH_MSG_DEBUG("Level-1 Met Triggers to look: " << m_l1_met_signatures_tolook_shifter.size());
  std::map<std::string,int>::iterator iter;
  for (iter = m_l1_met_signatures_tolook_shifter.begin(); iter!= m_l1_met_signatures_tolook_shifter.end(); ++iter) {
    ATH_MSG_DEBUG("Events passing " << iter->first << ": " << iter->second);
  }

  // EF
  ATH_MSG_DEBUG("HLT Met Triggers to look: "  << m_hlt_met_signatures_tolook_shifter.size());
  for (iter = m_hlt_met_signatures_tolook_shifter.begin(); iter != m_hlt_met_signatures_tolook_shifter.end(); ++iter) {
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
  if (std::abs(e) > epsilon)
    e_log = copysign(log10(std::abs(e)), e);
  else
    e_log = 0.01;

  return e_log;
}
