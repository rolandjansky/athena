/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_static_bunch_tool_test.cxx 618331 2014-09-24 11:55:26Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "TrigBunchCrossingTool/StaticBunchCrossingTool.h"

/// A little helper macro
#define SIMPLE_CHECK( EXP )                                       \
   do {                                                           \
      if( ! EXP.isSuccess() ) {                                   \
         std::cerr << "Failed to execute: " << #EXP << std::endl; \
            return 1;                                             \
      }                                                           \
   } while( 0 )

/// Another helper macro for evaluating logical tests
#define SIMPLE_ASSERT( EXP )                                            \
   do {                                                                 \
      const bool result = EXP;                                          \
      if( ! result ) {                                                  \
         std::cerr << "Expression \"" << #EXP << "\" failed the evaluation" \
                   << std::endl;                                        \
            return 1;                                                   \
      }                                                                 \
   } while( 0 )

int main() {

   // Create the tool to be tested:
   Trig::StaticBunchCrossingTool tool;
   SIMPLE_CHECK( tool.setProperty( "OutputLevel", MSG::DEBUG ) );

   // Try to load some configurations:
   SIMPLE_CHECK( tool.loadConfig( 1 ) );
   SIMPLE_CHECK( tool.loadConfig( 100 ) );
   SIMPLE_CHECK( tool.loadConfig( 109 ) );

   // Now ask some questions about the last configuration:
   SIMPLE_ASSERT( tool.isFilled( 7 ) );
   SIMPLE_ASSERT( tool.isInTrain( 13 ) );
   SIMPLE_ASSERT( tool.distanceFromFront( 1 ) == 0 );
   SIMPLE_ASSERT( tool.distanceFromFront( 163 ) == 300 );
   SIMPLE_ASSERT( tool.gapBeforeTrain( 157 ) == 2700 );
   SIMPLE_ASSERT( tool.bunchTrainSpacing() == 150 );

   // Return gracefully:
   return 0;
}
