/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "PixelCoralClientUtils/CoralClient.hh"

int main (int argc, char **argv){
  //printf("CORAL-fill starting \n");

    CAN::SerialNumber_t anal_id = 123456789;
    if (argc > 1)  anal_id = atoi(argv[1]);


  // --------------------------------------------------
  // Main part
  // --------------------------------------------------

  try {

    // Start CORAL client
    printf("CORAL-fill starting \n");
//     PixCoralClient coralClient(coralConnStr,app.context());
    const char* tableName = argc>2 ? "CALIB_DATA" : "CALIB_ANAL";
    PixCoralClient coralClient(true, coral::Update, tableName);

    if (argc>2) {
      coralClient.fillTables(argv[2]);
    } else {
      CAN::AnalysisResultList_t results;
      results.addValue("a_value","ROD_test",false);
      results.addValue("Status","Some module",true);
      results.addValue("Status","Other module",false);
      results.addValue<float>("aFloat","Other module",111);
      results.addValue<unsigned int>("Myint","New module",43);
      CAN::AverageResult_t avRes(3,13.4,2.3,2,50.2);
      results.addValue("FirstAv","AV module",avRes);

      PixelMap_t pixelmap;
      pixelmap[std::make_pair(2,3)]=4; //CLA
      pixelmap[std::make_pair(3,3)]=4;
      pixelmap[std::make_pair(3,23)]=4;
// special column pairs
//     for (int i=0; i<160; ++i) {
//       pixelmap.push_back(CAN::Pixel_t(i,0,3));
//       pixelmap.push_back(CAN::Pixel_t(i,1,3));
//       pixelmap.push_back(CAN::Pixel_t(i,24,5));
//       pixelmap.push_back(CAN::Pixel_t(i,25,5));
//     }
// special chip
//     for (int i=160; i<320; ++i) {
//       for (int j=36; j<54; ++j) {
// 	pixelmap.push_back(CAN::Pixel_t(i,j,6));
//       }
//     }
// special module
//     for (int i=0; i<320; ++i) {
//       for (int j=0; j<144; ++j) {
// 	pixelmap.push_back(CAN::Pixel_t(i,j,7));
//       }
//     }
      results.addValue("Map","this module",pixelmap);
      coralClient.fillTables(anal_id, &results);
    }

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
                                                      



