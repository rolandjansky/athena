/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//==================================================================================
// Include files...
//==================================================================================

// This files header
#include "InDetPerformanceMonitoring/IDPerfMonZmumu.h"
// Standard headers
#include <string>
#include "TTree.h"
#include "TLorentzVector.h"

#include "InDetPerformanceMonitoring/PerfMonServices.h"
#include "InDetPerformanceMonitoring/ZmumuEvent.h"
#include "InDetPerformanceMonitoring/FourMuonEvent.h"

//#include "TrkFitterInterfaces/ITrackFitter.h"
#include "egammaInterfaces/IegammaTrkRefitterTool.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
//#include "xAODTruth/xAODTruthHelpers.h"
//For extrapolation

// ATLAS headers
#include "GaudiKernel/IInterface.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/TrackTruthKey.h"

/** needed for IP resolution studies **/
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/ChainGroup.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

/** missing ET */
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

//==================================================================================
// Public Methods
//==================================================================================
IDPerfMonZmumu::IDPerfMonZmumu(const std::string& name,
  ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_isMC(false),
  m_doRefit(false),
  m_TrackRefitter1(""),
  m_TrackRefitter2(""),
  m_trackToVertexTool("Reco::TrackToVertex"),
  m_triggerDecision("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_selTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool"),
  m_trackToVertexIPEstimator("Trk::TrackToVertexIPEstimator"),
  m_beamSpotSvc("BeamCondSvc",name),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_validationMode(true),

  m_defaultTreeName("Default_InDetTrackParticle"),
  m_IDTreeName("ID_InDetTrackParticle"),
  m_refit1TreeName("Refit1_SiAndTRT"),
  m_refit2TreeName("Refit2_SiOnly"),
  m_truthTreeName("TruthParams"),
  m_combTreeName("CombinedTrackParticle"),
  m_combMuidTreeName("CombMuidParams"),
  m_FourMuTreeName("FourMu"),

  m_ValidationTreeDescription("Small Tree for Zmumu fits"),

  m_defaultTreeFolder("/ZmumuValidation/default"),
  m_IDTreeFolder("/ZmumuValidation/ID"),
  m_refit1TreeFolder("/ZmumuValidation/refit1"),
  m_refit2TreeFolder("/ZmumuValidation/refit2"),
  m_truthTreeFolder("/ZmumuValidation/truth"),
  m_combTreeFolder("/ZmumuValidation/comb"),
  m_FourMuTreeFolder("/ZmumuValidation/fourmu"),

  m_defaultTree(0),
  m_IDTree(0),
  m_refit1Tree(0),
  m_refit2Tree(0),
  m_truthTree(0),
  m_combTree(0),
  m_combMuidTree(0),
  m_FourMuTree(0),
  m_doRemoval(true),
  m_doDebug(false)

{
  // Properties that are set from the python scripts.

  declareProperty("UseTrigger"    ,    m_UseTrigger     = false);
  declareProperty("doIsoSelection",    m_doIsoSelection = true );
  declareProperty("doIPSelection",     m_doIPSelection = true );
  declareProperty("doMCPSelection",    m_doMCPSelection = true );
  declareProperty("isMC",              m_isMC = false);
  declareProperty("doRefit",           m_doRefit = false);
  declareProperty("doIPextrToPV",      m_doIP = false);
  declareProperty("BeamCondSvc",       m_beamSpotSvc);
  declareProperty("Extrapolator",      m_extrapolator );
  declareProperty("MassWindowLow",     m_MassWindowLow = 60.0, "Lower cut in mu+mu- invariant mass" );
  declareProperty("MassWindowHigh",    m_MassWindowHigh = 120.0, "Upper cut in mu+mu- invariant mass" );
  declareProperty("OpeningAngle",      m_OpeningAngleCut = 0.2, "Opening angle between the two muons (in radians)");
  declareProperty("OutputTracksName",  m_outputTracksName = "ZmumuTracks");
  declareProperty("PtLeadingMuon",     m_LeadingMuonPtCut = 20., "Pt cut on the leading muon");
  declareProperty("PtSecondMuon",      m_SecondMuonPtCut = 15., "Pt cut on the second muon");
  declareProperty("ReFitterTool1",     m_TrackRefitter1, "ToolHandle for track fitter implementation");
  declareProperty("ReFitterTool2",     m_TrackRefitter2, "ToolHandle for track fitter implementation");
  declareProperty("TrackToVertexTool", m_trackToVertexTool);
  declareProperty("TrackTruthName",    m_truthName="TrackTruthCollection");
  declareProperty("TrackParticleName", m_trackParticleName="InDetTrackParticles");
  declareProperty("triggerChainName",  m_sTriggerChainName               );
  declareProperty("ValidationMode",    m_validationMode);
  declareProperty("xAODTruthLinkVector",m_truthLinkVecName="xAODTruthLinks");
  declareProperty("Z0Gap",             m_Z0GapCut = 5.0, "maximum gap between the z0 of both muons (in mm)");
  declareProperty( "TrackSelectionTool", m_selTool );
  declareProperty("UseTrackSelectionTool",   m_useTrackSelectionTool = false);
  declareProperty("TrackToVertexIPEstimator", m_trackToVertexIPEstimator);


  declareProperty("defaultTreeFolder", m_defaultTreeFolder );
  declareProperty("IDTreeFolder", m_IDTreeFolder );
  
  declareProperty("refit1TreeFolder", m_refit1TreeFolder );
  declareProperty("refit2TreeFolder", m_refit2TreeFolder );
  declareProperty("truthTreeFolder", m_truthTreeFolder );
  declareProperty("combTreeFolder", m_combTreeFolder );
  declareProperty("UnbiasVertex", m_doRemoval);
  
  declareProperty("DefaultTree", m_defaultTreeName );
  declareProperty("IDTree", m_IDTreeName );
  declareProperty("RefitTree",   m_combTreeName );
  declareProperty("Refit1Tree",  m_refit1TreeName );
  declareProperty("Refit2Tree",  m_refit2TreeName );
  
  declareProperty("doFourMuAnalysis", m_doFourMuAnalysis = false);
  declareProperty("FourMuTreeFolder", m_FourMuTreeFolder);

  declareProperty("StoreZmumuNtuple", m_storeZmumuNtuple = true);
  return;
}



IDPerfMonZmumu::~IDPerfMonZmumu()
{}



//==================================================================================
StatusCode IDPerfMonZmumu::initialize()
{
  // Setup the services
  ISvcLocator* pxServiceLocator = serviceLocator();
  if ( pxServiceLocator != nullptr ) {
      StatusCode xSC = PerfMonServices::InitialiseServices( pxServiceLocator );
      if ( xSC == !StatusCode::SUCCESS ) {
	ATH_MSG_FATAL("Problem Initializing PerfMonServices");
	return StatusCode::FAILURE;
      }
  }

  // Retrieve fitter
  if (m_TrackRefitter1.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter1 );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_INFO("Retrieved tool m_TrackRefitter1: " << m_TrackRefitter1 );
  }

  // Retrieve the second fitter
  if (m_TrackRefitter2.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter2 );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_INFO("Retrieved tool m_TrackRefitter2: " << m_TrackRefitter2 );
  }


  if (m_trackToVertexTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve m_trackToVertexTool " << m_trackToVertexTool );
    return StatusCode::FAILURE;
  } 
  else {
    ATH_MSG_INFO("Retrieved tool m_trackToVertexTool " << m_trackToVertexTool );
  }

  if(m_useTrackSelectionTool){
    ATH_CHECK( m_selTool.retrieve() );
  }

  if (m_doIP) {
    ATH_MSG_DEBUG("Retrieving tool (trackToVertexIPEstimator)");
    ATH_CHECK (m_trackToVertexIPEstimator.retrieve());
  }

  ATH_CHECK( m_beamSpotSvc.retrieve());
  ATH_CHECK( m_extrapolator.retrieve());
  
  m_xZmm.setDebugMode(false);
  // START new place for initilization of params
  m_xZmm.setContainer       (PerfMonServices::MUON_COLLECTION);
  m_xZmm.doIsoSelection     (m_doIsoSelection);
  m_xZmm.doIPSelection      (m_doIPSelection);
  m_xZmm.doMCPSelection     (m_doMCPSelection);
  m_xZmm.SetMassWindowLow   (m_MassWindowLow);
  m_xZmm.SetMassWindowHigh  (m_MassWindowHigh);
  m_xZmm.SetLeadingMuonPtCut(m_LeadingMuonPtCut);
  m_xZmm.SetSecondMuonPtCut (m_SecondMuonPtCut);
  m_xZmm.SetOpeningAngleCut (m_OpeningAngleCut);
  m_xZmm.SetZ0GapCut        (m_Z0GapCut);
  m_xZmm.setDebugMode       (m_doDebug);
  // END new place for initialization of params

  if (m_doFourMuAnalysis) {
    m_4mu.Init();
    m_4mu.setDebugMode(false);
  }
  m_LooseT2VAssociationTool = std::make_unique<CP::LooseTrackVertexAssociationTool>("Loose");

  ATH_CHECK( this->bookTrees() );
  
  return StatusCode::SUCCESS;
}


  
//==================================================================================
StatusCode IDPerfMonZmumu::bookTrees()
{



  if( m_defaultTree == 0){
    ATH_MSG_INFO("initialize() ** defining m_defaultTree with name: " << m_defaultTreeName.c_str());    
    m_defaultTree = new TTree((m_defaultTreeName).c_str(), m_ValidationTreeDescription.c_str());

    m_defaultTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_defaultTree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_defaultTree->Branch("lumi_block"     ,  &m_lumi_block, "lumi_block/I");
    m_defaultTree->Branch("mu"             ,  &m_event_mu,   "mu/I");

    m_defaultTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_defaultTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_defaultTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_defaultTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_defaultTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_defaultTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_defaultTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");
    m_defaultTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_defaultTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_defaultTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_defaultTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_defaultTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_defaultTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_defaultTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
    
    if (m_doIP) {
      m_defaultTree->Branch("Negative_d0_PV",      &m_negative_d0_PV   ,  "Negative_d0_PV/D");
      m_defaultTree->Branch("Negative_z0_PV",      &m_negative_z0_PV   ,  "Negative_z0_PV/D");
      m_defaultTree->Branch("Positive_z0_PV",      &m_positive_z0_PV,  "Positive_z0_PV/D");
      m_defaultTree->Branch("Positive_d0_PV",      &m_positive_d0_PV,  "Positive_d0_PV/D");

      m_defaultTree->Branch("Negative_d0_PVerr",   &m_negative_d0_PVerr,  "Negative_d0_PVerr/D");
      m_defaultTree->Branch("Negative_z0_PVerr",   &m_negative_z0_PVerr,  "Negative_z0_PVerr/D");
      m_defaultTree->Branch("Positive_z0_PVerr",   &m_positive_z0_PVerr,  "Positive_z0_PVerr/D");
      m_defaultTree->Branch("Positive_d0_PVerr",   &m_positive_d0_PVerr,  "Positive_d0_PVerr/D");

      m_defaultTree->Branch("pv_x",      &m_pv_x   ,  "pv_x/D");
      m_defaultTree->Branch("pv_y",      &m_pv_y   ,  "pv_y/D");
      m_defaultTree->Branch("pv_z",      &m_pv_z   ,  "pv_z/D");
      m_defaultTree->Branch("nTrkInVtx", &m_nTrkInVtx,  "nTrkInVtx/I");
    }
  }
  
  
  if( m_IDTree == 0){
    ATH_MSG_INFO("initialize() ** defining m_IDTree with name: " << m_IDTreeName.c_str());    
    m_IDTree = new TTree((m_IDTreeName).c_str(), m_ValidationTreeDescription.c_str());
    
    m_IDTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_IDTree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_IDTree->Branch("lumi_bLock"     ,  &m_lumi_block,  "lumi_block/I");
    m_IDTree->Branch("mu"             ,  &m_event_mu,  "mu/I");

    m_IDTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_IDTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_IDTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_IDTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_IDTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_IDTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_IDTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");
    m_IDTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_IDTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_IDTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_IDTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_IDTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_IDTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_IDTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
    
    if(m_doIP){
      m_IDTree->Branch("Negative_d0_PV",      &m_negative_d0_PV   ,  "Negative_d0_PV/D");
      m_IDTree->Branch("Negative_z0_PV",      &m_negative_z0_PV   ,  "Negative_z0_PV/D");
      m_IDTree->Branch("Positive_z0_PV",      &m_positive_z0_PV,  "Positive_z0_PV/D");
      m_IDTree->Branch("Positive_d0_PV",      &m_positive_d0_PV,  "Positive_d0_PV/D");
      
      m_IDTree->Branch("Negative_d0_PVerr",   &m_negative_d0_PVerr,  "Negative_d0_PVerr/D");
      m_IDTree->Branch("Negative_z0_PVerr",   &m_negative_z0_PVerr,  "Negative_z0_PVerr/D");
      m_IDTree->Branch("Positive_z0_PVerr",   &m_positive_z0_PVerr,  "Positive_z0_PVerr/D");
      m_IDTree->Branch("Positive_d0_PVerr",   &m_positive_d0_PVerr,  "Positive_d0_PVerr/D");

      m_IDTree->Branch("pv_x",      &m_pv_x   ,  "pv_x/D");
      m_IDTree->Branch("pv_y",      &m_pv_y   ,  "pv_y/D");
      m_IDTree->Branch("pv_z",      &m_pv_z   ,  "pv_z/D");
      m_IDTree->Branch("nTrkInVtx", &m_nTrkInVtx,  "nTrkInVtx/I");
    }
  }
  
  if( m_doRefit && m_refit1Tree == 0){
    ATH_MSG_INFO("initialize() ** defining m_refit1Tree with name: " << m_refit1TreeName.c_str());    
    m_refit1Tree = new TTree((m_refit1TreeName).c_str(), m_ValidationTreeDescription.c_str()); 
    
    m_refit1Tree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_refit1Tree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_refit1Tree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");
    m_refit1Tree->Branch("mu"             ,  &m_event_mu,  "mu/I");
    m_refit1Tree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

    m_refit1Tree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_refit1Tree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_refit1Tree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_refit1Tree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_refit1Tree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_refit1Tree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_refit1Tree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");


    m_refit1Tree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_refit1Tree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_refit1Tree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_refit1Tree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_refit1Tree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_refit1Tree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_refit1Tree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");

    if(m_doIP){
      m_refit1Tree->Branch("Negative_d0_PV",      &m_negative_d0_PV   ,  "Negative_d0_PV/D");
      m_refit1Tree->Branch("Negative_z0_PV",      &m_negative_z0_PV   ,  "Negative_z0_PV/D");
      m_refit1Tree->Branch("Positive_z0_PV",      &m_positive_z0_PV,     "Positive_z0_PV/D");
      m_refit1Tree->Branch("Positive_d0_PV",      &m_positive_d0_PV,     "Positive_d0_PV/D");
      
      m_refit1Tree->Branch("Negative_d0_PVerr",      &m_negative_d0_PVerr   ,  "Negative_d0_PVerr/D");
      m_refit1Tree->Branch("Negative_z0_PVerr",      &m_negative_z0_PVerr   ,  "Negative_z0_PVerr/D");
      m_refit1Tree->Branch("Positive_z0_PVerr",     &m_positive_z0_PVerr,     "Positive_z0_PVerr/D");
      m_refit1Tree->Branch("Positive_d0_PVerr",     &m_positive_d0_PVerr,     "Positive_d0_PVerr/D");

      m_refit1Tree->Branch("pv_x",      &m_pv_x   ,  "pv_x/D");
      m_refit1Tree->Branch("pv_y",      &m_pv_y   ,  "pv_y/D");
      m_refit1Tree->Branch("pv_z",      &m_pv_z   ,  "pv_z/D");
      m_refit1Tree->Branch("nTrkInVtx", &m_nTrkInVtx,  "nTrkInVtx/I");
    }    
  }

  if( m_doRefit && m_refit2Tree == 0){
    ATH_MSG_INFO("initialize() ** defining m_refit2Tree with name: " << m_refit2TreeName.c_str());
    m_refit2Tree = new TTree((m_refit2TreeName).c_str(), m_ValidationTreeDescription.c_str());

    m_refit2Tree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_refit2Tree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_refit2Tree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");
    m_refit2Tree->Branch("mu"             ,  &m_event_mu,  "mu/I");
    m_refit2Tree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

    m_refit2Tree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_refit2Tree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_refit2Tree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_refit2Tree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_refit2Tree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_refit2Tree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_refit2Tree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");



    m_refit2Tree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_refit2Tree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_refit2Tree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_refit2Tree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_refit2Tree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_refit2Tree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_refit2Tree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");


    if(m_doIP){
      m_refit2Tree->Branch("Negative_d0_PV",      &m_negative_d0_PV   ,  "Negative_d0_PV/D");
      m_refit2Tree->Branch("Negative_z0_PV",      &m_negative_z0_PV   ,  "Negative_z0_PV/D");
      m_refit2Tree->Branch("Positive_d0_PV",      &m_positive_d0_PV   ,  "Positive_d0_PV/D");
      m_refit2Tree->Branch("Positive_z0_PV",      &m_positive_z0_PV   ,  "Positive_z0_PV/D");
      
      
      m_refit2Tree->Branch("Negative_z0_PVerr",  &m_negative_z0_PVerr,  "Negative_z0_PVerr/D");
      m_refit2Tree->Branch("Negative_d0_PVerr",  &m_negative_d0_PVerr,  "Negative_d0_PVerr/D");
      m_refit2Tree->Branch("Positive_z0_PVerr",  &m_positive_z0_PVerr,  "Positive_z0_PVerr/D");
      m_refit2Tree->Branch("Positive_d0_PVerr",  &m_positive_d0_PVerr,  "Positive_d0_PVerr/D");

      m_refit2Tree->Branch("pv_x",      &m_pv_x   ,  "pv_x/D");
      m_refit2Tree->Branch("pv_y",      &m_pv_y   ,  "pv_y/D");
      m_refit2Tree->Branch("pv_z",      &m_pv_z   ,  "pv_z/D");

      m_refit2Tree->Branch("nTrkInVtx", &m_nTrkInVtx,  "nTrkInVtx/I");
    }    
  }
  
  if( m_combTree == 0){
    ATH_MSG_INFO("initialize() ** defining m_combTree with name: " << m_combTreeName.c_str());
    m_combTree = new TTree((m_combTreeName).c_str(), m_ValidationTreeDescription.c_str());

    m_combTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_combTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_combTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");
    m_combTree->Branch("mu"             ,  &m_event_mu,  "mu/I");
    m_combTree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

    m_combTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_combTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_combTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_combTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_combTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_combTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_combTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");


    m_combTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_combTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_combTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_combTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_combTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_combTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_combTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
    
    if(m_doIP){
      m_combTree->Branch("Negative_z0_PV",  &m_negative_z0_PV,  "Negative_z0_PV/D");
      m_combTree->Branch("Negative_d0_PV",  &m_negative_d0_PV,  "Negative_d0_PV/D");
      m_combTree->Branch("Positive_z0_PV",  &m_positive_z0_PV,  "Positive_z0_PV/D");
      m_combTree->Branch("Positive_d0_PV",  &m_positive_d0_PV,  "Positive_d0_PV/D");

      m_combTree->Branch("Negative_z0_PVerr",  &m_negative_z0_PVerr,  "Negative_z0_PVerr/D");
      m_combTree->Branch("Negative_d0_PVerr",  &m_negative_d0_PVerr,  "Negative_d0_PVerr/D");
      m_combTree->Branch("Positive_z0_PVerr",  &m_positive_z0_PVerr,  "Positive_z0_PVerr/D");
      m_combTree->Branch("Positive_d0_PVerr",  &m_positive_d0_PVerr,  "Positive_d0_PVerr/D");

      m_combTree->Branch("pv_x",      &m_pv_x   ,  "pv_x/D");
      m_combTree->Branch("pv_y",      &m_pv_y   ,  "pv_y/D");
      m_combTree->Branch("pv_z",      &m_pv_z   ,  "pv_z/D");

      m_combTree->Branch("nTrkInVtx", &m_nTrkInVtx,  "nTrkInVtx/I");
    }
  }

  
  if( m_doRefit && m_combMuidTree == 0){
    m_combMuidTree = new TTree(m_combMuidTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_combMuidTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_combMuidTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_combMuidTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");
    m_combMuidTree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");
    
    m_combMuidTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_combMuidTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_combMuidTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_combMuidTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_combMuidTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_combMuidTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_combMuidTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");

    m_combMuidTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_combMuidTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_combMuidTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_combMuidTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_combMuidTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_combMuidTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_combMuidTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
  }

  if( m_isMC && m_truthTree == 0){
    m_truthTree = new TTree(m_truthTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_truthTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_truthTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_truthTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");
    m_truthTree->Branch("preScale"       ,  &m_triggerPrescale, "preScale/I");

    m_truthTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_truthTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_truthTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_truthTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_truthTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_truthTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_truthTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");

    m_truthTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_truthTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_truthTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_truthTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_truthTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_truthTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_truthTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");
  }

  if (m_doFourMuAnalysis && m_FourMuTree == 0) {
    ATH_MSG_INFO("initialize() ** defining m_FourMuTree ");
    m_FourMuTree = new TTree((m_FourMuTreeName).c_str(), "Four Muon monitoring");

    m_FourMuTree->Branch("runNumber"  ,  &m_runNumber,       "runNumber/I");
    m_FourMuTree->Branch("eventNumber",  &m_evtNumber,       "eventNumber/I");
    m_FourMuTree->Branch("lumi_block" ,  &m_lumi_block,      "lumi_block/I");
    m_FourMuTree->Branch("mu"         ,  &m_event_mu,        "mu/I");
    m_FourMuTree->Branch("preScale"   ,  &m_triggerPrescale, "preScale/I");
    m_FourMuTree->Branch("nVertex"    ,  &m_nVertex,         "nVertex/I");

    m_FourMuTree->Branch("Negative_1_Px",     &m_negative_px,   "Negative_1_Px/D");
    m_FourMuTree->Branch("Negative_1_Py",     &m_negative_py,   "Negative_1_Py/D");
    m_FourMuTree->Branch("Negative_1_Pz",     &m_negative_pz,   "Negative_1_Pz/D");
    m_FourMuTree->Branch("Negative_1_z0",     &m_negative_z0,   "Negative_1_z0/D");
    m_FourMuTree->Branch("Negative_1_d0",     &m_negative_d0,   "Negative_1_d0/D");
    m_FourMuTree->Branch("Negative_1_z0_err", &m_negative_z0_err,  "Negative_1_z0_err/D");
    m_FourMuTree->Branch("Negative_1_d0_err", &m_negative_d0_err,  "Negative_1_d0_err/D");
    m_FourMuTree->Branch("Negative_1_vtx",    &m_negative_1_vtx,  "Negative_1_vtx/I"); // vertex identifier

    m_FourMuTree->Branch("Negative_2_Px",     &m_negative_2_px,  "Negative_2_Px/D");
    m_FourMuTree->Branch("Negative_2_Py",     &m_negative_2_py,  "Negative_2_Py/D");
    m_FourMuTree->Branch("Negative_2_Pz",     &m_negative_2_pz,  "Negative_2_Pz/D");
    m_FourMuTree->Branch("Negative_2_z0",     &m_negative_2_z0,  "Negative_2_z0/D");
    m_FourMuTree->Branch("Negative_2_d0",     &m_negative_2_d0,  "Negative_2_d0/D");
    m_FourMuTree->Branch("Negative_2_z0_err", &m_negative_2_z0_err,  "Negative_2_z0_err/D");
    m_FourMuTree->Branch("Negative_2_d0_err", &m_negative_2_d0_err,  "Negative_2_d0_err/D");
    m_FourMuTree->Branch("Negative_2_vtx",    &m_negative_2_vtx,  "Negative_2_vtx/I");

    m_FourMuTree->Branch("Positive_1_Px",     &m_positive_px,     "Positive_1_Px/D");
    m_FourMuTree->Branch("Positive_1_Py",     &m_positive_py,     "Positive_1_Py/D");
    m_FourMuTree->Branch("Positive_1_Pz",     &m_positive_pz,     "Positive_1_Pz/D");
    m_FourMuTree->Branch("Positive_1_z0",     &m_positive_z0,     "Positive_1_z0/D");
    m_FourMuTree->Branch("Positive_1_d0",     &m_positive_d0,     "Positive_1_d0/D");
    m_FourMuTree->Branch("Positive_1_z0_err", &m_positive_z0_err, "Positive_1_z0_err/D");
    m_FourMuTree->Branch("Positive_1_d0_err", &m_positive_d0_err, "Positive_1_d0_err/D");
    m_FourMuTree->Branch("Positive_1_vtx",    &m_positive_1_vtx,  "Positive_1_vtx/I");

    m_FourMuTree->Branch("Positive_2_Px",     &m_positive_2_px,    "Positive_1_Px/D");
    m_FourMuTree->Branch("Positive_2_Py",     &m_positive_2_py,    "Positive_1_Py/D");
    m_FourMuTree->Branch("Positive_2_Pz",     &m_positive_2_pz,    "Positive_1_Pz/D");
    m_FourMuTree->Branch("Positive_2_z0",     &m_positive_2_z0,    "Positive_1_z0/D");
    m_FourMuTree->Branch("Positive_2_d0",     &m_positive_2_d0,    "Positive_1_d0/D");
    m_FourMuTree->Branch("Positive_2_z0_err", &m_positive_2_z0_err,"Positive_1_z0_err/D");
    m_FourMuTree->Branch("Positive_2_d0_err", &m_positive_2_d0_err,"Positive_1_d0_err/D");
    m_FourMuTree->Branch("Positive_2_vtx",    &m_positive_2_vtx,   "Positive_2_vtx/I");

    // electrons
    m_FourMuTree->Branch("ElNegative_1_Px",     &m_el_negative1_px,     "ElNegative_1_Px/D");
    m_FourMuTree->Branch("ElNegative_1_Py",     &m_el_negative1_py,     "ElNegative_1_Py/D");
    m_FourMuTree->Branch("ElNegative_1_Pz",     &m_el_negative1_pz,     "ElNegative_1_Pz/D");
    m_FourMuTree->Branch("ElNegative_1_z0",     &m_el_negative1_z0,     "ElNegative_1_z0/D");
    m_FourMuTree->Branch("ElNegative_1_d0",     &m_el_negative1_d0,     "ElNegative_1_d0/D");
    m_FourMuTree->Branch("ElNegative_1_z0_err", &m_el_negative1_z0_err, "ElNegative_1_z0_err/D");
    m_FourMuTree->Branch("ElNegative_1_d0_err", &m_el_negative1_d0_err, "ElNegative_1_d0_err/D");
    m_FourMuTree->Branch("ElNegative_1_vtx",    &m_el_negative1_vtx,    "ElNegative_1_vtx/I"); // vertex identifier

    m_FourMuTree->Branch("ElNegative_2_Px",     &m_el_negative2_px,     "ElNegative_2_Px/D");
    m_FourMuTree->Branch("ElNegative_2_Py",     &m_el_negative2_py,     "ElNegative_2_Py/D");
    m_FourMuTree->Branch("ElNegative_2_Pz",     &m_el_negative2_pz,     "ElNegative_2_Pz/D");
    m_FourMuTree->Branch("ElNegative_2_z0",     &m_el_negative2_z0,     "ElNegative_2_z0/D");
    m_FourMuTree->Branch("ElNegative_2_d0",     &m_el_negative2_d0,     "ElNegative_2_d0/D");
    m_FourMuTree->Branch("ElNegative_2_z0_err", &m_el_negative2_z0_err, "ElNegative_2_z0_err/D");
    m_FourMuTree->Branch("ElNegative_2_d0_err", &m_el_negative2_d0_err, "ElNegative_2_d0_err/D");
    m_FourMuTree->Branch("ElNegative_2_vtx",    &m_el_negative2_vtx,    "ElNegative_2_vtx/I"); // vertex identifier

    m_FourMuTree->Branch("ElPositive_1_Px",     &m_el_positive1_px,     "ElPositive_1_Px/D");
    m_FourMuTree->Branch("ElPositive_1_Py",     &m_el_positive1_py,     "ElPositive_1_Py/D");
    m_FourMuTree->Branch("ElPositive_1_Pz",     &m_el_positive1_pz,     "ElPositive_1_Pz/D");
    m_FourMuTree->Branch("ElPositive_1_z0",     &m_el_positive1_z0,     "ElPositive_1_z0/D");
    m_FourMuTree->Branch("ElPositive_1_d0",     &m_el_positive1_d0,     "ElPositive_1_d0/D");
    m_FourMuTree->Branch("ElPositive_1_z0_err", &m_el_positive1_z0_err, "ElPositive_1_z0_err/D");
    m_FourMuTree->Branch("ElPositive_1_d0_err", &m_el_positive1_d0_err, "ElPositive_1_d0_err/D");
    m_FourMuTree->Branch("ElPositive_1_vtx",    &m_el_positive1_vtx,    "ElPositive_1_vtx/I"); // vertex identifier

    m_FourMuTree->Branch("ElPositive_2_Px",     &m_el_positive2_px,     "ElPositive_2_Px/D");
    m_FourMuTree->Branch("ElPositive_2_Py",     &m_el_positive2_py,     "ElPositive_2_Py/D");
    m_FourMuTree->Branch("ElPositive_2_Pz",     &m_el_positive2_pz,     "ElPositive_2_Pz/D");
    m_FourMuTree->Branch("ElPositive_2_z0",     &m_el_positive2_z0,     "ElPositive_2_z0/D");
    m_FourMuTree->Branch("ElPositive_2_d0",     &m_el_positive2_d0,     "ElPositive_2_d0/D");
    m_FourMuTree->Branch("ElPositive_2_z0_err", &m_el_positive2_z0_err, "ElPositive_2_z0_err/D");
    m_FourMuTree->Branch("ElPositive_2_d0_err", &m_el_positive2_d0_err, "ElPositive_2_d0_err/D");
    m_FourMuTree->Branch("ElPositive_2_vtx",    &m_el_positive2_vtx,    "ElPositive_2_vtx/I"); // vertex identifier

    // other quantities
    m_FourMuTree->Branch("minv4mu",   &m_4mu_minv,  "minv4mu/D");
    m_FourMuTree->Branch("pv_x",      &m_pv_x   ,   "pv_x/D");
    m_FourMuTree->Branch("pv_y",      &m_pv_y   ,   "pv_y/D");
    m_FourMuTree->Branch("pv_z",      &m_pv_z   ,   "pv_z/D");
    m_FourMuTree->Branch("nTrkInVtx", &m_nTrkInVtx, "nTrkInVtx/I");
    
    m_FourMuTree->Branch("met",       &m_met,     "met/D");
    m_FourMuTree->Branch("metphi",    &m_metphi,  "metphi/D");
  }
  
  // now register the Trees
  ATH_MSG_INFO("initialize() Goint to register the trees");
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()){
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !");
    m_validationMode = false;
  }
  
  if ((tHistSvc->regTree(m_defaultTreeFolder, m_defaultTree)).isSuccess() ) {
    ATH_MSG_INFO("initialize() defaultTree succesfully registered!");
  }
  else {
    ATH_MSG_ERROR("initialize() Could not register the validation defaultTree -> Switching ValidationMode Off !");
    delete m_defaultTree; m_defaultTree = 0;
    m_validationMode = false;
  }
  
  if ((tHistSvc->regTree(m_IDTreeFolder, m_IDTree)).isSuccess() ) {
    ATH_MSG_INFO("initialize() IDTree succesfully registered!");
  }
  else {
    ATH_MSG_ERROR("initialize() Could not register the validation IDTree -> Switching ValidationMode Off !");
    delete m_IDTree; m_IDTree = 0;
    m_validationMode = false;
  }
  
  if ((tHistSvc->regTree(m_combTreeFolder, m_combTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation combTree -> Switching ValidationMode Off !");
    delete m_combTree; m_combTree = 0;
    m_validationMode = false;
  }
  
  if( m_doRefit ){
    if ((tHistSvc->regTree(m_refit1TreeFolder, m_refit1Tree)).isFailure() ) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_refit1Tree; m_refit1Tree = 0;
      m_validationMode = false;
    }
    if ((tHistSvc->regTree(m_refit2TreeFolder, m_refit2Tree)).isFailure() ) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_refit2Tree; m_refit2Tree = 0;
      m_validationMode = false;
    }
    if ((tHistSvc->regTree(m_combMuidTreeFolder, m_combMuidTree)).isFailure() ) {
      ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_combMuidTree; m_combMuidTree = 0;
      m_validationMode = false;
    }
  }
  
  if (m_isMC) {
    if ((tHistSvc->regTree(m_truthTreeFolder, m_truthTree)).isSuccess() ) {
      ATH_MSG_INFO("initialize() truthTree Tree succesfully registered!");
    }
    else {
      ATH_MSG_ERROR("initialize() Could not register the validation truth Tree -> Switching ValidationMode Off !");
      delete m_truthTree; m_truthTree = 0;
      m_validationMode = false;
    }
  }
  
  if (m_doFourMuAnalysis) {
    if ((tHistSvc->regTree(m_FourMuTreeFolder, m_FourMuTree)).isSuccess() ) {
      ATH_MSG_INFO("initialize() FourMu Tree succesfully registered!");
    }
    else {
      ATH_MSG_ERROR("initialize() Could not register the validation FourMu Tree -> Switching ValidationMode Off !");
      delete m_FourMuTree; m_FourMuTree = 0;
      m_validationMode = false;
    }
  }

  return StatusCode::SUCCESS;
}


//==================================================================================
void IDPerfMonZmumu::RegisterHistograms()
{
  return;
}


//==================================================================================
StatusCode IDPerfMonZmumu::execute()
{

  ATH_MSG_DEBUG("Retrieving event info.");
  const EventInfo * eventInfo;
  if (evtStore()->retrieve(eventInfo).isFailure()) {
    ATH_MSG_ERROR("Could not retrieve event info.");
  }
  else {
    m_runNumber = eventInfo->event_ID()->run_number();
    m_evtNumber = eventInfo->event_ID()->event_number();
    m_lumi_block = eventInfo->event_ID()->lumi_block();
    m_event_mu = eventInfo->actualInteractionsPerCrossing();
    ATH_MSG_DEBUG(" Execute() starting on --> Run: " << m_runNumber << "  event: " << m_evtNumber << "   Lumiblock: " << m_lumi_block);
  }

  /// --  START 4 lepton analysis
  if (m_doFourMuAnalysis) {
    StatusCode fourLeptAnaStatus = this->RunFourLeptonAnalysis();
    if (fourLeptAnaStatus.isSuccess()) ATH_MSG_DEBUG(" RunFourLeptonAnalysis() SUCCESS -> found a new event");
    if (fourLeptAnaStatus.isFailure()) ATH_MSG_DEBUG(" RunFourLeptonAnalysis() event did not pass selection criteria");
  }
  /// -- END 4 lepton analysis

  //
  //Fill Staco muon parameters only
  /* This shall not be here */
  /* comment and move to init
  m_xZmm.setContainer       (PerfMonServices::MUON_COLLECTION);
  m_xZmm.doIsoSelection     (m_doIsoSelection);
  m_xZmm.doIPSelection      (m_doIPSelection);
  m_xZmm.doMCPSelection     (m_doMCPSelection);
  m_xZmm.SetMassWindowLow   (m_MassWindowLow);
  m_xZmm.SetMassWindowHigh  (m_MassWindowHigh);
  m_xZmm.SetLeadingMuonPtCut(m_LeadingMuonPtCut);
  m_xZmm.SetSecondMuonPtCut (m_SecondMuonPtCut);
  m_xZmm.SetOpeningAngleCut (m_OpeningAngleCut);
  m_xZmm.SetZ0GapCut        (m_Z0GapCut);
  m_xZmm.setDebugMode       (m_doDebug);
  */

  // check if the muon-pair passed the resonance selection cuts:
  if( m_xZmm.Reco() ){
    ATH_MSG_INFO("Successful dimuon reconstruction in run " << m_runNumber 
		 << "  event: " << m_evtNumber 
		 << "  Lumiblock: " << m_lumi_block 
		 << "  Invariant mass = " << m_xZmm.GetInvMass() << " GeV ");
  }
  else {
    ATH_MSG_DEBUG("Failed dimuon reconstruction. m_xZmm.Reco() returned FALSE (leave IDPerfMonZmumu returning SUCCESS)");
    return StatusCode::SUCCESS;
  }

  //
  // If this point is reached -> there is a good mu+mu- pair that stisfies all selection cuts
  //

  if (m_UseTrigger) {
    bool isTriggerPassed = CheckTriggerStatusAndPrescale ();
    if (isTriggerPassed) {
      ATH_MSG_INFO("Trigger passed -> accept event");
    }
    else{
      ATH_MSG_INFO("Trigger Failed -> reject event --> leave event");
      return StatusCode::SUCCESS;
    }
  }


  // std::cout << " ** IDPerfMonZmumu ** extracting muon_pos and muon_neg... " << std::endl;
  const xAOD::Muon* muon_pos = m_xZmm.getCombMuon(m_xZmm.getPosMuon(ZmumuEvent::CB));
  const xAOD::Muon* muon_neg = m_xZmm.getCombMuon(m_xZmm.getNegMuon(ZmumuEvent::CB));

  const xAOD::TrackParticle* p1_comb = nullptr; const xAOD::Vertex* p1_comb_v = nullptr;
  const xAOD::TrackParticle* p2_comb = nullptr; const xAOD::Vertex* p2_comb_v = nullptr;
    
  //To protect against failures of the estimation
  bool success_pos =false;
  bool success_neg =false;

  if (muon_pos && muon_neg) { // if both combined muons exist and were retrieved    
    ATH_MSG_DEBUG("m_trackparticleName: " << m_trackParticleName.c_str());

    if (m_trackParticleName.find("InnerDetectorTrackParticles") != std::string::npos) {
      ATH_MSG_INFO("Retrieving InnerDetectorTrackParticles of the accepted muons");
      p1_comb = muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      p1_comb_v = p1_comb->vertex();
      p2_comb = muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
      p2_comb_v = p2_comb->vertex();
      
      if (!p1_comb || !p2_comb) {
	ATH_MSG_WARNING("Requested InnerDetectorTrackParticles but they are not present in the input file. Exiting event..");
	return StatusCode::SUCCESS;
      }
      
      if (!p1_comb_v || !p2_comb_v){
	ATH_MSG_WARNING( "Requested InnerDetectorTrackParticles have no vertex. Exiting event");
	return StatusCode::SUCCESS;
      }      
    }
    else {
      ATH_MSG_DEBUG("Retrieving CombinedTrackParticles of the accepted muons");
      p1_comb = muon_pos->trackParticle(xAOD::Muon::CombinedTrackParticle);
      p2_comb = muon_neg->trackParticle(xAOD::Muon::CombinedTrackParticle);
      
      if (!p1_comb || !p2_comb){
	ATH_MSG_WARNING( "Requested CombinedTrackParticles but not present in the input file. Exiting event");
	return StatusCode::SUCCESS;
      }
      
      // I'm afraid CB track has always NULL vertex, so use the one from the InnerDetectorTrackParticle
      if (muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) != nullptr) {
	p1_comb_v = (muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle))->vertex(); 
      }
      if (muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) != nullptr) {
	p2_comb_v = (muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle))->vertex();
      }
      
      if (!p1_comb_v || !p2_comb_v){
	ATH_MSG_WARNING( "Requested CombinedTrackParticles have no vertex. Exiting event");
	return StatusCode::SUCCESS;
      }      
    }
  }
  else { // this is the else of --> if (muon_pos && muon_neg)
    ATH_MSG_WARNING(" Could not find pos/neg muons");
    return StatusCode::SUCCESS;
  }
  
  // 
  // If this point is reached -> there is a good mu+mu- pair and the muons have been associated to a vertex
  //

  if ( m_xZmm.EventPassed() ) {
    ATH_MSG_DEBUG("Accepted muon pair. Going to fill combined Staco parameters.  Run: " << m_runNumber << "  event: " << m_evtNumber << "   Lumiblock: " << m_lumi_block);

    if (m_useTrackSelectionTool) {
      if ( !m_selTool->accept(muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)) ) {
	ATH_MSG_DEBUG("Exiting because the ID segment of muon_pos do not pass the TrackSelection");
	return StatusCode::SUCCESS;
      }	
      if ( !m_selTool->accept(muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)) ) {
	ATH_MSG_DEBUG("Exiting because the ID segment of muon_neg do not pass the TrackSelection");
	return StatusCode::SUCCESS;
      }
    } 
    
    
    ATH_MSG_DEBUG("######### >> before fill rec with default << #########");
    
    success_pos = FillRecParametersTP(muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->charge(),muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->vertex());
    success_neg = FillRecParametersTP(muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->charge(),muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle)->vertex());
    if (m_storeZmumuNtuple) {
      if (success_pos && success_neg) {
	m_IDTree->Fill();
	ATH_MSG_DEBUG("Filling  " << m_IDTree->GetName() << " with entry " << m_IDTree->GetEntries() << " for run: " << m_runNumber 
		      << "  event: " << m_evtNumber 
		      << "  Lumiblock: " << m_lumi_block 
		      << "  Invariant mass = " << m_xZmm.GetInvMass() << " GeV ");
      }
      else {
	ATH_MSG_DEBUG("FAILED filling " << m_IDTree->GetName() 
		      << "  for run: " << m_runNumber 
		      << "  event: " << m_evtNumber 
		      << "  Lumiblock: " << m_lumi_block);
      }
    }

    //
    if (m_storeZmumuNtuple) {
      success_pos = FillRecParametersTP(p1_comb, muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParametersTP(p2_comb, muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);
      if (success_pos && success_neg) {
	m_defaultTree->Fill();
	ATH_MSG_DEBUG("Filling  " << m_defaultTree->GetName() << " with entry " << m_defaultTree->GetEntries() << " for run: " << m_runNumber 
		      << "  event: " << m_evtNumber 
		      << "  Lumiblock: " << m_lumi_block 
		      << "  Invariant mass = " << m_xZmm.GetInvMass() << " GeV ");
      }
      else {
	ATH_MSG_DEBUG("FAILED filling " << m_defaultTree->GetName() 
		      << "  for run: " << m_runNumber 
		      << "  event: " << m_evtNumber 
		      << "  Lumiblock: " << m_lumi_block);
      }
    }

    // combined muons
    if (m_storeZmumuNtuple) {
      success_pos = FillRecParameters(p1_comb->track(),muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParameters(p2_comb->track(),muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);    
      if (success_pos && success_neg) {
	m_combTree->Fill();
	ATH_MSG_INFO("Filling  " << m_combTree->GetName() << " with entry " << m_combTree->GetEntries() << " for run: " << m_runNumber 
		     << "  event: " << m_evtNumber 
		     << "  Lumiblock: " << m_lumi_block 
		     << "  Invariant mass = " << m_xZmm.GetInvMass() << " GeV ");
      }
      else {
	ATH_MSG_DEBUG("FAILED filling " << m_combTree->GetName() 
		      << "  for run: " << m_runNumber 
		      << "  event: " << m_evtNumber 
		      << "  Lumiblock: " << m_lumi_block);
      }
    } // store Zmumu ntuple
  }


  // no good muon pair found, leave and continue
  if ( !m_xZmm.EventPassed()) {
    //failed cuts, continue to next event
    ATH_MSG_DEBUG ("No good muon pair found. Leaving Execute(). Run: " << m_runNumber << "  event: " << m_evtNumber);
    return StatusCode::SUCCESS;
  }

  //fill truth first in case no truth match found
  if (m_isMC) {
    if (FillTruthParameters(p1_comb).isFailure()){
      ATH_MSG_WARNING("Failed to fill truth parameters - skipping event");
      return StatusCode::SUCCESS;
    }
    if (FillTruthParameters(p2_comb).isFailure()){
      ATH_MSG_WARNING("Failed to fill truth parameters - skipping event");
      return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("fill truthTree with parameters : ");
    ATH_MSG_DEBUG("######   (negative)   ########## ");
    ATH_MSG_DEBUG("Negative px: " << m_negative_px << "\n"
		  << "Negative py: " << m_negative_py << "\n"
		  << "Negative pz: " << m_negative_pz << "\n"
		  << "Negative d0: " << m_negative_d0 << "\n"
		  << "Negative z0: " << m_negative_z0 << "\n");
    
    
    ATH_MSG_DEBUG("######   (positive)   ########## ");
    ATH_MSG_DEBUG( "Positive px: " << m_positive_px << "\n"
		   << "Positive py: " << m_positive_py << "\n"
		   << "Positive pz: " << m_positive_pz << "\n"
		   << "Positive d0: " << m_positive_d0 << "\n"
		   << "Positive z0: " << m_positive_z0 << "\n");
    
    if (m_storeZmumuNtuple) m_truthTree->Fill();
  } // if (m_isMC)


  // changed refitting to combinedparticles since run II DESDM_ZMUMU did not store InDetTrackParticles
  if (!p1_comb->track() || !p2_comb->track()) {
     ATH_MSG_WARNING("Track missing!  p1_comb->track() or p2_comb->track()  ** Skipping Event Run: " << m_runNumber << "  event: " << m_evtNumber);
     return StatusCode::SUCCESS;
  }


  if( m_doRefit ){
    ATH_MSG_DEBUG("Going to build TrackCollections: muonTrks, muonTrksRefit1 and muonTrksRefit2");
    TrackCollection* muonTrks  = new TrackCollection(SG::OWN_ELEMENTS);
    TrackCollection* muonTrksRefit1  = new TrackCollection(SG::OWN_ELEMENTS);
    TrackCollection* muonTrksRefit2  = new TrackCollection(SG::OWN_ELEMENTS);


    ATH_MSG_DEBUG(" Going to refit and store the track parameters ");
    Trk::Track* defaultMuonTrk1 = 0;
    Trk::Track* defaultMuonTrk2 = 0;
    Trk::Track* refit1MuonTrk1 = 0;
    Trk::Track* refit2MuonTrk1 = 0;
    Trk::Track* refit1MuonTrk2 = 0;
    Trk::Track* refit2MuonTrk2 = 0;

    const xAOD::Electron* egam = 0;

    StatusCode fitStatus;
    //save default and refit track parameters
    if( p1_comb->track() ) {
      defaultMuonTrk1 = new Trk::Track(*p1_comb->track());

      fitStatus = m_TrackRefitter1->refitTrack( p1_comb->track(),egam );
      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG("Track Refit1 Failed. Skipping Event");
	return StatusCode::SUCCESS;
      } else {
	refit1MuonTrk1 = m_TrackRefitter1->refittedTrack();
	muonTrksRefit1->push_back(refit1MuonTrk1);
	ATH_MSG_DEBUG("Successfully refitted (1) track");
      }


      fitStatus = m_TrackRefitter2->refitTrack( p1_comb->track(),egam );
      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG("Track Refit2 Failed. Skipping Event");
	return StatusCode::SUCCESS;
      } else {
	refit2MuonTrk1 = m_TrackRefitter2->refittedTrack();
	muonTrksRefit2->push_back(refit2MuonTrk1);
	ATH_MSG_DEBUG("Successfully refitted (2) track");
      }

    }

    if( p2_comb->track() ) {
      defaultMuonTrk2 = new Trk::Track(*p2_comb->track());

      fitStatus = m_TrackRefitter1->refitTrack( p2_comb->track(),egam );
      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG("Track Refit1 Failed. Skipping Event");
	return StatusCode::SUCCESS;
      } else {
	refit1MuonTrk2 = m_TrackRefitter1->refittedTrack();
	muonTrksRefit1->push_back(refit1MuonTrk2);
	ATH_MSG_DEBUG("Successfully refitted (1) track");
      }


      fitStatus = m_TrackRefitter2->refitTrack( p2_comb->track(),egam );
      if (fitStatus.isFailure()) {
	ATH_MSG_DEBUG("Track Refit2 Failed. Skipping Event");
	return StatusCode::SUCCESS;
      } else {
	refit2MuonTrk2 = m_TrackRefitter2->refittedTrack();
	muonTrksRefit2->push_back(refit2MuonTrk2);
	ATH_MSG_DEBUG("Successfully refitted (2) track");
      }

      if (muonTrksRefit1->size() != 2) {
	std::cout << " SALVA  size of muonTrksRefit1: " << muonTrksRefit1->size() << std::endl;
      }
    }

    //save tracks to storegrate	/
    muonTrks->push_back(defaultMuonTrk1);
    muonTrks->push_back(defaultMuonTrk2);

    StatusCode sc = evtStore()->record(muonTrks, m_outputTracksName);
    if (sc.isFailure()) {
      ATH_MSG_WARNING ("Failed storing " << m_outputTracksName);
    }
    else{
      ATH_MSG_DEBUG ("Stored "<< muonTrks->size() << " " << m_outputTracksName <<" into StoreGate");
    }

    sc = evtStore()->record(muonTrksRefit1, m_outputTracksName + "Refit1");
    if (sc.isSuccess()) {
      ATH_MSG_DEBUG ("Stored "<< muonTrksRefit1->size() << " " << m_outputTracksName  + "Refit1" <<" into StoreGate");
    } else {
      ATH_MSG_WARNING ("Failed storing " << m_outputTracksName + "Refit1");
    }

    sc = evtStore()->record(muonTrksRefit2, m_outputTracksName + "Refit2");
    if (sc.isSuccess()) {
      ATH_MSG_DEBUG ("Stored "<< muonTrksRefit2->size() << " " << m_outputTracksName + "Refit2" <<" into StoreGate");
    } else {
      ATH_MSG_WARNING ("Failed storing " << m_outputTracksName +"Refit2");
    }


    
    //fill refit1 ID parameters
    if (muonTrksRefit1->size()<2) {
      ATH_MSG_WARNING("Refit1 muon tracks are missing!");
    }
    else{
      ATH_MSG_DEBUG("-- >> going to fill refit1tree <<-- ");
      success_pos = FillRecParameters(refit1MuonTrk1, muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParameters(refit1MuonTrk2, muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);

      ATH_MSG_DEBUG("fill refit1Tree with parameters : ");
      ATH_MSG_DEBUG("mu- pxyz: " << m_negative_px
		    << ", " << m_negative_py
		    << ", " << m_negative_pz 
		    << "  d0: " << m_negative_d0
		    << "  z0: " << m_negative_z0);
      ATH_MSG_DEBUG( "mu+ --> pxyz " << m_positive_px 
		     << ", " << m_positive_py
		     << ", " << m_positive_pz
		     << "  d0: " << m_positive_d0 
		     << "  z0: " << m_positive_z0);
      if (m_storeZmumuNtuple) {
	if (success_pos && success_neg) {
	  ATH_MSG_DEBUG("       refit1   Tree muon_pos: " << muon_pos->pt() << "    m_pos: " << sqrt(m_positive_px*m_positive_px + m_positive_py*m_positive_py));
	  ATH_MSG_DEBUG("                     muon_neg: " << muon_neg->pt() << "    m_neg: " << sqrt(m_negative_px*m_negative_px + m_negative_py*m_negative_py)); 
	  m_refit1Tree->Fill();
	  ATH_MSG_INFO("Filling  " << m_refit1Tree->GetName() << " with entry " << m_refit1Tree->GetEntries() << " for run: " << m_runNumber 
		       << "  event: " << m_evtNumber 
		       << "  Lumiblock: " << m_lumi_block 
		       << "  Invariant mass = " << m_xZmm.GetInvMass() << " GeV ");
	}
	else {
	  ATH_MSG_DEBUG("FAILED filling " << m_refit1Tree->GetName() 
			<< "  for run: " << m_runNumber 
			<< "  event: " << m_evtNumber 
			<< "  Lumiblock: " << m_lumi_block);
	}
      }
    }

    //fill refit2 ID parameters
    if (muonTrksRefit2->size()<2) {
      ATH_MSG_WARNING("Refit2 muon tracks are missing!");
    }
    else{

      ATH_MSG_DEBUG("-- >> going to fill refit2params << --");
      success_pos = FillRecParameters(refit2MuonTrk1, muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParameters(refit2MuonTrk2, muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);
      
      if (m_storeZmumuNtuple) {
	if (success_pos && success_neg) {
	  ATH_MSG_DEBUG("       refit2   Tree muon_pos: " << muon_pos->pt() << "    m_pos: " << sqrt(m_positive_px*m_positive_px + m_positive_py*m_positive_py));
	  ATH_MSG_DEBUG("                     muon_neg: " << muon_neg->pt() << "    m_neg: " << sqrt(m_negative_px*m_negative_px + m_negative_py*m_negative_py)); 
	  m_refit2Tree->Fill();
	  ATH_MSG_INFO("Filling  " << m_refit2Tree->GetName() << " with entry " << m_refit2Tree->GetEntries() << " for run: " << m_runNumber 
		       << "  event: " << m_evtNumber 
		       << "  Lumiblock: " << m_lumi_block 
		       << "  Invariant mass = " << m_xZmm.GetInvMass() << " GeV ");
	}
	else {
	  ATH_MSG_DEBUG("FAILED filling " << m_refit2Tree->GetName() 
			<< "  for run: " << m_runNumber 
			<< "  event: " << m_evtNumber 
			<< "  Lumiblock: " << m_lumi_block);
	}
      }
    }
    
    if (!muon_pos || !muon_neg) {
      ATH_MSG_WARNING("CB Muons missing!");
    } 
    else {      
      ATH_MSG_DEBUG("-- >> going to fill combined muons params << --");
      success_pos = FillRecParameters(p1_comb->track(), muon_pos->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p1_comb->charge(),p1_comb_v);
      success_neg = FillRecParameters(p2_comb->track(), muon_neg->trackParticle(xAOD::Muon::InnerDetectorTrackParticle), p2_comb->charge(),p2_comb_v);
      if (m_storeZmumuNtuple) {
	if (success_pos && success_neg) { 
	  m_combMuidTree->Fill();
	}
      }
    }

    ATH_MSG_DEBUG(" Execute() completed for Run: " << m_runNumber << "  event: " << m_evtNumber);
  }

  ATH_MSG_DEBUG(" --IDPerfMonZmumu::execute--  event completed -- Run: " << m_runNumber << "  event: " << m_evtNumber);
  return StatusCode::SUCCESS;
}

