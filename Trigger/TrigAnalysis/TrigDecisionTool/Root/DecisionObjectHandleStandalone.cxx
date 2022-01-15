/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "TrigDecisionTool/DecisionObjectHandleStandalone.h"

#include "AsgTools/CurrentContext.h"
#include "AsgDataHandles/ReadHandle.h"

// Include for the event store type:
#ifdef XAOD_STANDALONE // AnalysisBase
#   include "AsgTools/SgTEvent.h"
#elif !defined(XAOD_STANDALONE) // AthAnalysis or full Athena
#   include "StoreGate/StoreGateSvc.h"
#else
#   error "Wrong environment configuration detected!"
#endif

namespace Trig {

   DecisionObjectHandleStandalone::DecisionObjectHandleStandalone( SG::ReadHandleKey<xAOD::TrigDecision>* deckey,
                                                                   SG::ReadHandleKey<xAOD::TrigNavigation>* navikey )
     : m_deckey( deckey ), m_navikey( navikey ) {

   }

   void DecisionObjectHandleStandalone::reset(bool) {

      invalidate();
   }

   const xAOD::TrigDecision*
   DecisionObjectHandleStandalone::getDecision() const {

      if( m_deckey->empty() ) return nullptr;

      const EventContext& ctx = Gaudi::Hive::currentContext();
      SG::ReadHandle<xAOD::TrigDecision> decisionReadHandle = SG::makeHandle(*m_deckey, ctx);
      if( ! decisionReadHandle.isValid() ) {
        [[maybe_unused]] static std::atomic<bool> warningPrinted =
          [&]() { ATH_MSG_WARNING( "xAOD::TrigDecision is not available on the input" );
          return true; }();
        return nullptr;
      }
      return decisionReadHandle.ptr();
   }

   const xAOD::TrigNavigation*
   DecisionObjectHandleStandalone::getNavigation() const {

      if( m_navikey->empty() ) return nullptr;

      const EventContext& ctx = Gaudi::Hive::currentContext();
      SG::ReadHandle<xAOD::TrigNavigation> navReadHandle = SG::makeHandle(*m_navikey, ctx);
      if( ! navReadHandle.isValid() ) {
        [[maybe_unused]] static std::atomic<bool> warningPrinted =
          [&]() { ATH_MSG_WARNING( "xAOD::TrigNavigation is not available on the input" );
          return true; }();
        return nullptr;
      }
      return navReadHandle.ptr();
   }

} // namespace Trig
