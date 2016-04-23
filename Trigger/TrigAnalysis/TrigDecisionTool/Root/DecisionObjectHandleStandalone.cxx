/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Local include(s):
#include "TrigDecisionTool/DecisionObjectHandleStandalone.h"

// Include for the event store type:
#ifdef ASGTOOL_STANDALONE
#   include "AsgTools/SgTEvent.h"
#elif defined(ASGTOOL_ATHENA)
#   include "StoreGate/StoreGateSvc.h"
#else
#   error "Wrong environment configuration detected!"
#endif

namespace Trig {

   DecisionObjectHandleStandalone::
   DecisionObjectHandleStandalone( EventPtr_t sg, const std::string& deckey,
                                   const std::string& navikey )
     : m_sg( sg ), m_deckey( deckey ), m_navikey( navikey ),
     m_decision( 0 ), m_navigation( 0 ) {

   }

   void DecisionObjectHandleStandalone::reset() {

      m_decision = 0;
      m_navigation = 0;
      invalidate();

      return;
   }

   const xAOD::TrigDecision*
   DecisionObjectHandleStandalone::getDecision() const {

      if( ! m_decision ) {
         if( ! m_sg->contains< xAOD::TrigDecision >( m_deckey ) ) {
            static bool warningPrinted = false;
            if( ! warningPrinted ) {
               ATH_MSG_WARNING( "xAOD::TrigDecision is not available on the "
                                "input" );
               warningPrinted = true;
            }
            return 0;
         }
         if( ! m_sg->retrieve( m_decision, m_deckey ).isSuccess() ) {
            ATH_MSG_ERROR( "Problems retrieving xAOD::TrigDecision" );
            return 0;
         }
      }
      return m_decision;
   }

   const xAOD::TrigNavigation*
   DecisionObjectHandleStandalone::getNavigation() const {

      if( ! m_navigation ) {
         if( ! m_sg->contains< xAOD::TrigNavigation >( m_navikey ) ) {
            static bool warningPrinted = false;
            if( ! warningPrinted ) {
               ATH_MSG_WARNING( "xAOD::TrigNavigation is not available on the "
                                "input" );
               warningPrinted = true;
            }
            return 0;
         }
         if( ! m_sg->retrieve( m_navigation, m_navikey ).isSuccess() ) {
            ATH_MSG_ERROR( "Problems retrieving xAOD::TrigNavigation" );
            return 0;
         }
      }
      return m_navigation;
   }

} // namespace Trig
