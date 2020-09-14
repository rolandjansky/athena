/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "PixelCoralClientUtils/CoralClient.hh"

int main (int argc, char **argv){
  //printf("CORAL-create starting \n");

    CAN::SerialNumber_t anal_id = 123456789;
    if (argc > 1)  anal_id = atoi(argv[1]);


  // --------------------------------------------------
  // Main part
  // --------------------------------------------------

  try {

    // Start CORAL client
    printf("CORAL-read starting \n");
//     PixCoralClient coralClient(coralConnStr,app.context());
    const char* tableName = argc>2 ? "CALIB_DATA" : "CALIB_ANAL";
    PixCoralClient coralClient(true, coral::ReadOnly, tableName);

    if (argc>2) {

    // ------------------------------------------------
    // read PCD table -- A.X.
    // ------------------------------------------------

    // Show the structure of the master table
    try {
      coralClient.printTableDesc("CALIB_DATA");
      coralClient.printTableContent("CALIB_DATA");
    } catch (coral::TableNotExistingException& err) {
      std::cout<<"TableNotExistingException: "<<err.what()<<std::endl;
    }

    // Get all results for given tag/revision
    try {
      coralClient.getCalibrationDataFromDB(argv[2]);
    } catch (coral::TableNotExistingException& err) {
      std::cout<<"TableNotExistingException: "<<err.what()<<std::endl;
    }

    } else {

    // ------------------------------------------------
    // old functionality -- A.X.
    // ------------------------------------------------

    // Show the structure of the master table
    try {
      coralClient.printTableDesc("CALIB_ANAL");
      coralClient.printTableContent("CALIB_ANAL");
    } catch (coral::TableNotExistingException& err) {
      std::cout<<"TableNotExistingException: "<<err.what()<<std::endl;
    }

    // Get all results for analysis anal_id
    try {
      // Print content on the screen
//       coralClient.queryTable(anal_id);
      // Return a CAN::AnalysisResultList_t object
      coralClient.getAnalysisResultsFromDB(anal_id);
    } catch (coral::TableNotExistingException& err) {
      std::cout<<"TableNotExistingException: "<<err.what()<<std::endl;
    }

    // Get results for variable FirstAv and analysis anal_id
    try {
      // Print content on the screen
//       coralClient.queryTable(anal_id,"FirstAv");
      // Return a CAN::AnalysisResultList_t object
      coralClient.getAnalysisResultsFromDB(anal_id,"FirstAv");
    } catch (coral::TableNotExistingException& err) {
      std::cout<<"TableNotExistingException: "<<err.what()<<std::endl;
    }

    // Get results for module L0_B08_S1_A6_M6A, variable Status
    //  and analysis anal_id
    try {
      // Print content on the screen
//       coralClient.queryTable(anal_id,"Status","L0_B08_S1_A6_M6A");
      // Return a CAN::AnalysisResultList_t object
      coralClient.getAnalysisResultsFromDB(anal_id,"Status","L0_B08_S1_A6_M6A");
    } catch (coral::TableNotExistingException& err) {
      std::cout<<"TableNotExistingException: "<<err.what()<<std::endl;
    }

    // Get results for vector of modules, variable ASpecialPixelMap
    //  and analysis anal_id
    try {
      // Return a CAN::AnalysisResultList_t object
      std::vector<std::string> mods;
      mods.push_back("L2_B25_S2_C6_M4C");
      mods.push_back("L2_B25_S1_C7_M2C");
      coralClient.getAnalysisResultsFromDB(anal_id,mods,"ASpecialPixelMap");
    } catch (coral::TableNotExistingException& err) {
      std::cout<<"TableNotExistingException: "<<err.what()<<std::endl;
    }

    } // endif -- A.X.

  // COOL, CORAL POOL exceptions inherit from std exceptions: catching
  // std::exception will catch all errors from COOL, CORAL and POOL
  } catch ( std::exception& e ) {
    std::cout << "std::exception caught: " << e.what() << std::endl;
    return -1;
  }

  catch (...) {
    std::cout << "Unknown exception caught!" << std::endl;
    return -1;
  }

  return 0;

}
                                                      



