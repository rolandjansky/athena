/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodeventinfo_printop_test.cxx 680052 2015-07-03 10:45:40Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODEventInfo/EventInfo.h"

int main() {

   // Create a test object:
   xAOD::EventInfo ei;
   ei.makePrivateStore();

   // Set some of its basic properties:
   ei.setRunNumber( 1234 );
   ei.setEventNumber( 2345 );
   ei.setLumiBlock( 34 );
   ei.setTimeStamp( 456789 );
   ei.setTimeStampNSOffset( 567 );
   ei.setBCID( 678 );
   ei.setDetectorMask( 0x12345678, 0x12345678 );

   // Try printing it:
   std::cout << ei << std::endl;

   // Return gracefully:
   return 0;
}
