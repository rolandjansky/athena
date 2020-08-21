/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Local include(s):
#include "TrigDecisionTool/DecisionObjectHandleStandalone.h"

// Include for the event store type:
#ifdef XAOD_STANDALONE
#   include "AsgTools/SgTEvent.h"
#elif !defined(XAOD_STANDALONE)
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
            static bool warningPrinted = false;
            if( ! warningPrinted ) {
               ATH_MSG_WARNING( "xAOD::TrigDecision is not available on the "
                                "input" );
               warningPrinted = true;
            }
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
            static bool warningPrinted = false;
            if( ! warningPrinted ) {
               ATH_MSG_WARNING( "xAOD::TrigNavigation is not available on the "
                                "input" );
               warningPrinted = true;
            }
            return nullptr;
         }
         m_navigation = navReadHandle.ptr();
      }
      return m_navigation;
   }

} // namespace Trig
