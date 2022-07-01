/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <exception>
#include <string>
#include "TestDriver.h"
#include "TestTools/initGaudi.h"
#include "AthenaKernel/errorcheck.h"

int main( int argc, char** argv ) {
  errorcheck::ReportMessage::hideFunctionNames (true);

  // hardcode here in case we only test reading, remember to keep in sync with classes.xml
  std::string testTypeID = "A2222222-B111-C111-D111-E22134511111";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi (svcloc)) std::abort();
  try {
    std::cout << "[OVAL] Creating the test driver." << std::endl;
    TestDriver driver;

    std::cout << "[OVAL] Loading the shared libraries." << std::endl;
    std::vector< std::string > libraries;
    libraries.push_back( "test_StorageSvc_AuxStoreDict" );
    driver.loadLibraries( libraries );

    if( argc<2 || *argv[1] == 'w' ) {
       std::cout << "[OVAL] Testing the writing operations" << std::endl;
       testTypeID = driver.testWriting();
       std::cout << "[OVAL] ...done" << std::endl;
    }
    if( argc<2 || *argv[1] == 'r' ) {
       std::cout << "[OVAL] Testing the reading operations" << std::endl;
       driver.testReading(testTypeID);
       std::cout << "[OVAL] ...done" << std::endl;
    }
  }
  catch ( std::exception& e ) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  std::cout << "[OVAL] Exiting..." << std::endl;
  return 0;
}
