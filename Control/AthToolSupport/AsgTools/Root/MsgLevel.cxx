// $Id: MsgLevel.cxx 612639 2014-08-20 12:26:10Z krasznaa $

// Local include(s):
#include "AsgTools/MsgLevel.h"

/// Helper macro for constructing the function's code
#define DECLARE_LEVEL( NAME )                   \
   case MSG::NAME:                              \
   {                                            \
      static const std::string name( #NAME );   \
      return name;                              \
   }                                            \
   break

namespace MSG {

   const std::string& name( Level lvl ) {

      switch( lvl ) {
         DECLARE_LEVEL( NIL );
         DECLARE_LEVEL( VERBOSE );
         DECLARE_LEVEL( DEBUG );
         DECLARE_LEVEL( INFO );
         DECLARE_LEVEL( WARNING );
         DECLARE_LEVEL( ERROR );
         DECLARE_LEVEL( FATAL );
         DECLARE_LEVEL( ALWAYS );
      default:
         static const std::string dummy( "<unknown>" );
         return dummy;
         break;
      }
   }

} // namespace MSG
