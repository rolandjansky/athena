/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "AsgMessaging/AsgMessaging.h"

// Gaudi/Athena include(s):
#ifdef ASGTOOL_ATHENA
#   include "GaudiKernel/Bootstrap.h"
#   include "GaudiKernel/ISvcLocator.h"
#   include "GaudiKernel/IMessageSvc.h"
#endif // ASGTOOL_ATHENA

namespace asg {

   AsgMessaging::AsgMessaging( const std::string& name )
      :
#ifdef ASGTOOL_ATHENA
     AthMessaging( Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc", false ),
                    name )
#elif defined(ASGTOOL_STANDALONE)
      m_msg( name )
#else
#   error "What environment are we in?!?"
#endif // Environment selection
   {

   }

   AsgMessaging::AsgMessaging( const INamedInterface* tool )
      :
#ifdef ASGTOOL_ATHENA
     AthMessaging( Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc", false ),
                    tool->name() )
#elif defined(ASGTOOL_STANDALONE)
      m_msg( tool )
#else
#   error "What environment are we in?!?"
#endif // Environment selection
   {

   }

   bool AsgMessaging::msgLvl( const MSG::Level lvl ) const {
#ifdef ASGTOOL_ATHENA
      return ::AthMessaging::msgLvl( lvl );
#elif defined(ASGTOOL_STANDALONE)
      return m_msg.msgLevel( lvl );
#else
#   error "What environment are we in?!?"
#endif // Environment selection
   }

   MsgStream& AsgMessaging::msg() const {
#ifdef ASGTOOL_ATHENA
      return ::AthMessaging::msg();
#elif defined(ASGTOOL_STANDALONE)
      return m_msg;
#else
#   error "What environment are we in?!?"
#endif // Environment selection
   }

   MsgStream& AsgMessaging::msg( const MSG::Level lvl ) const {
#ifdef ASGTOOL_ATHENA
      return ::AthMessaging::msg( lvl );
#elif defined(ASGTOOL_STANDALONE)
      m_msg << lvl;
      return m_msg;
#else
#   error "What environment are we in?!?"
#endif // Environment selection
   }

} // namespace asg
