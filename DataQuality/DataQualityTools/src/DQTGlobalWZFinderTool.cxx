/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTGlobalWZFinderTool.cxx
// PACKAGE:  DataQualityTools  
// 
// AUTHORS:   Jahred Adelman (jahred.adelman@cern.ch)
//            Simon Viel (svielcern.ch)
//            Sam King (samking@physics.ubc.ca)
//            Koos van Nieuwkoop (jvannieu@cern.ch)
//
// ********************************************************************

#include "DataQualityTools/DQTGlobalWZFinderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronxAODHelpers.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEgamma/EgammaTruthxAODHelpers.h"
#include "xAODEventInfo/EventInfo.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"

#include <vector>

#include "TProfile.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

using xAOD::Jet;
using xAOD::JetContainer;
using Gaudi::Units::GeV;
using Gaudi::Units::mm;
using namespace MCTruthPartClassifier;

//----------------------------------------------------------------------------------
DQTGlobalWZFinderTool::DQTGlobalWZFinderTool(const std::string & type, 
           const std::string & name,
           const IInterface* parent)
   : DataQualityFatherMonTool(type, name, parent),
     m_muon_Pt(nullptr),
     m_muon_Eta(nullptr),
     m_ele_Et(nullptr),
     m_ele_Eta(nullptr),
     m_electronContainerName("Electrons"),
     m_egDetailContainerName("egDetailAOD"),
     m_VxPrimContainerName("PrimaryVertices"),
     m_VxContainerName("SecVertices"), //Kshort
     m_METName("MET_Reference_AntiKt4EMTopo"),
     m_muonContainerName("Muons"),
     m_jetCollectionName("AntiKt4EMTopoJets"),
     m_tracksName("InDetTrackParticles"), //Kshort
     m_electronEtCut(25),
     m_muonPtCut(25),
     m_metCut(25),
     m_zCutLow(66.0),
     m_zCutHigh(116.0),
     m_muonMaxEta(2.4),
     m_muonSelectionTool("CP::MuonSelectionTool/MuonSelectionTool"),
     m_isolationSelectionTool("CP::IsolationSelectionTool/IsolationSelectionTool"),
     m_muTrigMatchTool("CP::TrigMuonMatching/TrigMuonMatching", this),
     m_useOwnMuonSelection(false),
     m_Jpsi_mm_trigger{"CATEGORY_primary_bphys"},
     m_Z_mm_trigger{"CATEGORY_monitoring_muonIso", "CATEGORY_monitoring_muonNonIso"},
     m_Z_ee_trigger{"CATEGORY_primary_single_ele_iso", "CATEGORY_primary_single_ele"},
     m_elTrigMatchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool", this),
     m_truthClassifier("MCTruthClassifier/IMCTruthClassifier", this),
     m_bcTool("Trig::TrigConfBunchCrossingTool/BunchCrossingTool", this)

//----------------------------------------------------------------------------------

{
  m_path = "GLOBAL/DQTGlobalWZFinder";
  declareProperty("ElectronContainerName", m_electronContainerName);
  declareProperty("ElectronDetailContainerName", m_egDetailContainerName);
  declareProperty("MetName", m_METName);
  declareProperty("MuonContainerName",m_muonContainerName);
  declareProperty("JetCollectionName", m_jetCollectionName);
  declareProperty("ElectronEtCut",m_electronEtCut);
  declareProperty("MuonPtCut",m_muonPtCut);
  declareProperty("MetCut",m_metCut);
  declareProperty("ZCutLow", m_zCutLow);
  declareProperty("ZCutHigh", m_zCutHigh);
  declareProperty("doTrigger", m_doTrigger = 1);    
  declareProperty("ZmmTriggers", m_Z_mm_trigger);
  declareProperty("ZeeTriggers", m_Z_ee_trigger);
  declareProperty("MuonSelectionTool", m_muonSelectionTool);
  declareProperty("IsolationSelectionTool", m_isolationSelectionTool);
  declareProperty("MuonTrigMatchingTool", m_muTrigMatchTool);
  declareProperty("TrigEgammaMatchingTool", m_elTrigMatchTool);
  declareProperty("UseOwnMuonSelection", m_useOwnMuonSelection);
  declareProperty("MuonMaxEta", m_muonMaxEta);
  declareProperty("BCTool", m_bcTool);
  declareProperty("FillBCIDTrees", m_do_BCID = false);

  m_W_mt_ele = 0;
  m_W_mt_mu = 0;
  m_W_pt_v_met_ele = 0;
  m_W_pt_v_met_mu = 0;
  m_Z_mass_opsele = 0;
  m_Z_mass_opsmu = 0;
  m_Z_mass_ssele = 0;
  m_Z_mass_ssmu = 0;
  m_Z_Q_ele = 0;
  m_Z_Q_mu = 0;
  m_metHist = 0;
  m_metPhiHist = 0;
  
  m_livetime_lb = nullptr;
  m_lblength_lb = nullptr;
  m_mu_lb = nullptr;
  m_Z_ee_trig_ps = nullptr;
  m_Z_mm_trig_ps = nullptr;

}

//----------------------------------------------------------------------------------
DQTGlobalWZFinderTool::~DQTGlobalWZFinderTool()
//----------------------------------------------------------------------------------
{
}

StatusCode DQTGlobalWZFinderTool::initialize() {
  ATH_CHECK(m_muTrigMatchTool.retrieve());
  ATH_CHECK(m_elTrigMatchTool.retrieve());
  ATH_CHECK(m_bcTool.retrieve());

  return ManagedMonitorToolBase::initialize();
}


