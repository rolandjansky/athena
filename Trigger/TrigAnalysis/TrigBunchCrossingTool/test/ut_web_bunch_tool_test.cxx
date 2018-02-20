/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_web_bunch_tool_test.cxx 745783 2016-05-10 06:47:18Z krasznaa $

// System include(s):
#include <iostream>

// Local include(s):
#include "TrigBunchCrossingTool/WebBunchCrossingTool.h"

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
   Trig::WebBunchCrossingTool tool;
   SIMPLE_CHECK( tool.setProperty( "OutputLevel", MSG::DEBUG ) );
   SIMPLE_CHECK( tool.setProperty( "ServerAddress",
                                   "atlas-trigconf.cern.ch" ) );

   // Try to load some configurations by key:
   SIMPLE_CHECK( tool.loadConfig( 104 ) );
   SIMPLE_CHECK( tool.loadConfig( 109 ) );

   // Now ask some questions about the last configuration:
   SIMPLE_ASSERT( tool.isFilled( 7 ) );
   SIMPLE_ASSERT( tool.isInTrain( 13 ) );
   SIMPLE_ASSERT( tool.distanceFromFront( 1 ) == 0 );
   SIMPLE_ASSERT( tool.distanceFromFront( 163 ) == 300 );
   SIMPLE_ASSERT( tool.gapBeforeTrain( 157 ) == 2700 );
   SIMPLE_ASSERT( tool.bunchTrainSpacing() == 150 );

   // Try to load some configurations by run and lumiblock:
   SIMPLE_CHECK( tool.loadConfig( 194367, 10 ) );
   SIMPLE_CHECK( tool.loadConfig( 284154, 40 ) );
   SIMPLE_CHECK( tool.loadConfig( 209109, 10 ) );
   SIMPLE_CHECK( tool.loadConfig( 209109, 20 ) );

   // Now ask some questions about the last configuration:
   SIMPLE_ASSERT( tool.isFilled( 68 ) );
   SIMPLE_ASSERT( tool.isInTrain( 148 ) );
   SIMPLE_ASSERT( tool.distanceFromFront( 66 ) == 0 );
   SIMPLE_ASSERT( tool.distanceFromFront( 238 ) == 300 );
   SIMPLE_ASSERT( tool.gapBeforeTrain( 146 ) == 250 );
   SIMPLE_ASSERT( tool.bunchTrainSpacing() == 50 );

   // Check a 8b4e configuration:
   SIMPLE_CHECK( tool.loadConfig( 334849, 100 ) );
   SIMPLE_ASSERT( tool.isFilled( 200 ) );
   SIMPLE_ASSERT( tool.isInTrain( 210 ) );
   SIMPLE_ASSERT( tool.distanceFromFront( 232 ) == 0 );
   SIMPLE_ASSERT( tool.distanceFromFront( 250 ) == 150 );
   SIMPLE_ASSERT( tool.gapBeforeTrain( 270 ) == 125 );
   SIMPLE_ASSERT( tool.bunchTrainSpacing() == 25 );

   // Return gracefully:
   return 0;
}
