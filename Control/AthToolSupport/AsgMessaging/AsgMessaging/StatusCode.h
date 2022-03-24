/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASGMESSAGING_STATUSCODE_H
#define ASGMESSAGING_STATUSCODE_H

// Local include(s):
#include "AsgMessaging/Check.h"

#ifndef XAOD_STANDALONE
// Pull in the definition from Gaudi:
#   include "GaudiKernel/StatusCode.h"
#else // not XAOD_STANDALONE

#include "CxxUtils/nodiscard.h"
/// Partial reimplementation of Gaudi's StatusCode for standalone usage
///
/// In order to be able to write dual-use code that makes use of StatusCodes
/// in Athena and ROOT in the same way, we provide a simplified version of
/// Gaudi's class.
///
/// @author David Adams <dladams@bnl.gov>
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
///
/// $Revision: 612639 $
/// $Date: 2014-08-20 14:26:10 +0200 (Wed, 20 Aug 2014) $
///
class ATLAS_NODISCARD StatusCode {

public:
   /// Convenience StatusCode types
   enum {
      FAILURE = 0,
      SUCCESS = 1,
      RECOVERABLE = 2
   };

   /// Constructor from an integer status code
   StatusCode( unsigned long rstat = SUCCESS ) noexcept : m_code( rstat ) { }

   /// Assignment from an integer code
   StatusCode& operator= ( unsigned long code ) noexcept { m_code = code; return *this; }

   /// Check if the operation was successful
   bool isSuccess() const noexcept { return m_code == SUCCESS; }
   /// Check if the operation was a failure
   bool isFailure() const noexcept { return !isSuccess(); }
   /// Check if the operation produced a recoverable issue
   bool isRecoverable() const noexcept { return m_code == RECOVERABLE; }

   /// Automatic conversion operator
   operator unsigned long() const noexcept { return m_code; }

   /// Ignore the status code
   void ignore() const noexcept { }

   /// Older functions for backward compatibility with the
   /// pre-nondiscard version of this class in which we tracked
   /// in the object itself whether the content was checked.  These
   /// may go away at some point.
   void setChecked() const noexcept { }
   static void enableFailure() noexcept { }
   static void enableChecking() noexcept { }
   static void disableFailure() noexcept { }
   static void disableChecking() noexcept { }

private:
   /// Code returned by some function
   unsigned long m_code;

}; // class StatusCode

#endif // not XAOD_STANDALONE
#endif // ASGMESSAGING_STATUSCODE_H