StatusCode DQTGlobalWZFinderTool::bookHistogramsRecurrent()
{ 
  updateTriggersForGroups(m_Jpsi_mm_trigger);
  updateTriggersForGroups(m_Z_mm_trigger);
  updateTriggersForGroups(m_Z_ee_trigger);

  ATH_MSG_DEBUG("Trigger information for WZFinder");
  ATH_MSG_DEBUG("For Z->mm:");

  for (auto trig : m_Z_ee_trigger){
    ATH_MSG_DEBUG("ZeeTrigger: " + trig);
  }

  bool failure(false);

  std::string  fullPathDQTGlobalWZFinder=m_path;
  

  const xAOD::EventInfo* thisEventInfo;
  StatusCode sc = evtStore()->retrieve(thisEventInfo);
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not find EventInfo in evtStore()");
    return sc;
  } else {
    m_this_lb = thisEventInfo->lumiBlock() ;
  }

  if (!m_isSimulation) {
    failure |= registerHist(fullPathDQTGlobalWZFinder, m_livetime_lb = new TProfile("m_livetime_lb", "Livetime", 1, m_this_lb-0.5, m_this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
    failure |= registerHist(fullPathDQTGlobalWZFinder, m_lblength_lb = new TProfile("m_lblength_lb", "LB length", 1, m_this_lb-0.5, m_this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
    failure |= registerHist(fullPathDQTGlobalWZFinder, m_mu_lb = new TProfile("m_mu_lb", "#mu", 1, m_this_lb-0.5, m_this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
    failure |= registerHist(fullPathDQTGlobalWZFinder, m_Z_ee_trig_ps = new TProfile("m_Z_ee_trig_ps", "Z->ee trigger PS", 1, m_this_lb-0.5, m_this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
    failure |= registerHist(fullPathDQTGlobalWZFinder, m_Z_mm_trig_ps = new TProfile("m_Z_mm_trig_ps", "Z->#mu#mu trigger PS", 1, m_this_lb-0.5, m_this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
    failure |= registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_El_os = new TH1F("m_Z_Counter_el_os","Z #rightarrow ee Count per Lumi Block", 1, m_this_lb-0.5, m_this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
    failure |= registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_El_ss = new TH1F("m_Z_Counter_el_ss","Z #rightarrow ee Count per Lumi Block", 1, m_this_lb-0.5, m_this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
    failure |= registerHist(fullPathDQTGlobalWZFinder, m_ZBosonCounter_Mu = new TH1F("m_Z_Counter_mu","Z #rightarrow #mu#mu Count per Lumi Block", 1, m_this_lb-0.5, m_this_lb+0.5), lumiBlock, ATTRIB_UNMANAGED, "merge").isFailure();
   
    if (!failure) {
      m_livetime_lb->Fill(m_this_lb, lbAverageLivefraction());
      m_lblength_lb->Fill(m_this_lb, lbDuration());
      m_mu_lb->Fill(m_this_lb, lbAverageInteractionsPerCrossing());
    }  

  }

  return (failure ? StatusCode::FAILURE : StatusCode::SUCCESS);
}

//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::bookHistograms( )
//----------------------------------------------------------------------------------
{
  bool failure(false);

  MsgStream log(msgSvc(), name());
  m_printedErrorEleContainer = false;
  m_printedErrorMuColl = false;
  m_printedErrorMet = false;
  m_printedErrorTrackContainer = false;

  //Initialize counters for rate monitors
  for (int index =0 ; index < 2 ; index++) {
    m_JPsiCounterSBG[index]=0;
    m_JPsiCounter2BG[index]=0;
    m_JPsiCounterSignal[index]=0;

    m_UpsilonCounterSBG[index]=0;
    m_UpsilonCounterBG[index]=0;
    m_UpsilonCounterSignal[index]=0;

    m_ZBosonCounterSBG_El[index]=0;
    m_ZBosonCounterSBG_Mu[index]=0;
  }
  
  ATH_MSG_DEBUG("in bookHistograms() and m_doRunCosmics = " << m_doRunCosmics << " and m_doRunBeam = " << m_doRunBeam);
  ATH_MSG_DEBUG("Using base path " << m_path);
     
 failure = bookDQTGlobalWZFinderTool();
  
  if (failure) {return  StatusCode::FAILURE;}
  else {return StatusCode::SUCCESS;}
}   

        

//----------------------------------------------------------------------------------
bool DQTGlobalWZFinderTool::bookDQTGlobalWZFinderTool()
//----------------------------------------------------------------------------------
{
  bool failure(false);
  MsgStream log(msgSvc(), name());
 
  std::string  fullPathDQTGlobalWZFinder=m_path;
  
  const xAOD::EventInfo* thisEventInfo;
  StatusCode sc = evtStore()->retrieve(thisEventInfo);
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not find EventInfo in evtStore()");
    return sc;
  } else {
    m_this_lb = thisEventInfo->lumiBlock() ;
  }
  
  
  m_isSimulation = thisEventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
  
  // Ideally want to also check for a job option so it can run on data
  if (m_isSimulation){
    m_writeTTrees = true;
  }
  else{
    m_writeTTrees = false;
  }

  if (m_doRunBeam) {
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_mt_ele = TH1F_LW::create("m_W_mt_ele","W#rightarrowe#nu M_{T} ",40,20*1000,400*1000)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_mt_mu = TH1F_LW::create("m_W_mt_mu","W#rightarrow#mu#nu M_{T} ",40,20*1000,400*1000)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_pt_v_met_ele = TH2F_LW::create("m_W_pt_v_met_ele","W#rightarrowe#nu e E_{T} vs met ",100,5*1000,200*1000, 100, 5*1000, 200*1000)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_W_pt_v_met_mu = TH2F_LW::create("m_W_pt_v_met_mu","W#rightarrow#mu#nu #mu p_{T} vs met ",100,5*1000,200*1000, 100, 5*1000, 200*1000)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_metHist = TH1F_LW::create("m_metHist","MET",50,0*1000,500*1000)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_metPhiHist = TH1F_LW::create("m_metPhiHist","MET phi",10,-1.6,1.6)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_JPsi_mass_opsmu = TH1F_LW::create("m_JPsi_mass_opsmu","JPsi#rightarrow#mu#mu (op. sign) Mass ",40, 2600, 3600)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Upsilon_mass_opsmu = TH1F_LW::create("m_Upsilon_mass_opsmu", "Upsilon#rightarrow#mu#mu", 40, 4500, 14500)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_Q_ele = TH1F_LW::create("m_Z_Q_ele","Z#rightarrowee Charge ",7, -3, 3)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_Q_mu = TH1F_LW::create("m_Z_Q_mu","Z#rightarrow#mu#mu Charge ",7, -3, 3)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_muon_Pt = TH1F_LW::create("m_muon_Pt", "Selected Muon p_{T}", 50, 0, 200)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_muon_Eta = TH1F_LW::create("m_muon_Eta", "Selected Muon #eta", 50, -2.5, 2.5)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_ele_Et = TH1F_LW::create("m_ele_Et", "Selected Electron E_{T}", 50, 0, 200)).isFailure();
    failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_ele_Eta = TH1F_LW::create("m_ele_Eta", "Selected Electron #eta", 50, -2.5, 2.5)).isFailure();


    if (!m_isSimulation){
      int nzbins = int(ceilf( m_zCutHigh - m_zCutLow ));
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_opsele = TH1F_LW::create("m_Z_mass_opsele","Z#rightarrowee (op. sign) Mass ", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_opsmu = TH1F_LW::create("m_Z_mass_opsmu","Z#rightarrow#mu#mu (op. sign) Mass ", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_ssele = TH1F_LW::create("m_Z_mass_ssele","Z#rightarrowee (same sign) Mass ", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_Z_mass_ssmu = TH1F_LW::create("m_Z_mass_ssmu","Z#rightarrow#mu#mu (same sign) Mass ", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();

      // T&P trigger rate
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_mutrigtp_matches = TH1F_LW::create("m_mutrigtp_matches", "Muon trigger TP stats", 3, -0.5, 2.5), lumiBlock).isFailure();
      

      // T&P muon eff
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_muloosetp_match_os = TH1F_LW::create("m_muloosetp_match_os", "Muon loose TP match OS", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_muloosetp_match_ss = TH1F_LW::create("m_muloosetp_match_ss", "Muon loose TP match SS", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_muloosetp_nomatch_os = TH1F_LW::create("m_muloosetp_nomatch_os", "Muon loose TP nomatch OS", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_muloosetp_nomatch_ss = TH1F_LW::create("m_muloosetp_nomatch_ss", "Muon loose TP nomatch SS", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      

      // T&P muon inner detector
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_mu_InDet_tp_match_os = TH1F_LW::create("m_mu_InDet_tp_match_os", "Muon inner detector TP match OS", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_mu_InDet_tp_match_ss = TH1F_LW::create("m_mu_InDet_tp_match_ss", "Muon inner detector TP match SS", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_mu_InDet_tp_nomatch_os = TH1F_LW::create("m_mu_InDet_tp_nomatch_os", "Muon inner detector TP nomatch OS", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_mu_InDet_tp_nomatch_ss = TH1F_LW::create("m_mu_InDet_tp_nomatch_ss", "Muon inner detector TP nomatch SS", nzbins, m_zCutLow*GeV, m_zCutHigh*GeV), lumiBlock).isFailure();

      // Clone of above but for electrons
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_eltrigtp_matches_os = TH1F_LW::create("m_eltrigtp_matches_os", "Electron trigger TP stats", 3, -0.5, 2.5), lumiBlock).isFailure();
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_eltrigtp_matches_ss = TH1F_LW::create("m_eltrigtp_matches_ss", "Electron trigger TP stats", 3, -0.5, 2.5), lumiBlock).isFailure();
     
      // T&P electron eff
      double m_zCutHigh_elTP = 250; 
      double m_zCutLow_elTP  = 66;
      int nzbins_elTP = int(ceilf(m_zCutHigh_elTP - m_zCutLow_elTP));

      failure |= registerHist(fullPathDQTGlobalWZFinder, m_ele_tight_bad_os  = TH1F_LW::create("m_ele_tight_bad_os"  , "" ,nzbins_elTP, m_zCutLow_elTP*GeV, m_zCutHigh_elTP*GeV), lumiBlock).isFailure();
      failure |= registerHist(fullPathDQTGlobalWZFinder, m_ele_tight_bad_ss  = TH1F_LW::create("m_ele_tight_bad_ss"  , "" ,nzbins_elTP, m_zCutLow_elTP*GeV, m_zCutHigh_elTP*GeV), lumiBlock).isFailure();
      failure |= registerHist(fullPathDQTGlobalWZFinder, m_ele_tight_good_os = TH1F_LW::create("m_ele_tight_good_os" , "",nzbins_elTP, m_zCutLow_elTP*GeV, m_zCutHigh_elTP*GeV), lumiBlock).isFailure();
      failure |= registerHist(fullPathDQTGlobalWZFinder, m_ele_tight_good_ss = TH1F_LW::create("m_ele_tight_good_ss" , "",nzbins_elTP, m_zCutLow_elTP*GeV, m_zCutHigh_elTP*GeV), lumiBlock).isFailure();
      failure |= registerHist(fullPathDQTGlobalWZFinder, m_ele_template_os   = TH1F_LW::create("m_ele_template_os"   , ""   ,nzbins_elTP, m_zCutLow_elTP*GeV, m_zCutHigh_elTP*GeV), lumiBlock).isFailure();
      failure |= registerHist(fullPathDQTGlobalWZFinder, m_ele_template_ss   = TH1F_LW::create("m_ele_template_ss"   , ""   ,nzbins_elTP, m_zCutLow_elTP*GeV, m_zCutHigh_elTP*GeV), lumiBlock).isFailure();
      
      
      failure |=  registerHist(fullPathDQTGlobalWZFinder, m_elContainertp_nomatch = TH1F_LW::create("m_elContainertp_nomatch" , "" ,nzbins_elTP, m_zCutLow_elTP*GeV, m_zCutHigh_elTP*GeV), lumiBlock).isFailure();
      failure |= registerHist(fullPathDQTGlobalWZFinder, m_ele_tight_passkine = TH1F_LW::create("m_ele_tight_passkine" , "" ,nzbins_elTP, m_zCutLow_elTP*GeV, m_zCutHigh_elTP*GeV), lumiBlock).isFailure();

    }

    // Fill trees with BCID information
    if(m_do_BCID){    
      failure |= registerTree(fullPathDQTGlobalWZFinder, m_bcid_ele_tight_good = new TTree("m_bcid_ele_tight_good","m_bcid_ele_tight_good")).isFailure();
      failure |= registerTree(fullPathDQTGlobalWZFinder, m_bcid_ele_tight_bad  = new TTree("m_bcid_ele_tight_bad","m_bcid_ele_tight_bad")).isFailure();
      failure |= registerTree(fullPathDQTGlobalWZFinder, m_bcid_ele_template   = new TTree("m_bcid_ele_template","m_bcid_ele_template")).isFailure();
      failure |= registerTree(fullPathDQTGlobalWZFinder, m_bcid_ele_trigtp     = new TTree("m_bcid_ele_trigtp","m_bcid_ele_trigtp")).isFailure();
      failure |= registerTree(fullPathDQTGlobalWZFinder, m_bcid_mu_tight_good  = new TTree("m_bcid_mu_tight_good","m_bcid_mu_tight_good")).isFailure();
      failure |= registerTree(fullPathDQTGlobalWZFinder, m_bcid_mu_tight_bad   = new TTree("m_bcid_mu_tight_bad","m_bcid_mu_tight_bad")).isFailure();
      failure |= registerTree(fullPathDQTGlobalWZFinder, m_bcid_mu_trigtp      = new TTree("m_bcid_mu_trigtp","m_bcid_mu_trigtp")).isFailure();
 
      setBCIDBranches(m_bcid_ele_tight_good, "mass"); 
      setBCIDBranches(m_bcid_ele_tight_bad, "mass"); 
      setBCIDBranches(m_bcid_ele_template, "mass"); 
      setBCIDBranches(m_bcid_ele_trigtp, "matched"); 
      
      setBCIDBranches(m_bcid_mu_tight_good, "mass"); 
      setBCIDBranches(m_bcid_mu_tight_bad, "mass"); 
      setBCIDBranches(m_bcid_mu_trigtp, "matched"); 
    }

    if (m_isSimulation) {
      failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_mcmatch = TH1F_LW::create("m_mcatch", "Muon matching to truth in acceptance", 2, -0.5, 1.5), lumiBlock).isFailure();
      
      if (m_writeTTrees){

        // Currently we hide the ttrees in the m_isSimulation check
        // To be done, change to a job option so runs on data
        failure = failure | registerTree(fullPathDQTGlobalWZFinder, m_muontree = new TTree("muontree","muontree")).isFailure();
        failure = failure | registerTree(fullPathDQTGlobalWZFinder, m_muon_reco_tptree = new TTree("muon_reco_tptree","muon_reco_tptree")).isFailure();
        failure = failure | registerTree(fullPathDQTGlobalWZFinder, m_muon_indet_tptree = new TTree("muon_indet_tptree","muon_indet_tptree")).isFailure();
        failure = failure | registerTree(fullPathDQTGlobalWZFinder, m_muon_trig_tptree = new TTree("muon_trig_tptree","muon_trig_tptree")).isFailure();
        failure = failure | registerTree(fullPathDQTGlobalWZFinder, m_electrontree = new TTree("electrontree","electrontree")).isFailure();
        failure = failure | registerTree(fullPathDQTGlobalWZFinder, m_electron_reco_tptree = new TTree("electron_reco_tptree","electron_reco_tptree")).isFailure();
        failure = failure | registerTree(fullPathDQTGlobalWZFinder, m_electron_container_tptree = new TTree("electron_container_tptree","electron_container_tptree")).isFailure();
        failure = failure | registerTree(fullPathDQTGlobalWZFinder, m_electron_trig_tptree = new TTree("electron_trig_tptree","electron_trig_tptree")).isFailure();
        setDQTGlobalWZFinderBranches();
      }
    }

     
     //Resonance particle rate monitoring
     m_minLumiBlock  = 0.0;
     m_maxLumiBlock = 1200.0;
     m_numBins = 30.0;
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_JPsiCounter_Mu   = TH1F_LW::create("m_JPsi_Counter_mu", "J/#psi #rightarrow #mu#mu Count per Lumi Block", m_numBins, m_minLumiBlock+0.5, m_maxLumiBlock+0.5)).isFailure();
     failure = failure | registerHist(fullPathDQTGlobalWZFinder, m_UpsilonCounter_Mu   = TH1F_LW::create("m_Upsilon_Counter_mu", "#Upsilon #rightarrow #mu#mu Count per Lumi Block", m_numBins, m_minLumiBlock+0.5, m_maxLumiBlock+0.5)).isFailure();

  }
   
  if (failure) {
     log << MSG::ERROR << "Error Booking histograms " << endmsg;
  }

  return failure;

}


//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::fillHistograms()
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());

  if (m_doRunBeam) {  
     StatusCode sc = StatusCode::SUCCESS;

     //Get LumiBlock and EventNumber
     const xAOD::EventInfo* thisEventInfo;
     sc = evtStore()->retrieve(thisEventInfo);
     if(sc.isFailure()){
       log << MSG::ERROR << "Could not find EventInfo in evtStore()" << endmsg;
       return sc;
     }else{
       m_this_lb = thisEventInfo->lumiBlock() ;
       m_eventNumber  = thisEventInfo->eventNumber();
     }
     
     //Get position in train
     m_distance_from_front = m_bcTool->distanceFromFront(thisEventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings);
     // Check if the bunch is in a train
     m_is_in_train = m_bcTool->isInTrain(thisEventInfo->bcid());
     // Get length of train
     m_train_length = m_bcTool->distanceFromFront(thisEventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings) + m_bcTool->distanceFromTail(thisEventInfo->bcid(), Trig::IBunchCrossingTool::BunchCrossings) + 1;
     // Get pileup per lb
     m_pileup_per_lb = lbInteractionsPerCrossing();

     m_evtWeight = 1;
     if (thisEventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
       m_evtWeight = thisEventInfo->mcEventWeight();
       ATH_MSG_DEBUG("Event Weight: " << m_evtWeight);
     }

     //Get MET     
     Double_t phiMet = 0, metMet = 0;
     const xAOD::MissingETContainer *missETcont(0);
     const xAOD::MissingET* missET(0);
     if ( evtStore()->contains<xAOD::MissingETContainer>(m_METName) ) {
       evtStore()->retrieve(missETcont,m_METName);
       missET = (*missETcont)["FinalClus"];
       if (!missET){
         ATH_MSG_WARNING("Cannot retrieve xAOD::MissingET " << m_METName);
         if (!m_printedErrorMet)
           ATH_MSG_WARNING("Cannot retrieve " << m_METName);
         m_printedErrorMet = true;
       }
       else {
         phiMet = missET->phi();
         metMet = missET->met();
       }
     }
     else {
        if (!m_printedErrorMet)
           log << MSG::WARNING << "Cannot retrieve xAOD::MissingETContainer " << m_METName << endmsg;
        m_printedErrorMet = true;
     }

     ATH_MSG_DEBUG(" MET = " << metMet << " and met phi = " << phiMet);

     //Get Electrons
     const xAOD::ElectronContainer* elecTES = 0;
     if ( evtStore()->contains<xAOD::ElectronContainer>(m_electronContainerName) ) {
        sc=evtStore()->retrieve( elecTES, m_electronContainerName);
        if( sc.isFailure()  ||  !elecTES ) {
      if (!m_printedErrorEleContainer) log << MSG::WARNING << "No electron container" <<  m_electronContainerName << " found in evtStore" << endmsg; 
           m_printedErrorEleContainer = true;
           return StatusCode::SUCCESS;
        }
     }
     else {
        if (!m_printedErrorEleContainer) log << MSG::WARNING << "No electron container" <<  m_electronContainerName << " found in evtStore" << endmsg; 
        m_printedErrorEleContainer = true;
        return StatusCode::SUCCESS;
     }
     
     ATH_MSG_DEBUG("ElectronContainer successfully retrieved");

     //Get Muons        
     const xAOD::MuonContainer* muons  = 0;
     if(evtStore()->contains<xAOD::MuonContainer>(m_muonContainerName)){
        sc = evtStore()->retrieve(muons,m_muonContainerName);
        if (sc.isFailure()) {
           if (!m_printedErrorMuColl) log << MSG::WARNING << "No muon collection with name " << m_muonContainerName << " found in evtStore" << endmsg;
           m_printedErrorMuColl = true;
           return StatusCode::SUCCESS;
        }
     } else {
        if (!m_printedErrorMuColl) log << MSG::WARNING << "evtStore() does not contain muon Collection with name "<< m_muonContainerName <<" " <<endmsg;
        m_printedErrorMuColl = true;
        return StatusCode::SUCCESS;
     }

     ATH_MSG_DEBUG("Got muon collection! ");

     std::vector<const xAOD::Electron*> goodelectrons;
     std::vector<const xAOD::Muon*> goodmuonsZ;
     std::vector<const xAOD::Muon*> goodmuonsTP;
     std::vector<CLHEP::HepLorentzVector> goodmuonsJPsi;
     std::vector<Int_t> goodmuonJPsicharge;

     //get primary vertex info
     const xAOD::VertexContainer* vertices(0);
     const xAOD::Vertex* pVtx(0);
     if ( evtStore()->contains<xAOD::VertexContainer>(m_VxPrimContainerName)) {
       sc = evtStore()->retrieve(vertices,m_VxPrimContainerName);

       if (sc.isFailure()) {
         log << MSG::WARNING << "No collection with name " << m_VxPrimContainerName << " found in evtStore()" << endmsg;
       }
       else {
         ATH_MSG_DEBUG("Collection with name " << m_VxPrimContainerName << " with size " << vertices->size() << " found in evtStore()");
         xAOD::VertexContainer::const_iterator vxItr = vertices->begin();
         xAOD::VertexContainer::const_iterator vxItrE = vertices->end();
         for (; vxItr != vxItrE; ++vxItr) {
           if ((*vxItr)->vertexType() == xAOD::VxType::PriVtx) {
             pVtx = *vxItr;
           }
         }
       }
     }



     //MET cleaning
     bool isBad = false;
     const JetContainer* jetTES;
     bool printedErrorJetCollection = false;
     if (evtStore()->contains<JetContainer>(m_jetCollectionName)) {
       sc=evtStore()->retrieve(jetTES,m_jetCollectionName);
     }
     else {
       log << MSG::WARNING << "Didn't find JetCollection" << endmsg;  
       if (!printedErrorJetCollection) log << MSG::WARNING << "No jet collection found in TDS" << endmsg;
       printedErrorJetCollection = true;
       isBad = true;
       return StatusCode::SUCCESS;
     }
     ATH_MSG_DEBUG("JetCollection successfully retrieved");
    

     const xAOD::Electron* leadingAllEle(0);
     const xAOD::Electron*  subleadingAllEle(0);
     std::vector<const xAOD::Electron*> allElectrons;

     // Electron Cut Flow
     Int_t El_N = 0;
     ATH_MSG_DEBUG("Start electron selection");

     for (xAOD::ElectronContainer::const_iterator itr=elecTES->begin(); itr != elecTES->end(); ++itr) {
       allElectrons.push_back(*itr);

    const xAOD::Electron* electron_itr = (*itr);
        if(goodElectrons(thisEventInfo, electron_itr, pVtx, isBad)){
          ATH_MSG_DEBUG("Good electron");
          El_N++;

          m_ele_Et->Fill((*itr)->pt()/GeV, m_evtWeight);
          m_ele_Eta->Fill((*itr)->eta(), m_evtWeight);
          goodelectrons.push_back(*itr);
       }
     }


     // Muon Cut Flow
     Int_t MuZ_N = 0;
     Int_t MuJPsi_N = 0;
     ATH_MSG_DEBUG("Start muon selection");

     for (xAOD::MuonContainer::const_iterator muonItr=muons->begin(); muonItr != muons->end(); ++muonItr){
       Float_t minptCutJPsi(1.0*CLHEP::GeV);
       auto muTrk = (*muonItr)->primaryTrackParticle();
       float d0sig;
       if (!muTrk) {
         ATH_MSG_WARNING("No muon track! " << thisEventInfo->runNumber() << " " << thisEventInfo->eventNumber());
         continue;
       }
       try {
         d0sig = xAOD::TrackingHelpers::d0significance(muTrk, thisEventInfo->beamPosSigmaX(), thisEventInfo->beamPosSigmaY(), thisEventInfo->beamPosSigmaXY());
       } catch (...) {
       ATH_MSG_DEBUG("Invalid beamspot - muon");
         try {
           d0sig = xAOD::TrackingHelpers::d0significance(muTrk);
         } catch (...) {
           ATH_MSG_WARNING("Ridiculous exception thrown - muon");
           continue;
         }
       }
     
     
       ATH_MSG_DEBUG("Muon accept: " << m_muonSelectionTool->accept(**muonItr));
       ATH_MSG_DEBUG("Muon pt: " << (*muonItr)->pt() << " " << m_muonPtCut*GeV);
       ATH_MSG_DEBUG("Muon iso: " << m_isolationSelectionTool->accept(**muonItr));
       ATH_MSG_DEBUG("Muon d0sig: " << d0sig);
       ATH_MSG_DEBUG("Muon Good vtx: " << pVtx);
       if (pVtx) ATH_MSG_DEBUG("Muon z0sinth: " << fabs((muTrk->z0()+muTrk->vz()-pVtx->z())*std::sin(muTrk->theta())) << " " << 0.5*mm);
       ATH_MSG_DEBUG("Muon isBad: " << isBad);
     
       if (m_muonSelectionTool->accept(**muonItr) &&
           ((*muonItr)->pt() > 0.8*m_muonPtCut*GeV) &&
           m_isolationSelectionTool->accept(**muonItr) &&
           fabs(d0sig) < 3 &&
           pVtx &&
           fabs((muTrk->z0()+muTrk->vz()-pVtx->z())*std::sin(muTrk->theta())) < 0.5*mm &&
           !isBad)
       {

           goodmuonsTP.push_back(*muonItr);
           if (((*muonItr)->pt() > m_muonPtCut*GeV))
           {
             MuZ_N++;
             m_muon_Pt->Fill((*muonItr)->pt()/GeV, m_evtWeight);
             m_muon_Eta->Fill((*muonItr)->eta(), m_evtWeight);
             goodmuonsZ.push_back(*muonItr);
           }
       }


       if (((*muonItr)->pt() > minptCutJPsi)){
         MuJPsi_N++;
         Float_t px = (*muonItr)->p4().Px();
         Float_t py = (*muonItr)->p4().Py();
         Float_t pz = (*muonItr)->p4().Pz();
         Float_t e = (*muonItr)->p4().E();
         Int_t charge = 0;

         CLHEP::HepLorentzVector thislepton;
         thislepton.setPx(px);
         thislepton.setPy(py);
         thislepton.setPz(pz);
         thislepton.setE(e);
         goodmuonsJPsi.push_back(thislepton);
         goodmuonJPsicharge.push_back(charge);
       }
     }

     if (m_isSimulation) {
       doMuonTruthEff(goodmuonsZ);
     }


     // Check Sum of Candidate Leptons, Return if None


     ATH_MSG_DEBUG("Candidate e+mu = " << El_N+MuZ_N+MuJPsi_N); 
     if ((El_N + MuZ_N + MuJPsi_N) < 1) return sc;

     for (UInt_t iEle = 0; iEle < allElectrons.size(); iEle++) {
       Float_t pt = allElectrons[iEle]->pt();
       ATH_MSG_DEBUG("Ele pt " << pt);
       if (!leadingAllEle || pt > leadingAllEle->pt()){
         subleadingAllEle = leadingAllEle;
         leadingAllEle = allElectrons[iEle];
       }
       else if (!subleadingAllEle || pt > subleadingAllEle->pt()){
         subleadingAllEle = allElectrons[iEle];
       }
     }

    
     // Perform all Tag and Probe Procedures 
     doMuonLooseTP(goodmuonsTP, pVtx);
     doMuonInDetTP(goodmuonsZ, pVtx);
     doEleTP(leadingAllEle, subleadingAllEle, pVtx, thisEventInfo, isBad);
     doEleContainerTP(allElectrons, goodelectrons);
     
     // Sort Candidates by Pt
     const xAOD::Electron* leadingEle(0);
     const xAOD::Electron* subleadingEle(0);
     const xAOD::Muon*      leadingMuZ(0);
     const xAOD::Muon*     subleadingMuZ(0);
     CLHEP::HepLorentzVector leadingMuJPsi;
     CLHEP::HepLorentzVector subleadingMuJPsi;
     
     leadingMuJPsi.setPerp(0);
     subleadingMuJPsi.setPerp(0);
     
     Int_t leadingMuJPsicharge = 0;
     Int_t subleadingMuJPsicharge = 0;


     ATH_MSG_DEBUG("Beginning ele loop");
     for (UInt_t iEle = 0; iEle < goodelectrons.size(); iEle++) {
       Float_t pt = goodelectrons[iEle]->pt();
       ATH_MSG_DEBUG("Ele pt " << pt);
       if (! leadingEle || pt > leadingEle->pt()) {
         subleadingEle = leadingEle;
         leadingEle = goodelectrons[iEle];
       }
       else if (! subleadingEle || pt > subleadingEle->pt()) {
         subleadingEle = goodelectrons[iEle];
       }
     }

      
     ATH_MSG_DEBUG("Done ele loop");
     ATH_MSG_DEBUG("Start mu Z loop");
     for (UInt_t iMu = 0; iMu < goodmuonsZ.size(); iMu++) {
        Float_t pt = goodmuonsZ[iMu]->pt();
        if (! leadingMuZ || pt > leadingMuZ->pt()) {
           subleadingMuZ = leadingMuZ;
           leadingMuZ = goodmuonsZ[iMu];
        }
        else if (! subleadingMuZ || pt > subleadingMuZ->pt()) {
           subleadingMuZ = goodmuonsZ[iMu];
        }
     }
     ATH_MSG_DEBUG("Done mu Z loop");
     ATH_MSG_DEBUG("Start mu J/psi loop");
     for (UInt_t iMu = 0; iMu < goodmuonsJPsi.size(); iMu++) {
        Float_t pt = goodmuonsJPsi[iMu].perp();
        if (pt > leadingMuJPsi.perp()) {
           subleadingMuJPsi = leadingMuJPsi;
           subleadingMuJPsicharge = leadingMuJPsicharge;
           leadingMuJPsi = goodmuonsJPsi[iMu];
           leadingMuJPsicharge = goodmuonJPsicharge[iMu];
        }
        else if (pt > subleadingMuJPsi.perp()) {
           subleadingMuJPsi = goodmuonsJPsi[iMu];
           subleadingMuJPsicharge = goodmuonJPsicharge[iMu];
        }
     }
     ATH_MSG_DEBUG("End mu J/psi loop");
     
     ATH_MSG_DEBUG("Fill MET");
     //MET histograms
     m_metHist->Fill(metMet, m_evtWeight);
     m_metPhiHist->Fill(phiMet, m_evtWeight);
     ATH_MSG_DEBUG("End MET");
     
     // Z Mass

     bool isZee = (goodelectrons.size() > 1);
     bool isZmumu = (goodmuonsZ.size() > 1);
     bool isJPsimumu = (goodmuonsJPsi.size() > 1);
     ATH_MSG_DEBUG("Evaluated Event"); 

     if (isZee){ 
       ATH_MSG_DEBUG("Zee found");
       TLorentzVector Zee = (leadingEle->p4() + subleadingEle->p4());
       Float_t mass = Zee.M();
       Int_t Zeecharge = leadingEle->charge() + subleadingEle->charge();      
       bool ignoreTrig = trigChainsArePassed(m_Z_ee_trigger) || !m_doTrigger;
       bool inMassWindow = (mass > m_zCutLow*GeV && mass < m_zCutHigh*GeV);
       bool os = false;
       bool ss = false;
       (Zeecharge == 0) ? (os = true) : (ss = true);       

       if (inMassWindow){
         m_Z_Q_ele->Fill(Zeecharge, m_evtWeight);
         ATH_MSG_DEBUG( "Found a Z to ee candidate!  Mass = " << mass << ", and charge = " << Zeecharge );
       }
       if(inMassWindow && os && ignoreTrig){
         // electrontree                                                                                                                                                    
         if(m_writeTTrees){
           m_electrontree_eta1 = leadingEle->caloCluster()->etaBE(2);
           m_electrontree_eta2 = subleadingEle->caloCluster()->etaBE(2);
           m_electrontree_phi1 = leadingEle->phi();
           m_electrontree_phi2 = subleadingEle->phi();
           m_electrontree_pT1 = leadingEle->pt();
           m_electrontree_pT2 = subleadingEle->pt();
           m_electrontree_weight = m_evtWeight;
           m_electrontree_lb = m_this_lb;
           m_electrontree_runnumber = thisEventInfo->runNumber();
           m_electrontree_eventnumber = thisEventInfo->eventNumber();
           m_electrontree_mass = mass;
           // Replace with truth matching code
           m_electrontree_isTruth = checkTruthElectron(leadingEle) & checkTruthElectron(subleadingEle);
           m_electrontree->Fill();
         }

         if (!m_isSimulation){
           m_Z_mass_opsele->Fill(mass, m_evtWeight);
           m_ZBosonCounter_El_os->Fill(m_this_lb, m_evtWeight);
         }
         ++m_ZBosonCounterSBG_El[0];
       }         
       if(inMassWindow && ss && ignoreTrig){
         if (!m_isSimulation){
           m_Z_mass_ssele->Fill(mass, m_evtWeight);
           m_ZBosonCounter_El_ss->Fill(m_this_lb, m_evtWeight);
         }   
       }
       if(inMassWindow && m_doTrigger){
         doEleTriggerTP(leadingEle, subleadingEle, os, ss);
       }
     }


     if (isZmumu){
       ATH_MSG_DEBUG("Zmumu found");
       TLorentzVector Zmumu = leadingMuZ->p4() + subleadingMuZ->p4();
       Float_t mass = Zmumu.M();
       Int_t Zmumucharge = leadingMuZ->charge() + subleadingMuZ->charge();
       // potentially ignore trigger...
       bool oktrig = trigChainsArePassed(m_Z_mm_trigger) || !m_doTrigger;
       bool inMassWindow = (mass > m_zCutLow*GeV && mass < m_zCutHigh*GeV);
       bool os = false;
       bool ss = false;
       (Zmumucharge == 0) ? (os = true) : (ss = true);

       if(inMassWindow){
         m_Z_Q_mu->Fill(Zmumucharge, m_evtWeight);
         ATH_MSG_DEBUG("Found a Z to mumu candidate!  Mass = " << mass << ", and charge = " << Zmumucharge);        
       }
       if(inMassWindow && os && oktrig){
         // muontree                                                                                                                                                    
         if(m_writeTTrees){
           m_muontree_eta1 = leadingMuZ->eta();
           m_muontree_eta2 = subleadingMuZ->eta();
           m_muontree_phi1 = leadingMuZ->phi();
           m_muontree_phi2 = subleadingMuZ->phi();
           m_muontree_pT1 = leadingMuZ->pt();
           m_muontree_pT2 = subleadingMuZ->pt();
           m_muontree_weight = m_evtWeight;
           m_muontree_lb = m_this_lb;
           m_muontree_runnumber = thisEventInfo->runNumber();
           m_muontree_eventnumber = thisEventInfo->eventNumber();
           m_muontree_isTruth = checkTruthMuon(leadingMuZ) & checkTruthMuon(subleadingMuZ);
           m_muontree_mass = mass;
           m_muontree->Fill();
         }

         if (!m_isSimulation){
           m_Z_mass_opsmu->Fill(mass, m_evtWeight);
           m_ZBosonCounter_Mu->Fill(m_this_lb, m_evtWeight);
         }
         ++m_ZBosonCounterSBG_Mu[0];
       }
       if(inMassWindow && os && !oktrig){
         ATH_MSG_DEBUG("Trigger failure!");      
       }
       if(inMassWindow && os && m_doTrigger){ 
         doMuonTriggerTP(leadingMuZ, subleadingMuZ);      
       }
       if(inMassWindow && ss && oktrig){
         if (!m_isSimulation){
           m_Z_mass_ssmu->Fill(mass, m_evtWeight);
         }
       }
     }   

     //JPsi and Upsilon counter
     if (isJPsimumu && trigChainsArePassed(m_Jpsi_mm_trigger)) {
       ATH_MSG_DEBUG("Jpsi mm found");
       CLHEP::HepLorentzVector JPsimumu = leadingMuJPsi + subleadingMuJPsi;
       Float_t mass = JPsimumu.m();
       Int_t JPsimumucharge = leadingMuJPsicharge + subleadingMuJPsicharge;

       if (JPsimumucharge == 0) {
         m_JPsi_mass_opsmu->Fill(mass, m_evtWeight);
         m_Upsilon_mass_opsmu->Fill(mass, m_evtWeight);

         if (mass > 3038 && mass < 3156) {
           ++m_JPsiCounterSBG[0];
           if(m_doTrigger) {
             ATH_MSG_DEBUG("ABOUT TO CHECK JPSI TRIGGER CHAIN");
             if( trigChainsArePassed(m_Jpsi_mm_trigger) ) {
               ++m_JPsiCounterSBG[1];
             }
           }
         }
         if ((mass > 2802 && mass < 2920) || (mass > 3274 && mass < 3391)) {
           ++m_JPsiCounter2BG[0];
         }
         //assuming JPsi mass of 3 097 MeV and range of 2 sigma (sigma = 59 MeV, based on root best fit of distribution)
         if (mass > 8242 && mass < 10678) {
           ++m_UpsilonCounterSBG[0];
         }
         if ((mass > 4588 && mass < 5806) || (mass > 13114 && mass < 14332)) {
           ++m_UpsilonCounterBG[0];
         }
       }
       ATH_MSG_DEBUG("Found a JPsi/Upsilon to mumu candidate!  Mass = " << mass << ", and charge = " << JPsimumucharge);
     }  

     // W Transverse Mass

     Float_t metx = metMet*cos(phiMet);
     Float_t mety = metMet*sin(phiMet);
     Float_t mtCut = 40*GeV;

     bool isWenu =  ((goodelectrons.size()  > 0) && (metMet > m_metCut*GeV) && !isZee);
     bool isWmunu = ((goodmuonsZ.size() > 0) && (metMet > m_metCut*GeV) && !isZmumu);


     if (isWenu) {
       ATH_MSG_DEBUG("Wenu found " << leadingEle);
       Float_t px = leadingEle->p4().Px();
       Float_t py = leadingEle->p4().Py();
       Float_t pz = leadingEle->p4().Pz();
       Float_t p = sqrt(px*px+py*py+pz*pz);
       Float_t et = leadingEle->e()*leadingEle->pt()/p;
       Float_t mt = sqrt(2*(et*metMet-px*metx-py*mety));     

       if (mt > mtCut){
        m_W_pt_v_met_ele->Fill(leadingEle->pt(), metMet, m_evtWeight);
        m_W_mt_ele->Fill(mt, m_evtWeight);
        ATH_MSG_DEBUG("Found a W to enu candidate!  M_T = " << mt << ", and MET = " << metMet << ", and ele_pt = " << leadingEle->pt());
       }
     }

     if (isWmunu) {
       ATH_MSG_DEBUG("Wmunu found");
       Float_t px = leadingMuZ->p4().Px();
       Float_t py = leadingMuZ->p4().Py();
       Float_t pz = leadingMuZ->p4().Pz();
       Float_t p = sqrt(px*px+py*py+pz*pz);
       Float_t et = leadingMuZ->e()*leadingMuZ->pt()/p;
       Float_t mt = sqrt(2*(et*metMet-px*metx-py*mety));
       
       if (mt > mtCut) {
       m_W_pt_v_met_mu->Fill(leadingMuZ->pt(), metMet, m_evtWeight);
       m_W_mt_mu->Fill(mt, m_evtWeight);
       ATH_MSG_DEBUG("Found a W to munu candidate!  M_T = " << mt << ", and MET = " << metMet << ", and mu_pt = " << leadingMuZ->pt());
       }
     }


     goodelectrons.clear();
     goodmuonsZ.clear();
     allElectrons.clear();

     return sc;
 
  } //end ifBeam  
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::procHistograms( )
//----------------------------------------------------------------------------------
{
  if ( endOfLumiBlockFlag() || endOfRunFlag() ) {
    MsgStream log(msgSvc(), name());

    ATH_MSG_DEBUG("in finalHists()");
  }


  //---Filling rate monitoring histograms---//
  if (endOfLumiBlockFlag()) {
         m_JPsiCounterSignal[0] = m_JPsiCounterSBG[0] - m_JPsiCounter2BG[0]/2;
         m_JPsiCounterSignal[1] = m_JPsiCounterSBG[1] - m_JPsiCounter2BG[1]/2;

         m_UpsilonCounterSignal[0] = m_UpsilonCounterSBG[0] - m_UpsilonCounterBG[0];
         m_UpsilonCounterSignal[1] = m_UpsilonCounterSBG[1] - m_UpsilonCounterBG[1];

         if (m_doRunBeam) {
             //Map lumiBlock to corresponding bin
             int binNumber = int((m_this_lb-0.50)/((m_maxLumiBlock - m_minLumiBlock)/m_numBins))+1;

         //JPsi->mu mu
         if(m_JPsiCounterSignal[0] < 0) {
           m_JPsiCounterSignal[0] = 0;
         }
         if(m_JPsiCounterSignal[1] < 0) {
           m_JPsiCounterSignal[1] = 0;
         }

         for(int i = 0 ; i < m_JPsiCounterSignal[0] ; i++) {
           m_JPsiCounter_Mu->Fill(m_this_lb); 
         }
         m_JPsiCounter_Mu->SetBinError(binNumber, TMath::Sqrt(m_JPsiCounter_Mu->GetBinContent( binNumber)));

         //Upsilon->mu mu
             if(m_UpsilonCounterSignal[0] < 0) {
           m_UpsilonCounterSignal[0]= 0;
             }
             if(m_UpsilonCounterSignal[1] < 0) {
           m_UpsilonCounterSignal[1]= 0;
             }

         for(int i = 0 ; i < m_UpsilonCounterSignal[0] ; i++) {
           m_UpsilonCounter_Mu->Fill(m_this_lb);
         }
         m_UpsilonCounter_Mu->SetBinError(binNumber, TMath::Sqrt(m_UpsilonCounter_Mu->GetBinContent( binNumber)));

             //Z->e e
         if(m_ZBosonCounterSBG_El[0] < 0) {
           m_ZBosonCounterSBG_El[0] = 0;
         }
         if(m_ZBosonCounterSBG_El[1] < 0) {
           m_ZBosonCounterSBG_El[1] = 0;
         }
        }
     for (int index =0 ; index < 2 ; index++) {
       m_JPsiCounterSBG[index]=0;
       m_JPsiCounterSBG[index]=0;
       m_JPsiCounter2BG[index]=0;
       m_JPsiCounterSignal[index]=0;
       
       m_UpsilonCounterSBG[index]=0;
       m_UpsilonCounterBG[index]=0;
       m_UpsilonCounterSignal[index]=0;
       
       m_ZBosonCounterSBG_El[index]=0;
       m_ZBosonCounterSBG_Mu[index]=0;
     }
   }



  return StatusCode::SUCCESS;
}



//------ Electron START ------//
void DQTGlobalWZFinderTool::doEleTriggerTP(const xAOD::Electron* el1, const xAOD::Electron* el2, bool os, bool ss) {

  const xAOD::EventInfo* thisEventInfo;
  StatusCode sc = evtStore()->retrieve(thisEventInfo);

  int matched = 0;
  std::vector<const xAOD::Electron*> electrons{el1, el2}; 
  for (const auto el: electrons) {
    for (const auto chain: m_Z_ee_trigger) { //triggerList
      if (m_elTrigMatchTool->match(el, chain)) { //.c_str()
        matched++;
        break;
      }
    }
  }

  if (!m_isSimulation){
    if(os){ 
      m_eltrigtp_matches_os->Fill(matched, m_evtWeight);
    }
      
    if(ss){
      m_eltrigtp_matches_ss->Fill(matched, m_evtWeight);
    }
  }

  if (m_do_BCID){
    m_nTrig_matches = matched;
    m_isOS = os;
    m_bcid_ele_trigtp->Fill();
  }

  // TTree Filling
  if (!m_writeTTrees){
    return;
  }

  for (const auto& tagel : electrons) {
    // only consider trigger-matched tags to avoid bias on probes 
    bool matched2 = false;
    for (const auto chain: m_Z_ee_trigger) {
      if (m_elTrigMatchTool->match(tagel, chain)) {
        matched2=true;
        break;
      }
    }

    auto tagelp4(tagel->p4());
    if (!matched2) continue;
    for (const auto& probeel : electrons) {
      if (tagel == probeel) {
        continue;
      }
      auto probeelp4(probeel->p4());
      Float_t mass = (tagelp4+probeelp4).M();
      bool matched2 = false;
      if (mass < m_zCutLow*GeV || mass > m_zCutHigh*GeV) continue;

      m_electron_trig_tptree_pT = probeel->pt();
      m_electron_trig_tptree_phi = probeel->phi();
      m_electron_trig_tptree_eta = probeel->caloCluster()->etaBE(2);
      m_electron_trig_tptree_eta = checkTruthElectron(probeel);
      m_electron_trig_tptree_mass = mass;
      m_electron_trig_tptree_runnumber =  thisEventInfo->runNumber();
      m_electron_trig_tptree_eventnumber = thisEventInfo->eventNumber();
      m_electron_trig_tptree_weight = m_evtWeight;
      m_electron_trig_tptree_lb = m_this_lb;

      for (const auto chain: m_Z_ee_trigger) {
        if (m_elTrigMatchTool->match(probeel, chain)) {
          matched2=true;
          break;
        }
      }

      if (matched2) {
        if (probeel->charge() != tagel->charge()) {
          m_electron_trig_tptree_mtype = 0;
        }
        else {
          m_electron_trig_tptree_mtype = 1;
        }
      }
      else if (!matched2) {
        if (probeel->charge() != tagel->charge()) {
          m_electron_trig_tptree_mtype = 2;
        }
        else {
          m_electron_trig_tptree_mtype = 3;
        }
      }

      if (m_writeTTrees){
        m_electron_trig_tptree->Fill();
      }

    }
  }
}

void 
DQTGlobalWZFinderTool::doEleTP(const xAOD::Electron* leadingAllEle, 
                    const xAOD::Electron* subleadingAllEle, 
                    const xAOD::Vertex* pVtx, 
                    const xAOD::EventInfo* thisEventInfo, 
                    bool isBad) {

  // first check we have both electrons
  if(leadingAllEle && subleadingAllEle){
    // then get all the parameters we will need ready
    Int_t Zeecharge = leadingAllEle->charge() + subleadingAllEle->charge();
    auto p1(leadingAllEle->p4());
    auto p2(subleadingAllEle->p4());  
    Float_t mass = (p1+p2).M();

    bool leadingPassKinematics = kinematicCuts(leadingAllEle);
    bool subleadPassKinematics = kinematicCuts(subleadingAllEle);

    if(!leadingPassKinematics || !subleadPassKinematics) return;

    bool leading_good    = goodElectrons(thisEventInfo, leadingAllEle, pVtx, isBad);
    bool subleading_good = goodElectrons(thisEventInfo, subleadingAllEle, pVtx, isBad);

    bool leading_antigood    = antiGoodElectrons(thisEventInfo, leadingAllEle, pVtx, isBad);
    bool subleading_antigood = antiGoodElectrons(thisEventInfo, subleadingAllEle, pVtx, isBad);

    // do trigger matching
    bool leading_trig = false;
    for (const auto chain: m_Z_ee_trigger) { 
      if (m_elTrigMatchTool->match(leadingAllEle, chain)){
        leading_trig = true;
        break;
      }
    }

    bool subleading_trig = false;
    for (const auto chain: m_Z_ee_trigger) {
      if (m_elTrigMatchTool->match(subleadingAllEle, chain)){ 
        subleading_trig = true;
        break;
      }
    }
   
    bool tag_good = false;
    bool opp_sign = false;
    opp_sign = (Zeecharge==0);

    if (!m_isSimulation){
      // leading electron as tag, subleading as probe
      tag_good = (leadingAllEle->passSelection("LHTight") && leading_trig && leading_good);
      fillEleEffHistos(tag_good, subleading_good, subleading_antigood, opp_sign, mass);
      // subleading electron as tag, leading as probe
      tag_good = (subleadingAllEle->passSelection("LHTight") && subleading_trig && subleading_good);
      fillEleEffHistos(tag_good, leading_good, leading_antigood, opp_sign, mass);
      return;
    }

    if (!m_writeTTrees)
      return;

    // now fill the trees
    if(leadingAllEle->passSelection("LHTight") && leading_trig && leading_good){

      m_electron_reco_tptree_pT = subleadingAllEle->pt();
      m_electron_reco_tptree_phi = subleadingAllEle->phi();
      m_electron_reco_tptree_eta = subleadingAllEle->caloCluster()->etaBE(2);
      m_electron_reco_tptree_isTruth = checkTruthElectron(subleadingAllEle);
      m_electron_reco_tptree_mass = mass;
      m_electron_reco_tptree_runnumber =  thisEventInfo->runNumber();
      m_electron_reco_tptree_eventnumber = thisEventInfo->eventNumber();
      m_electron_reco_tptree_weight = m_evtWeight;
      m_electron_reco_tptree_lb = m_this_lb;

      if(opp_sign){
        if(subleading_good) 
          m_electron_reco_tptree_mtype = 0;

        if(!subleading_good)
          m_electron_reco_tptree_mtype = 2;

        if(subleading_antigood)
          m_electron_reco_tptree_mtype = 4;
        
      }else{
          if(subleading_good) 
            m_electron_reco_tptree_mtype = 1;

          if(!subleading_good)
            m_electron_reco_tptree_mtype = 3;

          if(subleading_antigood)
            m_electron_reco_tptree_mtype = 5;
          
        }

        m_electron_reco_tptree->Fill();
        
    }  

    if(subleadingAllEle->passSelection("LHTight") && subleading_trig && subleading_good){

      m_electron_reco_tptree_pT = leadingAllEle->pt();
      m_electron_reco_tptree_phi = leadingAllEle->phi();
      m_electron_reco_tptree_eta = leadingAllEle->caloCluster()->etaBE(2);
      m_electron_reco_tptree_isTruth = checkTruthElectron(leadingAllEle);
      m_electron_reco_tptree_mass = mass;
      m_electron_reco_tptree_runnumber =  thisEventInfo->runNumber();
      m_electron_reco_tptree_eventnumber = thisEventInfo->eventNumber();
      m_electron_reco_tptree_weight = m_evtWeight;
      m_electron_reco_tptree_lb = m_this_lb;

      if(opp_sign){
        if(leading_good) 
          m_electron_reco_tptree_mtype = 0;
        
        if(!leading_good)  
          m_electron_reco_tptree_mtype = 2;
          
        if(leading_antigood)
          m_electron_reco_tptree_mtype = 4;
      }else{
        if(leading_good) 
          m_electron_reco_tptree_mtype = 1;
          
        if(!leading_good)
          m_electron_reco_tptree_mtype = 3;
          
        if(leading_antigood)
          m_electron_reco_tptree_mtype = 5;
      }
        m_electron_reco_tptree->Fill();
    } 
  }
}


void DQTGlobalWZFinderTool::doEleContainerTP(std::vector<const xAOD::Electron*> allElectrons, std::vector<const xAOD::Electron*> goodelectrons) {
  const xAOD::PhotonContainer* Photons(0);
  evtStore()->retrieve(Photons, "Photons");
  if (!Photons) {
    ATH_MSG_FATAL("Unable to retrieve photons to do electron T&P");
    return;
  }

  for (const auto& tagEl : goodelectrons) {
    bool matched = false;
    for (const auto chain: m_Z_ee_trigger) {
      if (m_elTrigMatchTool->match(tagEl, chain) || ! m_doTrigger) {
        matched=true;
        break;
      }
    }

    if (!matched) continue;
    auto tagElp4(tagEl->p4());
    
    if (tagEl->passSelection("LHTight")){
      for (const auto& el2 : allElectrons){
        if (el2 != tagEl && kinematicCuts(el2)){
          auto probeElp4(el2->p4());
          Float_t mass = (tagElp4+probeElp4).M();
          if (!m_isSimulation){
            m_ele_tight_passkine->Fill(mass);
          }
          break;
        }
      }
    }


    for (const auto& photon : *Photons) {
      auto photonp4(photon->p4());
      Float_t mass = (tagElp4+photonp4).M();

      bool passPT  = (photon->pt() >  m_muonPtCut*GeV);      
      bool passEta = true;
      if (fabs(photon->eta()) > 2.4) 
        passEta = false;
      if (fabs(photon->eta()) > 1.37 && fabs(photon->eta()) < 1.52) 
        passEta = false;
      //bool inMassWindow = (mass > m_zCutLow*GeV && mass < m_zCutHigh*GeV);

      if (!passPT || !passEta) // || !inMassWindow) 
        continue;

      for (const auto& el2 : allElectrons){
        // slightly relax pT cut for probe electron
        bool passKinematics = true;
        if (el2->pt() < (m_muonPtCut-2)*GeV)
          passKinematics = false; 
        if (fabs((el2)->caloCluster()->etaBE(2)) > 2.4)
          passKinematics = false;
        if (fabs((el2)->caloCluster()->etaBE(2)) > 1.37 && fabs((el2)->caloCluster()->etaBE(2)) < 1.52)
          passKinematics = false;

        double deltaR = (el2->p4()).DeltaR(photon->p4());        
        if (!passKinematics || tagEl == el2 || deltaR < 0.1) 
          continue;       

        if (!m_isSimulation){
          m_elContainertp_nomatch->Fill(mass);
        }
        break;
        
      }
    }
  }
}


bool DQTGlobalWZFinderTool::kinematicCuts(const xAOD::Electron* electron){

  bool isGood = true;
  if(electron->pt() < m_muonPtCut*GeV) isGood = false;

  if(fabs((electron)->caloCluster()->etaBE(2)) > 2.4) isGood = false;
  
  if(fabs((electron)->caloCluster()->etaBE(2)) > 1.37 && 
     fabs((electron)->caloCluster()->etaBE(2)) < 1.52) isGood = false;

  return isGood;
}

bool DQTGlobalWZFinderTool::goodElectrons(const xAOD::EventInfo* thisEventInfo, 
                              const xAOD::Electron* electron_itr, 
                      const xAOD::Vertex* pVtx, 
                      bool isBad){
  bool isGood = false;
  Float_t m_electronEtCut = m_muonPtCut; 

  bool passSel = false;
  if (!((electron_itr)->passSelection(passSel, "LHMedium"))) ATH_MSG_WARNING("Electron ID WP Not Defined");
  auto elTrk = (electron_itr)->trackParticle();
  if (!elTrk) {
    // ATH_MSG_WARNING("Electron with no track particle " << thisEventInfo->runNumber() << " " << thisEventInfo->eventNumber());
    return false;
  }
  float d0sig;
  try {
    d0sig = xAOD::TrackingHelpers::d0significance(elTrk, thisEventInfo->beamPosSigmaX(), thisEventInfo->beamPosSigmaY(), thisEventInfo->beamPosSigmaXY());
  } catch (...) {
     ATH_MSG_DEBUG("Invalid beamspot - electron");
    try {
      d0sig = xAOD::TrackingHelpers::d0significance(elTrk);
    } catch (...) {
       ATH_MSG_WARNING("Ridiculous exception thrown - electron");
      return false;
    }
  }

  if ( ((electron_itr)->pt() > m_electronEtCut*GeV) &&
       fabs((electron_itr)->caloCluster()->etaBE(2)) < 2.4 &&
       passSel &&
       m_isolationSelectionTool->accept(*electron_itr) &&
       fabs(d0sig) < 5 &&
       pVtx &&
       fabs((elTrk->z0()+elTrk->vz()-pVtx->z())*std::sin(elTrk->theta())) < 0.5*mm &&
       !isBad
     ){    // electron dead zone
    if (fabs((electron_itr)->caloCluster()->etaBE(2)) > 1.37 && fabs((electron_itr)->caloCluster()->etaBE(2)) < 1.52 ){
      isGood = false;
    } else{
      isGood = true;
    } 
  }
  return isGood;
}


bool DQTGlobalWZFinderTool::antiGoodElectrons(const xAOD::EventInfo* thisEventInfo,
                                              const xAOD::Electron* electron_itr,
                                              const xAOD::Vertex* pVtx,
                                              bool isBad){
  bool antiGood = false;
  Float_t m_electronEtCut = m_muonPtCut;

  bool passID  = electron_itr->passSelection("LHLoose"); 
  bool passIso = m_isolationSelectionTool->accept(*electron_itr);
  auto elTrk = (electron_itr)->trackParticle();

  if (!elTrk) {
    return false;
  }
  float d0sig;
  try {
    d0sig = xAOD::TrackingHelpers::d0significance(elTrk, thisEventInfo->beamPosSigmaX(), thisEventInfo->beamPosSigmaY(), thisEventInfo->beamPosSigmaXY());
  } catch (...) {
     ATH_MSG_DEBUG("Invalid beamspot - electron");
    try {
      d0sig = xAOD::TrackingHelpers::d0significance(elTrk);
    } catch (...) {
       ATH_MSG_WARNING("Ridiculous exception thrown - electron");
      return false;
    }
  }

  // pass basic selection, except ID+Isolation
  if (((electron_itr)->pt() > m_electronEtCut*GeV) &&
     fabs((electron_itr)->caloCluster()->etaBE(2)) < 2.4 &&
     fabs(d0sig) < 5 &&
     pVtx &&
     fabs((elTrk->z0()+elTrk->vz()-pVtx->z())*std::sin(elTrk->theta())) < 0.5*mm &&
     !isBad)
  { 
    if(fabs((electron_itr)->caloCluster()->etaBE(2)) > 1.37 && fabs((electron_itr)->caloCluster()->etaBE(2)) < 1.52){
      antiGood = false;
    }else{
      if(!passID && !passIso) antiGood = true;
    }
  }

  return antiGood;
}


// Compute trigger efficiencies
void DQTGlobalWZFinderTool::doMuonTriggerTP(const xAOD::Muon* mu1, const xAOD::Muon* mu2) {
  //algorithm: plot # events with zero, one, two SL triggers
  //zero triggers for MC closure checks
  
  const xAOD::EventInfo* thisEventInfo;
  StatusCode sc = evtStore()->retrieve(thisEventInfo);

  int matched = 0;
  std::vector<const xAOD::Muon*> muons{mu1, mu2};
  for (const auto mu: muons) {
    for (const auto chain: m_Z_mm_trigger) {
      if (m_muTrigMatchTool->match(mu, chain)) {
        matched++;
        break;
      }
    }
  }
  if (!m_isSimulation){
    m_mutrigtp_matches->Fill(matched,m_evtWeight); 
  }

  if (m_do_BCID){
    m_nTrig_matches = matched;
    m_isOS = true;
    m_bcid_mu_trigtp->Fill();
  }


  for (const auto& tagmu : muons) {
    // only consider trigger-matched tags to avoid bias on probes                                                     
    bool matched = false;
    for (const auto chain: m_Z_mm_trigger) {
      if (m_muTrigMatchTool->match(tagmu, chain)) {
        matched=true;
        break;
      }
    }
    auto tagmup4(tagmu->p4());
    if (!matched) continue;

    for (const auto& probemu : muons) {
      if (tagmu == probemu) {
        continue;
      }
      auto probemup4(probemu->p4());
      Float_t mass = (tagmup4+probemup4).M();
      bool matched = false;
      if (mass < m_zCutLow*GeV || mass > m_zCutHigh*GeV) continue;

      m_muon_trig_tptree_pT = probemu->pt();
      m_muon_trig_tptree_eta = probemu->eta();
      m_muon_trig_tptree_phi = probemu->phi();
      m_muon_trig_tptree_isTruth = checkTruthMuon(probemu);
      m_muon_trig_tptree_mass = mass;
      m_muon_trig_tptree_runnumber =  thisEventInfo->runNumber();
      m_muon_trig_tptree_eventnumber = thisEventInfo->eventNumber();
      m_muon_trig_tptree_weight = m_evtWeight;
      m_muon_trig_tptree_lb = m_this_lb;
   
      if (!m_doTrigger){
        std::cout << "Warning, the m_doTrigger activated" << std::endl;
      }

      for (const auto chain: m_Z_mm_trigger) {
        if (m_muTrigMatchTool->match(probemu, chain)) {
          matched=true;
          break;
        }
      }

      if (matched) {
        if (probemu->charge() != tagmu->charge()) {
          m_muon_trig_tptree_mtype = 0;
          if (m_writeTTrees){
            m_muon_trig_tptree->Fill();
          }
        }
        else {
          m_muon_trig_tptree_mtype = 1;
          if (m_writeTTrees){
            m_muon_trig_tptree->Fill();
          }
        }
        break;
      }
  

      else if (!matched) {
        if (probemu->charge() != tagmu->charge()) {
          m_muon_trig_tptree_mtype = 2;
          if (m_writeTTrees){
            m_muon_trig_tptree->Fill();
          }
        }
        else {
          m_muon_trig_tptree_mtype = 3;
          if (m_writeTTrees){
            m_muon_trig_tptree->Fill();
          }
        }
      }
    }

  }
}


void DQTGlobalWZFinderTool::doMuonTruthEff(std::vector<const xAOD::Muon*>& goodmuonsZ) {
  const xAOD::TruthParticleContainer* vtruth(0);
  evtStore()->retrieve(vtruth, "MuonTruthParticles");
  if (! vtruth) {
    ATH_MSG_WARNING("No muon truth particles");
    return;
  }
  for (const auto& truthmu : *vtruth) {
    if (truthmu->abseta() > m_muonMaxEta || truthmu->pt() < m_muonPtCut*GeV) {
      continue;
    }
    TLorentzVector truthp4(truthmu->p4());
    int match = 0;
    for (const auto& foundmu : goodmuonsZ) {
      if (foundmu->p4().DeltaR(truthp4) < 0.05) {
        match = 1;
        break;
      }
    }
    m_mcmatch->Fill(match);
  }
}

void DQTGlobalWZFinderTool::doMuonLooseTP(std::vector<const xAOD::Muon*>& goodmuonsTP, const xAOD::Vertex* pVtx) {

  const xAOD::EventInfo* thisEventInfo;
  StatusCode sc = evtStore()->retrieve(thisEventInfo);

  const xAOD::TrackParticleContainer* idTracks(0);
  evtStore()->retrieve(idTracks, "InDetTrackParticles");
  if (!idTracks) {
    ATH_MSG_FATAL("Unable to retrieve ID tracks to do muon T&P");
    return;
  }
  for (const auto& tagmu : goodmuonsTP) {
    // only consider trigger-matched tags to avoid bias on probes
    bool matched = false;
    for (const auto chain: m_Z_mm_trigger) {
      if (m_muTrigMatchTool->match(tagmu, chain) || ! m_doTrigger) {
        matched=true;
        break;
      }
    }

    if (!matched) continue;
    auto tagmup4(tagmu->p4());
    for (const auto& trk : *idTracks) {
      if (trk->pt() <  m_muonPtCut*GeV || fabs(trk->eta()) > m_muonMaxEta) continue;     
      if (fabs((trk->z0()+trk->vz()-pVtx->z())*std::sin(trk->theta())) > 2*mm)  continue;
      
      auto trkp4(trk->p4());
      Float_t mass = (tagmup4+trkp4).M();
      if (mass < m_zCutLow*GeV || mass > m_zCutHigh*GeV) continue; 
      m_muon_reco_tptree_pT = trk->pt();
      m_muon_reco_tptree_phi = trk->phi();
      m_muon_reco_tptree_eta = trk->eta();
      m_muon_reco_tptree_isTruth = checkTruthTrack(trk);
      m_muon_reco_tptree_mass = mass;
      m_muon_reco_tptree_runnumber =  thisEventInfo->runNumber();
      m_muon_reco_tptree_eventnumber = thisEventInfo->eventNumber();
      m_muon_reco_tptree_weight = m_evtWeight;
      m_muon_reco_tptree_lb = m_this_lb;
      
      bool opp_sign = (trk->charge() != tagmu->charge());
      bool matched = false;
      for (const auto& mu2 : goodmuonsTP) {
        if (tagmu == mu2) continue;
        auto idlink = mu2->inDetTrackParticleLink();
        if (*(idlink.cptr()) == trk) {
          matched = true;
          break;
        } 
      }

      m_mass = mass;
      m_isOS = opp_sign;
	  if (matched){
        m_muon_reco_tptree_mtype = (trk->charge() != tagmu->charge()) ? 0 : 1;
        if (!m_isSimulation){
          (opp_sign) ? m_muloosetp_match_os->Fill(mass)   : m_muloosetp_match_ss->Fill(mass);
        }
        if (m_writeTTrees){
          m_muon_reco_tptree->Fill();
        }

        if (m_do_BCID){
          m_bcid_mu_tight_good->Fill();
        }
      }
      else { 
        m_muon_reco_tptree_mtype = (trk->charge() != tagmu->charge()) ? 2 : 3;
        if (!m_isSimulation){
          (opp_sign) ? m_muloosetp_nomatch_os->Fill(mass) : m_muloosetp_nomatch_ss->Fill(mass);
        }
        if (m_writeTTrees){
          m_muon_reco_tptree->Fill();
        }     
        
        if (m_do_BCID){
          m_bcid_mu_tight_bad->Fill();
        } 

      }
    }
  }
}
 
void DQTGlobalWZFinderTool::doMuonInDetTP(std::vector<const xAOD::Muon*>& goodmuonsZ, const xAOD::Vertex* pVtx) {
  const xAOD::EventInfo* thisEventInfo;
  StatusCode sc = evtStore()->retrieve(thisEventInfo);

  const xAOD::TrackParticleContainer* msTracks(0);
  evtStore()->retrieve(msTracks, "ExtrapolatedMuonTrackParticles");

  const xAOD::TrackParticleContainer* idTracks(0);
  evtStore()->retrieve(idTracks, "InDetTrackParticles");
            
  if (!idTracks) {
    ATH_MSG_FATAL("Unable to retrieve ID tracks to do muon T&P");
    return;
  }
  if (!msTracks) {
    ATH_MSG_FATAL("Unable to retrieve MS tracks to do muon T&P");
    return;
  }
                                        
                                        
  for (const auto& tagmu : goodmuonsZ) {
    bool matched = false;
    for (const auto chain: m_Z_mm_trigger) {
      if (m_muTrigMatchTool->match(tagmu, chain) || ! m_doTrigger) {
        matched=true;
        break;
      }
    }
    if (!matched) continue;
    auto tagmup4(tagmu->p4());
    // For Every ID track....
    for (const auto& trk : *msTracks) {
      if (trk->pt() <  m_muonPtCut*GeV || fabs(trk->eta()) > m_muonMaxEta)     
        continue; 
      if (fabs((trk->z0()+trk->vz()-pVtx->z())*std::sin(trk->theta())) > 2*mm) 
        continue;
                                                                                                            
      auto trkp4(trk->p4());
      Float_t mass = (tagmup4+trkp4).M();
      bool matched = false;
                                                                                                                
      if (mass < m_zCutLow*GeV || mass > m_zCutHigh*GeV) continue;
      m_muon_indet_tptree_pT = trk->pt();
      m_muon_indet_tptree_phi = trk->phi();
      m_muon_indet_tptree_eta = trk->eta();
      m_muon_indet_tptree_isTruth = checkTruthTrack(trk);
      m_muon_indet_tptree_mass = mass;
      m_muon_indet_tptree_runnumber =  thisEventInfo->runNumber();
      m_muon_indet_tptree_eventnumber = thisEventInfo->eventNumber();
      m_muon_indet_tptree_weight = m_evtWeight;
      m_muon_indet_tptree_lb = m_this_lb;

                                                                                                                           
      //for all ms tracks
      for (const auto& mu2 : *idTracks) {
        auto idtrkp4(mu2->p4());
        auto mstrkp4(trk->p4());
                                                                                                                                        
        Float_t dR = idtrkp4.DeltaR(mstrkp4);
        Float_t dPT = mstrkp4.Pt() - idtrkp4.Pt();
    
        //Currently using magic numbers tuned by eye, may want to fix in the future...
        if (fabs(dPT) < 10000 && dR < 0.05) 
          matched = true;                
        
        if (!matched) 
          continue;
        if (!m_isSimulation){
          (trk->charge() != tagmu->charge()) ? static_cast<void>(m_mu_InDet_tp_match_os->Fill(mass)) : static_cast<void>(m_mu_InDet_tp_match_ss->Fill(mass));
        }
        m_muon_indet_tptree_mtype = (trk->charge() != tagmu->charge()) ? 0 : 1;
        if (m_writeTTrees){
          m_muon_indet_tptree->Fill();
        }

        return; // once a match is found no need to continue
      }
                                        
      // should only reach this point if a match was not made
      if (!m_isSimulation){
        (trk->charge() != tagmu->charge()) ? static_cast<void>(m_mu_InDet_tp_nomatch_os->Fill(mass)) : static_cast<void>(m_mu_InDet_tp_nomatch_ss->Fill(mass));
      }
      m_muon_indet_tptree_mtype = (trk->charge() != tagmu->charge()) ? 2 : 3;
      if (m_writeTTrees){
        m_muon_indet_tptree->Fill();
      }
    }
  }
}

void DQTGlobalWZFinderTool::setDQTGlobalWZFinderBranches(){

  // Muon
  m_muontree->Branch("isTruth",&m_muontree_isTruth);
  m_muontree->Branch("eta1",&m_muontree_eta1);
  m_muontree->Branch("eta2",&m_muontree_eta2);
  m_muontree->Branch("phi1",&m_muontree_phi1);
  m_muontree->Branch("phi2",&m_muontree_phi2);
  m_muontree->Branch("pT1",&m_muontree_pT1);
  m_muontree->Branch("pT2",&m_muontree_pT2);
  m_muontree->Branch("weight",&m_muontree_weight);
  m_muontree->Branch("mass",&m_muontree_mass);
  m_muontree->Branch("lb",&m_muontree_lb);
  m_muontree->Branch("runnumber",&m_muontree_runnumber);
  m_muontree->Branch("eventnumber",&m_muontree_eventnumber);

  m_muon_reco_tptree->Branch("isTruth",&m_muon_reco_tptree_isTruth);
  m_muon_reco_tptree->Branch("eta",&m_muon_reco_tptree_eta);
  m_muon_reco_tptree->Branch("phi",&m_muon_reco_tptree_phi);
  m_muon_reco_tptree->Branch("pT",&m_muon_reco_tptree_pT);
  m_muon_reco_tptree->Branch("mass",&m_muon_reco_tptree_mass);
  m_muon_reco_tptree->Branch("weight",&m_muon_reco_tptree_weight);
  m_muon_reco_tptree->Branch("mtype",&m_muon_reco_tptree_mtype);
  m_muon_reco_tptree->Branch("lb",&m_muon_reco_tptree_lb);
  m_muon_reco_tptree->Branch("runnumber",&m_muon_reco_tptree_runnumber);
  m_muon_reco_tptree->Branch("eventnumber",&m_muon_reco_tptree_eventnumber);

  m_muon_trig_tptree->Branch("isTruth",&m_muon_trig_tptree_isTruth);
  m_muon_trig_tptree->Branch("eta",&m_muon_trig_tptree_eta);
  m_muon_trig_tptree->Branch("phi",&m_muon_trig_tptree_phi);
  m_muon_trig_tptree->Branch("pT",&m_muon_trig_tptree_pT);
  m_muon_trig_tptree->Branch("mass",&m_muon_trig_tptree_mass);
  m_muon_trig_tptree->Branch("weight",&m_muon_trig_tptree_weight);
  m_muon_trig_tptree->Branch("mtype",&m_muon_trig_tptree_mtype);
  m_muon_trig_tptree->Branch("lb",&m_muon_trig_tptree_lb);
  m_muon_trig_tptree->Branch("runnumber",&m_muon_trig_tptree_runnumber);
  m_muon_trig_tptree->Branch("eventnumber",&m_muon_trig_tptree_eventnumber);

  m_muon_indet_tptree->Branch("isTruth",&m_muon_indet_tptree_isTruth);
  m_muon_indet_tptree->Branch("eta",&m_muon_indet_tptree_eta);
  m_muon_indet_tptree->Branch("phi",&m_muon_indet_tptree_phi);
  m_muon_indet_tptree->Branch("pT",&m_muon_indet_tptree_pT);
  m_muon_indet_tptree->Branch("mass",&m_muon_indet_tptree_mass);
  m_muon_indet_tptree->Branch("weight",&m_muon_indet_tptree_weight);
  m_muon_indet_tptree->Branch("mtype",&m_muon_indet_tptree_mtype);
  m_muon_indet_tptree->Branch("lb",&m_muon_indet_tptree_lb);
  m_muon_indet_tptree->Branch("runnumber",&m_muon_indet_tptree_runnumber);
  m_muon_indet_tptree->Branch("eventnumber",&m_muon_indet_tptree_eventnumber);

  // Electron
  m_electrontree->Branch("isTruth",&m_electrontree_isTruth);
  m_electrontree->Branch("eta1",&m_electrontree_eta1);
  m_electrontree->Branch("eta2",&m_electrontree_eta2);
  m_electrontree->Branch("phi1",&m_electrontree_phi1);
  m_electrontree->Branch("phi2",&m_electrontree_phi2);
  m_electrontree->Branch("pT1",&m_electrontree_pT1);
  m_electrontree->Branch("pT2",&m_electrontree_pT2);
  m_electrontree->Branch("weight",&m_electrontree_weight);
  m_electrontree->Branch("mass",&m_electrontree_mass);
  m_electrontree->Branch("lb",&m_electrontree_lb);
  m_electrontree->Branch("runnumber",&m_electrontree_runnumber);
  m_electrontree->Branch("eventnumber",&m_electrontree_eventnumber);

  m_electron_reco_tptree->Branch("isTruth",&m_electron_reco_tptree_isTruth);
  m_electron_reco_tptree->Branch("eta",&m_electron_reco_tptree_eta);
  m_electron_reco_tptree->Branch("phi",&m_electron_reco_tptree_phi);
  m_electron_reco_tptree->Branch("pT",&m_electron_reco_tptree_pT);
  m_electron_reco_tptree->Branch("mass",&m_electron_reco_tptree_mass);
  m_electron_reco_tptree->Branch("weight",&m_electron_reco_tptree_weight);
  m_electron_reco_tptree->Branch("mtype",&m_electron_reco_tptree_mtype);
  m_electron_reco_tptree->Branch("lb",&m_electron_reco_tptree_lb);
  m_electron_reco_tptree->Branch("runnumber",&m_electron_reco_tptree_runnumber);
  m_electron_reco_tptree->Branch("eventnumber",&m_electron_reco_tptree_eventnumber);

  m_electron_trig_tptree->Branch("isTruth",&m_electron_trig_tptree_isTruth);
  m_electron_trig_tptree->Branch("eta",&m_electron_trig_tptree_eta);
  m_electron_trig_tptree->Branch("phi",&m_electron_trig_tptree_phi);
  m_electron_trig_tptree->Branch("pT",&m_electron_trig_tptree_pT);
  m_electron_trig_tptree->Branch("mass",&m_electron_trig_tptree_mass);
  m_electron_trig_tptree->Branch("weight",&m_electron_trig_tptree_weight);
  m_electron_trig_tptree->Branch("mtype",&m_electron_trig_tptree_mtype);
  m_electron_trig_tptree->Branch("lb",&m_electron_trig_tptree_lb);
  m_electron_trig_tptree->Branch("runnumber",&m_electron_trig_tptree_runnumber);
  m_electron_trig_tptree->Branch("eventnumber",&m_electron_trig_tptree_eventnumber);

  m_electron_container_tptree->Branch("isTruth",&m_electron_container_tptree_isTruth);
  m_electron_container_tptree->Branch("eta",&m_electron_container_tptree_eta);
  m_electron_container_tptree->Branch("phi",&m_electron_container_tptree_phi);
  m_electron_container_tptree->Branch("pT",&m_electron_container_tptree_pT);
  m_electron_container_tptree->Branch("mass",&m_electron_container_tptree_mass);
  m_electron_container_tptree->Branch("weight",&m_electron_container_tptree_weight);
  m_electron_container_tptree->Branch("mtype",&m_electron_container_tptree_mtype);
  m_electron_container_tptree->Branch("lb",&m_electron_container_tptree_lb);
  m_electron_container_tptree->Branch("runnumber",&m_electron_container_tptree_runnumber);
  m_electron_container_tptree->Branch("eventnumber",&m_electron_container_tptree_eventnumber);

}

void DQTGlobalWZFinderTool::setBCIDBranches(TTree *tree, std::string variable_name){
  tree->Branch("lumiblock", &m_this_lb);
  tree->Branch("pileup", &m_pileup_per_lb);
  tree->Branch("BCID", &m_distance_from_front);
  tree->Branch("train_length", &m_train_length);
  tree->Branch("isOs", &m_isOS);
  
  if (variable_name == "mass"){
    tree->Branch(variable_name.c_str(), &m_mass);
  }else if (variable_name == "matched"){
    tree->Branch(variable_name.c_str(), &m_nTrig_matches);
  }
}


bool DQTGlobalWZFinderTool::checkTruthElectron(const xAOD::Electron* elec){

  // Check if input electron originates from a ZBoson, following EGamma recipe
  unsigned int iTypeOfPart;
  unsigned int iPartOrig;

  bool truthMatched = false;

  const xAOD::TruthParticle* lastElTruth = xAOD::EgammaHelpers::getBkgElectronMother(elec);
  if( lastElTruth ){
    auto res = m_truthClassifier->particleTruthClassifier(lastElTruth);
    iTypeOfPart = res.first;
    iPartOrig   = res.second;

    if((iTypeOfPart == 2 && iPartOrig == 13) || (iPartOrig == 40)){
      truthMatched = true;
    }
  }

  return truthMatched;

}


bool DQTGlobalWZFinderTool::checkTruthMuon(const xAOD::Muon* muon){

  // Check if input muon originates from a ZBoson
  bool truthMatched = false;
  
  std::pair<unsigned int, unsigned int> res;
  ParticleDef partDef;

  res=m_truthClassifier->particleTruthClassifier(muon);

  unsigned int iTypeOfPart = res.first;
  unsigned int iPartOrig   = res.second;

  const auto* thePart = m_truthClassifier->getGenPart();
  
  if(thePart){
    if(iTypeOfPart == 6 && iPartOrig == 13){
      truthMatched = true;
    }
  }

  return truthMatched;

}


bool DQTGlobalWZFinderTool::checkTruthTrack(const xAOD::TrackParticle* trk){

  // Check if input track originates from a ZBoson     
  bool truthMatched = false;

  std::pair<unsigned int, unsigned int> res;
  ParticleDef partDef;

  res=m_truthClassifier->particleTruthClassifier(trk);

  unsigned int iTypeOfPart = res.first;
  unsigned int iPartOrig   = res.second;

  const auto* thePart = m_truthClassifier->getGenPart();

  if(thePart){
    if(iTypeOfPart == 6 && iPartOrig == 13){
        truthMatched = true;
    }
  }

  return truthMatched;

}

void DQTGlobalWZFinderTool::fillEleEffHistos(bool tag_good, bool probe_good, bool probe_anti_good, bool os, double mass){
 
  if(!tag_good) 
    return; 
 
  m_mass = mass;
  m_isOS = os;
 

  if(os){
    if(probe_good){
      m_ele_tight_good_os->Fill(mass);  
    }
    else{
      m_ele_tight_bad_os->Fill(mass);
    }    
    if(probe_anti_good){
      m_ele_template_os->Fill(mass);  
    }
  }

  if (!os){
    if(probe_good){
      m_ele_tight_good_ss->Fill(mass);
    }
    else{
      m_ele_tight_bad_ss->Fill(mass);
    }
    if(probe_anti_good){
      m_ele_template_ss->Fill(mass);   
    }
  }

  if (m_do_BCID){
    if(probe_good)
      m_bcid_ele_tight_good->Fill();
    if(!probe_good)
      m_bcid_ele_tight_bad->Fill();
    if(probe_anti_good)
      m_bcid_ele_template->Fill();
  }
}


//----------------------------------------------------------------------------------
StatusCode DQTGlobalWZFinderTool::checkHists(bool /* fromFinalize */)
//----------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), name());

  ATH_MSG_DEBUG("in checkHists()");

  return StatusCode::SUCCESS;
}
