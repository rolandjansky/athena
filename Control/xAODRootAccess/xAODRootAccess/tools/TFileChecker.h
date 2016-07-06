// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TFileChecker.h 673572 2015-06-09 08:40:47Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TFILECHECKER_H
#define XAODROOTACCESS_TOOLS_TFILECHECKER_H

// System include(s):
#include <set>
#include <string>

// ROOT include(s):
#include <TObject.h>

// Local include(s):
#include "xAODRootAccess/tools/TReturnCode.h"

// Forward declaration(s):
namespace SG {
   class AuxVectorBase;
}

namespace xAOD {

   // Forward declaration(s):
   class TEvent;

   /// Tool for running sanity checks on xAOD files
   ///
   /// There are some typical issues that can show up in xAOD files due to
   /// common coding issues in the reconstruction/analysis code. This class
   /// can check for most of these issues in the file given to it.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 673572 $
   /// $Date: 2015-06-09 10:40:47 +0200 (Tue, 09 Jun 2015) $
   ///
   class TFileChecker : public ::TObject {

   public:
      /// Default constructor
      TFileChecker();

      /// Execute all sanity checks on a given file
      TReturnCode check( TEvent& event );

      /// Set whether the validation should stop when an error is found
      void setStopOnError( ::Bool_t value );
      /// Check whether the validation will stop when an error is found
      ::Bool_t stopOnError() const;

      /// Add a variable to be ignored in the test
      void addIgnoredVariable( const std::string& name );
      /// Set the list of variables to be ignored in the test
      void setIgnoredVariables( const std::set< std::string >& value );
      /// Get the list of variables which are ignored in the test
      const std::set< std::string >& ignoredVariables() const;

   private:
      /// Check the health of a container
      TReturnCode checkContainer( const SG::AuxVectorBase& vec,
                                  const std::string& name );

      /// Stop the validation when an issue is found?
      ::Bool_t m_stopOnError;
      /// List of variables to ignore in the test
      std::set< std::string > m_ignoredVariables;

      /// List of containers without an auxiliary store
      std::set< std::string > m_orpannedContainers;

      ClassDef( xAOD::TFileChecker, 0 )

   }; // class TFileChecker

} // namespace xAOD

#endif // XAODROOTACCESS_TOOLS_TFILECHECKER_H
