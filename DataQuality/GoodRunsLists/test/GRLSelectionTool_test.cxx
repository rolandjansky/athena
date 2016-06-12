/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GRLSelectionTool_test.cxx 751473 2016-06-01 13:54:05Z krasznaa $

// System include(s):
#include <iostream>

// Framework include(s):
#ifndef XAOD_STANDALONE
#   include "TestTools/initGaudi.h"
#endif // not XAOD_STANDALONE

// Local include(s):
#include "GoodRunsLists/GoodRunsListSelectionTool.h"

/// Helper macro for the tests
#define SIMPLE_ASSERT( EXP )                                      \
   do {                                                           \
      const bool result = EXP;                                    \
      if( ! result ) {                                            \
         std::cerr << "Failed to execute: " << #EXP << std::endl; \
         return 1;                                                \
      }                                                           \
   } while( 0 )

int main() {

#ifndef XAOD_STANDALONE
   // Set up Gaudi:
   ISvcLocator* svcLoc = 0;
   SIMPLE_ASSERT( Athena_test::initGaudi( svcLoc ) );
#endif // not XAOD_STANDALONE

   // Create the tool to be tested:
   GoodRunsListSelectionTool tool;

   // Set its properties:
   static const std::vector< std::string > grls = {
      "GoodRunsLists/data12_8TeV.periodAllYear_DetStatus-v54-pro13-04"
      "_DQDefects-00-00-33_PHYS_StandardGRL_All_Good.xml"
   };
   SIMPLE_ASSERT( tool.setProperty( "GoodRunsListVec", grls ).isSuccess() );

   // And finally initialise it:
   SIMPLE_ASSERT( tool.initialize().isSuccess() );

   // Ask some questions from it:
   SIMPLE_ASSERT( tool.passRunLB( 200842, 30 ) == true );
   SIMPLE_ASSERT( tool.passRunLB( 200842, 40 ) == false );

   // Return gracefully:
   return 0;
}
