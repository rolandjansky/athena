/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file AsgMessaging/MsgLevel.h
/// @author David Adams <dladams@bnl.gov>
/// @brief Definition of message levels and a helper function
///
/// $Revision: 690850 $
/// $Date: 2015-08-24 10:29:10 +0200 (Mon, 24 Aug 2015) $

#ifndef ASGMESSAGING_MSGLEVEL_H
#define ASGMESSAGING_MSGLEVEL_H

// System include(s):
#include <string>

// Local include(s):

// Pull in the definition from Gaudi in Athena:
#ifndef XAOD_STANDALONE
#   include "GaudiKernel/IMessageSvc.h"
#endif

// Not sure why this is needed...
#undef ERROR

namespace MSG {

#ifdef XAOD_STANDALONE

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

#endif // XAOD_STANDALONE

   /// Convenience function for translating message levels to strings
   const std::string& name( Level lvl );

   /// This struct is here only to make the dictionary generation for types in
   /// the MSG namespace successful.
   struct DummyStruct {};

}  // end namespace MSG

#endif // ASGMESSAGING_MSGLEVEL_H
