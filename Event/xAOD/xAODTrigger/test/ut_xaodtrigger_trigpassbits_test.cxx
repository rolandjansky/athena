/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>
#include <stdexcept>
#include <vector>

// Local include(s):
#include "xAODTrigger/TrigPassBits.h"

/// Helper macro for testing the code
#define SIMPLE_ASSERT( EXP )                                   \
   do {                                                        \
      const bool result = EXP;                                 \
      if( ! result ) {                                         \
         std::cerr << __FILE__ << ":" << __LINE__              \
                   << " Failed test: " << #EXP << std::endl;   \
         throw std::runtime_error( "Failed " #EXP );           \
      }                                                        \
   } while( 0 )

/// Helper struct
struct StructA {};
CLASS_DEF( std::vector< StructA* >, 12345678, 1 )

/// Testing the non-templated functions
void simpleFunctions() {

   // Create an object to test:
   xAOD::TrigPassBits bits;
   bits.makePrivateStore();

   // Initialise it:
   bits.setSize( 40 );
   bits.setPassBits( std::vector< uint32_t >( 2, 0 ) );

   // Run some simple tests on it:
   bits.markPassing( 20, true );
   for( int i = 0; i < 40; ++i ) {
      SIMPLE_ASSERT( bits.isPassing( i ) == ( i == 20 ) );
   }
   bits.markPassing( 30, true );
   for( int i = 0; i < 40; ++i ) {
      SIMPLE_ASSERT( bits.isPassing( i ) == ( i == 20 ||
                                              i == 30 ) );
   }
   bits.markPassing( 20, false );
   for( int i = 0; i < 40; ++i ) {
      SIMPLE_ASSERT( bits.isPassing( i ) == ( i == 30 ) );
   }

   // Test some failure modes:
   bool exceptionThrown = false;
   try {
      bits.markPassing( 50 );
   } catch( const std::exception& ) {
      exceptionThrown = true;
   }
   SIMPLE_ASSERT( exceptionThrown == true );
   exceptionThrown = false;
   try {
      bits.isPassing( 60 );
   } catch( const std::exception& ) {
      exceptionThrown = true;
   }
   SIMPLE_ASSERT( exceptionThrown == true );
   exceptionThrown = false;
   bits.setPassBits( std::vector< uint32_t >( 1, 0 ) );
   try {
      bits.markPassing( 35 );
   } catch( const std::exception& ) {
      exceptionThrown = true;
   }
   SIMPLE_ASSERT( exceptionThrown == true );

   return;
}

/// Testing the templated functions
void containerFunctions() {

   // Create a container emulating a DataVector:
   std::vector< StructA > structs;
   std::vector< StructA* > toyDV;
   for( int i = 0; i < 50; ++i ) {
      structs.push_back( StructA() );
      toyDV.push_back( &( structs.back() ) );
   }

   // Create an object to test:
   xAOD::TrigPassBits bits;
   bits.makePrivateStore();

   // Initialise it:
   bits.reset( &toyDV, 0x1234 );

   // Check that the initialisation succeeded:
   SIMPLE_ASSERT( bits.size() == 50 );
   SIMPLE_ASSERT( bits.passBits().size() == 2 );
   SIMPLE_ASSERT( bits.containerKey() == 0x1234 );
   SIMPLE_ASSERT( bits.containerClid() == 12345678 );

   // Run some simple tests:
   bits.markPassing( toyDV[ 20 ], &toyDV, true );
   for( int i = 0; i < 50; ++i ) {
      SIMPLE_ASSERT( bits.isPassing( toyDV[ i ], &toyDV ) == ( i == 20 ) );
   }
   bits.markPassing( toyDV[ 30 ], &toyDV, true );
   for( int i = 0; i < 50; ++i ) {
      SIMPLE_ASSERT( bits.isPassing( toyDV[ i ], &toyDV ) == ( i == 20 ||
                                                               i == 30 ) );
   }
   bits.markPassing( toyDV[ 20 ], &toyDV, false );
   for( int i = 0; i < 50; ++i ) {
      SIMPLE_ASSERT( bits.isPassing( toyDV[ i ], &toyDV ) == ( i == 30 ) );
   }

   // Test the reset:
   bits.reset( &toyDV );
   for( int i = 0; i < 50; ++i ) {
      SIMPLE_ASSERT( bits.isPassing( toyDV[ i ], &toyDV ) == false );
   }

   // Test some failure modes:
   bool exceptionThrown = false;
   try {
      bits.markPassing( ( StructA* ) 0x12345678, &toyDV );
   } catch( const std::exception& ) {
      exceptionThrown = true;
   }
   SIMPLE_ASSERT( exceptionThrown == true );
   exceptionThrown = false;
   try {
      bits.isPassing( ( StructA* ) 0x12345678, &toyDV );
   } catch( const std::exception& ) {
      exceptionThrown = true;
   }
   SIMPLE_ASSERT( exceptionThrown == true );

   // Test initialising the object to different container sizes:
   toyDV.resize( 0, 0 );
   bits.reset( &toyDV );
   SIMPLE_ASSERT( bits.size() == 0 );
   SIMPLE_ASSERT( bits.passBits().size() == 0 );
   toyDV.resize( 20, 0 );
   bits.reset( &toyDV );
   SIMPLE_ASSERT( bits.size() == 20 );
   SIMPLE_ASSERT( bits.passBits().size() == 1 );
   toyDV.resize( 32, 0 );
   bits.reset( &toyDV );
   SIMPLE_ASSERT( bits.size() == 32 );
   SIMPLE_ASSERT( bits.passBits().size() == 1 );
   toyDV.resize( 64, 0 );
   bits.reset( &toyDV );
   SIMPLE_ASSERT( bits.size() == 64 );
   SIMPLE_ASSERT( bits.passBits().size() == 2 );
   toyDV.resize( 65, 0 );
   bits.reset( &toyDV );
   SIMPLE_ASSERT( bits.size() == 65 );
   SIMPLE_ASSERT( bits.passBits().size() == 3 );

   return;
}

/// Testing the templated helper functions
void helperFunctions() {

   // Create a container emulating a DataVector:
   std::vector< StructA > structs;
   std::vector< StructA* > toyDV;
   for( int i = 0; i < 50; ++i ) {
      structs.push_back( StructA() );
      toyDV.push_back( &( structs.back() ) );
   }

   // Construct a TrigPassBits object describing it:
   std::unique_ptr< xAOD::TrigPassBits > bits =
      xAOD::makeTrigPassBits( &toyDV, 0x1234 );

   // Check that the initialisation succeeded:
   SIMPLE_ASSERT( bits->size() == 50 );
   SIMPLE_ASSERT( bits->passBits().size() == 2 );
   SIMPLE_ASSERT( bits->containerKey() == 0x1234 );
   SIMPLE_ASSERT( bits->containerClid() == 12345678 );

   return;
}

int main() {

   // Run the tests:
   simpleFunctions();
   containerFunctions();
   helperFunctions();

   // Return gracefully:
   return 0;
}
