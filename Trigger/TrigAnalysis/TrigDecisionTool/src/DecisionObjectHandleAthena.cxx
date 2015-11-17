/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "StoreGate/StoreGateSvc.h"

#include "TrigDecisionTool/DecisionObjectHandleAthena.h"

using namespace Trig;

DecisionObjectHandleAthena::DecisionObjectHandleAthena( StoreGateSvc* sg, const std::string& key )
  : m_sg(sg),
    m_key(key),
    m_object(nullptr)
{
}

TrigDec::TrigDecision const * DecisionObjectHandleAthena::getDecision() const {
  // register handle
  if ( !isInitialized() ) {
    if ( m_sg->regHandle(*this, m_key).isFailure() ) {
      ATH_MSG_INFO("Can't register handle for TrigDecision objects from SG for a key  " << m_key << " (check file with checkSG.py)"); 
    }
  }
  
  if ( !m_object ) {
    m_object = cptr(); 
  }
  return m_object;
}
TrigDec::TrigDecision const * DecisionObjectHandleAthena::getNavigation() const {
  return getDecision();
}


void DecisionObjectHandleAthena::reset() {
  DataHandle<TrigDec::TrigDecision>::reset();
  m_object = 0;
  invalidate();
  ATH_MSG_DEBUG("invalidated decision object");
}
#endif
