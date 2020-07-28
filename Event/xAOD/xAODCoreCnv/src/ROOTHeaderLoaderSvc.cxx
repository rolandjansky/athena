//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "ROOTHeaderLoaderSvc.h"

// ROOT include(s).
#include <TInterpreter.h>
#include <TString.h>
#include <TROOT.h>

namespace xAODMaker {

   StatusCode ROOTHeaderLoaderSvc::initialize() {

      // Load all the configured headers.
      for( const std::string& headerName : m_headerNames.value() ) {
         ATH_MSG_INFO( "Loading header \"" << headerName << "\"" );
         ATH_CHECK( loadHeader( headerName ) );
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode
   ROOTHeaderLoaderSvc::loadHeader( const std::string& headerName ) const {

      // Load the requested header.
      Int_t errorCode = 0;
      gROOT->ProcessLine( TString::Format( "#include \"%s\"",
                                           headerName.c_str() ),
                          &errorCode );

      // Check if the call succeeded.
      if( errorCode != TInterpreter::kNoError ) {
         ATH_MSG_FATAL( "Failed to load header \"" << headerName << "\"" );
         return StatusCode::FAILURE;
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
