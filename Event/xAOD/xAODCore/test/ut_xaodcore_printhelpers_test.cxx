/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xaodcore_printhelpers_test.cxx 646509 2015-02-12 16:43:18Z krasznaa $

// System include(s):
#include <cstdint>
#include <iostream>
#include <vector>

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxStoreInternal.h"

// Local include(s):
#include "xAODCore/tools/PrintHelpers.h"

/// Dummy type
struct TypeA {
   int m_value;
};

int main() {

   // Construct a standalone, empty object:
   SG::AuxElement empty;
   std::cout << empty << std::endl;

   // Create an object with a private store, and fill it with some variables:
   SG::AuxElement standalone;
   standalone.makePrivateStore();
   standalone.auxdata< int >( "IntValue" ) = 12;
   standalone.auxdata< float >( "FloatValue" ) = 3.14;
   standalone.auxdata< std::vector< int > >( "VecIntValue" ) =
      std::vector< int >{ 1, 2, 3 };
   standalone.auxdata< std::vector< float > >( "VecFloatValue" ) =
      std::vector< float >{ 1.2, 2.3, 3.4 };
   standalone.auxdata< TypeA >( "TypeAValue" ) = TypeA{ 5 };
   standalone.auxdata< std::vector< TypeA > >( "VecTypeAValue" ) =
      std::vector< TypeA >{ TypeA{ 6 }, TypeA{ 7 } };
   std::cout << standalone << std::endl;

   // Create a vector, and decorate its first element:
   DataVector< SG::AuxElement > vec;
   SG::AuxStoreInternal store;
   vec.setStore( &store );
   SG::AuxElement* element = new SG::AuxElement();
   vec.push_back( element );
   element->auxdata< long >( "LongValue" ) = 234;
   element->auxdata< uint32_t >( "UInt32Value" ) = 0x123;
   std::cout << *element << std::endl;

   // Finally, do one test with the dump(...) function as well:
   xAOD::dump( standalone );

   // Return gracefully:
   return 0;
}
