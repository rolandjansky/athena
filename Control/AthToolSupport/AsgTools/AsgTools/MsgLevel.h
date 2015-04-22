// Dear emacs, this is -*- c++ -*-
// $Id: MsgLevel.h 612639 2014-08-20 12:26:10Z krasznaa $
/// @file AsgTools/MsgLevel.h
/// @author David Adams <dladams@bnl.gov>
/// @brief Definition of message levels and a helper function
///
/// $Revision: 612639 $
/// $Date: 2014-08-20 14:26:10 +0200 (Wed, 20 Aug 2014) $
///
#ifndef ASGTOOLS_MSGLEVEL_H
#define ASGTOOLS_MSGLEVEL_H

// System include(s):
#include <string>

// Local include(s):
#include "AsgTools/AsgToolsConf.h"

// Pull in the definition from Gaudi in Athena:
#ifdef ASGTOOL_ATHENA
#   include "GaudiKernel/IMessageSvc.h"
#endif

// Not sure why this is needed...
#undef ERROR

namespace MSG {

#ifdef ASGTOOL_STANDALONE

   /// Message level definitions
   enum Level {
      NIL = 0,
      VERBOSE,
      DEBUG,
      INFO,
      WARNING,
      ERROR,
      FATAL,
      ALWAYS,
      NUM_LEVELS
   }; // enum Level

#endif // ASGTOOL_STANDALONE

   /// Convenience function for translating message levels to strings
   const std::string& name( Level lvl );

}  // end namespace MSG

#endif // ASGTOOLS_MSGLEVEL_H
