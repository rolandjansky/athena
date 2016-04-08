/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////// 
/// Analysis skeleton
/// also for distributed analysis examples
/// Author: Ketevi A. Assamagan
/// BNL, July 22, 2004
///
/// DESCRIPTION:
///
/// This class is an analysis skeleton - The user can implement her analysis here
/// Ketevi A. Assamagan on June 22, 2004
///
/// Vivek Jain - March 11, 2010
/// This file has been extensively modified since creation. It now shows you how to access
/// trigger information. Preparation of objects, e.g., electrons, muons, jets, etc., 
/// has been pushed off into the UserAnalysisUtils package
///
/// The code here also does overlap removal
///
/// Please see https://twiki.cern.ch/twiki/bin/view/AtlasProtected/PhysicsAnalysisWorkBookAODAnalysisRel15#Writing_your_own_analysis_code for details relating to release 15
///
///////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////

// the first two come for free when using AthAlgorithm
//#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"


#include "xAODEventInfo/EventInfo.h"

#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/egammaParamDefs.h"

#include "McParticleEvent/TruthParticleContainer.h"

#include "VxVertex/VxContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"

#include "muonEvent/MuonContainer.h"
#include "egammaEvent/PhotonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "JetEvent/JetCollection.h"
#include "MissingETEvent/MissingET.h"

#include "NavFourMom/IParticleContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "JetTagInfo/TruthInfo.h"

#include "TrigDecisionTool/ChainGroup.h"

#include "UserAnalysis/AnalysisSkeleton.h"

#include "TrigParticle/TrigTau.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/Tau1P3PDetails.h"
#include "tauEvent/TauRecDetails.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigParticle/TrigTau.h"
#include "TrigParticle/TrigTauContainer.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEvent/TrigTauCluster.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

//static const double mZ = 91.19*CLHEP::GeV;
//static const int  MAX_PARTICLES = 20;

using namespace Analysis;
using namespace Rec;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

