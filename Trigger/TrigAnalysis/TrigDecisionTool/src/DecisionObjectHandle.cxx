/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/




#include "TrigDecisionTool/DecisionObjectHandle.h"


using namespace Trig;

DecisionObjectHandle::DecisionObjectHandle()
: m_object(0), m_validity(false) {  
}

void DecisionObjectHandle::invalidate() {
  m_validity = false;
  m_object = 0;
}

void DecisionObjectHandle::validate() {
  m_validity = true;
}

bool DecisionObjectHandle::valid() const {
  return m_validity;
}
DecisionObjectHandleARA::DecisionObjectHandleARA(TrigDec::TrigDecision const *object) {
  m_object = object;
}

TrigDec::TrigDecision const * DecisionObjectHandleARA::get() const {
  return m_object;
}

DecisionObjectHandleAthena::DecisionObjectHandleAthena( StoreGateSvc* sg, const std::string& key )
  : m_sg(sg), m_key(key) {
}

TrigDec::TrigDecision const * DecisionObjectHandleAthena::get() const {
  
  // register handle
  if ( !isInitialized() ) {
    if ( m_sg->regHandle(*this, m_key).isFailure() ) {
       if (msgLvl(MSG::INFO))
          log() << MSG::INFO << "Can't register handle for TrigDecision objects from SG for a key  "
                << m_key << " (check file with checkSG.py)"<< endreq; 
    }
  }

  if ( !m_object ) {
    m_object = cptr(); 
  }
  return m_object;
}


void DecisionObjectHandleAthena::reset() {
  DataHandle<TrigDec::TrigDecision>::reset();
  invalidate();
  if (msgLvl(MSG::DEBUG))
     log() << MSG::DEBUG << "invalidated decision object" << endreq;
}
