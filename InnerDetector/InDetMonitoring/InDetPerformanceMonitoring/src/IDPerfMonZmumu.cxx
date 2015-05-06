/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//==================================================================================

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

//#include "TrkFitterInterfaces/ITrackFitter.h"
#include "egammaInterfaces/IegammaTrkRefitterTool.h"


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


//==================================================================================
// Public Methods
//==================================================================================
IDPerfMonZmumu::IDPerfMonZmumu(const std::string& name,
  ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_isMC(false),
  m_TrackRefitter1(""),
  m_TrackRefitter2(""),
  m_trackToVertexTool("Reco::TrackToVertex"),
  m_validationMode(true),
  m_defaultTreeName("DefaultParams"),
  m_refit1TreeName("Refit1Params"),
  m_refit2TreeName("Refit2Params"),
  m_truthTreeName("TruthParams"),
  //  m_meStacoTreeName("MEStacoParams"),  //not existent in xAOD anymore
  m_combStacoTreeName("CombStacoParams"),
  m_combMuidTreeName("CombMuidParams"),
  m_ValidationTreeDescription("Small Tree for Zmumu fits"),
  m_defaultTreeFolder("/ZmumuValidation/default"),
  m_refit1TreeFolder("/ZmumuValidation/refit1"),
  m_refit2TreeFolder("/ZmumuValidation/refit2"),
  m_truthTreeFolder("/ZmumuValidation/truth"),
  //  m_meStacoTreeFolder("/ZmumuValidation/mestaco"),
  m_combStacoTreeFolder("/ZmumuValidation/combstaco"),
  m_defaultTree(0),
  m_refit1Tree(0),
  m_refit2Tree(0),
  m_truthTree(0),
  //  m_meStacoTree(0),   // not existent in xAOD anymore
  m_combStacoTree(0),
  m_combMuidTree(0)

{
  // Properties that are set from the python scripts.


  declareProperty("triggerChainName", m_sTriggerChainName               );
  declareProperty("OutputTracksName", m_outputTracksName = "ZmumuTracks");
  declareProperty("doIsoSelection",   m_doIsoSelection = true           );

  declareProperty("ReFitterTool1",    m_TrackRefitter1, "ToolHandle for track fitter implementation");
  declareProperty("ReFitterTool2",    m_TrackRefitter2, "ToolHandle for track fitter implementation");

  declareProperty ("TrackToVertexTool", m_trackToVertexTool);

  declareProperty("ValidationMode",   m_validationMode);

  declareProperty("TrackTruthName",     m_truthName="TrackTruthCollection");
  //  declareProperty("TrackParticleName",  m_trackParticleName="InDetTrackParticles");
  declareProperty("TrackParticleName",  m_trackParticleName="TruthParticles");
  declareProperty("xAODTruthLinkVector",m_truthLinkVecName="xAODTruthLinks");

  declareProperty("isMC",   m_isMC = false);
}



IDPerfMonZmumu::~IDPerfMonZmumu()
{}



StatusCode IDPerfMonZmumu::initialize()
{

  // Setup the services
  ISvcLocator* pxServiceLocator = serviceLocator();
  if ( pxServiceLocator != NULL ) {
      StatusCode xSC = PerfMonServices::InitialiseServices( pxServiceLocator );
      if ( xSC == !StatusCode::SUCCESS )
	{
          ATH_MSG_FATAL("Problem Initializing PerfMonServices");
	  //return PARENT::initialize();
	}
  }

  // Retrieve fitter
  if (m_TrackRefitter1.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter1 );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_TrackRefitter1 );
  }

  // Retrieve the second fitter
  if (m_TrackRefitter2.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_TrackRefitter2 );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_TrackRefitter2 );
  }


  if (m_trackToVertexTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_trackToVertexTool );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_trackToVertexTool );
  }



  if( m_defaultTree == 0){

    m_defaultTree = new TTree(m_defaultTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_defaultTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_defaultTree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_defaultTree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

    m_defaultTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_defaultTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_defaultTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_defaultTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_defaultTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");

    m_defaultTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_defaultTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_defaultTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_defaultTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_defaultTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
  }

  if( m_refit1Tree == 0){

    m_refit1Tree = new TTree(m_refit1TreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_refit1Tree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_refit1Tree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_refit1Tree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

    m_refit1Tree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_refit1Tree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_refit1Tree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_refit1Tree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_refit1Tree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");

    m_refit1Tree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_refit1Tree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_refit1Tree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_refit1Tree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_refit1Tree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
  }

  if( m_refit2Tree == 0){

    m_refit2Tree = new TTree(m_refit2TreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_refit2Tree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_refit2Tree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_refit2Tree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

    m_refit2Tree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_refit2Tree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_refit2Tree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_refit2Tree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_refit2Tree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");

    m_refit2Tree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_refit2Tree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_refit2Tree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_refit2Tree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_refit2Tree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
   }

  //  if( m_meStacoTree == 0){

  //m_meStacoTree = new TTree(m_meStacoTreeName.c_str(), m_ValidationTreeDescription.c_str());

  //m_meStacoTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
  //m_meStacoTree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
  //m_meStacoTree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

  //m_meStacoTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
  //m_meStacoTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
  //m_meStacoTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
  //m_meStacoTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
  //m_meStacoTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");

  //m_meStacoTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
  //m_meStacoTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
  //m_meStacoTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
  //m_meStacoTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
  //m_meStacoTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
  //}

  if( m_combStacoTree == 0){

    m_combStacoTree = new TTree(m_combStacoTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_combStacoTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_combStacoTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_combStacoTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");

    m_combStacoTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_combStacoTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_combStacoTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_combStacoTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_combStacoTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");

    m_combStacoTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_combStacoTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_combStacoTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_combStacoTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_combStacoTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
  }


  if( m_combMuidTree == 0){

    m_combMuidTree = new TTree(m_combMuidTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_combMuidTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_combMuidTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_combMuidTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");

    m_combMuidTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_combMuidTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_combMuidTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_combMuidTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_combMuidTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");

    m_combMuidTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_combMuidTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_combMuidTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_combMuidTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_combMuidTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
  }

  if( m_isMC && m_truthTree == 0){

    m_truthTree = new TTree(m_truthTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_truthTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_truthTree->Branch("eventNumber"      ,  &m_evtNumber,  "eventNumber/I");
    m_truthTree->Branch("lumi_block"      ,  &m_lumi_block,  "lumi_block/I");

    m_truthTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_truthTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_truthTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_truthTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_truthTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");

    m_truthTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_truthTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_truthTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_truthTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_truthTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
  }

    // now register the Trees
  ITHistSvc* tHistSvc = 0;
  if (service("THistSvc",tHistSvc).isFailure()){
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !");
    m_validationMode = false;
  }

  if ((tHistSvc->regTree(m_defaultTreeFolder, m_defaultTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_defaultTree; m_defaultTree = 0;
    m_validationMode = false;
  }

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

  //  if ((tHistSvc->regTree(m_meStacoTreeFolder, m_meStacoTree)).isFailure() ) {
  //ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
  //delete m_meStacoTree; m_meStacoTree = 0;
  //m_validationMode = false;
  //}

  if ((tHistSvc->regTree(m_combStacoTreeFolder, m_combStacoTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_combStacoTree; m_combStacoTree = 0;
    m_validationMode = false;
  }

  if ((tHistSvc->regTree(m_combMuidTreeFolder, m_combMuidTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_combMuidTree; m_combMuidTree = 0;
    m_validationMode = false;
  }

  if (m_isMC) {
  if ((tHistSvc->regTree(m_truthTreeFolder, m_truthTree)).isFailure() ) {
    ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !");
    delete m_truthTree; m_truthTree = 0;
    m_validationMode = false;
  }
  }

  return StatusCode::SUCCESS;
}


void IDPerfMonZmumu::RegisterHistograms()
{
  return;
}


StatusCode IDPerfMonZmumu::execute()
{

  ATH_MSG_DEBUG("Retrieving event info.");
  const EventInfo * eventInfo;
  if (evtStore()->retrieve(eventInfo).isFailure())
    ATH_MSG_ERROR("Could not retrieve event info.");
  else
  {
    m_runNumber = eventInfo->event_ID()->run_number();
    m_evtNumber = eventInfo->event_ID()->event_number();
    m_lumi_block = eventInfo->event_ID()->lumi_block();
  }

  //Fill Staco muon parameters only
  m_xZmm.setContainer(PerfMonServices::MUON_COLLECTION);
  m_xZmm.Reco();
  if ( m_xZmm.EventPassed() ) {
    //fill Combined Staco parameters
    const xAOD::Muon* muon_pos = m_xZmm.getCombMuon(m_xZmm.getPosMuon(ZmumuEvent::CB));
    const xAOD::Muon* muon_neg = m_xZmm.getCombMuon(m_xZmm.getNegMuon(ZmumuEvent::CB));
    if (!muon_pos || !muon_neg) {
      ATH_MSG_WARNING("CB Staco Muons missing!");
    } else {
      //       FillRecParameters(muon_pos->combinedMuonTrackParticle()->originalTrack(), muon_pos->combinedMuonTrackParticle()->charge());
      //       FillRecParameters(muon_neg->combinedMuonTrackParticle()->originalTrack(), muon_neg->combinedMuonTrackParticle()->charge());
      FillRecParameters(muon_pos->trackParticle(xAOD::Muon::CombinedTrackParticle)->track(), muon_pos->trackParticle(xAOD::Muon::CombinedTrackParticle)->charge());
      FillRecParameters(muon_neg->trackParticle(xAOD::Muon::CombinedTrackParticle)->track(), muon_neg->trackParticle(xAOD::Muon::CombinedTrackParticle)->charge());
      m_combStacoTree->Fill();
    }

  }


  //Now use MUID collection to find ID tracks
  //m_xZmm.setContainer(PerfMonServices::MUID_COLLECTION);
  //m_xZmm.Reco();

  if ( !m_xZmm.EventPassed() ) {
    //failed cuts, continue to next event
    return StatusCode::SUCCESS;
  }
  //const std::string region = m_xZmm.getRegion();
  const xAOD::TrackParticle* p1 = m_xZmm.getIDTrack(m_xZmm.getPosMuon(ZmumuEvent::ID));
  const xAOD::TrackParticle* p2 = m_xZmm.getIDTrack(m_xZmm.getNegMuon(ZmumuEvent::ID));
  TrackCollection* muonTrks  = new TrackCollection(SG::OWN_ELEMENTS);
  TrackCollection* muonTrksRefit1  = new TrackCollection(SG::OWN_ELEMENTS);
  TrackCollection* muonTrksRefit2  = new TrackCollection(SG::OWN_ELEMENTS);


  if (!p1 || !p2) {
     ATH_MSG_WARNING("TrackParticles missing!  Skipping Event");
     return StatusCode::SUCCESS;
  }

  //swap muons for consistent charge definition
  //if (p1->charge() == -1 && p2->charge() == 1) {
  //  const Rec::TrackParticle* tmp = p1;
  //  p1 = p2;
  //  p2 = tmp;
  //}

  Trk::Track* defaultMuonTrk1 = 0;
  Trk::Track* defaultMuonTrk2 = 0;
  Trk::Track* refit1MuonTrk1 = 0;
  Trk::Track* refit2MuonTrk1 = 0;
  Trk::Track* refit1MuonTrk2 = 0;
  Trk::Track* refit2MuonTrk2 = 0;

  const xAOD::Electron* egam = 0;

  StatusCode fitStatus;
  //save default and refit track parameters
  if( p1->track() ) {
    defaultMuonTrk1 = new Trk::Track(*p1->track());

    fitStatus = m_TrackRefitter1->refitTrack( p1->track(),egam );
    if (fitStatus == StatusCode::SUCCESS) {
      refit1MuonTrk1 = m_TrackRefitter1->refittedTrack();
			muonTrksRefit1->push_back(refit1MuonTrk1);
    } else {
       ATH_MSG_WARNING("Track Refit1 Failed. Skipping Event");
       return StatusCode::SUCCESS;
    }

    fitStatus = m_TrackRefitter2->refitTrack( p1->track(),egam );
    if (fitStatus == StatusCode::SUCCESS) {
      refit2MuonTrk1 = m_TrackRefitter2->refittedTrack();
			muonTrksRefit2->push_back(refit2MuonTrk1);
    } else {
       ATH_MSG_WARNING("Track Refit2 Failed. Skipping Event");
       return StatusCode::SUCCESS;
    }

  }

  if( p2->track() ) {
    defaultMuonTrk2 = new Trk::Track(*p2->track());

    fitStatus = m_TrackRefitter1->refitTrack( p2->track(),egam );
    if (fitStatus == StatusCode::SUCCESS) {
      refit1MuonTrk2 = m_TrackRefitter1->refittedTrack();
      muonTrksRefit1->push_back(refit1MuonTrk2);
		} else {
       ATH_MSG_WARNING("Track Refit1 Failed. Skipping Event");
       return StatusCode::SUCCESS;
    }

    fitStatus = m_TrackRefitter2->refitTrack( p2->track(),egam );
    if (fitStatus == StatusCode::SUCCESS) {
      refit2MuonTrk2 = m_TrackRefitter2->refittedTrack();
      muonTrksRefit2->push_back(refit2MuonTrk2);
		} else {
       ATH_MSG_WARNING("Track Refit2 Failed. Skipping Event");
       return StatusCode::SUCCESS;
    }
  }

  //save tracks to storegrate	/
  muonTrks->push_back(defaultMuonTrk1);
  muonTrks->push_back(defaultMuonTrk2);

  StatusCode sc = evtStore()->record(muonTrks, m_outputTracksName, false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName << endreq;
  }
  else{
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrks->size() << " " << m_outputTracksName <<" into StoreGate" << endreq;
  }

  sc = evtStore()->record(muonTrksRefit1, m_outputTracksName + "Refit1", false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName + "Refit1" << endreq;
  } else {
		if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrksRefit1->size() << " " << m_outputTracksName  + "Refit1" <<" into StoreGate" << endreq;
	}

  sc = evtStore()->record(muonTrksRefit2, m_outputTracksName + "Refit2", false);
  if (sc.isFailure()) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed storing " << m_outputTracksName +"Refit2" << endreq;
  } else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Stored "<< muonTrksRefit2->size() << " " << m_outputTracksName + "Refit2" <<" into StoreGate" << endreq;
  }

  //fill truth first in case no truth match found
  if (m_isMC) {

     if (FillTruthParameters(p1).isFailure()){
	ATH_MSG_WARNING("Failed to fill truth parameters - skipping event");
  	return StatusCode::SUCCESS;
     }
     if (FillTruthParameters(p2).isFailure()){
	ATH_MSG_WARNING("Failed to fill truth parameters - skipping event");
  	return StatusCode::SUCCESS;
     }

     m_truthTree->Fill();
  }

  //fill default ID parameters
  FillRecParameters(defaultMuonTrk1, p1->charge());
  FillRecParameters(defaultMuonTrk2, p2->charge());
  m_defaultTree->Fill();

  //fill refit1 ID parameters
  FillRecParameters(refit1MuonTrk1, p1->charge());
  FillRecParameters(refit1MuonTrk2, p2->charge());
  m_refit1Tree->Fill();

  //fill refit2 ID parameters
  FillRecParameters(refit2MuonTrk1, p1->charge());
  FillRecParameters(refit2MuonTrk2, p2->charge());
  m_refit2Tree->Fill();


  //fill Combined parameters
  const xAOD::Muon* muon_pos = m_xZmm.getCombMuon(m_xZmm.getPosMuon(ZmumuEvent::CB));
  const xAOD::Muon* muon_neg = m_xZmm.getCombMuon(m_xZmm.getNegMuon(ZmumuEvent::CB));

  if (!muon_pos || !muon_neg) {
    ATH_MSG_WARNING("CB Muons missing!");
  } else {
    FillRecParameters(muon_pos->trackParticle(xAOD::Muon::CombinedTrackParticle)->track(), muon_pos->trackParticle(xAOD::Muon::CombinedTrackParticle)->charge());
    FillRecParameters(muon_neg->trackParticle(xAOD::Muon::CombinedTrackParticle)->track(), muon_neg->trackParticle(xAOD::Muon::CombinedTrackParticle)->charge());
    m_combMuidTree->Fill();
  }

  return StatusCode::SUCCESS;
}


void IDPerfMonZmumu::FillRecParameters(const Trk::Track* track, double charge)
{

  if (!track){
    ATH_MSG_WARNING("Empty Track. Skipping.");
    return;
  }
  const Trk::Perigee* trkPerigee = track->perigeeParameters();

  double px = 0;
  double py = 0;
  double pz = 0;
  double d0 = 0;
  double z0 = 0;

  if(trkPerigee){
    double qOverP   = trkPerigee->parameters()[Trk::qOverP];
    if (qOverP) {
      px = trkPerigee->momentum().x();
      py = trkPerigee->momentum().y();
      pz = trkPerigee->momentum().z();
    }
  }

  const Trk::AtaStraightLine*  atBL =
       dynamic_cast<const Trk::AtaStraightLine*>(m_trackToVertexTool->trackAtBeamline( *track ));

  if (atBL){
    double qOverP   = atBL->parameters()[Trk::qOverP];
    if(qOverP){
      px = atBL->momentum().x();
      py = atBL->momentum().y();
      pz = atBL->momentum().z();
      z0 = atBL->parameters()[Trk::z0];
      d0 = atBL->parameters()[Trk::d0];
    }
  }


  if (charge == 1) {
    m_positive_px = px;
    m_positive_py = py;
    m_positive_pz = pz;
    m_positive_z0 = z0;
    m_positive_d0 = d0;
  } else  if (charge == -1) {
    m_negative_px = px;
    m_negative_py = py;
    m_negative_pz = pz;
    m_negative_z0 = z0;
    m_negative_d0 = d0;
  }

  return;
}

StatusCode IDPerfMonZmumu::FillTruthParameters(const xAOD::TrackParticle* track)
{
  // Code adjusted from (http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Tracking/TrkAlgorithms/TrkTruthAlgs/src/TrackParticleTruthAlg.cxx)
  xAOD::TrackParticleContainer* particles = 0;
  if(evtStore()->contains<xAOD::TrackParticleContainer>(m_trackParticleName)){
    ATH_CHECK(evtStore()->retrieve(particles, m_trackParticleName));
  }else return StatusCode::SUCCESS;

   ElementLink<xAOD::TrackParticleContainer> tlink;
   tlink.setElement(track);
   tlink.setStorableObject((*particles));

  const xAODTruthParticleLinkVector* truthParticleLinkVec = 0;
  if(evtStore()->contains<xAODTruthParticleLinkVector>(m_truthLinkVecName)){
    ATH_CHECK(evtStore()->retrieve(truthParticleLinkVec, m_truthLinkVecName));
  }else return StatusCode::SUCCESS;

  const TrackTruthCollection* truthTracks = 0;
  // Retrieve the input
  if( evtStore()->contains<TrackTruthCollection>(m_truthName)){
    ATH_CHECK(evtStore()->retrieve(truthTracks, m_truthName));
  }else return StatusCode::SUCCESS;


  const HepMC::GenParticle *HEPparticle = 0;

  for( auto particle : *particles ) {

    if( !particle->trackLink().isValid() ){
      ATH_MSG_WARNING("Found TrackParticle with Invalid element link, skipping");
      continue;
    }
    ATH_MSG_DEBUG("Looking up truth for pt " << particle->pt() << " eta " << particle->eta() << " phi " << particle->phi());

    Trk::TrackTruthKey key((*tlink)->trackLink());
    auto found = truthTracks->find(key);

    if(found != truthTracks->end()) {
      ATH_MSG_VERBOSE("Found track Truth: barcode  " << found->second.particleLink().barcode() << " evt " << found->second.particleLink().eventIndex());
      HEPparticle = found->second.particleLink().cptr();

    } else {
      //No truth found
      ATH_MSG_WARNING("Unable to find truth particles. Skipping event");
      return StatusCode::FAILURE;
    }

    // get your track parameters.
    HepMC::GenVertex* pvtx = HEPparticle->production_vertex();

    //create the perigee here
    Amg::Vector3D pos(pvtx->position().x(), pvtx->position().y(),pvtx->position().z());
    Amg::Vector3D mom(HEPparticle->momentum().x(),HEPparticle->momentum().y(),HEPparticle->momentum().z());
    double charge = 0;
    if ( HEPparticle->pdg_id() == 13 )       charge = -1.;
    else if ( HEPparticle->pdg_id() == -13 ) charge = 1.;

    Trk::Perigee* candidatePerigee  = new Trk::Perigee(pos,mom,charge,pos);

    if (charge == 1) {

      double qOverP   = candidatePerigee->parameters()[Trk::qOverP];
      if (qOverP) {
	m_positive_px = HEPparticle->momentum().x();
	m_positive_py = HEPparticle->momentum().y();
	m_positive_pz = HEPparticle->momentum().z();
	m_positive_z0 = candidatePerigee->parameters()[Trk::z0];
	m_positive_d0 = candidatePerigee->parameters()[Trk::d0];
      }
    } else  if (charge == -1) {

      double qOverP   = candidatePerigee->parameters()[Trk::qOverP];
      if (qOverP) {
	m_positive_px = HEPparticle->momentum().x();
	m_positive_py = HEPparticle->momentum().y();
	m_positive_pz = HEPparticle->momentum().z();
	m_negative_z0 = candidatePerigee->parameters()[Trk::z0];
	m_negative_d0 = candidatePerigee->parameters()[Trk::d0];
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode IDPerfMonZmumu::finalize()
{
  return StatusCode::SUCCESS;
}
