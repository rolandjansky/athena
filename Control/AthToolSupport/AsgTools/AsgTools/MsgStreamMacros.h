// Dear emacs, this is -*- c++ -*-
// $Id: MsgStreamMacros.h 613600 2014-08-27 08:39:48Z krasznaa $
#ifndef ASGTOOLS_MSGSTREAMMACROS_H
#define ASGTOOLS_MSGSTREAMMACROS_H

// Local include(s):
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/MsgLevel.h"

// Pull in the definition from Athena:
#ifdef ASGTOOL_ATHENA
#   include "AthenaBaseComps/AthMsgStreamMacros.h"
#elif defined(ASGTOOL_STANDALONE)

// Not sure why this is needed...
#undef ERROR

// This is a GCC extension for getting the name of the current function.
#if defined( __GNUC__ )
#   define MSGSTREAM_FNAME __PRETTY_FUNCTION__
#else
#   define MSGSTREAM_FNAME ""
#endif

/// Common prefix for the non-usual messages
///
/// The idea is that a regular user usually only wants to see DEBUG, INFO
/// and some WARNING messages. So those should be reasonably short. On the other
/// hand serious warnings (ERROR, FATAL) messages should be as precise
/// as possible to make debugging the issue easier.
///
#define MSGSTREAM_REPORT_PREFIX \
   __FILE__ << ":" << __LINE__ << " (" << MSGSTREAM_FNAME << "): "

/// Macro used to print "serious" messages
#define ATH_MSG_LVL_SERIOUS( lvl, xmsg )                    \
   msg( lvl ) << MSGSTREAM_REPORT_PREFIX << xmsg << endmsg

/// Macro used to print "regular" messages
#define ATH_MSG_LVL_NOCHK( lvl, xmsg )          \
   msg( lvl ) << xmsg << endmsg

/// Macro used to print "protected" messages
#define ATH_MSG_LVL( lvl, xmsg )                \
   do {                                         \
      if( msg().msgLevel( lvl ) ) {             \
         ATH_MSG_LVL_NOCHK( lvl, xmsg );        \
      }                                         \
   } while( 0 )

/// Macro printing verbose messages
#define ATH_MSG_VERBOSE( xmsg )  ATH_MSG_LVL( MSG::VERBOSE, xmsg )
/// Macro printing debug messages
#define ATH_MSG_DEBUG( xmsg )    ATH_MSG_LVL( MSG::DEBUG, xmsg )
/// Macro printing info messages
#define ATH_MSG_INFO( xmsg )     ATH_MSG_LVL_NOCHK( MSG::INFO,  xmsg )
/// Macro printing warning messages
#define ATH_MSG_WARNING( xmsg )  ATH_MSG_LVL_NOCHK( MSG::WARNING, xmsg )
/// Macro printing error messages
#define ATH_MSG_ERROR( xmsg )    ATH_MSG_LVL_SERIOUS( MSG::ERROR, xmsg )
/// Macro printing fatal messages
#define ATH_MSG_FATAL( xmsg )    ATH_MSG_LVL_SERIOUS( MSG::FATAL, xmsg )
/// Macro printing messages that should always appear
#define ATH_MSG_ALWAYS( xmsg )   ATH_MSG_LVL_NOCHK( MSG::ALWAYS, xmsg )

#else
#   error "What environment are we in?!?"
#endif // Environment selection
#endif // ASGTOOLS_MSGSTREAMMACROS_H