//==================================================================================
bool IDPerfMonZmumu::FillRecParametersTP(const xAOD::TrackParticle* trackp, const xAOD::TrackParticle* trackp_for_unbias,double charge, const xAOD::Vertex* vertex)
{
  if (!trackp){
    //ATH_MSG_WARNING("Empty Trackparticle. Skipping.");
    return false;
  }

  if (m_doRemoval && !trackp_for_unbias){
    return StatusCode::FAILURE;
  }
    
  double px = 0;
  double py = 0;
  double pz = 0;
  double d0 = 0;
  double z0 = 0;
  double d0res = 0;
  double z0res = 0;
  double PVd0res = 0;
  double PVz0res = 0;
  double PVd0 = 0;
  double PVz0 = 0;

  
  px = trackp->p4().Px();
  py = trackp->p4().Py();
  pz = trackp->p4().Pz();
  d0 = trackp->d0();
  z0 = trackp->z0();
  
  d0res = trackp->definingParametersCovMatrix()(0,0);
  z0res = trackp->definingParametersCovMatrix()(1,1);
  
  if (vertex == nullptr) {
    ATH_MSG_WARNING("in FillRecParametersTP. WARNING: Vertex is NULL");
    return false;
  }
  if (m_trackToVertexIPEstimator == 0) {
    ATH_MSG_WARNING("in FillRecParametersTP. WARNING: m_trackToVertexIPEstimator is NULL");
    return false;
  }

  if (m_doIP && vertex != nullptr && m_trackToVertexIPEstimator != 0){
    const Trk::ImpactParametersAndSigma* iPandSigma(nullptr);
    ATH_MSG_DEBUG("-- using the trackToVertexIPEstimator --");
    
    //Calling the estimate(trackp,newtrackp,vertex,doRemoval)
    //The first track is used to unbias the vertex, the second to get the extrapolation
    iPandSigma = m_trackToVertexIPEstimator->estimate(trackp_for_unbias, trackp, vertex, m_doRemoval);
    ATH_MSG_DEBUG("return from the trackToVertexIPEstimator->estimate()");
    
    if( iPandSigma == NULL ){
      ATH_MSG_WARNING ("FillRecParametersTP::trackToVertexIPEstimator failed !");
      return StatusCode::FAILURE;
    }
    else{
      ATH_MSG_DEBUG("FillRecParametersTP::trackToVertexIPEstimator success !");
      PVd0 = iPandSigma->IPd0;
      PVd0res = iPandSigma->PVsigmad0;
      d0res = iPandSigma->sigmad0;
      PVz0 = iPandSigma->IPz0;
      PVz0res = iPandSigma->PVsigmaz0;
      z0res = iPandSigma->sigmaz0;
    }
  }

  if (charge == 1) {
    m_positive_px = px;
    m_positive_py = py;
    m_positive_pz = pz;
    m_positive_z0 = z0;
    m_positive_z0_err = z0res;
    m_positive_d0 = d0;
    m_positive_d0_err = d0res;
    if(m_doIP){
      m_positive_z0_PV = PVz0;
      m_positive_d0_PV = PVd0;
      m_positive_z0_PVerr = PVz0res;
      m_positive_d0_PVerr = PVd0res;
      ATH_MSG_DEBUG("(Filled charge == 1 ) (reco)-> px : "<< px <<" py: "<<py <<" pz: "<<pz <<" d0: "<<m_positive_d0 << " d0res : "<< d0res << " PVd0res : "<< PVd0res <<" z0: "<< m_positive_z0 << " z0res : " << z0res <<  " PVz0res : "<< PVz0res );
    }

  } else  if (charge == -1) {
    m_negative_px = px;
    m_negative_py = py;
    m_negative_pz = pz;
    m_negative_z0 = z0;
    m_negative_z0_err = z0res;
    m_negative_d0 = d0;
    m_negative_d0_err = d0res;
    if(m_doIP){
      m_negative_z0_PV = PVz0;
      m_negative_d0_PV = PVd0;
      m_negative_z0_PVerr = PVz0res;
      m_negative_d0_PVerr = PVd0res;
      ATH_MSG_DEBUG("(Filled charge == -1 ) (reco)-> px : "<< px <<" py: "<<py <<" pz: "<<pz <<" d0: "<<m_negative_d0 << " d0res : "<< d0res << " PVd0res : "<< PVd0res <<" z0: "<< m_negative_z0 << " z0res : " << z0res <<  " PVz0res : "<< PVz0res );
    }
  }

  return true;
}


