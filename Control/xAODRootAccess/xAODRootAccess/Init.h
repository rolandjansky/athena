// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Init.h 687022 2015-08-03 09:48:01Z krasznaa $
#ifndef XAODROOTACCESS_INIT_H
#define XAODROOTACCESS_INIT_H

// System include(s):
#include <cstddef>

// Local include(s):
#include "xAODRootAccess/tools/TReturnCode.h"

/// Namespace holding all the xAOD EDM/tool classes
namespace xAOD {

   /// Function initialising ROOT/PyROOT for using the ATLAS EDM
   ///
   /// This simplified form of the initialisation function is especially
   /// meant for PyROOT that doesn't like the char** argument of the other
   /// version of this function. But this function just calls the other one
   /// in the background...
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 687022 $
   /// $Date: 2015-08-03 11:48:01 +0200 (Mon, 03 Aug 2015) $
   ///
   /// @param appname Name of the application/script [optional]
   /// @returns <code>TReturnCode::kSuccess</code> if successful,
   ///          <code>TReturnCode::kFailure</code> if not
   ///
   TReturnCode Init( const char* appname = "xAOD::Init" );

   /// Function initialising an application for using the ATLAS EDM
   ///
   /// As a few steps may need to be taken to initialise a standalone
   /// ROOT environment for reading/writing xAOD files, all of these
   /// should just be hidden from the user behind this single function.
   ///
   /// The user is expected to call this function at the start of
   /// his/her application, before an xAOD file would be opened.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 687022 $
   /// $Date: 2015-08-03 11:48:01 +0200 (Mon, 03 Aug 2015) $
   ///
   /// @param appname Name of the application
   /// @param argc The argc argument from main(...)
   /// @param argv The argv argument from main(...)
   /// @returns <code>TReturnCode::kSuccess</code> if successful,
   ///          <code>TReturnCode::kFailure</code> if not
   ///
   TReturnCode Init( const char* appname, int* argc, char** argv );

   /// Set the width of the source strings for the printed messages
   ///
   /// The <code>xAOD::Init</code> function installs a custom message printing
   /// function into the application. One that prints all messages passing
   /// through the ROOT messaging system similar to Athena's message service.
   /// The output format uses a fixed width source string, which gets chopped
   /// if the specified string is too long.
   ///
   /// By default the source string width is 25 characters, which should be fine
   /// for most purposes. But in case the user wants to change this value, it
   /// can be done using this function.
   ///
   /// @param value The width of the source strings for the printed messages
   ///
   void SetMessageSourceWidth( size_t value );

} // namespace xAOD

#endif // XAODROOTACCESS_INIT_H
