/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS // Full Athena only
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/TriggerInfo.h"
#include "TrigDecisionTool/DecisionObjectHandleEventInfo.h"
#include "GaudiKernel/ThreadLocalContext.h"

using namespace Trig;

DecisionObjectHandleEventInfo::DecisionObjectHandleEventInfo( SG::ReadHandleKey<EventInfo>* oldEventInfoKey )
  : m_oldEventInfoKey(oldEventInfoKey)
{
}

TriggerInfo const * DecisionObjectHandleEventInfo::getDecision() const {

  if ( m_oldEventInfoKey->empty() ) return nullptr;

  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<EventInfo> oldEventInfo = SG::makeHandle(*m_oldEventInfoKey, ctx);
  if( ! oldEventInfo.isValid() ) {
    [[maybe_unused]] static std::atomic<bool> warningPrinted =
      [&]() { ATH_MSG_WARNING( "EventInfo is not available on the input" );
      return true; }();
    return nullptr;
  }

  return oldEventInfo->trigger_info();
}

void const * DecisionObjectHandleEventInfo::getNavigation() const {
  ATH_MSG_ERROR("TDT configured with EventInfo object handle. It does not support navigation access!"); 
  return nullptr;
}

void DecisionObjectHandleEventInfo::reset (bool) {
  invalidate();
  ATH_MSG_DEBUG("invalidated decision object");
}
#endif