//==================================================================================
bool IDPerfMonZmumu::FillRecParameters(const Trk::Track* track, const xAOD::TrackParticle* trackp_for_unbias, double charge,const xAOD::Vertex* vertex)
{

  if (!track){
    //ATH_MSG_WARNING("Empty Track. Skipping.");
    return false;
  }

  if (m_doRemoval && !trackp_for_unbias) {
    return false;
  }

  const Trk::Perigee* trkPerigee = track->perigeeParameters();
  const Trk::Perigee* trk_for_unbiasPerigee = &(trackp_for_unbias->perigeeParameters());
  //  const AmgSymMatrix(5)* covariance = trkPerigee ? trkPerigee->covariance() : NULL;
  //  if (covariance == NULL) {
  //    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No measured perigee parameters assigned to the track" << endreq;
  //  }

  double px = 0;
  double py = 0;
  double pz = 0;
  double d0 = 0;
  double z0 = 0;
  
  double d0_err = 999.;
  double z0_err = 999.;
  
  double PVd0 = 0;
  double PVz0 = 0;
  double PVd0res = 0;
  double PVz0res = 0;
  
  

  if (trkPerigee != NULL){
    //    std::cout << "################## >>> does not fail trkperigee" << std::endl;
    double qOverP   = trkPerigee->parameters()[Trk::qOverP];
    if (qOverP) {
      px = trkPerigee->momentum().x();
      py = trkPerigee->momentum().y();
      pz = trkPerigee->momentum().z();
      d0 = trkPerigee->parameters()[Trk::d0];
      z0 = trkPerigee->parameters()[Trk::z0];
      d0_err = Amg::error(*trkPerigee->covariance(),Trk::d0);
      z0_err = Amg::error(*trkPerigee->covariance(),Trk::z0);
    }
    //    std::cout << " -- trkPerigee -- q: " << (int) charge << "  px: " << px << "  py: " << py << "  pz: " << pz << "  d0: "<< d0 << "   z0: "<< z0 << " d0_err: " << d0_err << std::endl;
    //    delete trkPerigee;
  }

  //const Trk::AtaStraightLine*  atBL =   dynamic_cast<const Trk::AtaStraightLine*>(m_trackToVertexTool->trackAtBeamline( *track ));
  auto atBL = m_trackToVertexTool->trackAtBeamline( *track );

  if (atBL){
    // std::cout << "################## >>> does not fail atBL extrapolation" << std::endl;
    double qOverP   = atBL->parameters()[Trk::qOverP];
    if(qOverP){
      px = atBL->momentum().x();
      py = atBL->momentum().y();
      pz = atBL->momentum().z();
      d0 = atBL->parameters()[Trk::d0];
      z0 = atBL->parameters()[Trk::z0];
      //      z0_err = Amg::error(*trkPerigee->covariance(),Trk::z0);  //->Why not?
      //      d0_err = Amg::error(*trkPerigee->covariance(),Trk::d0);  //->Why not?
    } 
    //    std::cout << " -- atBL -- px: " << px << "  py: " << py << "  pz: " << pz << "  d0: "<< d0 << "   z0: "<< z0 << std::endl;
    delete atBL;
  }
  else {
    ATH_MSG_WARNING("FillRecParameters::Failed extrapolation to the BeamLine");
  }
  
  if(m_doIP && vertex && track->perigeeParameters()){ //I assume that the vertex is the same of the original track
    const Trk::ImpactParametersAndSigma* iPandSigma(NULL);
    iPandSigma = m_trackToVertexIPEstimator->estimate(trk_for_unbiasPerigee,trkPerigee,vertex,m_doRemoval);
    if( iPandSigma==0 ){
      ATH_MSG_WARNING ("FillRecParameters::trackToVertexIPEstimator failed !");
      return false;
    }
    else{
      ATH_MSG_DEBUG("FillRecParameters::trackToVertexIPEstimator success !");
      PVd0 = iPandSigma->IPd0;
      PVd0res = iPandSigma->PVsigmad0;
      //d0res = iPandSigma->sigmad0;  //-> ? 
      PVz0 = iPandSigma->IPz0;
      PVz0res = iPandSigma->PVsigmaz0;
      //z0res = iPandSigma->sigmaz0;  //-> ?
      
      if (vertex) {
	ATH_MSG_INFO(" FillRecParameters::trackToVertexIPEstimator vertex does exist ");
	m_pv_x = vertex->x();
	m_pv_y = vertex->y();
	m_pv_z = vertex->z();
      }
      m_nTrkInVtx = vertex->nTrackParticles();
    }
  }
    
  if (charge == 1) {
    m_positive_px = px;
    m_positive_py = py;
    m_positive_pz = pz;
    m_positive_z0 = z0;
    m_positive_d0 = d0;
    m_positive_d0_err = d0_err;
    m_positive_z0_err = z0_err;
    if(m_doIP){
      m_positive_z0_PV    = PVz0;
      m_positive_d0_PV    = PVd0;
      m_positive_z0_PVerr = PVz0res;
      m_positive_d0_PVerr = PVd0res;
    }
    ATH_MSG_DEBUG("(Filled charge == 1 ) (reco)-> px : "<< px <<" py: "<<py <<" pz: "<<pz <<" d0: "<<d0<<" z0: "<< z0);
  }
  if (charge == -1) {
    m_negative_px = px;
    m_negative_py = py;
    m_negative_pz = pz;
    m_negative_z0 = z0;
    m_negative_d0 = d0;
    m_negative_d0_err = d0_err;
    m_negative_z0_err = z0_err;
    if(m_doIP){
      m_negative_z0_PV = PVz0;
      m_negative_d0_PV = PVd0;
      m_negative_z0_PVerr = PVz0res;
      m_negative_d0_PVerr = PVd0res;
    }
    ATH_MSG_DEBUG("(Filled charge == -1 ) (reco)-> px : "<< px <<" py: "<<py <<" pz: "<<pz <<" d0: "<<d0<<" z0: "<<z0 );
  }

  if (m_doIP && false){
    m_event_mu = 0.;
  }

  return true;
}

