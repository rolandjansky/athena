/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
// @file Cool2Json_test.cxx
// Cool2Json test executable, in Boost test framework
// @author Shaun Roe
// @date June 2019


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#include <boost/test/unit_test.hpp>
//
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IAddressCreator.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
//
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolApplication/DatabaseSvcFactory.h"
#include "CoolKernel/Record.h"
//
//#include "../src/IOVDbFolder.h"
#include "../src/IOVDbConn.h"
#include "../src/Cool2Json.h"
#include "../src/IOVDbParser.h"

//
#include "GaudiKernelFixtureBase.h"

using namespace IOVDbNamespace;
struct GaudiKernelFixture:public GaudiKernelFixtureBase{
  GaudiKernelFixture():GaudiKernelFixtureBase(){
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
    unlink ("Cool2JsonTest.db");
    cool::IDatabaseSvc& dbSvc=cool::DatabaseSvcFactory::databaseService();
    coolDb = dbSvc.createDatabase("sqlite://;schema=Cool2JsonTest.db;dbname=OFLP200");
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

struct IOVDbConnFixture{
  ServiceHandle<IMessageSvc> msgSvc;
  const std::string connectionString;
  MsgStream log;
  IOVDbConn connection;
  IOVDbConnFixture():msgSvc("msgSvc","test"),
   connectionString("sqlite://;schema=Cool2JsonTest.db;dbname=OFLP200"),
   log(msgSvc.get(), "Cool2Json_test"),
   connection(connectionString, true, log){
  }
};

struct IOVDbParserFixture{
  ServiceHandle<IMessageSvc> msgSvc;
  const std::string descriptionString;
  MsgStream log;
  IOVDbParser parser;
  IOVDbParserFixture():msgSvc("msgSvc","test"),
   descriptionString{"/key1<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"},
   log(msgSvc.get(), "Cool2Json_test"),
   parser(descriptionString, log){
  }
};


BOOST_FIXTURE_TEST_SUITE(Cool2JsonTestSetup , GaudiKernelFixture)
  BOOST_AUTO_TEST_CASE( SanityCheck ){
    BOOST_TEST(svcLoc!=nullptr);
  }
  //pre-requisites
  IOVDbConnFixture connectionFixture;
  IOVDbParserFixture parserFixture;
  //need IClassIDSvc
  ServiceHandle<IClassIDSvc> clidSvc("ClassIDSvc","test");
  BOOST_FIXTURE_TEST_SUITE(Cool2JsonTest, TestFolderFixture)
    BOOST_AUTO_TEST_CASE(Construction){
      cool::ChannelSelection selection(0,10);
      const std::string folderTag{};
      BOOST_CHECK_NO_THROW(Cool2Json(folderPtr, (10ull<<32) + 10, (10ull<<32) + 20, selection, folderTag));
    }
    BOOST_AUTO_TEST_CASE(PublicMethods){
      //simple returns
      cool::ChannelSelection selection(0,10);
      const std::string folderTag{};
      Cool2Json testObject(folderPtr, (10ull<<32) + 10, (10ull<<32) + 20, selection, folderTag);
      BOOST_TEST(testObject.open() == "{");
      BOOST_TEST(testObject.close() == "}");
      BOOST_TEST(testObject.delimiter() == ", ");
      std::string referenceDescription=R"delim("node_description" : "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>")delim";
      BOOST_CHECK(testObject.description() == referenceDescription);
      std::string referenceSpec=R"delim("folder_payloadspec": "int: Int32")delim";
      BOOST_CHECK(testObject.payloadSpec() == referenceSpec);
      //careful about editing, the following is a multi-line raw string literal:
      std::string referencePayload = R"delim("data_array" : [{ "0" : [ 1]}])delim";
      BOOST_CHECK(testObject.payload() ==  referencePayload);
      std::string referenceIov = R"delim("iov" : [42949672970, 42949672980])delim";
      BOOST_CHECK(testObject.iov() == referenceIov);
      BOOST_CHECK(testObject.nchans() == 1);
      BOOST_CHECK(testObject.iovBase() == "run-lumi");
      BOOST_CHECK(testObject.tag() == "");
      
    }
  BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
