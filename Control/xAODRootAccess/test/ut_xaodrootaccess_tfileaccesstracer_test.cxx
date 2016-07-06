/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodrootaccess_tfileaccesstracer_test.cxx 678490 2015-06-26 07:53:50Z krasznaa $

// System include(s):
#include <memory>

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "xAODRootAccess/tools/TFileAccessTracer.h"

int main() {

   // The application's name:
   static const char* APP_NAME = "ut_xaodrootaccess_tfileaccesstracer_test";

   // First test that an invalid address will not cause any problems:
   std::unique_ptr< xAOD::TFileAccessTracer >
      tracer1( new xAOD::TFileAccessTracer() );
   tracer1->setServerAddress( "unknown" );
   tracer1->setMonitoredFraction( 1.0 );
   tracer1.reset();

   // Next, check that not processing any events is not an issue:
   std::unique_ptr< xAOD::TFileAccessTracer >
      tracer2( new xAOD::TFileAccessTracer() );
   tracer2->setMonitoredFraction( 1.0 );
   tracer2.reset();

   // Tell the user what happened:
   ::Info( APP_NAME, "All tests passed" );

   // Return gracefully:
   return 0;
}
