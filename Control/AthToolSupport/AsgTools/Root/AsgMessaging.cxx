// $Id: AsgMessaging.cxx 784571 2016-11-16 14:27:23Z will $

// Local include(s):
#include "AsgTools/AsgMessaging.h"

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

   AsgMessaging::AsgMessaging( const IAsgTool* tool )
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
