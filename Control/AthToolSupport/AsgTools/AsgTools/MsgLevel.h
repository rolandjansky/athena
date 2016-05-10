// Dear emacs, this is -*- c++ -*-
// $Id: MsgLevel.h 690850 2015-08-24 08:29:10Z krasznaa $
/// @file AsgTools/MsgLevel.h
/// @author David Adams <dladams@bnl.gov>
/// @brief Definition of message levels and a helper function
///
/// $Revision: 690850 $
/// $Date: 2015-08-24 10:29:10 +0200 (Mon, 24 Aug 2015) $
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

   /// This struct is here only to make the dictionary generation for types in
   /// the MSG namespace successful.
   struct DummyStruct {};

}  // end namespace MSG

#endif // ASGTOOLS_MSGLEVEL_H
