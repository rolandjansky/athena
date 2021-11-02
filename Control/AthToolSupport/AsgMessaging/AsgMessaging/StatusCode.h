/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
   StatusCode( unsigned long rstat = SUCCESS );
   /// Copy constructor
   StatusCode( const StatusCode& parent );
   /// Destructor
   ~StatusCode();

   /// Assignment operator
   StatusCode& operator= ( const StatusCode& rhs );
   /// Assignment from an integer code
   StatusCode& operator= ( unsigned long code );

   /// Check if the operation was successful
   bool isSuccess() const;
   /// Check if the operation was a failure
   bool isFailure() const;
   /// Check if the operation produced a recoverable issue
   bool isRecoverable() const;

   /// Automatic conversion operator
   operator unsigned long() const;

   /// Mark the status code as checked, ignoring it thereby
   void setChecked() const { m_checked = true; }
   /// Ignore the status code, marking it as checked
   void ignore() const { setChecked(); }

   /// Enable failure (with a backtrace) on an unchecked status code
   static void enableFailure();
   static void enableChecking() {enableFailure();};
   /// Disable failure (no backtrace) on an unchecked status code
   static void disableFailure();
   static void disableChecking() {disableFailure();};

private:
   /// Code returned by some function
   unsigned long m_code;
   /// Internal status flag of whether the code was checked by the user
   mutable bool m_checked;

}; // class StatusCode

#endif // not XAOD_STANDALONE
#endif // ASGMESSAGING_STATUSCODE_H
