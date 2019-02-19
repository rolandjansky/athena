/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ut_xAODObjectType_test.cxx 618658 2014-09-26 09:31:10Z krasznaa $
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
   std::cout << xAODType::Electron << ", " << xAODType::CaloCluster
             << std::endl;
   std::cout << static_cast< xAODType::ObjectType >( 1500 ) << std::endl;

   // Return gracefully:
   return 0;
}
