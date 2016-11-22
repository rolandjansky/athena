/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2DijetCombo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   Simon Owen, Ignacio Aracena, Teresa Fonseca Martin, Josh McFayden
//
// ********************************************************************


#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"


//#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigJetHypo/TrigL2DijetCombo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigCaloEvent/TrigT2Jet.h"

#include <math.h>

class ISvcLocator;

TrigL2DijetCombo::TrigL2DijetCombo(const std::string & name, ISvcLocator* pSvcLocator)
 : HLT::ComboAlgo(name, pSvcLocator)
{

  //Monitor
  declareProperty("doMonitoring_L2", m_doMonitoring = true, "switch on/off monitoring" );
  declareMonitoredVariable("nTe2jCounter", m_nTe2jCounter);
  
}

HLT::ErrorCode TrigL2DijetCombo::hltInitialize()
{
  // Not timing internal to algorithm. Should be done automatically.
  /*
  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
        << ": Unable to locate TrigTimer Service" << endmsg;
  }
  if (m_timersvc) {

    TrigTimer* tmp = m_timersvc->addItem("TrigL2DijetCombo.TrigL2DijetComboTot");
    m_timers.push_back(tmp);
  }
  */
  
  return HLT::OK;
}

TrigL2DijetCombo::~TrigL2DijetCombo(){
}

HLT::ErrorCode TrigL2DijetCombo::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {

  // Time total TrigL2DijetCombo execution time.
  //if (m_timersvc) m_timers[0]->start();

  pass=false;
  m_nTe2jCounter = -1; // Monitoring variable

  // sanity check there has to be exactly 2 TEs as input
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigL2DijetCombo::acceptInputs, got: " << inputTE.size() << " TE as input" << endmsg;

  m_nTe2jCounter = inputTE.size(); // Monitoring variable

  if ( inputTE.size() != 2 ) {
    msg() << MSG::WARNING << "Got different than 2 number of input TEs" << endmsg;
    //if (m_timersvc) m_timers[0]->stop();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); 
  }
  
  pass=true;

  //if (m_timersvc) m_timers[0]->stop();
  return HLT::OK;
}

HLT::ErrorCode TrigL2DijetCombo::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* /*outputTE*/){

  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigL2DijetCombo::hltExecute" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode TrigL2DijetCombo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}

