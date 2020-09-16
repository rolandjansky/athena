/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "PixelCoralClientUtils/CoralClient.hh"

int main (int argc, char **argv){
//printf("COOL-create starting \n");


  // --------------------------------------------------
  // Main part
  // --------------------------------------------------

  try {

    // Start CORAL client
    printf("CORAL-create starting \n");
//     PixCoralClient coralClient(coralConnStr,app.context());
    const char* tableName = argc>1 ? "CALIB_DATA" : "CALIB_ANAL";
    PixCoralClient coralClient(true, coral::Update, tableName);

    //coralClient.dropTables();
    if (argc>1) {
      coralClient.createTables(argv[1]);
    } else {
      coralClient.createTables();
    }
    //    coralClient.fillCoralTables();
    std::cout << "DONE" << std::endl;
  }

  // COOL, CORAL POOL exceptions inherit from std exceptions: catching
  // std::exception will catch all errors from COOL, CORAL and POOL

  catch ( std::exception& e ) {
    std::cout << "std::exception caught: " << e.what() << std::endl;
    return -1;
  }

  catch (...) {
    std::cout << "Unknown exception caught!" << std::endl;
    return -1;
  }

  return 0;

}
                                                      



