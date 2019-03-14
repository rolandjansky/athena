/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <exception>
#include "TestDriver.h"

#include "StorageSvc/DbType.h"
#include "StorageSvc/DbInstanceCount.h"
#include "PersistentDataModel/Token.h"

void testTechnology( TestDriver& driver, const pool::DbType& tech )
{
   driver.m_storageType = tech;
   std::cout << "[OVAL] Testing StorageSvc functionality for " << tech.storageName()
             << " storage technology" << std::endl;
   std::cout << "[OVAL] Testing the writing operations" << std::endl;
   driver.testWriting();
   std::cout << "[OVAL] ...done" << std::endl;

   std::cout << "[OVAL] Testing the reading operations" << std::endl;
   driver.testReading();
   std::cout << "[OVAL] ...done" << std::endl;
}


int main( int, char** )
{
   pool::DbInstanceCount::doTracing( true );
   try {
      std::cout << "[OVAL] Creating the test driver." << std::endl;
      TestDriver driver;

      std::cout << "[OVAL] Loading the shared libraries." << std::endl;
      std::vector< std::string > libraries;
      libraries.push_back( "test_TestDictionaryDict" );
      driver.loadLibraries( libraries );

      testTechnology( driver, pool::ROOTTREE_StorageType );
      testTechnology( driver, pool::ROOTKEY_StorageType );
      testTechnology( driver, pool::ROOTTREEINDEX_StorageType );
   }
   catch ( std::exception& e ) {
      std::cerr << e.what() << std::endl;
      return 1;
   }

   if( Token::numInstances() == 0 ) {
      // disable debug dump
      pool::DbInstanceCount::doTracing( false );
   }

   std::cout << "[OVAL] Exiting..." << std::endl;
   return 0;
}
