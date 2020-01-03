//
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "helpers.h"
#include "../Root/RDataSource.h"

// xAOD include(s).
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/Message.h"

// ROOT include(s).
#include <TError.h>

// System include(s).
#include <iostream>

int main() {

   // Set up the runtime environment.
   CHECK( xAOD::Init() );

   // Set up the data source.
   xAOD::RDataSource ds( "${ASG_TEST_FILE_DATA}" );
   ds.Initialise();

   // Print the ideal entry ranges to process.
   std::cout << ds.GetEntryRanges() << std::endl;
   // Print the available column/object names.
   std::cout << ds.GetColumnNames() << std::endl;

   // Return gracefully.
   return 0;
}