//==================================================================================
StatusCode IDPerfMonZmumu::FillTruthParameters(const xAOD::TrackParticle* trackParticle)
{
  if (!trackParticle ){//|| !trackParticle->vertex()){
    ATH_MSG_WARNING("Empty Trackparticle. Skipping.");
    return StatusCode::FAILURE;
  }
  float result(std::numeric_limits<float>::quiet_NaN());
  if (trackParticle->isAvailable<float>("truthMatchProbability")){
    result = trackParticle->auxdata<float>("truthMatchProbability" );
    std::cout <<"DEBUG :: TruthMatchProbablity of trackpartile : > " << result<< std::endl;
  }

  const xAOD::TruthParticle* particle = getTruthParticle( *trackParticle );
  double charge = 0;
  if (!particle) {
    ATH_MSG_DEBUG(" Failure while retrieving the truth particle");
    return StatusCode::FAILURE;
  }
  if(particle->pdgId() == 13) charge = -1.;
  else if(particle->pdgId() == -13) charge = 1.;
  if( particle->isNeutral() ) return StatusCode::FAILURE;
  const Amg::Vector3D momentum(particle->px(), particle->py(), particle->pz());
  
  const xAOD::TruthVertex * ptruthVertex(0);
  ptruthVertex=particle->prodVtx();
  if (!ptruthVertex){
    ATH_MSG_DEBUG("A production vertex pointer was retrieved, but it is NULL");
    return StatusCode::FAILURE;
  }

  const auto xPos=ptruthVertex->x();
  const auto yPos=ptruthVertex->y();
  const auto z_truth=ptruthVertex->z();

  const Amg::Vector3D position(xPos, yPos, z_truth);
  //  const float prodR_truth = std::sqrt(xPos*xPos+ yPos*yPos);
  //delete ptruthVertex;ptruthVertex=0;
  const Trk::CurvilinearParameters cParameters(position, momentum, charge);

  Trk::PerigeeSurface persf( m_beamSpotSvc->beamPos() );

  const Trk::TrackParameters* tP = m_extrapolator->extrapolate(cParameters,persf, Trk::anyDirection, false);


  double px = 0;
  double py = 0;
  double pz = 0;
  double d0 = 0;
  double d0res = 0;
  double PVd0res = 0;
  double z0 = 0;
  double z0res = 0;
  double PVz0res = 0;


  double d0recoPos =  m_positive_d0;
  double z0recoPos =  m_positive_z0;
  double d0recoNeg =  m_negative_d0;
  double z0recoNeg =  m_negative_z0;

  ATH_MSG_DEBUG("reco IPs (pos): > d0 : "<<d0recoPos << " z0: " << z0recoPos << " trackp z0 : " << trackParticle->z0() << " trackp d0 : " << trackParticle->d0());
  ATH_MSG_DEBUG("reco IPs (neg): > d0 : "<<d0recoNeg << " z0: " << z0recoNeg << " trackp z0 : " << trackParticle->z0() <<" trackp d0 : " << trackParticle->d0() );

  if (tP){
    double qOverP_truth = tP->parameters()[Trk::qOverP];
    if( qOverP_truth ){

      px = tP->momentum().x();
      py = tP->momentum().y();
      pz = tP->momentum().z();
      d0 = tP->parameters()[Trk::d0];
      z0 = tP->parameters()[Trk::z0];

      ATH_MSG_DEBUG("cand perig HEP particle (truth) px : "<< tP->momentum().x());
      ATH_MSG_DEBUG("cand perig HEP particle (truth) py : "<< tP->momentum().y());
      ATH_MSG_DEBUG("cand perig HEP particle (truth) pz : "<< tP->momentum().z());
      ATH_MSG_DEBUG("cand perig HEP particle (truth) d0 : "<< tP->parameters()[Trk::d0]);
      ATH_MSG_DEBUG("cand perig HEP particle (truth) z0 : "<< tP->parameters()[Trk::z0]);
      delete tP;tP=0;
    }
  }


  if (charge == 1) {
    m_positive_px = px;
    m_positive_py = py;
    m_positive_pz = pz;
    m_positive_z0 = z0recoPos -z0;
    //    m_positive_z0 = z0;

    m_positive_z0_err = z0res;
    m_positive_z0_PVerr = PVz0res;
    m_positive_d0 = d0recoPos -d0;
    //    m_positive_d0 = d0;

    m_positive_d0_err = d0res;
    m_positive_d0_PVerr = PVd0res;
    //    m_px = px;
    //    m_py = py;
    //    m_pz = pz;
    //    m_z0 = z0;
    //    m_d0 = d0;
    //    m_d0_rec_truth = d0recoPos -d0;
    //    m_z0_rec_truth = z0recoPos -z0;
    ATH_MSG_DEBUG("(Filled charge == 1 ) (truth)-> px : "<< m_positive_px <<" py: "<<m_positive_py <<" pz: "<<m_positive_pz <<" d0: "<<m_positive_d0 << " z0: "<< m_positive_z0);

  } else  if (charge == -1) {
    m_negative_px = px;
    m_negative_py = py;
    m_negative_pz = pz;
    m_negative_z0 = z0recoNeg-z0;
    //    m_negative_z0 = z0;

    m_negative_z0_err = z0res;
    m_negative_z0_PVerr = PVz0res;
    m_negative_d0 = d0recoNeg-d0;
    //    m_negative_d0 = d0;

    m_negative_d0_err = d0res;
    m_negative_d0_PVerr = PVd0res;

    //    m_px = px;
    //    m_py = py;
    //    m_pz = pz;
    //    m_z0 = z0;
    //    m_d0 = d0;
    //    m_d0_rec_truth = d0recoNeg -d0;
    //    m_z0_rec_truth = z0recoNeg -z0;
    ATH_MSG_DEBUG("(Filled charge == -1 ) (truth)-> px : "<< m_negative_px <<" py: "<<m_negative_py <<" pz: "<<m_negative_pz <<" d0: "<<m_negative_d0 << " z0: "<< m_negative_z0);
  }
  return StatusCode::SUCCESS;
}


