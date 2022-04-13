/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "EventInfoMgt/ITagInfoMgr.h"
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
#include "../src/IOVDbStringFunctions.h"
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
  ServiceHandle<ITagInfoMgr> tagInfoMgr;
  const std::string fixtureFoldername;
  const std::string invalidFoldername;
  cool::IFolderPtr folderPtr;
  cool::IDatabasePtr coolDb;
  cool::RecordSpecification spec;
  TestFolderFixture():iovdbsvc("IOVDbSvc", "test"), tagInfoMgr("TagInfoMgr",""), fixtureFoldername("/key1"),invalidFoldername("nonsense") {
    if (not iovdbsvc.retrieve().isSuccess()){
      throw (std::runtime_error("IOVDbSvc could not be retrieved in the TestFolderFixture"));
    }
    if (not tagInfoMgr.retrieve().isSuccess()){
      throw (std::runtime_error("tagInfoMgr could not be retrieved in the TestFolderFixture"));
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
    BOOST_TEST(svcLoc!=nullptr);
  }
  //
  BOOST_FIXTURE_TEST_SUITE(TagFunctionsTest, TestFolderFixture)
    BOOST_AUTO_TEST_CASE(AllFunctions){
      BOOST_TEST(getTagInfo("dummy", tagInfoMgr.get()).empty(),"Getting tag info for a non existent tag but valid tagInfoMgr should return an empty string");
      BOOST_TEST(getTagInfo("dummy",nullptr) == "","Getting tag info for a non existent tag and null tagInfoMgr should return an empty string");

      using namespace IOVDbNamespace;
      const std::string dummyMagicTag{"TagInfoMajor/AtlasLayerMat_v21_/Dummy"};
      const std::string dummyTagName = tag2PrefixTarget( parseMagicTag(dummyMagicTag) ).second;
      const std::string trialMagicTag{"TagInfoMajor/AtlasLayerMat_v21_/Trial"};
      const std::string trialTagName = tag2PrefixTarget( parseMagicTag(trialMagicTag) ).second;
     
      const std::string empty{};
      const std::string someTag{"MyTag_6Char"};
      tagInfoMgr->addTag(dummyTagName, empty).ignore();
      tagInfoMgr->addTag(trialTagName, someTag).ignore();
      
      BOOST_TEST_MESSAGE("Calling resolveUsingTagInfo(..) with dummy magic tag should throw (test mode: TagInfo object passed in)");
      BOOST_CHECK_THROW(resolveUsingTagInfo(dummyMagicTag, tagInfoMgr.get()), std::runtime_error);
      BOOST_TEST(resolveUsingTagInfo(trialMagicTag,  tagInfoMgr.get()) == "AtlasLayerMat_v21_MyTag", "resolveUsingTagInfo(..) with magic tag returns valid string (test mode)");
      BOOST_TEST(!checkTagLock(folderPtr,trialMagicTag).has_value(), "checkTagLock(..) doesn't have a value if there are no tags in the db");
      BOOST_TEST_MESSAGE("getGeoAtlasVersion() throws if the GeoModelSvc has not been set up");
      BOOST_CHECK_THROW(getGeoAtlasVersion(),std::runtime_error); //cant get
      //@todo : Create database with tags to check valid returns on these methods
    }
  BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

