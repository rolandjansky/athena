/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS // Full Athena only
#include "StoreGate/StoreGateSvc.h"

#include "TrigDecisionTool/DecisionObjectHandleAthena.h"

using namespace Trig;

DecisionObjectHandleAthena::DecisionObjectHandleAthena( SG::ReadHandleKey<TrigDec::TrigDecision>* olddeckey )
  : m_oldDecKey(olddeckey)
{
}

TrigDec::TrigDecision const * DecisionObjectHandleAthena::getDecision() const {
  
  if ( m_oldDecKey->empty() ) return nullptr;

  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<TrigDec::TrigDecision> oldDecisionReadHandle = SG::makeHandle(*m_oldDecKey, ctx);
  if( ! oldDecisionReadHandle.isValid() ) {
    [[maybe_unused]] static std::atomic<bool> warningPrinted =
      [&]() { ATH_MSG_WARNING( "TrigDec::TrigDecision is not available on the input" );
      return true; }();
    return nullptr;
  }
  return oldDecisionReadHandle.ptr();
}

TrigDec::TrigDecision const * DecisionObjectHandleAthena::getNavigation() const {
  return getDecision();
}

void DecisionObjectHandleAthena::reset (bool) {
  invalidate();
  ATH_MSG_DEBUG("invalidated decision object");
}
#endif
