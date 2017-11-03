/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <exception>
#include "TestDriver.h"

int main( int argc, char** ) {
  try {
    std::cout << "[OVAL] Creating the test driver." << std::endl;
    TestDriver driver;

    std::cout << "[OVAL] Loading the shared libraries." << std::endl;
    std::vector< std::string > libraries;
    libraries.push_back( "test_StorageSvc_AuxStoreDict" );
    driver.loadLibraries( libraries );

    if( argc == 1 ) {
       std::cout << "[OVAL] Testing the writing operations" << std::endl;
       driver.testWriting();
       std::cout << "[OVAL] ...done" << std::endl;
    }
    std::cout << "[OVAL] Testing the reading operations" << std::endl;
    driver.testReading();
    std::cout << "[OVAL] ...done" << std::endl;
  }
  catch ( std::exception& e ) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  std::cout << "[OVAL] Exiting..." << std::endl;
  return 0;
}
