/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/TriggerInfo.h"
#include "TrigDecisionTool/DecisionObjectHandleEventInfo.h"
#include "GaudiKernel/ThreadLocalContext.h"

using namespace Trig;

DecisionObjectHandleEventInfo::DecisionObjectHandleEventInfo( SG::ReadHandleKey<EventInfo>* oldEventInfoKey )
  : m_oldEventInfoKey(oldEventInfoKey),
    m_object(nullptr)
{
}

TriggerInfo const * DecisionObjectHandleEventInfo::getDecision() const {

  if ( !m_object && !m_oldEventInfoKey->empty() ) {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadHandle<EventInfo> oldEventInfo = SG::makeHandle(*m_oldEventInfoKey, ctx);
    if( ! oldEventInfo.isValid() ) {
      static bool warningPrinted = false;
      if( ! warningPrinted ) {
         ATH_MSG_WARNING( "EventInfo is not available on the "
                          "input" );
         warningPrinted = true;
      }
      return nullptr;
    }
    m_object = oldEventInfo.ptr(); 
  }

  return m_object->trigger_info();
}
void const * DecisionObjectHandleEventInfo::getNavigation() const {
  ATH_MSG_ERROR("TDT configured with EventInfo object handle. It does not support navigation access!"); 
  return nullptr;
}

void DecisionObjectHandleEventInfo::reset (bool hard) {
  DataHandle<EventInfo>::reset (hard);
  m_object = nullptr;
  invalidate();
  ATH_MSG_DEBUG("invalidated decision object");
}
#endif
