/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "AsgMessaging/AsgMessaging.h"

// Gaudi/Athena include(s):
#ifndef XAOD_STANDALONE
#   include "GaudiKernel/Bootstrap.h"
#   include "GaudiKernel/ISvcLocator.h"
#   include "GaudiKernel/IMessageSvc.h"
#endif // not XAOD_STANDALONE

namespace asg {

   AsgMessaging::AsgMessaging( const std::string& name )
      :
#ifndef XAOD_STANDALONE
     AthMessaging( Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc", false ),
                    name )
#else // not XAOD_STANDALONE
      m_msg( name )
#endif // not XAOD_STANDALONE
   {

   }

   AsgMessaging::AsgMessaging( const INamedInterface* tool )
      :
#ifndef XAOD_STANDALONE
     AthMessaging( Gaudi::svcLocator()->service< IMessageSvc >( "MessageSvc", false ),
                    tool->name() )
#else // not XAOD_STANDALONE
      m_msg( tool )
#endif // not XAOD_STANDALONE
   {

   }

   bool AsgMessaging::msgLvl( const MSG::Level lvl ) const {
#ifndef XAOD_STANDALONE
      return ::AthMessaging::msgLvl( lvl );
#else // not XAOD_STANDALONE
      return m_msg.msgLevel( lvl );
#endif // not XAOD_STANDALONE
   }

   MsgStream& AsgMessaging::msg() const {
#ifndef XAOD_STANDALONE
      return ::AthMessaging::msg();
#else // not XAOD_STANDALONE
      return m_msg;
#endif // not XAOD_STANDALONE
   }

   MsgStream& AsgMessaging::msg( const MSG::Level lvl ) const {
#ifndef XAOD_STANDALONE
      return ::AthMessaging::msg( lvl );
#else // not XAOD_STANDALONE
      m_msg << lvl;
      return m_msg;
#endif // not XAOD_STANDALONE
   }

} // namespace asg
