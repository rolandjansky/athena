/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/TagFunctions_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for Tag utility functions in the Boost framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#include <boost/test/unit_test.hpp>
//
#include "AthenaKernel/IIOVDbSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CoolApplication/DatabaseSvcFactory.h"
#include "CoolKernel/pointers.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/IFolder.h"
//
#include "../src/TagFunctions.h"
#include "GaudiKernelFixtureBase.h" 

using namespace IOVDbNamespace;

struct GaudiKernelFixture:public GaudiKernelFixtureBase{
  GaudiKernelFixture():GaudiKernelFixtureBase(__FILE__){
    //nop, everything in base.
  }
};

//note destructor is separate here
struct TestFolderFixture{
  ServiceHandle<IIOVDbSvc> iovdbsvc;
  ServiceHandle<StoreGateSvc> detStore;
  const std::string fixtureFoldername;
  const std::string invalidFoldername;
  cool::IFolderPtr folderPtr;
  cool::IDatabasePtr coolDb;
  cool::RecordSpecification spec;
  TestFolderFixture():iovdbsvc("IOVDbSvc", "test"),detStore("DetectorStore",""),fixtureFoldername("/key1"),invalidFoldername("nonsense") {
    if (not iovdbsvc.retrieve().isSuccess()){
      throw (std::runtime_error("IOVDbSvc could not be retrieved in the TestFolderFixture"));
    }
    if (not detStore.retrieve().isSuccess()){
      throw (std::runtime_error("detStore could not be retrieved in the TestFolderFixture"));
    }
    unlink ("TagFunctionsTest.db");
    cool::IDatabaseSvc& dbSvc=cool::DatabaseSvcFactory::databaseService();
    coolDb = dbSvc.createDatabase("sqlite://;schema=TagFunctionsTest.db;dbname=OFLP200");
    spec.extend ("int", cool::StorageType::Int32);
    cool::FolderSpecification fSpec (cool::FolderVersioning::SINGLE_VERSION,spec,cool::PayloadMode::INLINEPAYLOAD);
    std::string desc = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";
    {
      folderPtr = coolDb->createFolder (fixtureFoldername, fSpec, desc);
      cool::Record payload (folderPtr->payloadSpecification());
      payload[0].setValue (1);
      folderPtr->storeObject ((10ull<<32) + 10, (10ull<<32) + 20, payload, 0);
      payload[0].setValue (2);
      folderPtr->storeObject ((10ull<<32) + 30, cool::ValidityKeyMax, payload, 0);
    }
    
  }
  ~TestFolderFixture(){
    coolDb->closeDatabase();
  }
};


BOOST_FIXTURE_TEST_SUITE(TagFunctionsBasicTest , GaudiKernelFixture)
  BOOST_AUTO_TEST_CASE( SanityCheck ){
    BOOST_TEST(gaudiIsInitialised);
    BOOST_TEST(svcLoc!=nullptr);
  }
  //
  BOOST_FIXTURE_TEST_SUITE(TagFunctionsTest, TestFolderFixture)
    BOOST_AUTO_TEST_CASE(AllFunctions){
      BOOST_TEST(getTagInfo("dummy", detStore.get()).empty());
      BOOST_TEST(!checkTagLock(folderPtr,"dummy").has_value());
      //BOOST_TEST(getGeoAtlasVersion().empty()); cant get
    }
  BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

