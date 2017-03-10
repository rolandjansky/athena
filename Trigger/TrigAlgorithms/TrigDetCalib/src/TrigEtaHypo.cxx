/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEtaHypo.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigDetCalib
//
// ********************************************************************

#include "TrigEtaHypo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include <cmath>

//#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;


TrigEtaHypo::TrigEtaHypo(const std::string& name, ISvcLocator* pSvcLocator):
    HLT::HypoAlgo(name, pSvcLocator) 
{
  declareProperty("EtaMinCut",    m_etaMinCut = -1, "minimum eta value"); // Default: None
  declareProperty("EtaMaxCut",    m_etaMaxCut = -1, "maximum eta value"); // Default: None
  declareProperty("RoILabel",     m_roILabel  = "initialRoI", "RoI feature label");
  declareProperty("doMonitoring", m_doMonitoring = false, "switch on/off monitoring" );
  declareProperty("AcceptAll",    m_acceptAll = false);
}


TrigEtaHypo::~TrigEtaHypo()
{ }


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEtaHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endmsg;

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timerSvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	<< ": Unable to locate TrigTimer Service" << endmsg;
  }
  if (m_timerSvc)    
    m_timers.push_back(m_timerSvc->addItem("TrigEtaHypo.TrigEtaHypoTot"));

  m_acceptedEvts = 0;
  m_rejectedEvts = 0;
  m_errorEvts    = 0;

  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEtaHypo::hltFinalize(){
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_acceptedEvts <<" / "<< m_rejectedEvts << " / "<< m_errorEvts << endmsg;
  return HLT::OK;
}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEtaHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
// ----------------------------------------------------------------------

  // Time total TrigL2JetHypo execution time.
  if (m_timerSvc) m_timers[0]->start();

  pass = false;

  // get the trigger element and extract the RoI information

  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(outputTE, roiDescriptor, m_roILabel) == HLT::OK && roiDescriptor) {
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
          << " located at phi = " <<  roiDescriptor->phi() << ", eta = " << roiDescriptor->eta() << endmsg;
    else
      msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;  
    
    if ((m_etaMinCut < 0 || std::fabs(roiDescriptor->eta()) > m_etaMinCut) &&
	(m_etaMaxCut < 0 || std::fabs(roiDescriptor->eta()) < m_etaMaxCut)) {
      pass = true;
      m_acceptedEvts++;
    }
    else
      m_rejectedEvts++;
  }
  else
    m_errorEvts++;
    
  if (m_timerSvc) m_timers[0]->stop();
  return HLT::OK;
}
