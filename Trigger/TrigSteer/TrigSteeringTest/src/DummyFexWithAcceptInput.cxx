/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringTest/DummyFexWithAcceptInput.h"


DummyFexWithAcceptInput::DummyFexWithAcceptInput(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator),
    m_counter(0),
    m_prescale(1)
{
  declareProperty("prescale", m_prescale=1, "fraction of accept input returns true");
}


     
HLT::ErrorCode DummyFexWithAcceptInput::acceptInput(const HLT::TriggerElement* input, bool& pass ) {
  pass = false;
  m_descriptor=0;
  m_counter++;
  const TrigRoiDescriptor* l1(0);
  if ( getFeature( input, l1) == HLT::OK && l1 ) {
    msg() << MSG::DEBUG << "Got L1 RoI: " << *l1 << endreq;
  }


  if ( m_counter % m_prescale == 0 ) {
    pass = true;
    m_descriptor = new TrigRoiDescriptor();
    msg() << MSG::DEBUG << "Accepting" << endreq;
  } else {
    msg() << MSG::DEBUG << "Rejecting" << endreq;
  }
  
  return HLT::OK;
}

HLT::ErrorCode DummyFexWithAcceptInput::hltExecute(const HLT::TriggerElement* /*input*/,
						   HLT::TriggerElement* output) {

  if (m_descriptor) {
    attachFeature(output, m_descriptor, "xyz");
  }
  return HLT::OK;
}


