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
  m_isoType(1),
  m_requireCombined(false),
  m_useVarIso(false),
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
  declareProperty("IsoType", m_isoType);  
  declareProperty("IdTrackParticles", m_idTrackParticlesName);
  declareProperty("FTKTrackParticles", m_FTKTrackParticlesName);
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
	  << "FTKTrackParticles:             " << m_FTKTrackParticlesName << endmsg;
    msg() << MSG::DEBUG
	  << "requireCombinedMuon:           " << m_requireCombined << endmsg;
    msg() << MSG::DEBUG
	  << "doMyTiming                     " << m_doMyTiming << endmsg;
    msg() << MSG::DEBUG
	  << "useVarIso                      " << m_useVarIso << endmsg;
    msg() << MSG::DEBUG
	  << "IsoType =                      " << m_isoType << endmsg;

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
 
  // Get ID Track & EF Muons
  const xAOD::TrackParticleContainer* idTrackParticles = 0;
  const xAOD::MuonContainer* EFmuonContainer(0);
  if (m_isoType==1) { // isoType==1 -> ID+EF
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
  }  else {
    if (m_debug) msg() << MSG::DEBUG << "No IDTrk / EF Muon isolation requested" << endmsg;
  }// ID + EF Retrieval

  // Get FTK Tracks and L2 Muons
  const xAOD::TrackParticleContainer* FTKTrackParticles = 0;
  const xAOD::L2CombinedMuonContainer* L2muonContainer(0);
  if (m_isoType==2) { // isoType==2 -> FTK+L2

    // FTK tracks
    HLT::ErrorCode hltFTKStatus = getFeature(inputTE, FTKTrackParticles, m_FTKTrackParticlesName);
    if (hltFTKStatus != HLT::OK) {
      msg() << MSG::WARNING << "Failed to retrieve FTK track particles" << endmsg;
      return hltFTKStatus;
    }
    if(!FTKTrackParticles) {
      std::string label;
      std::string inlabel;
      TrigConf::HLTTriggerElement::getLabel (TEout->getId(), label );
      TrigConf::HLTTriggerElement::getLabel (inputTE->getId(), inlabel );
      msg() << MSG::WARNING << "Pointer to xAOD::TrackParticleContainer[" << m_FTKTrackParticlesName << "] for FTK tracks = 0, stopping processing of ROI" << endmsg;
      msg() << MSG::WARNING << "Input TE = " << inlabel << ", Output TE = " << label << endmsg;
       return HLT::MISSING_FEATURE;
    }  else {
      if(m_debug) msg() << MSG::DEBUG << "FTK track particles retrieved with size: " << FTKTrackParticles->size() << endmsg;
    }// Get FTK tracks

    // extract L2 Muons
    HLT::ErrorCode status = getFeature(inputTE, L2muonContainer);
    if (status != HLT::OK || ! L2muonContainer) {
      msg() << MSG::ERROR << " L2CombinedMuonContainer not found --> ABORT" << endmsg;
      return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
    }else{
      ATH_MSG_DEBUG("L2 MuonContainer extracted with size = " << L2muonContainer->size());
    }
  } else {
    if (m_debug) msg() << MSG::DEBUG << "No FTK / L2 Muon isolation requested" << endmsg;
  }
      
  if(m_doMyTiming){
    if (m_dataPrepTime) m_dataPrepTime->stop();
    if (m_calcTime) m_calcTime->start();
  }

  if (m_isoType==1) {
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
      StatusCode result = m_efIsoTool->calcTrackIsolation( muon, idTrackParticles, m_coneSizes,  isoResults, &dzvals, &drvals, false, &selfremoval);

      m_trkdz.insert(m_trkdz.begin(), dzvals.begin(), dzvals.end());
      m_trkdr.insert(m_trkdr.begin(), drvals.begin(), drvals.end());
     
      if (selfremoval.size() == 3)
	{
	  m_muon_selfpt.push_back(selfremoval[0] * 1e-3);
	  m_muon_combinedpt.push_back(selfremoval[1] * 1e-3);
	  m_muon_removedpt.push_back(selfremoval[2] * 1e-3);
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

	if (m_useVarIso){
	  ((xAOD::Muon*)muon)->setIsolation( ptcone20, xAOD::Iso::ptvarcone20 );
	  ((xAOD::Muon*)muon)->setIsolation( ptcone30, xAOD::Iso::ptvarcone30 );
	} else { 
	  ((xAOD::Muon*)muon)->setIsolation( ptcone20, xAOD::Iso::ptcone20 );
	  ((xAOD::Muon*)muon)->setIsolation( ptcone30, xAOD::Iso::ptcone30 );
	}
      }// isolation tool ok for ID
    }// EF Muon Loop
  } // If EFID loop to run

  
  // Container to store the L2 isomuon
  xAOD::L2IsoMuonContainer *L2muonIsoContainer = 0;
  xAOD::L2IsoMuonAuxContainer caux;

  if (m_isoType==2) {

    L2muonIsoContainer = new xAOD::L2IsoMuonContainer();
    L2muonIsoContainer->setStore(&caux);
    
    // loop on L2 muons  
    for(auto muon : *L2muonContainer) {
    
      if (m_debug)
	msg() << MSG::DEBUG << "Running L2 Muons and FTK Tracks now" << endmsg;

      std::vector<double> isoResultsFTK;
      std::vector<double> dzvalsFTK; // for monitoring
      std::vector<double> drvalsFTK; // for monitoring
      std::vector<double> selfremovalFTK;

      // FTK tracks
      StatusCode resultFTK = m_efIsoTool->calcTrackIsolation( muon, FTKTrackParticles, m_coneSizes,  isoResultsFTK, &dzvalsFTK, &drvalsFTK, true, &selfremovalFTK);
      
      m_trkdz.insert(m_trkdz.begin(), dzvalsFTK.begin(), dzvalsFTK.end());
      m_trkdr.insert(m_trkdr.begin(), drvalsFTK.begin(), drvalsFTK.end());

      if (selfremovalFTK.size() == 3)
	{
	  m_muon_selfpt.push_back(selfremovalFTK[0] * 1e-3);
	  m_muon_combinedpt.push_back(selfremovalFTK[1] * 1e-3);
	  m_muon_removedpt.push_back(selfremovalFTK[2] * 1e-3);
	}
      else
	msg() << MSG::DEBUG << "Muon pT not stored correctly - histograms have not been filled for this muon" << endmsg;

    
      if(resultFTK.isFailure() || isoResultsFTK.size() != 2) {	
	if(resultFTK.isFailure()) {
	  msg() << MSG::WARNING << "Isolation tool failed for this muon (FTK) - isolation will not be set for this muon" << endmsg;
	} else {
	  if(isoResultsFTK.size() != 2) {
	    msg() << MSG::WARNING << "Wrong number of FTK isolation results - isolation will not be set for this muon." << endmsg;
	  }
	}
      } else { //isolation tool was ok - store results

	const float ptcone20FTK = isoResultsFTK[0]; 
	const float ptcone30FTK = isoResultsFTK[1]; 
       
	m_trkptiso_cone2.push_back(ptcone20FTK * 1e-3); // convert to GeV 
	m_trkptiso_cone3.push_back(ptcone30FTK * 1e-3); // convert to GeV

	// Set the iso on the L2 muon
	xAOD::L2IsoMuon* muonIso = new xAOD::L2IsoMuon();
	L2muonIsoContainer->push_back(muonIso);
	muonIso->setPt(muon->pt());
	muonIso->setEta(muon->eta());
	muonIso->setPhi(muon->phi());
	muonIso->setCharge(muon->charge());
	muonIso->setSumPt02(ptcone20FTK);
	muonIso->setSumPt03(ptcone30FTK);
      
      }// isolation tool ok for FTK
    }// Loop on L2 muons / FTK tracks
  } // if running L2FTK loop

  
  if(m_doMyTiming){
    if(m_calcTime) m_calcTime->stop();
    if(m_dataOutputTime) m_dataOutputTime->start();
  }

  if (m_isoType == 1)
    m_n_trks.push_back(idTrackParticles->size());
  if (m_isoType == 2){
    m_n_trks.push_back(FTKTrackParticles->size());
    
    // Record the L2 muon
    std::string muonIsoKey = "MuonL2ISInfo";
    HLT::ErrorCode L2IsoStatus = attachFeature(TEout, L2muonIsoContainer, muonIsoKey);
    if (L2IsoStatus != HLT::OK){
      TEout->setActiveState(false);
      delete L2muonIsoContainer;
      msg() << MSG::ERROR << " Record of xAOD::L2IsoMuonContainer in TriggerElement failed" << endmsg;
      return L2IsoStatus;
    }else{
      if (m_debug)
	msg() << MSG::DEBUG << " xAOD::L2IsoMuonContainer attached to the trigger element" << endmsg;
    }
  }

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
