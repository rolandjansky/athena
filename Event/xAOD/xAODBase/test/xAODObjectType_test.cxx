/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODObjectType_test.cxx 592971 2014-04-15 14:13:50Z krasznaa $
//
// This is an extremely simple test to check the health of the output operator
// defined in xAODBase/ObjectType.h.
///

// System include(s):
#include <iostream>

// Local include(s):
#include "xAODBase/ObjectType.h"

int main() {

   // Print some random values:
   std::cout << xAOD::Type::Electron << ", " << xAOD::Type::CaloCluster
             << std::endl;
   std::cout << static_cast< xAOD::Type::ObjectType >( 1500 ) << std::endl;

   // Return gracefully:
   return 0;
}
