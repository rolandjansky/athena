/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
     : m_deckey( deckey ), m_navikey( navikey ),
     m_decision( nullptr ), m_navigation( nullptr ) {

   }

   void DecisionObjectHandleStandalone::reset(bool) {

      m_decision = nullptr;
      m_navigation = nullptr;
      invalidate();

      return;
   }

   const xAOD::TrigDecision*
   DecisionObjectHandleStandalone::getDecision() const {

      if( ! m_decision && !m_deckey->empty() ) {
         const EventContext& ctx = Gaudi::Hive::currentContext();
         SG::ReadHandle<xAOD::TrigDecision> decisionReadHandle = SG::makeHandle(*m_deckey, ctx);
         if( ! decisionReadHandle.isValid() ) {
            [[maybe_unused]] static std::atomic<bool> warningPrinted =
               [&]() { ATH_MSG_WARNING( "xAOD::TrigDecision is not available on the input" );
                       return true; }();
            return nullptr;
         }
         m_decision = decisionReadHandle.ptr();
      }
      return m_decision;
   }

   const xAOD::TrigNavigation*
   DecisionObjectHandleStandalone::getNavigation() const {

      if( ! m_navigation && !m_navikey->empty() ) {
         const EventContext& ctx = Gaudi::Hive::currentContext();
         SG::ReadHandle<xAOD::TrigNavigation> navReadHandle = SG::makeHandle(*m_navikey, ctx);
         if( ! navReadHandle.isValid() ) {
            [[maybe_unused]] static std::atomic<bool> warningPrinted =
               [&]() { ATH_MSG_WARNING( "xAOD::TrigNavigation is not available on the input" );
                       return true; }();
            return nullptr;
         }
         m_navigation = navReadHandle.ptr();
      }
      return m_navigation;
   }

} // namespace Trig