//==================================================================================
const xAOD::TruthParticle* IDPerfMonZmumu::getTruthParticle( const xAOD::IParticle& p ) {

  /// A convenience type declaration
  typedef ElementLink< xAOD::TruthParticleContainer > Link_t;

  /// A static accessor for the information
  static SG::AuxElement::ConstAccessor< Link_t > acc( "truthParticleLink" );

  // Check if such a link exists on the object:
  if( ! acc.isAvailable( p ) ) {
    return 0;
  }

  // Get the link:
  const Link_t& link = acc( p );

  // Check if the link is valid:
  if( ! link.isValid() ) {
    return 0;
  }

  // Everything has passed, let's return the pointer:
  return *link;
}

//==================================================================================
StatusCode IDPerfMonZmumu::finalize()
{
  return StatusCode::SUCCESS;
}

//==================================================================================
StatusCode IDPerfMonZmumu::CheckTriggerStatusAndPrescale ()
{
  m_triggerPrescale = 1; // default value

  // check trigger status
  if(m_triggerDecision.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_triggerDecision << " turn it off");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG("retrieved tool: " << m_triggerDecision );
  }

  float thisEventTriggerPrescale = 999999.9;
  float thisHLTTriggerPrescale = 999999.9;
  bool thisHLTIsPassed = false;

  // HLT triggers
  const std::vector<std::string> myHLTtriglist = m_triggerDecision->getChainGroup("HLT_mu.*, HLT_2mu.*")->getListOfTriggers();
  for (int i=0; i < (int) myHLTtriglist.size(); i++) {
    if ( m_triggerDecision->isPassed(myHLTtriglist.at(i)) ) {
      ATH_MSG_DEBUG("HLT trigger = " << i << " out of " << myHLTtriglist.size() << " --> " << myHLTtriglist.at(i) <<  "  isPassed? " << m_triggerDecision->isPassed(myHLTtriglist.at(i)) << "  prescale: " << m_triggerDecision->getChainGroup(myHLTtriglist.at(i))->getPrescale() );
    }
    if ( m_triggerDecision->isPassed(myHLTtriglist.at(i)) ) { // trigger is passed
      thisHLTIsPassed = true;
      float thisprescale = m_triggerDecision->getChainGroup(myHLTtriglist.at(i))->getPrescale();
      if (thisprescale < thisHLTTriggerPrescale) {
	thisHLTTriggerPrescale = thisprescale;
      }
    }
  }
  if (thisHLTTriggerPrescale < 1) thisHLTTriggerPrescale = 1;
  ATH_MSG_DEBUG("Event HLT trigger prescale = " << thisHLTTriggerPrescale);

  //if (thisL1IsPassed && thisHLTIsPassed) {
  if (thisHLTIsPassed) {
    ATH_MSG_DEBUG("Trigger passed -> accept event");
  }
  else{
    ATH_MSG_DEBUG("Trigger Failed -> reject event");
    return StatusCode::FAILURE;
  }

  // event prescale
  thisEventTriggerPrescale = thisHLTTriggerPrescale;
  ATH_MSG_INFO("Event trigger prescale = " << thisEventTriggerPrescale);

  m_triggerPrescale = thisEventTriggerPrescale;

  return StatusCode::SUCCESS;
}


