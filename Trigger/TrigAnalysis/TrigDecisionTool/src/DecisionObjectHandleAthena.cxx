/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
#include "StoreGate/StoreGateSvc.h"

#include "TrigDecisionTool/DecisionObjectHandleAthena.h"

using namespace Trig;

DecisionObjectHandleAthena::DecisionObjectHandleAthena( SG::ReadHandleKey<TrigDec::TrigDecision>* olddeckey )
  : m_oldDecKey(olddeckey),
    m_object(nullptr)
{
}

TrigDec::TrigDecision const * DecisionObjectHandleAthena::getDecision() const {
  
  if ( !m_object && !m_oldDecKey->empty() ) {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadHandle<TrigDec::TrigDecision> oldDecisionReadHandle = SG::makeHandle(*m_oldDecKey, ctx);
    if( ! oldDecisionReadHandle.isValid() ) {
      static bool warningPrinted = false;
      if( ! warningPrinted ) {
         ATH_MSG_WARNING( "TrigDec::TrigDecision is not available on the "
                          "input" );
         warningPrinted = true;
      }
      return nullptr;
    }
    m_object = oldDecisionReadHandle.ptr(); 
  }
  
  return m_object;
}
TrigDec::TrigDecision const * DecisionObjectHandleAthena::getNavigation() const {
  return getDecision();
}


void DecisionObjectHandleAthena::reset (bool hard) {
  DataHandle<TrigDec::TrigDecision>::reset (hard);
  m_object = nullptr;
  invalidate();
  ATH_MSG_DEBUG("invalidated decision object");
}
#endif
