/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "TrigMuonToolInterfaces/IMuonEFTrackIsolationTool.h"
#include "TrigMuonEFTrackIsolation.h"

#include "FourMomUtils/P4Helpers.h"

// edm includes
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEvent/TrigMuonEFIsolation.h"
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
  m_debug(false),
  m_coneSizes(),
  m_efIsoTool("TrigMuonEFTrackIsolationTool/TrigMuonEFTrackIsolationTool", this),
  m_trkptiso_cone2(),
  m_trkptiso_cone3(),
  m_doMyTiming(false),
  m_dataPrepTime(0),
  m_calcTime(0),
  m_dataOutputTime(0) {

  declareProperty("IsolationTool", m_efIsoTool);
  declareProperty("IdTrackParticles", m_idTrackParticlesName);
  declareProperty("doMyTiming", m_doMyTiming);
  declareProperty("requireCombinedMuon", m_requireCombined);
  

  ///////// Monitoring Variables
  declareProperty("histoPathBase",m_histo_path_base="/EXPERT/");
  declareMonitoredStdContainer("EF_trks_trkptiso_cone2", m_trkptiso_cone2, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_trks_trkptiso_cone3", m_trkptiso_cone3, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_trks_idtrkdz", m_idtrkdz, IMonitoredAlgo::AutoClear);
  declareMonitoredStdContainer("EF_trks_idtrkdr", m_idtrkdr, IMonitoredAlgo::AutoClear);

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
    msg() << MSG::DEBUG << "Initializing TrigMuonEFTrackIsolation[" << name() << "]" << endreq;
    msg() << MSG::DEBUG
	  << "package version = " << PACKAGE_VERSION << endreq;
    msg() << MSG::DEBUG
	  << "Properties set as follows: " << endreq;
    msg() << MSG::DEBUG
	  << "Track isolation tool:          " << m_efIsoTool << endreq;
    msg() << MSG::DEBUG
	  << "IdTrackParticles:              " << m_idTrackParticlesName << endreq;
    msg() << MSG::DEBUG
	  << "requireCombinedMuon:           " << m_requireCombined << endreq;
    msg() << MSG::DEBUG
	  << "doMyTiming                     " << m_doMyTiming << endreq;
  }//debug

  StatusCode sc = m_efIsoTool.retrieve();
  if(sc.isSuccess()) {
    msg() << MSG::INFO << "Retrieved " << m_efIsoTool << endreq;
  } else {
    msg() << MSG::FATAL << "Could not retrieve " << m_efIsoTool << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  if(m_doMyTiming){
    m_dataPrepTime = addTimer("dataPrepTime");
    m_calcTime = addTimer("calcTime");
    m_dataOutputTime = addTimer("dataOutputTime");
  }

  if (m_debug) msg() << MSG::DEBUG
		     << "End of init TrigMuonEFTrackIsolation" << endreq;
  return HLT::OK;

}

// ------------------------------------------------------------------------------------------------------
/**
 * Execute function - called for each roi
 */
