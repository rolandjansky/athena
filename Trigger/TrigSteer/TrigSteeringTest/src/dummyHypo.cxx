/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigPassBits.h"
#include "xAODTrigger/TrigPassBits.h"
#include "TrigNavigation/TriggerElement.h"

#include "TrigSteeringTest/dummyHypo.h"

using namespace PESA;

dummyHypo::dummyHypo(const std::string& name, ISvcLocator* pSvcLocator)
  :HypoAlgo(name, pSvcLocator), m_counter(0) {
  declareProperty("PreScale", m_prescale=1, "prescale from 1 to inf, defines how often accept");
}

HLT::ErrorCode dummyHypo::hltInitialize() {
  msg() << MSG::DEBUG << "prescale set to: " << m_prescale << endmsg;
  return HLT::OK;
}

HLT::ErrorCode dummyHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  m_counter++;
  if ( m_counter % m_prescale == 0 ) {
    pass = true;
    msg() << MSG::DEBUG << "Accepting" << endmsg;
  }
  else {
    pass = false;
    msg() << MSG::DEBUG << "Rejecting" << endmsg;
  }

  TrigPassBits* old = new TrigPassBits(10);
  old->markPassing(7);
  ATH_MSG_DEBUG("Attaching old bits");

  // Deliberately using a deprecated interface here for testing purposes,
  // so suppress deprecation warnings.
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
   HLT::ErrorCode ec = attachBits(outputTE, old, "oldbits");
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif

  ATH_MSG_DEBUG("Making new pass bits");
  xAOD::TrigPassBits* newbits = new xAOD::TrigPassBits();
  
  newbits->makePrivateStore();
  newbits->setSize(24);
  newbits->setPassBits( std::vector<uint32_t>(1) );
  newbits->markPassing(7, true);
  ATH_MSG_DEBUG("Attaching new pass bits");
  ec = attachFeature(outputTE, newbits, "newbits");
  ATH_MSG_DEBUG("Attached new pass bits");
  return HLT::OK;
}



HLT::ErrorCode dummyHypo::hltFinalize() {
  return HLT::OK;
}
