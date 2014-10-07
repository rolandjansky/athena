// Dear emacs, this is -*- c++ -*-
// $Id: StatusCode.h 612639 2014-08-20 12:26:10Z krasznaa $
#ifndef ASGTOOLS_STATUSCODE_H
#define ASGTOOLS_STATUSCODE_H

// Local include(s):
#include "AsgTools/AsgToolsConf.h"
#include "AsgTools/Check.h"

#ifdef ASGTOOL_ATHENA
// Pull in the definition from Gaudi:
#   include "GaudiKernel/StatusCode.h"
#elif defined(ASGTOOL_STANDALONE)

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
class StatusCode {

public:
   /// Convenience StatusCode types
   enum {
      FAILURE = 0,
      SUCCESS = 1
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

   /// Automatic conversion operator
   operator unsigned long() const;

   /// Mark the status code as checked, ignoring it thereby
   void setChecked() const { m_checked = true; }
   /// Ignore the status code, marking it as checked
   void ignore() const { setChecked(); }

   /// Enable failure (with a backtrace) on an unchecked status code
   static void enableFailure();
   /// Disable failure (no backtrace) on an unchecked status code
   static void disableFailure();

private:
   /// Code returned by some function
   unsigned long m_code;
   /// Internal status flag of whether the code was checked by the user
   mutable bool m_checked;

}; // class StatusCode

#endif // Environment selection
#endif // ASGTOOLS_STATUSCODE_H
