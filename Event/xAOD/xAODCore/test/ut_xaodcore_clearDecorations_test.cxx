/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodcore_clearDecorations_test.cxx 696772 2015-09-25 08:09:13Z krasznaa $

// System include(s):
#undef NDEBUG
#include <iostream>

// Core include(s):
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"

// Local include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODCore/tools/PrintHelpers.h"

/// Convenience function to print the payload of the test container
template< class CONTAINER >
void printContainer( const CONTAINER& c ) {

   for( const auto* obj : c ) {
      std::cout << *obj << std::endl;
   }

   return;
}

int main() {

   // Create the container to be tested:
   DataVector< SG::AuxElement > interface;
   xAOD::AuxContainerBase aux;
   interface.setStore( &aux );

   // Add some simple objects to it:
   for( int i = 0; i < 3; ++i ) {
      SG::AuxElement* obj = new SG::AuxElement();
      interface.push_back( obj );
      obj->auxdata< int >( "IntVar1" ) = i;
      obj->auxdata< float >( "FloatVar1" ) = static_cast< float >( i );
   }

   // Print what it looks like now:
   std::cout << "Container contents at the start:\n" << std::endl;
   printContainer( interface );

   // Clear the decorations, and see what happens:
   assert (interface.clearDecorations() == false);
   std::cout << "\nContainer contents after clearDecorations():\n" << std::endl;
   printContainer( interface );

   // Lock the container:
   interface.lock();

   // Create some decorations:
   for( const SG::AuxElement* obj : interface ) {

      obj->auxdecor< int >( "IntVar2" ) = 2;
      obj->auxdecor< float >( "FloatVar2" ) = 3.141592;
   }

   // Print what it looks like now:
   std::cout << "\nContainer contents after decoration:\n" << std::endl;
   printContainer( interface );

   // Clear the decorations, and see what happens:
   assert (interface.clearDecorations() == true);
   std::cout << "\nContainer contents after clearDecorations():\n" << std::endl;
   printContainer( interface );

   return 0;
}