//==================================================================================
const xAOD::Vertex* IDPerfMonZmumu::GetDiMuonVertex(const xAOD::TrackParticle* muon1, const xAOD::TrackParticle* muon2) {
  
  const xAOD::VertexContainer* vxContainer(0);
  const xAOD::Vertex* myVtx(0);
  vxContainer = PerfMonServices::getContainer<xAOD::VertexContainer> (PerfMonServices::VTX_COLLECTION);
  if (!vxContainer) { 
    return myVtx;
  }
  
  for (int ivtx=0; ivtx< (int) vxContainer->size();ivtx++)  {
    myVtx = (*vxContainer)[ivtx];
    if ((myVtx->vertexType() == xAOD::VxType::PriVtx)) {
      if (m_LooseT2VAssociationTool->isCompatible(*muon1,*myVtx) && (m_LooseT2VAssociationTool->isCompatible(*muon2,*myVtx)))
	return myVtx;
    }
  }
  return myVtx;
}


//==================================================================================
void IDPerfMonZmumu::Clear4MuNtupleVariables() 
{
  m_positive_px = 0.;
  m_positive_py = 0.;
  m_positive_pz = 0.;
  m_positive_d0 = 0.;
  m_positive_z0 = 0.;
  m_positive_d0_err = 0.;
  m_positive_z0_err = 0.;
  m_positive_1_vtx = 0;
	
  m_negative_px = 0.;
  m_negative_py = 0.;
  m_negative_pz = 0.;
  m_negative_d0 = 0.;
  m_negative_z0 = 0.;
  m_negative_d0_err = 0.;
  m_negative_z0_err = 0.;
  m_negative_1_vtx = 0;
	
  m_positive_2_px = 0.;
  m_positive_2_py = 0.;
  m_positive_2_pz = 0.;
  m_positive_2_d0 = 0.;
  m_positive_2_z0 = 0.;
  m_positive_2_d0_err = 0.;
  m_positive_2_z0_err = 0.;
  m_positive_2_vtx = 0;
	
  m_negative_2_px = 0.;
  m_negative_2_py = 0.;
  m_negative_2_pz = 0.;
  m_negative_2_d0 = 0.;
  m_negative_2_z0 = 0.;
  m_negative_2_d0_err = 0.;
  m_negative_2_z0_err = 0.;
  m_negative_2_vtx = 0;

  // electrons
  m_el_negative1_px = 0.;
  m_el_negative1_py = 0.;
  m_el_negative1_pz = 0.;
  m_el_negative1_d0 = 0.;
  m_el_negative1_z0 = 0.;
  m_el_negative1_d0_err = 0.;
  m_el_negative1_z0_err = 0.;
  m_el_negative1_vtx = 0;

  m_el_negative2_px = 0.;
  m_el_negative2_py = 0.;
  m_el_negative2_pz = 0.;
  m_el_negative2_d0 = 0.;
  m_el_negative2_z0 = 0.;
  m_el_negative2_d0_err = 0.;
  m_el_negative2_z0_err = 0.;
  m_el_negative2_vtx = 0;

  m_el_positive1_px = 0.;
  m_el_positive1_py = 0.;
  m_el_positive1_pz = 0.;
  m_el_positive1_d0 = 0.;
  m_el_positive1_z0 = 0.;
  m_el_positive1_d0_err = 0.;
  m_el_positive1_z0_err = 0.;
  m_el_positive1_vtx = 0;
  
  m_el_positive2_px = 0.;
  m_el_positive2_py = 0.;
  m_el_positive2_pz = 0.;
  m_el_positive2_d0 = 0.;
  m_el_positive2_z0 = 0.;
  m_el_positive2_d0_err = 0.;
  m_el_positive2_z0_err = 0.;
  m_el_positive2_vtx = 0;
  
  // other quantities
  m_nVertex =        0;
  m_pv_x = 0; m_pv_y = 0; m_pv_z = 0;
  m_4mu_minv = 0.;

  return;
}
//==================================================================================
StatusCode IDPerfMonZmumu::RunFourLeptonAnalysis()
{
  StatusCode thisStatus = StatusCode::SUCCESS;

  if (m_doFourMuAnalysis) {
    ATH_MSG_DEBUG ("** RunFourLeptonAnalysis ** START **    run: " << m_runNumber << "    event: " << m_evtNumber << "    lumiblock:" << m_lumi_block);  
    if (m_doDebug) {
      std::cout << " ==================================== " << std::endl;
      std::cout << " ** RunFourLeptonAnalysis ** START **    run: " << m_runNumber << "    event: " << m_evtNumber << "    lumiblock:" << m_lumi_block << std::endl;
      std::cout << " ==================================== " << std::endl;
    }
    // Four lepton event
    m_4mu.setContainer(PerfMonServices::MUON_COLLECTION);
    m_4mu.doIsoSelection     (m_doIsoSelection);
    m_4mu.doIPSelection      (m_doIPSelection);
    m_4mu.doMCPSelection     (m_doMCPSelection);
    m_4mu.SetMassWindowLow   (m_MassWindowLow);
    m_4mu.SetMassWindowHigh  (m_MassWindowHigh);
    m_4mu.SetLeadingMuonPtCut(m_LeadingMuonPtCut);
    m_4mu.SetSecondMuonPtCut (m_SecondMuonPtCut);
    m_4mu.SetOpeningAngleCut (m_OpeningAngleCut);
    m_4mu.SetZ0GapCut        (m_Z0GapCut);
    m_4mu.setDebugMode       (m_doDebug);

    // This is a report of the number of true electrons in the event
    if (false) {
      const xAOD::TruthParticleContainer* electronTruth = evtStore()->retrieve< const xAOD::TruthParticleContainer >( "egammaTruthParticles" );
      if( electronTruth) {
	ATH_MSG_DEBUG ( "retrieving electron Truth container with key: \"egammaTruthParticles\" SUCCESS ");
	// Check that the auxiliary store association was made successfully:
	if( electronTruth->hasStore() ) {
	  ATH_MSG_DEBUG ( "Size of Truth gamma-electrons: " << electronTruth->size() );
	  auto tr_itr = electronTruth->begin();
	  auto tr_end = electronTruth->end();
	  int electronCount = 0;
	  for( int i = 0; tr_itr != tr_end; ++tr_itr, ++i ) {
	    ATH_MSG_DEBUG( "Truth : Investigating truth electron #" << i << "    pdgID= " << ( *tr_itr )->pdgId());
	    if (fabs(( *tr_itr )->pdgId()) == 11) {
	      if (( *tr_itr )->pt() > 10000 && fabs(( *tr_itr )->eta()) < 2.47) {
		ATH_MSG_INFO( "        central electron found --> pt: " << ( *tr_itr )->charge()*( *tr_itr )->pt() << " eta  " <<( *tr_itr )->eta()  );
		electronCount++;
	      }
	    }
	  }
	  ATH_MSG_INFO( "#central electrons: " << electronCount );
	}
	else {
	  ATH_MSG_WARNING ( "electronTruth container has no Store !!! FAILURE");
	}
      }
      else {
	ATH_MSG_WARNING ( "retrieve electron Truth container with key: \"egammaTruthParticles\" FAILURE");
      }
    }

    if (m_doDebug) std::cout << " ** RunFourLeptonAnalysis ** calling m_4mu.Reco() .... " << std::endl;  
    bool statusOf4LeptReco = m_4mu.Reco();

    // print message 
    if(statusOf4LeptReco) {
      thisStatus = StatusCode::SUCCESS;
      ATH_MSG_INFO ("4-lepton reconstruction SUCCESS. # accepted events " << m_4mu.getAcceptedEvents() << "  Invariant mass = " << m_4mu.GetInvMass() << " GeV ");
      if (m_doDebug) std::cout << " ** RunFourLeptonAnalysis ** 4-lepton reconstruction SUCCESS. # accepted events " << m_4mu.getAcceptedEvents() << "  Invariant mass = " << m_4mu.GetInvMass() << " GeV " << std::endl;
    }
    else {
      thisStatus = StatusCode::FAILURE;
      ATH_MSG_DEBUG ("4-lepton reconstruction FAILURE.  m_4mu.Reco() returned FALSE --> event failed selection");
      if (m_doDebug) std::cout << " ** RunFourLeptonAnalysis ** 4-lepton reconstruction FAILURE.  m_4mu.Reco() returned FALSE --> event failed selection" << std::endl;
    }

    if(statusOf4LeptReco) {
      if ( m_4mu.EventPassed() ) {

	this->Clear4MuNtupleVariables(); 

	const xAOD::TrackParticle* muon1_pos = m_4mu.getIDTrack(m_4mu.getPosMuon(1));
	const xAOD::TrackParticle* muon2_pos = m_4mu.getIDTrack(m_4mu.getPosMuon(2));
	const xAOD::TrackParticle* muon1_neg = m_4mu.getIDTrack(m_4mu.getNegMuon(1));
	const xAOD::TrackParticle* muon2_neg = m_4mu.getIDTrack(m_4mu.getNegMuon(2));

	// muons
	if (muon1_pos) {
	  m_positive_px = muon1_pos->p4().Px();
	  m_positive_py = muon1_pos->p4().Py();
	  m_positive_pz = muon1_pos->p4().Pz();
	  m_positive_d0 = muon1_pos->d0();
	  m_positive_z0 = muon1_pos->z0();
	  m_positive_d0_err = muon1_pos->definingParametersCovMatrix()(0,0);
	  m_positive_z0_err = muon1_pos->definingParametersCovMatrix()(1,1);
	}
	if (muon1_neg) {
	  m_negative_px = muon1_neg->p4().Px();
	  m_negative_py = muon1_neg->p4().Py();
	  m_negative_pz = muon1_neg->p4().Pz();
	  m_negative_d0 = muon1_neg->d0();
	  m_negative_z0 = muon1_neg->z0();
	  m_negative_d0_err = muon1_neg->definingParametersCovMatrix()(0,0);
	  m_negative_z0_err = muon1_neg->definingParametersCovMatrix()(1,1);
	}
	if (muon2_pos) {
	  m_positive_2_px = muon2_pos->p4().Px();
	  m_positive_2_py = muon2_pos->p4().Py();
	  m_positive_2_pz = muon2_pos->p4().Pz();
	  m_positive_2_d0 = muon2_pos->d0();
	  m_positive_2_z0 = muon2_pos->z0();
	  m_positive_2_d0_err = muon2_pos->definingParametersCovMatrix()(0,0);
	  m_positive_2_z0_err = muon2_pos->definingParametersCovMatrix()(1,1);
	}
	if (muon2_neg) {
	  m_negative_2_px = muon2_neg->p4().Px();
	  m_negative_2_py = muon2_neg->p4().Py();
	  m_negative_2_pz = muon2_neg->p4().Pz();
	  m_negative_2_d0 = muon2_neg->d0();
	  m_negative_2_z0 = muon2_neg->z0();
	  m_negative_2_d0_err = muon2_neg->definingParametersCovMatrix()(0,0);
	  m_negative_2_z0_err = muon2_neg->definingParametersCovMatrix()(1,1);
	}
	
	// electrons
	const xAOD::TrackParticle* elec1_neg = m_4mu.getELTrack(0);
	const xAOD::TrackParticle* elec2_neg = m_4mu.getELTrack(1);
	const xAOD::TrackParticle* elec1_pos = m_4mu.getELTrack(2);
	const xAOD::TrackParticle* elec2_pos = m_4mu.getELTrack(3);
	if (elec1_neg) {
	  m_el_negative1_px = elec1_neg->p4().Px();
	  m_el_negative1_py = elec1_neg->p4().Py();
	  m_el_negative1_pz = elec1_neg->p4().Pz();
	  m_el_negative1_d0 = elec1_neg->d0();
	  m_el_negative1_z0 = elec1_neg->z0();
	  m_el_negative1_d0_err = elec1_neg->definingParametersCovMatrix()(0,0);
	  m_el_negative1_z0_err = elec1_neg->definingParametersCovMatrix()(1,1);
	}
	if (elec2_neg) {
	  m_el_negative2_px = elec2_neg->p4().Px();
	  m_el_negative2_py = elec2_neg->p4().Py();
	  m_el_negative2_pz = elec2_neg->p4().Pz();
	  m_el_negative2_d0 = elec2_neg->d0();
	  m_el_negative2_z0 = elec2_neg->z0();
	  m_el_negative2_d0_err = elec2_neg->definingParametersCovMatrix()(0,0);
	  m_el_negative2_z0_err = elec2_neg->definingParametersCovMatrix()(1,1);
	}
	if (elec1_pos) {
	  m_el_positive1_px = elec1_pos->p4().Px();
	  m_el_positive1_py = elec1_pos->p4().Py();
	  m_el_positive1_pz = elec1_pos->p4().Pz();
	  m_el_positive1_d0 = elec1_pos->d0();
	  m_el_positive1_z0 = elec1_pos->z0();
	  m_el_positive1_d0_err = elec1_pos->definingParametersCovMatrix()(0,0);
	  m_el_positive1_z0_err = elec1_pos->definingParametersCovMatrix()(1,1);
	}
	if (elec2_pos) {
	  m_el_positive2_px = elec2_pos->p4().Px();
	  m_el_positive2_py = elec2_pos->p4().Py();
	  m_el_positive2_pz = elec2_pos->p4().Pz();
	  m_el_positive2_d0 = elec2_pos->d0();
	  m_el_positive2_z0 = elec2_pos->z0();
	  m_el_positive2_d0_err = elec2_pos->definingParametersCovMatrix()(0,0);
	  m_el_positive2_z0_err = elec2_pos->definingParametersCovMatrix()(1,1);
	}
	
	// other quantities
	m_nVertex =        m_4mu.GetNVertex ();
	m_negative_1_vtx = m_4mu.GetVertexMuNeg1();
	m_negative_2_vtx = m_4mu.GetVertexMuNeg2();
	m_positive_1_vtx = m_4mu.GetVertexMuPos1();
	m_positive_2_vtx = m_4mu.GetVertexMuPos2();

	m_el_negative1_vtx = m_4mu.GetVertexElec(0);
	m_el_negative2_vtx = m_4mu.GetVertexElec(1);
	m_el_positive1_vtx = m_4mu.GetVertexElec(2);
	m_el_positive2_vtx = m_4mu.GetVertexElec(3);

	m_pv_x = 0; m_pv_y = 0; m_pv_z = 0;

	if (muon1_neg) {
	  if (muon1_neg->vertex() != nullptr) { // check vertex exists
	    m_pv_x = muon1_neg->vertex()->x();
	    m_pv_y = muon1_neg->vertex()->y();
	    m_pv_z = muon1_neg->vertex()->z();
	  }
	}
	else if (elec1_neg) {
	  if (elec1_neg->vertex() != nullptr) { // check vertex exists
	    m_pv_x = elec1_neg->vertex()->x();
	    m_pv_y = elec1_neg->vertex()->y();
	    m_pv_z = elec1_neg->vertex()->z();
	  }
	}

	m_4mu_minv = m_4mu.GetInvMass();
	
	// Obtain MET 
	std::string m_metName = "MET_Reference_AntiKt4LCTopo";
	std::string m_metRefFinalName = "FinalClus";
	const xAOD::MissingETContainer* final_met = 0;

	m_met = -1; // default value
	m_metphi = -1;

	if (!evtStore()->contains<xAOD::MissingETContainer>(m_metName)) {
	  ATH_MSG_WARNING ( "No Collection with name " << m_metName << " found in StoreGate");
	  // return StatusCode::SUCCESS;
	}
	else {
	  StatusCode sc = evtStore()->retrieve(final_met,m_metName);
	  if (sc.isFailure()) {
	    ATH_MSG_DEBUG ( "Could not retrieve Collection " << m_metName << " from StoreGate");
	    // return StatusCode::SUCCESS;
	  }
	}
	const xAOD::MissingET *met = nullptr;
	if (final_met) met = (*final_met)[m_metRefFinalName];
	if (met) { // load MET values
	  m_met = met->met();
	  m_metphi = met->phi();
	}
	ATH_MSG_DEBUG (" Zmumu event with MET = " << m_met);
	ATH_MSG_INFO (" -- IDPerfMonZmumu::execute -- Accepted event " << m_4mu.getAcceptedEvents() << " with m_4mu.GetInvMass= " << m_4mu_minv);
	m_FourMuTree->Fill();
      }
    } // succesful 4mu reco

  } // end of fourMuon Analysis

  return thisStatus;
}
