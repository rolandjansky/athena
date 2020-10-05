/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s).
#include "xAODEventFormat/EventFormat.h"

// System include(s).
#undef NDEBUG
#include <cassert>
#include <iostream>

void test1( const xAOD::EventFormat& ef ) {

   // Some simple checks:
   assert( ef.exists( "SGKey1" ) == true );
   assert( ef.exists( 0x102 ) == true );
   assert( ef.exists( "SGKey3" ) == false );
   assert( ef.exists( 0x123 ) == false );
   assert( ef.get( 0x101 ) );
   assert( ef.get( "SGKey2" ) );
   assert( ef.get( 0x101 )->branchName() == "SGKey1" );
   assert( ef.get( "SGKey1" )->className() == "xAOD::Type1" );
   assert( ef.get( "SGKey2" )->hash() == 0x102 );
   assert( ef.get( 0x102 )->parentName() == "Bla" );
}

int main() {

   // Create the object to use in the tests:
   xAOD::EventFormat ef;

   // Add some simple elements to it:
   ef.add( xAOD::EventFormatElement( "SGKey1", "xAOD::Type1", "Bla", 0x101 ) );
   ef.add( xAOD::EventFormatElement( "SGKey2", "xAOD::Type2", "Bla", 0x102 ) );

   // Execute simple tests on it.
   test1( ef );

   // Test that the object can be copied correctly using the automatically
   // generated constructors/copy operators.
   xAOD::EventFormat copy1( ef );
   xAOD::EventFormat copy2;
   copy2 = ef;

   // Test the two copies.
   test1( copy1 );
   test1( copy2 );

   // And finally print the object.
   std::cout << ef << std::endl;

   // Return gracefully:
   return 0;
}
