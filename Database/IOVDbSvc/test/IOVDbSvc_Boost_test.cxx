/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/IOVDbSvc_Boost_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for IOVDbSvc in the Boost framework; transferred from original work by Scott Snyder
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#include <boost/test/unit_test.hpp>
//
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/EventContext.h"
//
#include "GaudiKernelFixtureBase.h"
#include "TestFolderFixture.h"

struct GaudiKernelFixture:public GaudiKernelFixtureBase{
  GaudiKernelFixture():GaudiKernelFixtureBase(__FILE__){
    //nop, everything in base.
  }
};

struct TestFolderFixture:public TestFolderFixtureBase{
  TestFolderFixture():TestFolderFixtureBase("IOVDbSvc_Boost_test.db"){
    //nop, everything in base.
  }
};

//Basic tests that the service can be retrieved
BOOST_FIXTURE_TEST_SUITE(IOVDbSvcTest , GaudiKernelFixture)
  BOOST_AUTO_TEST_CASE( SanityCheck ){
    BOOST_TEST(gaudiIsInitialised);
    BOOST_TEST(svcLoc!=nullptr);
  }

  BOOST_AUTO_TEST_CASE(IOVDbSvcRetrieved){
    ServiceHandle<IIOVDbSvc> iovdbsvc ("IOVDbSvc", "test");
    BOOST_TEST (iovdbsvc.retrieve().isSuccess());
  }
  
  BOOST_AUTO_TEST_CASE(interfaceID){
    ServiceHandle<IIOVDbSvc> iovdbsvc ("IOVDbSvc", "test");
    BOOST_TEST (iovdbsvc.retrieve().isSuccess());
    InterfaceID testId("IOVDbSvc",1,0);
    BOOST_TEST(iovdbsvc->interfaceID()==testId);
  }

  BOOST_AUTO_TEST_CASE(preloadAddresses){
    ServiceHandle<IIOVDbSvc> iovdbsvc ("IOVDbSvc", "test");
    BOOST_TEST (iovdbsvc.retrieve().isSuccess());
    IAddressProvider* iovdbsvc_ap =  dynamic_cast<IAddressProvider*> (iovdbsvc.get());
    IAddressProvider::tadList tlist;
    BOOST_TEST( iovdbsvc_ap->preLoadAddresses (StoreID::DETECTOR_STORE, tlist).isSuccess() );
    
  }
  
  //tests requiring a database
  BOOST_FIXTURE_TEST_SUITE(FolderTest, TestFolderFixture)
    BOOST_AUTO_TEST_CASE(getRange){
      IOVRange range;
      std::string tag{};
      IOpaqueAddress* addr = nullptr;
      //normal folder
      BOOST_TEST_MESSAGE("Testing getRange for folder");
      BOOST_TEST( iovdbsvc->getRange (1238547719, fixtureFoldername, IOVTime (10, 15),range, tag, addr).isSuccess() );
      BOOST_TEST (std::string(range) == "{[10,10] - [10,20]}");
      BOOST_TEST(tag=="/key1");//setting this value is commented with 'FIXME' in the IOVDbSvc getRange method
      //
      BOOST_TEST( iovdbsvc->getRange (1238547719, fixtureFoldername, IOVTime (10, 35),range, tag, addr).isSuccess() );
      BOOST_TEST (std::string(range) == "{[10,30] - [2147483647,4294967295]}");
      //
      //Things which should fail
      //wrong clid
      BOOST_TEST( iovdbsvc->getRange (999, fixtureFoldername, IOVTime (10, 15),range, tag, addr).isFailure() );
      //wrong key
      BOOST_TEST( iovdbsvc->getRange (1238547719, invalidFoldername, IOVTime (10, 15),range, tag, addr).isFailure() );
      //wrong global tag (todo)
    }
    /** no need to test the setRange method; it does nothing
    BOOST_AUTO_TEST_CASE(setRange){
      BOOST_TEST( iovdbsvc->setRange (1238547719, "/key1",range,tag).isSuccess() );      
    }
    **/
    BOOST_AUTO_TEST_CASE(signalBeginRun){
      EventContext ctx;
      ctx.setExtension<Atlas::ExtendedEventContext>( Atlas::ExtendedEventContext(nullptr,10));
      BOOST_TEST( iovdbsvc->signalBeginRun(IOVTime(10,10), ctx).isSuccess());
    }
    
    BOOST_AUTO_TEST_CASE(signalEndProxyPreload){
      BOOST_CHECK_NO_THROW( iovdbsvc->signalEndProxyPreload());
    }
    
    BOOST_AUTO_TEST_CASE(getKeyList){
      auto keyList=iovdbsvc->getKeyList();
      BOOST_TEST(keyList.size() ==1);
      auto firstEntry=keyList.front();
      BOOST_TEST(firstEntry == fixtureFoldername);
    }
    
    BOOST_AUTO_TEST_CASE(getKeyInfo){
      std::string foldername{};
      std::string tag{"dummy"};
      IOVRange range;
      bool retrieved{};
      unsigned long long bytesRead(0);
      float readTime(0.);
      BOOST_TEST( iovdbsvc->getKeyInfo(fixtureFoldername, foldername, tag, range, retrieved,bytesRead, readTime));
      std::string rangeString(range);

      BOOST_TEST(fixtureFoldername == foldername);
      BOOST_TEST(tag.empty());
      BOOST_TEST("{[10,30] - [2147483647,4294967295]}"==rangeString);
      BOOST_TEST(retrieved);
      BOOST_TEST(bytesRead == 12);//why is this 12 and not 8? two ints were written.
      BOOST_TEST(readTime<0.5);
    }
  BOOST_AUTO_TEST_CASE(dropObject){ 
    //not sure I can meaningfully test that the object was really dropped
    //BOOST_TEST(iovdbsvc->dropObject(fixtureFoldername, true)); //will fail, the proxy is nullptr
    BOOST_TEST(!iovdbsvc->dropObject(fixtureFoldername,false));
    //test invalid folderName
    BOOST_TEST(!iovdbsvc->dropObject(invalidFoldername,true));
  }
  BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
