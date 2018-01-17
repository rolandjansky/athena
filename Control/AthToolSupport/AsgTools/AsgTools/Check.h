// Dear emacs, this is -*- c++ -*-
// $Id: Check.h 615760 2014-09-09 12:50:01Z krasznaa $
#ifndef ASGTOOLS_CHECK_H
#define ASGTOOLS_CHECK_H

// Local include(s):
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/MsgStreamMacros.h"

/// Helper macro for checking the status code returned by a function call
///
/// To be able to write code in a compact form, we can use this macro to
/// exchange code like:
///
/// <code>
///    if( someFunction().isFailure() ) {<br/>
///       ATH_MSG_FATAL( "Failed to call function someFunction" );<br/>
///       return StatusCode::FAILURE;<br/>
///    }
/// </code>
///
/// With something as simple as:
///
/// <code>
///    ASG_CHECK( someFunction() );
/// </code>
///
/// or for functions that do not return a StatusCode:
///
/// <code>
///    ASG_CHECK( someFunction(), -1 );
/// </code>
///
/// The macro may only be used inside of member functions of dual-use tools.
///
#define ASG_CHECK(...)  \
   BOOST_PP_OVERLOAD(ASG_CHECK_, __VA_ARGS__)(__VA_ARGS__)

#define ASG_CHECK_1( EXP )                                     \
   do {                                                        \
      const StatusCode sc__(EXP);                              \
      if( ! sc__.isSuccess() ) {                               \
         ATH_MSG_ERROR( "Failed to call \"" << #EXP << "\"" ); \
         return sc__;                                          \
      }                                                        \
   } while( 0 )

#define ASG_CHECK_2( EXP, RET )                                \
   do {                                                        \
      const StatusCode sc__(EXP);                              \
      if( ! sc__.isSuccess() ) {                               \
         ATH_MSG_ERROR( "Failed to call \"" << #EXP << "\"" ); \
         return RET;                                           \
      }                                                        \
   } while( 0 )

/// Helper macro for checking the status code of a call outside of an ASG tool
///
/// The other version of this macro can be used to write very compact code
/// inside a dual-use tool. But since it relies on functions provided by the
/// dual-use tool, it can't be used in general code. For instance to check
/// the status code returned by the initialisation function of a dual-use tool.
///
/// This version of the macro can be used in such a setup.
///
#define ASG_CHECK_SA( SOURCE, EXP )                                     \
   do {                                                                 \
      const StatusCode sc__(EXP);                                       \
      if( ! sc__.isSuccess() ) {                                        \
         static MsgStream msg( SOURCE );                                \
         msg << MSGSTREAM_REPORT_PREFIX << MSG::ERROR                   \
             << "Failed to call \"" << #EXP << "\"" << endmsg;          \
         return sc__;                                                   \
      }                                                                 \
   } while( 0 )

/// In standalone mode use the ASG specific macro as a replacement for ATH_CHECK
#ifdef ASGTOOL_STANDALONE
#   define ATH_CHECK ASG_CHECK
#endif

#endif // ASGTOOLS_CHECK_H