HLT::ErrorCode
TrigMuonEFTrackIsolation::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* TEout) {

  if(m_doMyTiming) m_dataPrepTime->start();

  if (m_debug) msg() << MSG::DEBUG
		     << ": Executing TrigMuonEFTrackIsolation::execHLTAlgorithm()" << endreq;

  // Get ID Track particles
  const xAOD::TrackParticleContainer* idTrackParticles = 0;
  HLT::ErrorCode hltStatus = getFeature(inputTE, idTrackParticles, m_idTrackParticlesName);
  if (hltStatus != HLT::OK) {
    msg() << MSG::WARNING << "Failed to retrieve inner detector track particles" << endreq;
    return hltStatus;
  }
  if(!idTrackParticles) {
    std::string label;
    std::string inlabel;
    TrigConf::HLTTriggerElement::getLabel (TEout->getId(), label );
    TrigConf::HLTTriggerElement::getLabel (inputTE->getId(), inlabel );
    msg() << MSG::WARNING << "Pointer to xAOD::TrackParticleContainer[" << m_idTrackParticlesName << "] for id tracks = 0, stopping processing of ROI" << endreq;
    msg() << MSG::WARNING << "Input TE = " << inlabel << ", Output TE = " << label << endreq;
    return HLT::MISSING_FEATURE;
  }  else {
    if(m_debug) msg() << MSG::DEBUG << "Inner detector track particles retrieved with size: " << idTrackParticles->size() << endreq;
  }


  /// extract EF muons
  const xAOD::MuonContainer* muonContainer(0);
  if(HLT::OK != getFeature(inputTE, muonContainer)) {
    ATH_MSG_WARNING("Could not get xAOD::MuonContainer from the trigger element");
    return HLT::MISSING_FEATURE;
  } else {
    if(!muonContainer) {
      ATH_MSG_WARNING("muonContainer is 0 pointer");
      return HLT::MISSING_FEATURE;
    }
    ATH_MSG_DEBUG("MuonContainer extracted with size = " << muonContainer->size());
  }

  // loop on muons
  for(auto muon : *muonContainer) {

    if(m_debug) {
      msg() << MSG::DEBUG << "Processing next EF muon " << muon << endreq;
    }
    const xAOD::Muon::MuonType muontype = muon->muonType();
    if( muontype == xAOD::Muon::MuonType::Combined || muontype == xAOD::Muon::MuonType::SegmentTagged ) {
      if(m_debug) {
	msg() << MSG::DEBUG << "EF muon has combined or segment tagged muon" << endreq;
      }
    } else {
      if(m_requireCombined) {
	if(m_debug) {
	  msg() << MSG::DEBUG << "Not a combined or segment tagged muon & requireCombined=true, so ignore this muon" << endreq;
	}
	continue;
      }//requireCombined
    }//no combined muon
        
    std::vector<double> isoResults;
    std::vector<double> dzvals; // for monitoring
    std::vector<double> drvals; // for monitoring
    // use the tool to calculate the isolation
    StatusCode result = m_efIsoTool->calcTrackIsolation( muon, idTrackParticles, m_coneSizes,  isoResults, &dzvals, &drvals);    
    m_idtrkdz.insert(m_idtrkdz.begin(), dzvals.begin(), dzvals.end());
    m_idtrkdr.insert(m_idtrkdr.begin(), drvals.begin(), drvals.end());
      
    if(result.isFailure() || isoResults.size() != 2) {
	
      if(result.isFailure()) {
	msg() << MSG::WARNING << "Isolation tool failed for this muon - isolation will not be set for this muon" << endreq;
      } else {
	if(isoResults.size() != 2) {
	  msg() << MSG::WARNING << "Wrong number of isolation results - isolation will not be set for this muon." << endreq;
	}
      }
    } else { //isolation tool was ok - store results
      
      m_trkptiso_cone2.push_back(isoResults.at(0) / 1000.0); // convert to GeV

      m_trkptiso_cone3.push_back(isoResults.at(1) / 1000.0); // convert to GeV
      
      float ptcone20 = isoResults[0];
      float ptcone30 = isoResults[0];
      ((xAOD::Muon*)muon)->setIsolation( ptcone20, xAOD::Iso::ptcone20 );
      ((xAOD::Muon*)muon)->setIsolation( ptcone30, xAOD::Iso::ptcone30 );
    }// isolation tool ok
  }//loop on muon container

  //validate sequence
  TEout->setActiveState(true);

  return HLT::OK;
  
}//hltExecute

HLT::ErrorCode TrigMuonEFTrackIsolation:: hltFinalize()
{
  if (m_debug)
    msg() << MSG::DEBUG << "Finalizing TrigMuonEFTrackIsolation" << endreq;

  return  HLT::OK;
}

/**
 * Here we can handle incidents like end of event, but for now
 * nothing to do.
 */
void TrigMuonEFTrackIsolation::handle(const Incident& /*inc*/)
{

}
