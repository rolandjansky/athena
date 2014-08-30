/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TrigMinBias/TrigTrackCounterHypo.h>
#include <xAODTrigMinBias/TrigTrackCounts.h>
#include <xAODTrigMinBias/TrigHisto2D.h>


//---------------------------------------------------------------------------------

TrigTrackCounterHypo::TrigTrackCounterHypo(const std::string& name, ISvcLocator* pSvcLocator): HLT::HypoAlgo(name, pSvcLocator),
m_log(msgSvc(), name) {
  declareProperty("Max_z0", m_max_z0 = 200.0); // (mm)
  declareProperty("Min_pt", m_min_pt = 0.100); // (GeV)
  declareProperty("Required_ntrks", m_required_ntrks = 1); // (#)
  declareProperty("AcceptAll_EF", m_acceptAll = false );
  
  // Monitoring
  declareMonitoredVariable("NTrksHypo", m_ntrksHypo);
  declareMonitoredVariable("NTrksSelected", m_ntrksSelected);
}

//---------------------------------------------------------------------------------

TrigTrackCounterHypo::~TrigTrackCounterHypo(){;}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounterHypo::hltInitialize() {
  m_log.setLevel( outputLevel() );
  
  if (msgLvl() <= MSG::INFO) {
    m_log << MSG::INFO << "Initialising TrigTrackCounterHypo, name = " << name() << endreq;
  }
  
  if( msgLvl() <= MSG::INFO ) {
    m_log << MSG::INFO << "max_z0 = " << m_max_z0 << " mm" << endreq;
    m_log << MSG::INFO << "min_pt = " << m_min_pt << " GeV" << endreq;
    m_log << MSG::INFO << "required_ntrks = " << m_required_ntrks << " #" << endreq;
    m_log << MSG::INFO << "AcceptAll = "
    << (m_acceptAll==true ? "True" : "False") << endreq; 
  }
  
  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounterHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  pass=false;
  
  if( msgLvl() <= MSG::DEBUG ) {
    m_log << MSG::DEBUG << "Executing this TrigTrackCounterHypo " << name() << endreq;
  }
  
  if( m_acceptAll ){
    pass = true;
    return HLT::OK;
  }
  
  // Retrieve the TrigTrackCounts object stored by the TrigTrackCounter class.
  m_trigTrackCounts = 0;
  HLT::ErrorCode hltStatus = getFeature(outputTE, m_trigTrackCounts, "trackcounts");
  if(hltStatus != HLT::OK || !m_trigTrackCounts){
    if( msgLvl() <= MSG::WARNING ){
      m_log << MSG::WARNING << "Failed to retrieve features from TE." << endreq;
    }
    
    return HLT::OK;
  }
  
  // Count the number of tracks within the selected region
  xAOD::TrigHisto2D *th2dZ0_pt = new xAOD::TrigHisto2D;
  
  th2dZ0_pt->initialize(m_trigTrackCounts->z0Bins(),
		       m_trigTrackCounts->z0Min(),
		       m_trigTrackCounts->z0Max(),
		       m_trigTrackCounts->ptBins(),
		       m_trigTrackCounts->ptMin(),
		       m_trigTrackCounts->ptMax());
  th2dZ0_pt->setContents(m_trigTrackCounts->z0_pt());
  
  m_ntrksSelected =th2dZ0_pt->sumEntries(m_max_z0, m_min_pt, xAOD::TrigHistoCutType::BELOW_X_ABOVE_Y);
  
  if(msgLvl() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "There are " << m_ntrksSelected << " tracks within selection window.  ";
  }
  
  // Check the trigger condition.
  if(m_ntrksSelected >= m_required_ntrks) {
    pass=true;
    if(msgLvl() <= MSG::DEBUG) {
      m_log << MSG::DEBUG << "The event passes." << endreq;
    }
  }
  else{
    if(msgLvl() <= MSG::DEBUG) {
      m_log << MSG::DEBUG << "The event fails." << endreq;
    }
  }
  
  
  // for monitoring
  m_ntrksHypo = m_ntrksSelected;
  if( !pass ) m_ntrksSelected = -999;
  
  return HLT::OK;
}

//---------------------------------------------------------------------------------

HLT::ErrorCode TrigTrackCounterHypo::hltFinalize() {
  
  if( msgLvl() <= MSG::INFO )
    m_log << MSG::INFO << " Finalizing this TrigTrackCounterHypo" << name() <<"." << endreq; 
  return HLT::OK;  
}
