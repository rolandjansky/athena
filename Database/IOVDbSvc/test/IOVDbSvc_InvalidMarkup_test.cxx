/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/IOVDbSvc_InvalidMarkup_test.cxx
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
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IOpaqueAddress.h"
//
#include "GaudiKernelFixtureBase.h"
#include "TestFolderFixture.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

namespace utf = boost::unit_test;

struct GaudiKernelFixture:public GaudiKernelFixtureBase{
  GaudiKernelFixture():GaudiKernelFixtureBase("IOVDbSvc_InvalidMarkup.txt"){
    //nop, everything in base.
  }
};

struct TestFolderFixture:public TestFolderFixtureBase{
  //using IOVDbSvc default connection to sqlite file cooldummy.db
  TestFolderFixture():TestFolderFixtureBase("cooldummy.db"){
    //nop, everything in base.
  }
};

//Basic tests that the service can be retrieved
BOOST_AUTO_TEST_SUITE(IOVDbSvcTest )
  GaudiKernelFixture g;
  const auto & svcLoc=g.svcLoc;
  ServiceHandle<IIOVDbSvc> iovdbsvc ("IOVDbSvc", "test");

  BOOST_AUTO_TEST_CASE( SanityCheck ){
    const bool svcLocatorIsNotOk=(svcLoc != nullptr);
    BOOST_TEST(svcLocatorIsNotOk);
  }

  BOOST_AUTO_TEST_CASE(IOVDbSvcRetrieved){
    BOOST_TEST (iovdbsvc.retrieve().isFailure());
  }
  
  BOOST_AUTO_TEST_CASE(preloadAddresses){
    IAddressProvider* iovdbsvc_ap =  dynamic_cast<IAddressProvider*> (iovdbsvc.get());
    BOOST_TEST( iovdbsvc_ap == nullptr);
  }
  
BOOST_AUTO_TEST_SUITE_END()

