/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>

// Core include(s):
#include "AsgTools/AnaToolHandle.h"

// Local include(s):
#include "PFlowUtils/IRetrievePFOTool.h"
#include "PFlowUtils/IWeightPFOTool.h"

// Helper macro:
#define SC_CHECK( EXP )                                           \
   do {                                                           \
      const auto sc = EXP;                                        \
      if( ! sc.isSuccess() ) {                                    \
         std::cerr << "Failed to execute: " << #EXP << std::endl; \
         return 1;                                                \
      }                                                           \
   } while( false )

int main() {

   // Test that CP::RetrievePFOTool can be constructed through a handle:
   asg::AnaToolHandle< CP::IRetrievePFOTool >
      retrieveTool( "CP::RetrievePFOTool/RetrievePFOTool" );
   SC_CHECK( retrieveTool.retrieve() );

   // Test that CP::WeightPFOTool can be constructed through a handle:
   asg::AnaToolHandle< CP::IWeightPFOTool >
      weightTool( "CP::WeightPFOTool/WeightPFOTool" );
   SC_CHECK( weightTool.retrieve() );

   // Return gracefully:
   return 0;
}
