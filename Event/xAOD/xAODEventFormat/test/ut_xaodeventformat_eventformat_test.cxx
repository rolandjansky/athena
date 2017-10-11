/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODEventFormat/EventFormat.h"

/// The name of the application
static const char* APP_NAME = "ut_xaodeventformat_eventformat_test";

/// Simple macro for checking boolean expressions
#define SIMPLE_ASSERT( EXP )                                \
   do {                                                     \
      const bool ret = EXP;                                 \
      if( ! ret ) {                                         \
         std::cerr << APP_NAME << " " __FILE__ << ":"       \
                   << __LINE__ << " ("                      \
                   << __PRETTY_FUNCTION__ << ") Failed to " \
                   << "evaluate: \"" << #EXP << "\""        \
                   << std::endl;                            \
         return 1;                                          \
      }                                                     \
   } while( 0 )

int main() {

   // Create the object to use in the tests:
   xAOD::EventFormat ef;

   // Add some simple elements to it:
   ef.add( xAOD::EventFormatElement( "SGKey1", "xAOD::Type1", "Bla", 0x101 ) );
   ef.add( xAOD::EventFormatElement( "SGKey2", "xAOD::Type2", "Bla", 0x102 ) );

   // Some simple checks:
   SIMPLE_ASSERT( ef.exists( "SGKey1" ) == true );
   SIMPLE_ASSERT( ef.exists( 0x102 ) == true );
   SIMPLE_ASSERT( ef.exists( "SGKey3" ) == false );
   SIMPLE_ASSERT( ef.exists( 0x123 ) == false );
   SIMPLE_ASSERT( ef.get( 0x101 ) );
   SIMPLE_ASSERT( ef.get( "SGKey2" ) );
   SIMPLE_ASSERT( ef.get( 0x101 )->branchName() == "SGKey1" );
   SIMPLE_ASSERT( ef.get( "SGKey1" )->className() == "xAOD::Type1" );
   SIMPLE_ASSERT( ef.get( "SGKey2" )->hash() == 0x102 );
   SIMPLE_ASSERT( ef.get( 0x102 )->parentName() == "Bla" );

   // And finally print it:
   std::cout << ef << std::endl;

   // Return gracefully:
   return 0;
}
