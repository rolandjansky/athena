//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "helpers.h"
#include "../Root/RDataSourceEvent.h"

// xAOD include(s).
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/Message.h"
#include "AsgMessaging/MessageCheck.h"

// ROOT include(s).
#include <TFile.h>
#include <TError.h>

// System include(s).
#include <iostream>
#include <memory>

int main() {

   ANA_CHECK_SET_TYPE (int);
   using namespace asg::msgUserCode;

   // Set up the runtime environment.
   ANA_CHECK( xAOD::Init() );

   // Open the input file.
   std::unique_ptr< TFile > ifile( TFile::Open( "${ASG_TEST_FILE_DATA}",
                                                "READ" ) );
   if( ( ! ifile ) || ifile->IsZombie() ) {
      Error( "dataSourceEvent_test",
             XAOD_MESSAGE( "Could not open the input file" ) );
      return 1;
   }

   // Set up the event object.
   xAOD::RDataSourceEvent event;
   ANA_CHECK( event.readFrom( ifile.get() ) );
   if( event.getEntry( 0 ) < 0 ) {
      Error( "dataSourceEvent_test",
             XAOD_MESSAGE( "Couldn't load the first event of the input "
                           "file" ) );
      return 1;
   }

   // Print the column names and their C++ types.
   std::cout << event.columnAndTypeNames() << std::endl;

   // Return gracefully.
   return 0;
}
