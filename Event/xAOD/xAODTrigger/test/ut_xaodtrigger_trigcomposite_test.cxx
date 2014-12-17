/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodtrigger_trigcomposite_test.cxx 631223 2014-11-26 15:46:35Z krasznaa $

// System include(s):
#include <cmath>
#include <iostream>

// EDM include(s):
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"

/// Helper macro for testing the code
#define SIMPLE_ASSERT( EXP )                                         \
   do {                                                              \
      const bool result = EXP;                                       \
      if( ! result ) {                                               \
         std::cerr << __FILE__ << ":" << __LINE__                    \
                   << "  Failed to execute: " << #EXP << std::endl;  \
         return 1;                                                   \
      }                                                              \
   } while( 0 )

/// Helper print operator
template< typename T >
std::ostream& operator<< ( std::ostream& out, const std::vector< T >& vec ) {

   out << "[";
   for( size_t i = 0; i < vec.size(); ++i ) {
      out << vec[ i ];
      if( ( i + 1 ) < vec.size() ) {
         out << ", ";
      }
   }
   out << "]";
   return out;
}

int main() {

   // Create the container that we want to test:
   xAOD::TrigCompositeAuxContainer aux;
   xAOD::TrigCompositeContainer c;
   c.setStore( &aux );

   // Create a test object:
   xAOD::TrigComposite* obj = new xAOD::TrigComposite();
   c.push_back( obj );

   // Set some simple properties on it:
   obj->setName( "TestObj" );
   obj->setDetail( "IntValue", 12 );
   obj->setDetail( "FloatValue", 3.14f );
   obj->setDetail( "IntVecValue", std::vector< int >( { 1, 2, 3 } ) );
   obj->setDetail( "FloatVecValue", std::vector< float >( { 1.23, 2.34 } ) );

   // Check them:
   SIMPLE_ASSERT( obj->name() == "TestObj" );
   int intValue = 0;
   SIMPLE_ASSERT( obj->getDetail( "IntValue", intValue ) );
   SIMPLE_ASSERT( intValue == 12 );
   float floatValue = 0;
   SIMPLE_ASSERT( obj->getDetail( "FloatValue", floatValue ) );
   SIMPLE_ASSERT( std::abs( floatValue - 3.14 ) < 0.001 );
   std::vector< int > intVector;
   SIMPLE_ASSERT( obj->getDetail( "IntVecValue", intVector ) );
   SIMPLE_ASSERT( intVector == std::vector< int >( { 1, 2, 3 } ) );
   std::vector< float > floatVector;
   SIMPLE_ASSERT( obj->getDetail( "FloatVecValue", floatVector ) );

   // Simply just print the last one:
   std::cout << "FloatVecValue = " << floatVector << std::endl;

   // Now test the ElementLink functionality in a basic way:
   obj->setObjectLink( "MuonRoI",
                       ElementLink< xAOD::MuonRoIContainer >( 123, 456 ) );
   SIMPLE_ASSERT( obj->hasObjectLink( "MuonRoI" ) );
   SIMPLE_ASSERT( obj->linkColNames().size() == 1 );
   SIMPLE_ASSERT( obj->linkColKeys().size() == 1 );
   SIMPLE_ASSERT( obj->linkColIndices().size() == 1 );
   SIMPLE_ASSERT( obj->linkColClids().size() == 1 );
   SIMPLE_ASSERT( obj->linkColKeys()[ 0 ] == 123 );
   SIMPLE_ASSERT( obj->linkColIndices()[ 0 ] == 456 );
   SIMPLE_ASSERT( obj->linkColClids()[ 0 ] ==
                  ClassID_traits< xAOD::MuonRoIContainer >::ID() );

   // Apparently everything went well:
   std::cout << "All tests successful." << std::endl;

   // Return gracefully:
   return 0;
}