AnalysisSkeleton::AnalysisSkeleton(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_analysisTools( "AnalysisTools" ),
  m_analysisSelectionTool( "UserAnalysisSelectionTool" ),
  m_analysisPreparationTool( "UserAnalysisPreparationTool" ),
  m_analysisOverlapCheckingTool( "UserAnalysisOverlapCheckingTool" ),
  m_analysisOverlapRemovalTool( "UserAnalysisOverlapRemovalTool" ),
  m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
  m_doTrigger(true),
  m_investigateChain("EF_tau16i_loose_2j23")
{

  /** switches to control the analysis through job options */

  declareProperty( "AnalysisTools",               m_analysisTools );
  declareProperty( "AnalysisSelectionTool",       m_analysisSelectionTool);
  declareProperty( "AnalysisPreparationTool",     m_analysisPreparationTool);
  declareProperty( "AnalysisOverlapCheckingTool", m_analysisOverlapCheckingTool);
  declareProperty( "AnalysisOverlapRemovalTool",  m_analysisOverlapRemovalTool);
  declareProperty( "TrigDecisionTool",            m_trigDec, "The tool to access TrigDecision");

  declareProperty("ElectronContainer", m_electronContainerName="ElectronAODCollection"); 
  declareProperty("McParticleContainer", m_truthParticleContainerName = "SpclMC");
  declareProperty("MissingETObject",m_missingETObjectName="MET_RefFinal");

  /** the cuts - default values - to be modified in job options */

  declareProperty("DeltaRMatchCut", m_deltaRMatchCut = 0.2);
  declareProperty("MaxDeltaR", m_maxDeltaR = 0.9999);

  /** additiona cuts for electrons */
  declareProperty("ElectronEtCut", m_etElecCut = 10.0*CLHEP::GeV);
  declareProperty("ElectronEtaCut", m_etaElecCut = 2.5);
  declareProperty("ElectronCone", m_elecCone = 0.9);

  /** additional cuts for bjet tagging */
  declareProperty("bjetWt_IP3DSV1Cut", m_bjetWt_ip3dsv1Cut = 6);
  declareProperty("bjet_etaCut", m_bjet_etaCut = 2.5);
  declareProperty("bjet_etCut", m_bjet_etCut = 15.0*CLHEP::GeV);

  /** missing ET options */
  declareProperty("MissingETCut",m_missingETCut=20.0*CLHEP::GeV);

  /** is this AtlFast */
  declareProperty("IsAtlFastData",m_isAtlFastData="False");

  // is this MC or not?
  declareProperty("DoTruth",m_doTruth="False");

  /** count number of jets with ET > min value - for SUSY studies */
  declareProperty("SusyJetMinEt", m_SusyJetMinEt = 50*CLHEP::GeV);

  /** trigger properties */
  declareProperty("DoTrigger", m_doTrigger, "enable trigger example");
  declareProperty("StatTriggerChains", m_triggerChains, "list of triggers for which to print statistics");
  declareProperty("InvestigateChain", m_investigateChain, "chain to investigate");

}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

AnalysisSkeleton::~AnalysisSkeleton() {}

////////////////////////////////////////////////////////////////////////////////////
/// Initialize
/// initialize StoreGate
/// get a handle on the analysis tools
/// book histograms

StatusCode AnalysisSkeleton::beginRun() {

  ATH_MSG_ERROR("Initializing AnalysisSkeleton (before eventloop)");

  // retrieve trigger decision tool
  // needs to be done before the first run/event since a number of
  // BeginRun/BeginEvents are registered by dependent services
  StatusCode sc = StatusCode::SUCCESS;

  if ( m_doTrigger ) {
     sc = m_trigDec.retrieve();
     if ( sc.isFailure() )ATH_MSG_DEBUG("Can't get handle on TrigDecisionTool");
     else ATH_MSG_DEBUG("Got handle on TrigDecisionTool");
     
  }

  // Initialize the trigger passed counters
  // this can not be done before initialize, since the properties need to be set from job-options first
  std::vector<std::string>::const_iterator it;
  for(it = m_triggerChains.begin();it != m_triggerChains.end(); it++)
     m_triggersPassed[*it] = 0;

  return sc;
} 

StatusCode AnalysisSkeleton::initialize() {

  ATH_MSG_INFO("Initializing AnalysisSkeleton");

  /// get a handle on the analysis tools
  StatusCode sc = m_analysisTools.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get handle on analysis tools");
    return sc;
  }

  /// get a handle on the preparartion analysis tools
  sc = m_analysisSelectionTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get handle on analysis selection tool");
    return sc;
  }

  sc = m_analysisPreparationTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get handle on analysis preparation tool");
    return sc;
  }

  sc = m_analysisOverlapCheckingTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get handle on analysis overlap checking tool");
    return sc;
  }

  sc = m_analysisOverlapRemovalTool.retrieve();
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Can't get handle on analysis overlap removal tool");
    return sc;
  }

  /** get a handle on the NTuple and histogramming service */
  sc = service("THistSvc", m_thistSvc);
  if (sc.isFailure()) {
     ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
     return sc;
  }

  // at this point declare new vectors to put in ntuple
  /*
  // don't really need this (as per Attila K.) May 23, 2012
  m_lvl1TriggerInfo = new std::vector<unsigned int>;
  m_lvl2TriggerInfo = new std::vector<unsigned int>;
  m_evtFilterInfo = new std::vector<unsigned int>;
  m_streamTagName = new std::vector<std::string>;
  m_streamTagType = new std::vector<std::string>;
  */

  m_aan_eta = new std::vector<double>;
  m_aan_pt = new std::vector<double>;
  m_aan_elecetres = new std::vector<double>;

  m_aan_JetEta = new std::vector<double>;
  m_aan_JetEt = new std::vector<double>;
  m_aan_JetBTagWt = new std::vector<double>;

  m_aan_FinalElEta = new std::vector<double>;
  m_aan_FinalElPt = new std::vector<double>;
  m_aan_FinalElEtCone20 = new std::vector<double>;

  m_aan_FinalMuEta = new std::vector<double>;
  m_aan_FinalMuPt = new std::vector<double>;
  m_aan_FinalMuEtCone20 = new std::vector<double>;
  m_aan_FinalMuBestMat = new std::vector<int>;
  m_aan_FinalMuMatChi2 = new std::vector<double>;

  

  /** now add branches and leaves to the AAN tree */
  // the TTree
  m_tree_AS = new TTree("tree_AS","TTree of AnalysisSkleton");
  sc = m_thistSvc->regTree("/AANT/tree_AS", m_tree_AS);

  // first add Event info stuff
  m_tree_AS->Branch("Run",  &m_runNumber,   "Run/I");    // run number
  m_tree_AS->Branch("Event",&m_eventNumber, "Event/I");  // event number
  m_tree_AS->Branch("Time", &m_eventTime,   "Time/I");   // time stamp
  m_tree_AS->Branch("LumiBlock", &m_lumiBlock,"LumiBlock/I"); // lum block num 
  m_tree_AS->Branch("BCID", &m_bCID,"BCID/I"); // bunch crossing ID
  m_tree_AS->Branch("Weight", &m_eventWeight, "Weight/D"); // weight
  /*
  // don't really need this (as per Attila K.) May 23, 2012

  m_tree_AS->Branch("LVL1ID", &m_lVL1ID,"LVL1ID/I"); // trigger LVL1 ID
  m_tree_AS->Branch("StatusElement",  &m_statusElement, "StatusElement/I");
  m_tree_AS->Branch("LVL1TriggerType",  &m_lvl1TriggerType, "LVL1TriggerType/I");
  m_tree_AS->Branch("LVL1TriggerInfo",&m_lvl1TriggerInfo);
  m_tree_AS->Branch("LVL2TriggerInfo",&m_lvl2TriggerInfo);
  m_tree_AS->Branch("EventFilterInfo",&m_evtFilterInfo);
  m_tree_AS->Branch("StreamTagName",&m_streamTagName);
  m_tree_AS->Branch("StreamTagType",&m_streamTagType);
  */

  m_tree_AS->Branch("NElectrons",    &m_aan_size, "NElectrons/i");
  m_tree_AS->Branch("ElectronEta",   &m_aan_eta);
  m_tree_AS->Branch("ElectronPt",    &m_aan_pt);
  m_tree_AS->Branch("ElecPtRatio",   &m_aan_elecetres);

  m_tree_AS->Branch("NJets",          &m_aan_njets, "NJets/i");
  m_tree_AS->Branch("NJets_etaLT25",  &m_aan_njets_etaLT25, "NJets_etaLT25/i");
  m_tree_AS->Branch("NJets_SusyETCut",&m_aan_njets_SusyETCut, "NJets_SusyETCut/i");
  m_tree_AS->Branch("JetsEta"        ,&m_aan_JetEta);
  m_tree_AS->Branch("JetsEt"         ,&m_aan_JetEt);
  m_tree_AS->Branch("JetsBTagWt"     ,&m_aan_JetBTagWt);
  m_tree_AS->Branch("MissingET",      &m_aan_ptMiss, "MissingET/d");
  m_tree_AS->Branch("EffMass",        &m_aan_effmass, "EffMass/d");
  m_tree_AS->Branch("HT",             &m_aan_ht,"HT/d");
  m_tree_AS->Branch("NBJets",         &m_aan_nbjets, "NBJets/i");
  m_tree_AS->Branch("MaxJetET",       &m_aan_maxJetET, "MaxJetET/d");

  m_tree_AS->Branch("NFinalEl",       &m_aan_NFinalEl, "NFinalEl/i");
  m_tree_AS->Branch("FinalElEta",     &m_aan_FinalElEta);
  m_tree_AS->Branch("FinalElPt",      &m_aan_FinalElPt);
  m_tree_AS->Branch("FinalElEtCone20",&m_aan_FinalElEtCone20);

  m_tree_AS->Branch("NFinalMu",       &m_aan_NFinalMu, "NFinalMu/i");
  m_tree_AS->Branch("FinalMuEta",     &m_aan_FinalMuEta);
  m_tree_AS->Branch("FinalMuPt",      &m_aan_FinalMuPt);
  m_tree_AS->Branch("FinalMuEtCone20",&m_aan_FinalMuEtCone20);
  m_tree_AS->Branch("FinalMuBestMat", &m_aan_FinalMuBestMat);
  m_tree_AS->Branch("FinalMuMatChi2", &m_aan_FinalMuMatChi2);

  m_tree_AS->Branch("FinalLepEtSum",  &m_aan_FinalLepEtSum, "FinalLepEtSum/d");
  m_tree_AS->Branch("FinalElEtSum",   &m_aan_FinalElEtSum, "FinalElEtSum/d");
  m_tree_AS->Branch("FinalMuEtSum",   &m_aan_FinalMuEtSum, "FinalMuEtSum/d");

  m_tree_AS->Branch("NumberTopQ",     &m_aan_NumTopQ, "NumberTopQ/i");
  m_tree_AS->Branch("pTtop1",         &m_aan_pTtop1,  "pTtop1/d");
  m_tree_AS->Branch("pTtop2",         &m_aan_pTtop2,  "pTtop2/d");

  m_tree_AS->Branch("Trig_efJet_et",       &m_aan_Trig_efJet_et  ,             "Trig_efJet_et/f");
  m_tree_AS->Branch("Trig_efJet_eta",      &m_aan_Trig_efJet_eta ,             "Trig_efJet_eta/f");
  m_tree_AS->Branch("Trig_efJet_phi",      &m_aan_Trig_efJet_phi ,             "Trig_efJet_phi/f");
  m_tree_AS->Branch("Trig_l2Jet_et",       &m_aan_Trig_l2Jet_et  ,             "Trig_l2Jet_et/f");
  m_tree_AS->Branch("Trig_l2Jet_eta",      &m_aan_Trig_l2Jet_eta ,             "Trig_l2Jet_eta/f");
  m_tree_AS->Branch("Trig_l2Jet_phi",      &m_aan_Trig_l2Jet_phi ,             "Trig_l2Jet_phi/f");
  m_tree_AS->Branch("Trig_l1Jet_et88",     &m_aan_Trig_l1Jet_et88,             "Trig_l1Jet_et88/f");
  m_tree_AS->Branch("Trig_l1Jet_eta",      &m_aan_Trig_l1Jet_eta ,             "Trig_l1Jet_eta/f");
  m_tree_AS->Branch("Trig_l1Jet_phi",      &m_aan_Trig_l1Jet_phi ,             "Trig_l1Jet_phi/f");

  /// ROOT histograms ---------------------------------------

  /// electrons
  m_h_elecpt     = new TH1F("elec_pt","pt el",50,0,250.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/Electron/elec_pt",m_h_elecpt);

  m_h_eleceta    = new TH1F("elec_eta","eta el",70,-3.5,3.5);
  sc = m_thistSvc->regHist("/AANT/Electron/elec_eta",m_h_eleceta);

  m_h_elec_deltaRMatch    = new TH1F("elec_deltaRMatch","elec reco/MC deltaR",50,0.,1.);
  sc = m_thistSvc->regHist("/AANT/Electron/elec_deltaRMatch",m_h_elec_deltaRMatch);

  /// jets - before OverlapRemoval
  m_h_jet_eta_beforeOR = new TH1F("jet_eta_beforeOR","jet_eta before OR",50,-5.,5.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_eta_beforeOR",m_h_jet_eta_beforeOR);

  m_h_jet_et_beforeOR = new TH1F("jet_et_beforeOR","jet_et before OR",100,0.,500.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_et_beforeOR",m_h_jet_et_beforeOR);

  m_h_jet_ip3dsv1Wt_beforeOR = new TH1F("jet_ip3dsv1Wt_beforeOR","jet_ip3dsv1Wt before OR",120,-20.,40.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_ip3dsv1Wt_beforeOR",m_h_jet_ip3dsv1Wt_beforeOR);

  m_h_jet_label_beforeOR = new TH1F("jet_label_beforeOR","jet_label before OR",20,0.,20.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_label_beforeOR",m_h_jet_label_beforeOR);

  m_h_jet_ip3dsv1Wt_bjet_beforeOR = new TH1F("jet_ip3dsv1Wt_bjet_beforeOR","b jet_ip3dsv1Wt before OR",120,-20.,40.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_ip3dsv1Wt_bjet_beforeOR",m_h_jet_ip3dsv1Wt_bjet_beforeOR);

  m_h_jet_ip3dsv1Wt_ujet_beforeOR = new TH1F("jet_ip3dsv1Wt_ujet_beforeOR","u jet_ip3dsv1Wt before OR",120,-20.,40.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_ip3dsv1Wt_ujet_beforeOR",m_h_jet_ip3dsv1Wt_ujet_beforeOR);

  /// jets - after OverlapRemoval
  m_h_jet_eta_afterOR = new TH1F("jet_eta_afterOR","jet_eta after OR",50,-5.,5.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_eta_afterOR",m_h_jet_eta_afterOR);

  m_h_jet_et_afterOR = new TH1F("jet_et_afterOR","jet_et after OR",100,0.,500.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_et_afterOR",m_h_jet_et_afterOR);

  m_h_jet_ip3dsv1Wt_afterOR = new TH1F("jet_ip3dsv1Wt_afterOR","jet_ip3dsv1Wt after OR",120,-20.,40.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_ip3dsv1Wt_afterOR",m_h_jet_ip3dsv1Wt_afterOR);

  m_h_jet_label_afterOR = new TH1F("jet_label_afterOR","jet_label after OR",20,0.,20.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_label_afterOR",m_h_jet_label_afterOR);

  m_h_jet_ip3dsv1Wt_bjet_afterOR = new TH1F("jet_ip3dsv1Wt_bjet_afterOR","b jet_ip3dsv1Wt after OR",120,-20.,40.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_ip3dsv1Wt_bjet_afterOR",m_h_jet_ip3dsv1Wt_bjet_afterOR);

  m_h_jet_ip3dsv1Wt_ujet_afterOR = new TH1F("jet_ip3dsv1Wt_ujet_afterOR","u jet_ip3dsv1Wt after OR",120,-20.,40.);
  sc = m_thistSvc->regHist("/AANT/Jet/jet_ip3dsv1Wt_ujet_afterOR",m_h_jet_ip3dsv1Wt_ujet_afterOR);

  /// missing ET

  m_pxMis   = new TH1F("MissingPx", "MissingPx",200,-500.0*CLHEP::GeV,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/MissingET/MissingPx", m_pxMis);
  m_pyMis   = new TH1F("MissingPy","MissingPy",200,-500.0*CLHEP::GeV,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/MissingET/MissingPy", m_pyMis);
  m_ptMis   = new TH1F("MissingPt","MissingPt",100,0.0,500.*CLHEP::GeV);
  sc = m_thistSvc->regHist("/AANT/MissingET/MissingPt", m_ptMis);

  // trigger

  m_triggerAccepts = new TH1F("TriggerAccepts", "TriggerAccepts",3,0,3);
  sc = m_thistSvc->regHist("/AANT/Trigger/TriggerAccepts", m_triggerAccepts);

  if (sc.isFailure()) { 
    ATH_MSG_ERROR("ROOT Hist registration failed"); 
    return sc; 
  }
  /// end ROOT Histograms ------------------------------------------

  // define chain groups using regular expressions and relying on the
  // trigger chain name convention: all L1 items start their name from
  // L1_ etc; in fact, the TrigDecisionTool already defines these
  // groups by default, but let's do it again as an example
  m_all   = m_trigDec->getChainGroup(".*");
  m_allL1 = m_trigDec->getChainGroup("L1_.*"); 
  m_allL2 = m_trigDec->getChainGroup("L2_.*");
  m_allEF = m_trigDec->getChainGroup("EF_.*");

  m_eventNr=0;

  /** Print out bjet cut values */
  ATH_MSG_DEBUG(":b jet cuts: Et/eta/IP3DSV1 wt. "<<m_bjet_etCut<< ","<<m_bjet_etaCut <<","<<m_bjetWt_ip3dsv1Cut);


  return StatusCode::SUCCESS;
}		 

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode AnalysisSkeleton::finalize() {
   
  //
  if(m_doTrigger) {
     // print trigger statistics
    ATH_MSG_INFO("STAT Trigger Statistics on " << m_eventNr << " processed events");
     for(  std::vector<std::string>::const_iterator it = m_triggerChains.begin();it != m_triggerChains.end(); it++)
       ATH_MSG_INFO("STAT Passed events for chain " << *it << "  " << m_triggersPassed[*it] << " ("<< 100.*m_triggersPassed[*it]/m_eventNr <<"%)");
  }
  return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
/// Clear - clear CBNT members
StatusCode AnalysisSkeleton::initEvent() {
  /// For Athena-Aware NTuple

  m_aan_size = 0;
  m_aan_eta->clear();
  m_aan_pt->clear();
  m_aan_elecetres->clear();

  m_aan_njets=0;
  m_aan_maxJetET = -1000.;
  m_aan_JetEta->clear();
  m_aan_JetEt->clear();
  m_aan_JetBTagWt->clear();  
  //
  m_aan_njets_etaLT25=0;
  m_aan_njets_SusyETCut = 0;

  m_aan_ptMiss = -1.;
  m_aan_effmass = 0.;
  m_aan_ht = 0;
  m_aan_nbjets = 0;

  //
  m_aan_NFinalEl = 0;
  m_aan_FinalElPt->clear();
  m_aan_FinalElEta->clear();
  m_aan_FinalElEtCone20->clear();


  m_aan_NFinalMu = 0;
  m_aan_FinalMuPt->clear();
  m_aan_FinalMuBestMat->clear();
  m_aan_FinalMuEta->clear();
  m_aan_FinalMuEtCone20->clear();
  m_aan_FinalMuMatChi2->clear();

  //
  m_aan_FinalLepEtSum = 0.;
  m_aan_FinalElEtSum  = 0.;
  m_aan_FinalMuEtSum  = 0.;

  // 
  m_aan_NumTopQ=0;
  m_aan_pTtop1=-1;
  m_aan_pTtop2=-1;

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode AnalysisSkeleton::execute() {
  //
  m_eventNr++;
  ATH_MSG_DEBUG(" in execute()");

  StatusCode sc;

  // initialize first before processing each event
  sc = initEvent();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("initEvent failed. Continue");
  }

  // initialize variables in ntuple



  /** it shows how to get the Electron and the TruthParticle
      containers shows matching between reconstructed and MC electrons

      Can be commented out if you want to do so If you do so, some of
      the electron histograms/ntuple variables will be unfilled */

  // this method is discussed in the Computing workbook - uncomment
  // VJ Oct. 29'08
  // protect with m_doTruth - VJ, Feb 16, 2010

  if(m_doTruth) {
    sc = electronSkeleton();
    if (sc.isFailure()) {
      ATH_MSG_WARNING("The method electronSkeleton() failed");
      return StatusCode::SUCCESS;
    }
  }
  /** an minimal example using the TrigDecisionTool */
  if ( m_doTrigger ) {
    sc = triggerSkeleton();
    if (sc.isFailure()) {
      ATH_MSG_WARNING("The method triggerSkeleton() failed");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_INFO("Pass state L1_MBTS_1 = " << m_trigDec->isPassed("L1_MBTS_1"));
  }
 
  /** an example of analysis preparation, including:
      - pre-selections based on the reocmmendations of performance groups
      - overlap checking
      - overlap removal */

  /** Do not comment the next method. This is where we do all the
      selection/overlap removal Those results are then used in the
      methods later on */

  sc = analysisPreparation();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Analysis Preparation Failed ");
    return StatusCode::SUCCESS;
  }

  /** The following methods were added by Vivek Jain They just show
      you how to access different variables and store them in
      histograms and/or ntuples */

  /** get basic event info. These variables are already in the output ntuple
      here we show you how to access them yourself */

  sc = addEventInfo();
  if (sc.isFailure() ) {
    ATH_MSG_WARNING("Failure in getEventInfo() ");
    return StatusCode::SUCCESS;
  }

  /** look at bjet tagging information in the jets after overlap
      removal */

  sc = bjetInfo();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("Failure in bjetInfo ");
    return StatusCode::SUCCESS;
  } 

  /** get missing Et information */

  sc = getMissingET();
  if( sc.isFailure() ) {
    ATH_MSG_WARNING("Failed to retrieve Et object found in TDS");
    return StatusCode::SUCCESS;
  }  

  /** do SUSY studies */

  sc = SusyStudies();
  if( sc.isFailure() ) {
    ATH_MSG_WARNING("Failed to do SUSY studies");
    return StatusCode::SUCCESS;
  }  

  //
  m_tree_AS->Fill();
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Trigger method - called by execute() on event by event
/// to be removed if not needed

StatusCode AnalysisSkeleton::triggerSkeleton() {

  ATH_MSG_INFO("Event Number " << m_eventNr);

  // print out list of chains in each level for the first event:
  if (m_eventNr==1) {
    ATH_MSG_INFO("L1 Items : " << m_allL1->getListOfTriggers());
    ATH_MSG_INFO("L2 Chains: " << m_allL2->getListOfTriggers());
    ATH_MSG_INFO("EF Chains: " << m_allEF->getListOfTriggers());
  }

  // simple example of isPassed():
  // isPassed([chain], [condition]) is called with the default argument condition = Physics
  // a ChainGroup is defined implicitly by the regular expression given by "EF.*" in the call to isPassed()
  ATH_MSG_INFO("Pass state L1 = " << m_trigDec->isPassed("L1_.*")) ;
  ATH_MSG_INFO("Pass state L2 = " << m_trigDec->isPassed("L2_.*")) ;
  ATH_MSG_INFO("Pass state EF = " << m_trigDec->isPassed("EF_.*")) ;

  ATH_MSG_INFO("Pass state L2_tau16i_loose_3j23 = " << m_trigDec->isPassed("L2_tau16i_loose_3j23"));
  ATH_MSG_INFO("Pass state EF_mu10              = " << m_trigDec->isPassed("EF_mu10"));
  ATH_MSG_INFO("Pass state EF_mu20              = " << m_trigDec->isPassed("EF_mu20"));
  ATH_MSG_INFO("Pass state EF_e15_medium        = " << m_trigDec->isPassed("EF_e15_medium"));
  ATH_MSG_INFO("Pass state EF_e20_loose         = " << m_trigDec->isPassed("EF_e20_loose"));

  // on the first event we are printing out prescale factors for all
  // EF chains
  // note that the prescales

  std::vector<std::string>::const_iterator it;
  if (m_eventNr==1) {
     const std::vector<std::string> allEF = m_allEF->getListOfTriggers();
     for(it = allEF.begin(); it != allEF.end(); it++) {
       ATH_MSG_INFO("Prescale info: chain " << std::left << *it << " has prescale " << m_trigDec->getPrescale(*it));
     }
     ATH_MSG_INFO("Stream info: " << m_trigDec->getListOfStreams());

     for(it = m_triggerChains.begin();it != m_triggerChains.end(); it++) {
        std::vector<std::string> chgrcnt = m_trigDec->getChainGroup(*it)->getListOfTriggers();
        for(std::vector<std::string>::iterator chgrit = chgrcnt.begin(); chgrit != chgrcnt.end(); chgrit++) {
	  ATH_MSG_INFO("Chain belonging to " << *it << ": " << *chgrit);
        }
     }
  }
  
  
  // Now we'd like to collect some trigger statistics for the chains specified in 
  for(it = m_triggerChains.begin();it != m_triggerChains.end(); it++)
     if( m_trigDec->isPassed(*it) ) {
        m_triggersPassed[*it]++;
        m_triggerAccepts->Fill(it->c_str(),1);
     }


  std::string chain(m_investigateChain);

  ATH_MSG_INFO("FLAT Pass state " << chain << " = " << m_trigDec->isPassed(chain));

  // first declare a FeatureContainer; fill it using the features(std::string chain_name) method 
  FeatureContainer f = m_trigDec->features(chain /*, broken in 15.2.0: TrigDefs::alsoDeactivateTEs*/ );
  
  std::vector< Feature<JetCollection> > jetColls = f.get<JetCollection>();
  ATH_MSG_INFO("FLAT Number of JetCollections in " << chain << ": " << jetColls.size());
  if(jetColls.size()>0) {

     m_aan_Trig_efJet_et   = 0;
     m_aan_Trig_efJet_eta  = 0;
     m_aan_Trig_efJet_phi  = 0;
     m_aan_Trig_l2Jet_et   = 0;
     m_aan_Trig_l2Jet_eta  = 0;
     m_aan_Trig_l2Jet_phi  = 0;
     m_aan_Trig_l1Jet_et88 = 0;
     m_aan_Trig_l1Jet_eta  = 0;
     m_aan_Trig_l1Jet_phi  = 0;
     
     const Feature<JetCollection>& jcf = jetColls[0];
     ATH_MSG_INFO("FLAT TE Label: " << jcf.label());
     const JetCollection* jc = jcf.cptr();
     ATH_MSG_INFO("FLAT Number of Jets in JetCollection: " << jc->size());
     JetCollection::const_iterator jIt = jc->begin();
     for (; jIt != jc->end(); ++jIt ) {
        Jet* jet = const_cast<Jet*> (*jIt);
	//        Jet* jet = *jIt;
        ATH_MSG_INFO("FLAT Jet e   : " << jet->e())   ;
        ATH_MSG_INFO("FLAT     eta : " << jet->eta()) ;
        ATH_MSG_INFO("FLAT     phi : " << jet->phi()) ;
        ATH_MSG_INFO("FLAT     pt  : " << jet->pt())  ;
        ATH_MSG_INFO("FLAT     et  : " << jet->et())  ;
        m_aan_Trig_efJet_et   = jet->et();
        m_aan_Trig_efJet_eta  = jet->eta();
        m_aan_Trig_efJet_phi  = jet->phi();
     }

     // let us find the corresponding jets in Lvl2
     Feature<TrigT2Jet> l2jetF = m_trigDec->ancestor<TrigT2Jet>(jcf);
     ATH_MSG_INFO("FLAT Found " << (l2jetF.empty()?"no ":"") << "corresponding L2 Jet.");
     if ( !l2jetF.empty() ) {
        const TrigT2Jet* t2jet = l2jetF.cptr();
        ATH_MSG_INFO("FLAT    e    : " << t2jet->e()); 
        ATH_MSG_INFO("FLAT    eta  : " << t2jet->eta()); 
        ATH_MSG_INFO("FLAT    phi  : " << t2jet->phi()); 
        ATH_MSG_INFO("FLAT    ehad : " << t2jet->ehad0()); 
        ATH_MSG_INFO("FLAT    eem  : " << t2jet->eem0()); 
        m_aan_Trig_l2Jet_et   = t2jet->e()/cosh(t2jet->eta());
        m_aan_Trig_l2Jet_eta  = t2jet->eta();
        m_aan_Trig_l2Jet_phi  = t2jet->phi();
     }
     
     // we can also access the L1 Jet_ROI using the ancestor method of the TrigDecisionTool
     Feature<Jet_ROI> jRoIF =  m_trigDec->ancestor<Jet_ROI>(jcf);
     ATH_MSG_INFO("FLAT Found " << (jRoIF.empty()?"no ":"") << "corresponding Jet_ROI"); 
     if ( !jRoIF.empty() ) {
        const Jet_ROI* jroi = jRoIF.cptr();
        ATH_MSG_INFO("FLAT Passed thresholds" << jroi->getThresholdNames()); 
        ATH_MSG_INFO("FLAT    ET4x4 : " << jroi->getET4x4()); 
        ATH_MSG_INFO("FLAT    ET6x6 : " << jroi->getET6x6()); 
        ATH_MSG_INFO("FLAT    ET8x8 : " << jroi->getET8x8());
        ATH_MSG_INFO("FLAT    eta   : " << jroi->eta()); 
        ATH_MSG_INFO("FLAT    phi   : " << jroi->phi()); 
        m_aan_Trig_l1Jet_et88 = jroi->getET8x8();
        m_aan_Trig_l1Jet_eta  = jroi->eta();
        m_aan_Trig_l1Jet_phi  = jroi->phi();
     }
  }


  // now we like to look at the Combinations of jets and tau that make up the chain decision
  const std::vector<Trig::Combination>& tauJetCombinations = f.getCombinations();
     ATH_MSG_INFO("COMB Pass state " << chain << " = " << m_trigDec->isPassed(chain));
     ATH_MSG_INFO("COMB Number of TauJetCombinations in " << chain << ": " << tauJetCombinations.size()); 
  std::vector<Trig::Combination>::const_iterator cIt;
  for ( cIt = tauJetCombinations.begin(); cIt != tauJetCombinations.end(); ++cIt ) {

     const Trig::Combination& comb = *cIt;
     
     std::vector< Feature<TauJetContainer> > tauC = comb.get<TauJetContainer>();
     std::vector< Feature<JetCollection> >   jetC = comb.get<JetCollection>();

     ATH_MSG_INFO("COMB Combination was " << (comb.active()?"":"not ") << "active.");

     if(tauC.size()>0 || jetC.size()>0) {
        ATH_MSG_INFO("COMB Combination has " << tauC.size() << " TauJetContainer Fs and " 
		     << jetC.size() << " JetCollection Fs");

        const TauJetContainer* taus = tauC[0];
        const JetCollection* jets = jetC[0];
        
        ATH_MSG_INFO("COMB In the TauJetContainer are " << taus->size() << " taus and in the JetCollection are "
		     << jets->size() << " jets."); 
     } else {
       ATH_MSG_INFO("COMB TauJetContainer or JetCollection missing."); 
     }

     std::vector< Feature<TrigTau> >   tauFV = comb.get<TrigTau>();
     std::vector< Feature<TrigT2Jet> > jetFV = comb.get<TrigT2Jet>();

     ATH_MSG_INFO("COMB Combination has " << tauFV.size() << " TrigTau Fs and " << jetFV.size() << " TrigT2Jet Fs."); 
    
  }
    
   
  return StatusCode::SUCCESS;
}






//////////////////////////////////////////////////////////////////////////////////
/// Electron method - called by execute() on event by event
/// to be removed if not needed

StatusCode AnalysisSkeleton::electronSkeleton() {
  
  ATH_MSG_DEBUG("in electronSkeleton()");

  /** get the MC truth particle AOD container from StoreGate */
  const TruthParticleContainer*  mcpartTES = 0;
  StatusCode sc=evtStore()->retrieve( mcpartTES, m_truthParticleContainerName);
  if( sc.isFailure()  ||  !mcpartTES ) {
     ATH_MSG_WARNING("No AOD MC truth particle container found in TDS");
     return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("MC Truth Container Successfully Retrieved");
  
  /** get the container of the original AOD electron - without any selection */
  /** get the AOD electron container for TES */
  const ElectronContainer* elecTES = 0;
  sc=evtStore()->retrieve( elecTES, m_electronContainerName);
  if( sc.isFailure()  ||  !elecTES ) {
     ATH_MSG_WARNING("No AOD electron container found in TDS");
     return StatusCode::FAILURE;
  }  
  ATH_MSG_DEBUG("ElectronContainer successfully retrieved - size is " << elecTES->size() << " electrons ");


  /** iterators over the electron container - the pre-selected ones or the original ones */ 
  ElectronContainer::const_iterator elecItr  = elecTES->begin();
  ElectronContainer::const_iterator elecItrE = elecTES->end();

  for (; elecItr != elecItrE; ++elecItr) {

    /** apply further selections if necessary */
    /** check for the author of the electron */
    bool author = (*elecItr)->author(egammaParameters::AuthorElectron);
    if ( !author || (*elecItr)->pt() < m_etElecCut ) continue;

    m_aan_size++;


    /** fill histograms */
    m_h_elecpt->Fill( (*elecItr)->pt(), 1.);
    m_h_eleceta->Fill( (*elecItr)->eta(), 1.);

    /** fill Athena-Aware NTuple */
    m_aan_eta->push_back((*elecItr)->eta());
    m_aan_pt->push_back((*elecItr)->pt());

    /** find a match to this electron in the MC truth container
        the index and deltaR are returned */
    int index = -1;
    double deltaRMatch;
    if( (*elecItr)->trackParticle() && (*elecItr)->pt()> m_etElecCut ) {
       const TruthParticleContainer * truthContainer = mcpartTES;
       bool findAMatch = m_analysisTools->matchR((*elecItr), truthContainer, 
		       index, deltaRMatch, (*elecItr)->pdgId());
       if (findAMatch) {
          deltaRMatch = (deltaRMatch > m_maxDeltaR) ? m_maxDeltaR : deltaRMatch;

          m_h_elec_deltaRMatch->Fill(deltaRMatch);
          ATH_MSG_DEBUG("Electron: MC/Reco DeltaR " << deltaRMatch);
          /** check for good match */
          if ( deltaRMatch < m_deltaRMatchCut) {
             const TruthParticle*  electronMCMatch = (*mcpartTES)[index]; 
             double res = (*elecItr)->pt() / electronMCMatch->pt();
             m_aan_elecetres->push_back(res);
          }
       }
    }    
  }

  ATH_MSG_DEBUG("electronSkeleton() succeeded");
  		
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Analysis Preparation method - called by execute() on event by event
/// A lot of the AOD container are read in
/// pre-selection is done using the UserAnalysisSelectionTool
/// An example of overlap checking is demonstrated
/// An example of overlap removal is demonstration
StatusCode AnalysisSkeleton::analysisPreparation() {

  ATH_MSG_DEBUG("in analysisPreparation()");

  /** loop over Electrons from the AOD and see which pass the recommended electron selection 
      These selections are defined in m_analysisSelectionTool - to be changed if necessary */
  const ElectronContainer* elecTES = 0;
  StatusCode sc=evtStore()->retrieve( elecTES, m_electronContainerName);
  if( sc.isFailure()  ||  !elecTES ) {
     ATH_MSG_WARNING("No AOD electron container found in TDS");
  }
  ElectronContainer::const_iterator elecItr  = elecTES->begin();
  ElectronContainer::const_iterator elecItrE = elecTES->end();
  for (; elecItr != elecItrE; ++elecItr) {
     bool passedSelection = m_analysisSelectionTool->isSelected( *elecItr );
     if ( passedSelection ) ATH_MSG_DEBUG("Found a potential good Electron ");
  }

  /** do analysis preparation using the AnalysisPreparationTool
      selections based or recommended selections from performance groups  
      The tool outputs various containers of pre-selected objects */
  sc = m_analysisPreparationTool->execute();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING("AnalysisPreparation Failed - selection ");
    return StatusCode::SUCCESS;
  }

  /** get the pre-selected Electrons - given by the AnalysisPreparationTool */
  const ElectronContainer* preselectedElecTES = m_analysisPreparationTool->selectedElectrons();
  if ( !preselectedElecTES ) ATH_MSG_WARNING("Selected Electrons Not Found");
  ATH_MSG_DEBUG("Pre-selected Electrons successfully retrieved - size is " << preselectedElecTES->size() << " electrons ");

  /** get the pre-selected Muons - given by the AnalysisPreparationTool */
  const MuonContainer* preselectedMuonTES = m_analysisPreparationTool->selectedMuons();
  if ( !preselectedMuonTES )ATH_MSG_WARNING( "Selected Muons Not Found ");
  ATH_MSG_DEBUG( "Pre-selected Muons successfully retrieved - size is " << preselectedMuonTES->size() << " muons ");

  /** Check if the leading Electron and the Leadign Muon overlap or not */
  double deltaR = -1.0;
  if ( preselectedElecTES->size() > 0 && preselectedMuonTES->size() > 0) {
     const Electron * leadingElectron = preselectedElecTES->at(0);
     const Analysis::Muon     * leadingMuon     = preselectedMuonTES->at(0);
     bool areOverlapping = m_analysisOverlapCheckingTool->overlap( leadingElectron, leadingMuon, deltaR ); 
     if ( areOverlapping ) ATH_MSG_INFO("Leading Electron and Leading Muon overlap - deltaR = ");
  }

  /** now remove the overlaps using this tool 
      The input to the tool is the collection of pre-selected obeject obtained from the AnalysisPreparationTool 
      The output is various collections of final state non-overlapping particles 
      You can change the order of the overlap removal by change the input to the tool in job options */
  sc = m_analysisOverlapRemovalTool->execute();
  if ( sc.isFailure() ) {
    ATH_MSG_WARNING( "AnalysisPreparation Failed - overlap removal ");
    return StatusCode::SUCCESS;
  }

  /** get the final state Electrons - given by the AnalysisOverlapRemovalTool */
  const ElectronContainer* finalStateElecTES = m_analysisOverlapRemovalTool->finalStateElectrons();
  if ( !finalStateElecTES ) ATH_MSG_WARNING( "Final State Electrons Not Found ");
  ATH_MSG_DEBUG( "Final State Electrons successfully retrieved - size is " << finalStateElecTES->size() << " electrons ");

  ATH_MSG_DEBUG( "AnalysisPreparation() succeeded");
 
  return StatusCode::SUCCESS;

}
//////////////////////////////////////////////////////////////////////////////////
/// Method to look at bjetInfo - called by execute() on event by event
/// Use jets after overlap removal and look at the b-tagging weights within
///
//////////////////////////////////////////////////////////////////////////////////
StatusCode AnalysisSkeleton::bjetInfo() {

  ATH_MSG_DEBUG( "in bjetInfo()");

  /** loop over jet container after overlap removal 
      As a check first get the container after selection cuts, but BEFORE Overlap Removal */

  const JetCollection* selectedJetTES = m_analysisPreparationTool->selectedJets();
  if ( !selectedJetTES ) ATH_MSG_WARNING( "Selected Particle Jets Not Found ");
  else ATH_MSG_DEBUG( "Selected Jets successfully retrieved - size is " << selectedJetTES->size() << " jets ");
  // 
  const JetCollection* finalStateJetTES = m_analysisOverlapRemovalTool->finalStateJets();
  if ( !finalStateJetTES ) {
    ATH_MSG_WARNING( "Final State Particle Jets Not Found ");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Final State Jets successfully retrieved - size is " << finalStateJetTES->size() << " jets ");

  /** now look at some variables before and after overlap removal */

  int iflav;
  JetCollection::const_iterator jetItr_sel  = selectedJetTES->begin();
  JetCollection::const_iterator jetItrE_sel = selectedJetTES->end();
  for (; jetItr_sel != jetItrE_sel; ++jetItr_sel) {

    CLHEP::HepLorentzVector p4((*jetItr_sel)->px(),
			(*jetItr_sel)->py(),
			(*jetItr_sel)->pz(),
			(*jetItr_sel)->e());

    m_h_jet_eta_beforeOR->Fill(p4.pseudoRapidity());
    m_h_jet_et_beforeOR->Fill(p4.et()/1000.);

    /** get b-tagging info */

    double w_cmb = (*jetItr_sel)->getFlavourTagWeight(); // weight for IP3DSV1
    m_h_jet_ip3dsv1Wt_beforeOR->Fill(w_cmb);

    /** get quark flavour that originates this jet */
    // if we are looking at data, set it to -10
    if(m_doTruth) iflav = getQuarkJetFlavour(jetItr_sel);
    else iflav=-10;
    m_h_jet_label_beforeOR->Fill((float) iflav);

    if(p4.et() > m_bjet_etCut && fabs(p4.pseudoRapidity()) < m_bjet_etaCut) {
      if(iflav==5) m_h_jet_ip3dsv1Wt_bjet_beforeOR->Fill(w_cmb);
      if(iflav==0) m_h_jet_ip3dsv1Wt_ujet_beforeOR->Fill(w_cmb);
    }

  }
  /** after overlapRemoval */
  JetCollection::const_iterator jetItr_fin  = finalStateJetTES->begin();
  JetCollection::const_iterator jetItrE_fin = finalStateJetTES->end();
  for (; jetItr_fin != jetItrE_fin; ++jetItr_fin) {

    CLHEP::HepLorentzVector p4((*jetItr_fin)->px(),
			(*jetItr_fin)->py(),
			(*jetItr_fin)->pz(),
			(*jetItr_fin)->e());

    m_h_jet_eta_afterOR->Fill(p4.pseudoRapidity());
    m_h_jet_et_afterOR->Fill(p4.et()/1000.);

    /** get b-tagging info */



    if(p4.et() > m_bjet_etCut && fabs(p4.pseudoRapidity()) < m_bjet_etaCut) {

      double w_cmb = (*jetItr_fin)->getFlavourTagWeight(); // weight for IP3DSV1
      m_h_jet_ip3dsv1Wt_afterOR->Fill(w_cmb);

      /** get quark flavour that originates this jet */
      if(m_doTruth) iflav = getQuarkJetFlavour(jetItr_fin);
      else iflav=-10;
      m_h_jet_label_afterOR->Fill((float) iflav);
      //

      if(w_cmb > m_bjetWt_ip3dsv1Cut) m_aan_nbjets++; // count # of bjets in event

      if(iflav==5) m_h_jet_ip3dsv1Wt_bjet_afterOR->Fill(w_cmb);
      if(iflav==0) m_h_jet_ip3dsv1Wt_ujet_afterOR->Fill(w_cmb);
    }

  }

  return StatusCode::SUCCESS;

}
////////

int AnalysisSkeleton::getQuarkJetFlavour(JetCollection::const_iterator jetItr) {


    /** flavour of quark that originated this jet */
    // --- get the true label of the jet from MC Truth

    std::string label("N/A");
    
    const Analysis::TruthInfo* mcinfo = (*jetItr)->tagInfo<Analysis::TruthInfo>("TruthInfo");
    if(mcinfo) {
      label = mcinfo->jetTruthLabel();
    } else {
      ATH_MSG_VERBOSE("could not find TruthInfo for matching jet");
    }
    int iflav(0);
    if(label=="B") {
      return iflav = 5;
    }
    if(label=="C") {
      return iflav = 4;
    }
    if(label=="T") {
      return iflav = 15;
    }

    return iflav;
}
////////////////////////////////////////////////////////////////////////////////////////////////
/// missing Et object

StatusCode AnalysisSkeleton::getMissingET() {

  ATH_MSG_DEBUG( "in getMissingEt()");

  StatusCode sc = StatusCode::SUCCESS;

  if (!m_isAtlFastData) {
    /// retrieve the missing Et object from TDS
    sc = evtStore()->retrieve(m_pMissing,m_missingETObjectName);
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Could not retrieve MissingET Object");
      return StatusCode::SUCCESS;
    }
    else ATH_MSG_DEBUG(" retreived missing ET from AOD");

    m_pxMiss = m_pMissing->etx();
    m_pyMiss = m_pMissing->ety();
    m_ptMiss = m_pMissing->et();
  } else {
    /// retrieve the missing Et object from TDS
    sc=evtStore()->retrieve(m_pMissing, "AtlfastMissingEt");
    if( sc.isFailure()  ||  !m_pMissing ) {
      ATH_MSG_WARNING("No Atlfast missing Et object found in TDS");
      return StatusCode::SUCCESS;
    }  
    m_pxMiss = m_pMissing->etx();
    m_pyMiss = m_pMissing->ety();
    m_ptMiss = m_pMissing->et();
  }

  /// fill missing energy histograms
  m_pxMis->Fill(m_pxMiss);
  m_pyMis->Fill(m_pyMiss);
  m_ptMis->Fill(m_ptMiss);

  m_aan_ptMiss = m_ptMiss;

  return sc;

}
////////////////////////////////////////////////////////
StatusCode AnalysisSkeleton::SusyStudies() {

  /** Make some introductory plots */

  ATH_MSG_DEBUG( "in SusyStudies()");

  /** loop over truth container and get pT of the earliest top quarks */

  double pTtop1;
  double pTtop2;
  int numTops;

  StatusCode sc = StatusCode::SUCCESS; 
  if(m_doTruth) {
    sc = getTopQpT(numTops, pTtop1, pTtop2);  
    if(!sc) ATH_MSG_DEBUG( "Something wrong with finding top quark pT");
    else{
      
      ATH_MSG_DEBUG(" found the top quarks ");
      m_aan_NumTopQ = 2;
      m_aan_pTtop1 = pTtop1;
      m_aan_pTtop2 = pTtop2;
    }
  }
  else {m_aan_NumTopQ=0; m_aan_pTtop1 = -10; m_aan_pTtop2 = -10;}

  /** loop over jet container after overlap removal */

  const JetCollection* finalStateJetTES = m_analysisOverlapRemovalTool->finalStateJets();
  if ( !finalStateJetTES ) {
    ATH_MSG_WARNING( "SusyStudies: Final State Particle Jets Not Found ");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "SusyStudies: Final State Jets successfully retrieved - size is " << finalStateJetTES->size() << " jets ");

  double w_cmb=-100;
  JetCollection::const_iterator jetItr_fin  = finalStateJetTES->begin();
  JetCollection::const_iterator jetItrE_fin = finalStateJetTES->end();
  for (; jetItr_fin != jetItrE_fin; ++jetItr_fin) {

    CLHEP::HepLorentzVector p4((*jetItr_fin)->px(),
			(*jetItr_fin)->py(),
			(*jetItr_fin)->pz(),
			(*jetItr_fin)->e());

    /** variables for the ntuple  */
    m_aan_njets++;
    if(fabs(p4.pseudoRapidity()) < m_bjet_etaCut ) m_aan_njets_etaLT25++;
    if(p4.et()> m_SusyJetMinEt ) m_aan_njets_SusyETCut++;

    m_aan_JetEta->push_back((*jetItr_fin)->eta());
    m_aan_JetEt->push_back(p4.et());

    w_cmb = -100;
    if(p4.et() > m_bjet_etCut && fabs(p4.pseudoRapidity()) < m_bjet_etaCut) {
      w_cmb = (*jetItr_fin)->getFlavourTagWeight();} // weight for IP3DSV1
    m_aan_JetBTagWt->push_back(w_cmb);

    m_aan_ht += p4.et(); // scalar sum of jet ET
    if(p4.et()>m_aan_maxJetET) m_aan_maxJetET = p4.et(); // Jet with max ET

  } // loop over jets

  /** Get final state leptons. We need these in the determination of effective mass
      We should also store the leptons in the ntuple, so that we can re-do the calculation later */
  
  /** First do electrons */

  const ElectronContainer* finalStateElecTES = m_analysisOverlapRemovalTool->finalStateElectrons();
  if ( !finalStateElecTES ) {

    ATH_MSG_WARNING( "SusyStudies: Final State Electrons Not Found ");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "SusyStudies: Final State Electrons successfully retrieved - size is " << finalStateElecTES->size() << " electrons ");


  /** iterators over the electron container - final state */ 
  ElectronContainer::const_iterator felecItr  = finalStateElecTES->begin();
  ElectronContainer::const_iterator felecItrE = finalStateElecTES->end();

  double sum_elET=0;

  for (; felecItr != felecItrE; ++felecItr) {

    /** apply further selections if necessary */
    /** check for the author of the electron */
    bool author = (*felecItr)->author(egammaParameters::AuthorElectron);
    if ( !author || (*felecItr)->pt() < m_etElecCut ) continue;


    const EMShower* eshow = (*felecItr)->detail<EMShower>("egDetailAOD");
    double etisol = -1;
    if( eshow ) etisol = eshow->parameter(egammaParameters::etcone20);

    ATH_MSG_DEBUG( "SusyStudies:isEM/etisol "<< (*felecItr)->isem()<<","<<etisol);

    m_aan_NFinalEl++;
    m_aan_FinalElPt->push_back((*felecItr)->pt());
    m_aan_FinalElEta->push_back((*felecItr)->eta());
    m_aan_FinalElEtCone20->push_back(etisol);
    
    /** isEM cut already picks out isolated electrons, so this is just a sanity check */
    if(etisol/1000.<10) sum_elET += (*felecItr)->pt();
  }


  /** Now look at muons */

  const MuonContainer* finalStateMuonTES = m_analysisOverlapRemovalTool->finalStateMuons();
  if ( !finalStateMuonTES ) {

    ATH_MSG_WARNING( "SusyStudies: Final State Muons Not Found ");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "SusyStudies: Final State Muons successfully retrieved - size is " << finalStateMuonTES->size() << " muons ");


  /** iterators over the muon container - final state  */ 
  MuonContainer::const_iterator fmuonItr  = finalStateMuonTES->begin();
  MuonContainer::const_iterator fmuonItrE = finalStateMuonTES->end();

  double sum_muET = 0;

  for (; fmuonItr != fmuonItrE; ++fmuonItr) {

    /** apply further selections if necessary */
    /** check for the author of the muon */

    double etIsol = (*fmuonItr)->parameter( static_cast<MuonParameters::ParamDef>(1) ); // dR of 0.2

    ATH_MSG_DEBUG( "SusyStudies:Muon etisol/best match "<< etIsol<<","<<(*fmuonItr)->bestMatch());
   
    m_aan_NFinalMu++;
    m_aan_FinalMuPt->push_back( (*fmuonItr)->pt());
    m_aan_FinalMuEta->push_back( (*fmuonItr)->eta());
    m_aan_FinalMuEtCone20->push_back( etIsol);
    m_aan_FinalMuBestMat->push_back( (*fmuonItr)->bestMatch());
    m_aan_FinalMuMatChi2->push_back((*fmuonItr)->matchChi2());

    /** require bestMatch, chi2 and isolation cuts */
    if((*fmuonItr)->bestMatch()==1 && (*fmuonItr)->matchChi2() <100. && etIsol/1000. < 10) sum_muET +=(*fmuonItr)->pt();

  }

  ATH_MSG_DEBUG( "SusyStudies: here ");

  /** now calculate effmass */
  m_aan_FinalLepEtSum = sum_elET + sum_muET; // keep leptons separate for now.
  m_aan_FinalElEtSum  = sum_elET;
  m_aan_FinalMuEtSum  = sum_muET;

  m_aan_effmass = m_aan_ptMiss + m_aan_ht; // scalar sum of jet ET + missing ET 

  ATH_MSG_DEBUG( "SusyStudies: here now ");

  return sc;

}
StatusCode AnalysisSkeleton::getTopQpT(int& numTops, double& top1, double& top2) {

  ATH_MSG_DEBUG( "in getTopQpT()");

  //
  top1 = -1; top2 = -1;
  //
  double topPt[2];
  topPt[0]=-1;
  topPt[1]=-1;
  //
  /** get the MC truth particle AOD container from StoreGate */

  const TruthParticleContainer*  mcpartTES = 0;
  StatusCode sc=evtStore()->retrieve( mcpartTES, m_truthParticleContainerName);
  if( sc.isFailure()  ||  !mcpartTES ) {
    ATH_MSG_WARNING("No AOD MC truth particle container found in TDS");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("MC Truth Container Successfully Retrieved");

  /** loop over particles and get the top quarks produced at the hard scatter */

  TruthParticleContainer::const_iterator mcpItr  = mcpartTES->begin();
  TruthParticleContainer::const_iterator mcpItrE = mcpartTES->end();

  numTops=0;
  for (; mcpItr != mcpItrE; ++mcpItr) {

    const HepMC::GenParticle* part =(*mcpItr)->genParticle();
    int pdgid = part->pdg_id();

    if(numTops==2) break; // quit if we have two already

    if(abs(pdgid)==6) { // it is a top

      HepMC::GenVertex* prod_vtx = part->production_vertex();
      int vtx_barcode = 1;
      if(prod_vtx) vtx_barcode = prod_vtx->barcode();

      if(vtx_barcode == -1) {

        topPt[numTops] = (part->momentum()).perp();
        numTops++;

      }
    

    }
  }

  top1 = topPt[0];
  top2 = topPt[1];
  return StatusCode::SUCCESS;
}
////////////////////////////////////////////////////////////
// here we get event Information and store it in our new ntuple
// In the old days, when we inherited from CBNT_AthenaAwareBase, this information was put in the
// ntuple by default
////////////////////////////////////////////////////////
StatusCode AnalysisSkeleton::addEventInfo() {


  ATH_MSG_DEBUG( "in addEventInfo");

  // this code has been taken from AnalysisExamples/VFitZmmOnAOD
  // I have the actual EventNumber, but skipped the sequential count of event #
  // 

  //get EventInfo for run and event number

  const xAOD::EventInfo* eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  
  if (sc.isFailure())
    {
      ATH_MSG_WARNING("Could not retrieve event info");
      return sc;
    }
  
  //
  m_runNumber=eventInfo->runNumber();
  m_eventNumber=eventInfo->eventNumber();
  ATH_MSG_DEBUG( "event "<<m_eventNumber);

  m_eventTime=eventInfo->timeStamp() ; 
  m_lumiBlock=eventInfo->lumiBlock() ;
  m_bCID=eventInfo->bcid();
  m_eventWeight=eventInfo->mcEventWeight();

  /*
  // see code in triggerSkeleton()
  //
  const TriggerInfo* myTriggerInfo=eventInfo->trigger_info();
  if (myTriggerInfo!=0) {
    m_lVL1ID=myTriggerInfo->extendedLevel1ID();
    m_statusElement=myTriggerInfo->statusElement();
    m_lvl1TriggerType=myTriggerInfo->level1TriggerType();

    std::vector<TriggerInfo::number_type>::const_iterator lvl1TrigIt=myTriggerInfo->level1TriggerInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator lvl1TrigIt_e=myTriggerInfo->level1TriggerInfo().end();
    for (;lvl1TrigIt!=lvl1TrigIt_e;lvl1TrigIt++)
      m_lvl1TriggerInfo->push_back(*lvl1TrigIt);


    std::vector<TriggerInfo::number_type>::const_iterator lvl2TrigIt=myTriggerInfo->level2TriggerInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator lvl2TrigIt_e=myTriggerInfo->level2TriggerInfo().end();
    for (;lvl2TrigIt!=lvl2TrigIt_e;lvl2TrigIt++)
      m_lvl2TriggerInfo->push_back(*lvl2TrigIt);

    std::vector<TriggerInfo::number_type>::const_iterator evtFilterIt=myTriggerInfo->eventFilterInfo().begin();
    std::vector<TriggerInfo::number_type>::const_iterator evtFilterIt_e=myTriggerInfo->eventFilterInfo().end();
    for (;evtFilterIt!=evtFilterIt_e;evtFilterIt++)
      m_evtFilterInfo->push_back(*evtFilterIt);


    std::vector<TriggerInfo::StreamTag>::const_iterator streamInfoIt=myTriggerInfo->streamTags().begin();
    std::vector<TriggerInfo::StreamTag>::const_iterator streamInfoIt_e=myTriggerInfo->streamTags().end();
    for (;streamInfoIt!=streamInfoIt_e;streamInfoIt++) { 
      m_streamTagName->push_back(streamInfoIt->name());
      m_streamTagType->push_back(streamInfoIt->type());
    }

  }else
    {
      m_lVL1ID=0;
      m_statusElement=0;
      m_lvl1TriggerType=0;
    }
  */
    
  return StatusCode::SUCCESS;

}

