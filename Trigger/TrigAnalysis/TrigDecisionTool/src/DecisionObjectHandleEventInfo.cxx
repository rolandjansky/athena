/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/TriggerInfo.h"
#include "TrigDecisionTool/DecisionObjectHandleEventInfo.h"

using namespace Trig;

DecisionObjectHandleEventInfo::DecisionObjectHandleEventInfo( StoreGateSvc* sg, const std::string& key )
  : m_sg(sg),
    m_key(key),
    m_object(nullptr)
{
}

TriggerInfo const * DecisionObjectHandleEventInfo::getDecision() const {
  // register handle
  if ( !isInitialized() ) {
    if ( m_sg->regHandle(*this, m_key).isFailure() ) {
      ATH_MSG_INFO("Can't register handle for TrigDecision objects from SG for a key  " << m_key << " (check file with checkSG.py)");    }
  }
  
  if ( !m_object ) {
    m_object = cptr(); 
  }
  return m_object->trigger_info();
}
void const * DecisionObjectHandleEventInfo::getNavigation() const {
  ATH_MSG_ERROR("TDT configured with EventInfo object handle. It does not support navigation access!"); 
  return 0;
}

void DecisionObjectHandleEventInfo::reset() {
  DataHandle<EventInfo>::reset();
  m_object = 0;
  invalidate();
  ATH_MSG_DEBUG("invalidated decision object");
}
#endif
