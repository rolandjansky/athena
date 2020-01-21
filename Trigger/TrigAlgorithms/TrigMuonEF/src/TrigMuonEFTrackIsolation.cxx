/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigMuonToolInterfaces/IMuonEFTrackIsolationTool.h"
#include "TrigMuonEFTrackIsolation.h"

#include "FourMomUtils/P4Helpers.h"

// edm includes
#include "Particle/TrackParticleContainer.h"
#include "xAODMuon/MuonContainer.h"

#include <iostream>

using Rec::TrackParticleContainer;
using Rec::TrackParticle;

using std::endl;

// ------------------------------------------------------------------------------------------------------
/**
 * Standard constructor for the track isolation algo.
 */
TrigMuonEFTrackIsolation::TrigMuonEFTrackIsolation(const std::string& name, ISvcLocator* pSvcLocator) :
  FexAlgo(name, pSvcLocator),
  m_requireCombined(false),
  m_useVarIso(false),
  m_muonContName("MuonEFInfo"),
  m_debug(false),
  m_coneSizes(),
  m_efIsoTool("TrigMuonEFTrackIsolationTool/TrigMuonEFTrackIsolationTool", this),
 
  m_trkptiso_cone2(),
  m_trkptiso_cone3(), 
  m_trkdz(),
  m_trkdr(),
  m_muon_selfpt(),
  m_muon_removedpt(),
  m_muon_combinedpt(),
  m_n_trks(),

  m_doMyTiming(false),
  m_dataPrepTime(0),
  m_calcTime(0),
  m_dataOutputTime(0){


  declareProperty("IsolationTool", m_efIsoTool);
  declareProperty("MuonContName", m_muonContName);
  declareProperty("IdTrackParticles", m_idTrackParticlesName);
  declareProperty("doMyTiming", m_doMyTiming);
  declareProperty("requireCombinedMuon", m_requireCombined);
  declareProperty("useVarIso", m_useVarIso);
  

  ///////// Monitoring Variables
  declareProperty("histoPathBase",m_histo_path_base="/EXPERT/");
  declareMonitoredStdContainer("trks_trkptiso_cone2", m_trkptiso_cone2, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("trks_trkptiso_cone3", m_trkptiso_cone3, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("trks_trkdz", m_trkdz, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("trks_trkdr", m_trkdr, IMonitoredAlgo::AutoClear);  
  declareMonitoredStdContainer("n_trks", m_n_trks, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("muon_selfpt", m_muon_selfpt, IMonitoredAlgo::AutoClear);  
  declareMonitoredStdContainer("muon_removedpt", m_muon_removedpt, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("muon_combinedpt", m_muon_combinedpt, IMonitoredAlgo::AutoClear);
  
  // cone sizes are hard-coded to ensure the correct result goes to the edm
  m_coneSizes.push_back(0.2);
  m_coneSizes.push_back(0.3);
}

// ------------------------------------------------------------------------------------------------------
/**
 * Detructor
 */
TrigMuonEFTrackIsolation::~TrigMuonEFTrackIsolation() {

}

// ------------------------------------------------------------------------------------------------------
/**
 * Initialize the algo
 */
HLT::ErrorCode TrigMuonEFTrackIsolation::hltInitialize() {

  m_debug   = msgLvl() <= MSG::DEBUG;
  
  if(m_debug) {
    msg() << MSG::DEBUG << "Initializing TrigMuonEFTrackIsolation[" << name() << "]" << endmsg;
    msg() << MSG::DEBUG
	  << "package version = " << PACKAGE_VERSION << endmsg;
    msg() << MSG::DEBUG
	  << "Properties set as follows: " << endmsg;
    msg() << MSG::DEBUG
	  << "Track isolation tool:          " << m_efIsoTool << endmsg;
    msg() << MSG::DEBUG
	  << "IdTrackParticles:              " << m_idTrackParticlesName << endmsg;
    msg() << MSG::DEBUG
	  << "requireCombinedMuon:           " << m_requireCombined << endmsg;
    msg() << MSG::DEBUG
	  << "doMyTiming                     " << m_doMyTiming << endmsg;
    msg() << MSG::DEBUG
	  << "useVarIso                      " << m_useVarIso << endmsg;

  }//debug

  StatusCode sc = m_efIsoTool.retrieve();
  if(sc.isSuccess()) {
    msg() << MSG::INFO << "Retrieved " << m_efIsoTool << endmsg;
  } else {
    msg() << MSG::FATAL << "Could not retrieve " << m_efIsoTool << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if(m_doMyTiming){
    msg() << MSG::INFO << "Running Timing for IsoTool" << endmsg;
    m_dataPrepTime = addTimer("dataPrepTime");
    m_calcTime = addTimer("calcTime");
    m_dataOutputTime = addTimer("dataOutputTime");
    msg() << MSG::INFO << "Timers: " << m_dataPrepTime << " " << m_calcTime << " " << m_dataOutputTime << endmsg;
  }

  if (m_debug) msg() << MSG::DEBUG
		     << "End of init TrigMuonEFTrackIsolation" << endmsg;
  return HLT::OK;

}

// ------------------------------------------------------------------------------------------------------
/**
 * Execute function - called for each roi
 */
HLT::ErrorCode
TrigMuonEFTrackIsolation::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout) {

  if(m_doMyTiming && m_dataPrepTime) m_dataPrepTime->start();

  if (m_debug) msg() << MSG::DEBUG
		     << ": Executing TrigMuonEFTrackIsolation::execHLTAlgorithm()" << endmsg;

  // prepare output
  auto muonContainer = std::make_unique<xAOD::MuonContainer>();
  xAOD::MuonAuxContainer muonAuxContainer;
  muonContainer->setStore( &muonAuxContainer );
  ATH_MSG_DEBUG("Created xAOD::MuonContainer");
  
  // Get ID Track & EF Muons
  const xAOD::TrackParticleContainer* idTrackParticles = 0;
  const xAOD::MuonContainer* EFmuonContainer(0);

  // ID Tracks
  HLT::ErrorCode hltStatus = getFeature(inputTE, idTrackParticles, m_idTrackParticlesName);
  if (hltStatus != HLT::OK) {
    msg() << MSG::WARNING << "Failed to retrieve inner detector track particles" << endmsg;
    return hltStatus;
  }
  if(!idTrackParticles) {
    std::string label;
    std::string inlabel;
    TrigConf::HLTTriggerElement::getLabel (TEout->getId(), label );
    TrigConf::HLTTriggerElement::getLabel (inputTE->getId(), inlabel );
    msg() << MSG::WARNING << "Pointer to xAOD::TrackParticleContainer[" << m_idTrackParticlesName << "] for id tracks = 0, stopping processing of ROI" << endmsg;
    msg() << MSG::WARNING << "Input TE = " << inlabel << ", Output TE = " << label << endmsg;
    return HLT::MISSING_FEATURE;
  }  else {
    if(m_debug) msg() << MSG::DEBUG << "Inner detector track particles retrieved with size: " << idTrackParticles->size() << endmsg;
  } // Get ID tracks
  
    // EF Muons
  if(HLT::OK != getFeature(inputTE, EFmuonContainer)) {
    ATH_MSG_WARNING("Could not get xAOD::MuonContainer from the trigger element");
    return HLT::MISSING_FEATURE;
  } else {
    if(!EFmuonContainer) {
      ATH_MSG_WARNING("muonContainer is 0 pointer");
      return HLT::MISSING_FEATURE;
    }
    ATH_MSG_DEBUG("EF MuonContainer extracted with size = " << EFmuonContainer->size());
    
  } // Get EF Muons
   

   
  if(m_doMyTiming){
    if (m_dataPrepTime) m_dataPrepTime->stop();
    if (m_calcTime) m_calcTime->start();
  }

  // loop on EF muons
  for(auto muon : *EFmuonContainer) {
    if(m_debug) {
      msg() << MSG::DEBUG << "Processing next EF muon w/ ID Track Isolation " << muon << endmsg;
    }
    const xAOD::Muon::MuonType muontype = muon->muonType();
    if( muontype == xAOD::Muon::MuonType::Combined || muontype == xAOD::Muon::MuonType::SegmentTagged ) {
      if(m_debug) {
	msg() << MSG::DEBUG << "EF muon has combined or segment tagged muon" << endmsg;
      }
    } else {
      if(m_requireCombined) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "Not a combined or segment tagged muon & requireCombined=true, so ignore this muon" << endmsg;
	}
	continue;
      }//requireCombined
      else{
	if( muontype == xAOD::Muon::MuonType::MuonStandAlone ){
	  if(m_debug) {
	    msg() << MSG::DEBUG << "EF muon has standalone muon" << endmsg;
	  }
	}
	else{
	  if(m_debug) {
	    msg() << MSG::DEBUG << "EF muon has neither combined, segment tagged, nor standalone muon" << endmsg;
	  }
	  continue;
	}
      }
    }//no combined muon
    
    std::vector<double> isoResults;
    std::vector<double> dzvals; // for monitoring
    std::vector<double> drvals; // for monitoring
    std::vector<double> selfremoval;

    // use the tool to calculate the isolation
    // ID tracks
    if (m_debug)
      msg() << MSG::DEBUG << "Running ID Tracks now" << endmsg;
    StatusCode result = m_efIsoTool->calcTrackIsolation( muon, idTrackParticles, m_coneSizes,  isoResults, &dzvals, &drvals, &selfremoval);

    m_trkdz.insert(m_trkdz.begin(), dzvals.begin(), dzvals.end());
    m_trkdr.insert(m_trkdr.begin(), drvals.begin(), drvals.end());
     
    if (selfremoval.size() == 2)
      {
	m_muon_selfpt.push_back(selfremoval[0] * 1e-3);
	m_muon_combinedpt.push_back(selfremoval[1] * 1e-3);
      }
    else
      msg() << MSG::DEBUG << "Muon pT not stored correctly - histograms have not been filled for this muon" << endmsg;

    if(result.isFailure() || isoResults.size() != 2) {	
      if(result.isFailure()) {
	msg() << MSG::WARNING << "Isolation tool failed for this muon - isolation will not be set for this muon" << endmsg;
      } else {
	if(isoResults.size() != 2) {
	  msg() << MSG::WARNING << "Wrong number of isolation results - isolation will not be set for this muon." << endmsg;
	}
      }
    } else { //isolation tool was ok - store results
      
      const float ptcone20 = isoResults[0]; 
      const float ptcone30 = isoResults[1]; 

      m_trkptiso_cone2.push_back(ptcone20 * 1e-3); // convert to GeV 
      m_trkptiso_cone3.push_back(ptcone30 * 1e-3); // convert to GeV

      // deep copy muon (since otherwise we risk overwriting isolation results from other algos)
      muonContainer->push_back( new xAOD::Muon(*muon) );
      xAOD::Muon* outputmuon = muonContainer->back();
	    
      if (m_useVarIso){
	outputmuon->setIsolation( ptcone20, xAOD::Iso::ptvarcone20 );
	outputmuon->setIsolation( ptcone30, xAOD::Iso::ptvarcone30 );
      } else { 
	outputmuon->setIsolation( ptcone20, xAOD::Iso::ptcone20 );
	outputmuon->setIsolation( ptcone30, xAOD::Iso::ptcone30 );
      }
    }// isolation tool ok for ID
  }// EF Muon Loop

  const size_t noutputMuons = muonContainer->size();
  hltStatus = attachFeature(TEout, muonContainer.release() , m_muonContName);
  if(hltStatus!=HLT::OK) {
    msg() << MSG::WARNING << "Attaching xAOD::MuonContainer to TEout: unsuccessful" << endmsg;
    return hltStatus;
  } else {
    ATH_MSG_DEBUG( "Successfully attached to TEout the muon container with size " << noutputMuons );
  } 
    

  
  
  if(m_doMyTiming){
    if(m_calcTime) m_calcTime->stop();
    if(m_dataOutputTime) m_dataOutputTime->start();
  }

  m_n_trks.push_back(idTrackParticles->size());

  //validate sequence
  TEout->setActiveState(true);
  if(m_doMyTiming && m_dataOutputTime){
    m_dataOutputTime->stop();
  }
  return HLT::OK;
  
}//hltExecute

HLT::ErrorCode TrigMuonEFTrackIsolation:: hltFinalize()
{
  if (m_debug)
    msg() << MSG::DEBUG << "Finalizing TrigMuonEFTrackIsolation" << endmsg;

  return  HLT::OK;
}

/**
 * Here we can handle incidents like end of event, but for now
 * nothing to do.
 */
void TrigMuonEFTrackIsolation::handle(const Incident& /*inc*/)
{

}
