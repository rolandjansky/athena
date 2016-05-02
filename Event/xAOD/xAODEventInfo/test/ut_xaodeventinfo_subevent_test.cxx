/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodeventinfo_subevent_test.cxx 696792 2015-09-25 09:33:48Z krasznaa $

// EDM include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODCore/tools/PrintHelpers.h"

// Local include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"

int main() {

   // Create the object to be tested:
   xAOD::EventInfo ei;
   ei.makePrivateStore();

   // Add some sub-events to it:
   std::vector< xAOD::EventInfo::SubEvent > subEvents( {
      xAOD::EventInfo::SubEvent( 0, 0, xAOD::EventInfo::Signal,
                                ElementLink< xAOD::EventInfoContainer >() ),
      xAOD::EventInfo::SubEvent( 1, 1, xAOD::EventInfo::MinimumBias,
                                 ElementLink< xAOD::EventInfoContainer >() ) } );
   ei.setSubEvents( subEvents );

   // Check what the object looks like in memory now:
   xAOD::dump( ei );

   // Add one more sub-event to it:
   ei.addSubEvent(
      xAOD::EventInfo::SubEvent( 2, 3, xAOD::EventInfo::Cavern,
                                 ElementLink< xAOD::EventInfoContainer >() ) );

   // Check what the object looks like in memory now:
   xAOD::dump( ei );

   // Clear the sub-events:
   ei.clearSubEvents();

   // Check what the object looks like in memory now:
   xAOD::dump( ei );

   return 0;
}
