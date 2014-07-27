/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigNavigation/TriggerElement.h"

#include "TrigSteeringTest/dummyHypo.h"

using namespace PESA;

dummyHypo::dummyHypo(const std::string& name, ISvcLocator* pSvcLocator)
  :HypoAlgo(name, pSvcLocator), m_counter(0) {
  declareProperty("PreScale", m_prescale=1, "prescale from 1 to inf, defines how often accept");
}

HLT::ErrorCode dummyHypo::hltInitialize() {
  msg() << MSG::DEBUG << "prescale set to: " << m_prescale << endreq;
  return HLT::OK;
}

HLT::ErrorCode dummyHypo::hltExecute(const HLT::TriggerElement* /*outputTE*/, bool& pass) {
  m_counter++;
  if ( m_counter % m_prescale == 0 ) {
    pass = true;
    msg() << MSG::DEBUG << "Accepting" << endreq;
  }
  else {
    pass = false;
    msg() << MSG::DEBUG << "Rejecting" << endreq;
  }
  return HLT::OK;
}



HLT::ErrorCode dummyHypo::hltFinalize() {
  return HLT::OK;
}
