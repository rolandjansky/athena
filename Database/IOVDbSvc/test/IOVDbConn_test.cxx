/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/IOVDbConn_Boost_test.cxx
 * @author Shaun Roe
 * @date Jan, 2019
 * @brief Some tests for IOVDbConn in the Boost framework
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOVDBSVC

#include <boost/test/unit_test.hpp>
//
#include "CoolKernel/IDatabase.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "../src/IOVDbConn.h"

#include "GaudiKernelFixtureBase.h"


struct GaudiKernelFixture:public GaudiKernelFixtureBase{
  GaudiKernelFixture():GaudiKernelFixtureBase(__FILE__){
    //nop, everything in base.
  }
};

struct IOVDbConnFixture{
  ServiceHandle<IMessageSvc> msgSvc;
  const std::string connectionString;
  MsgStream log;
  IOVDbConn connection;
  IOVDbConnFixture():msgSvc("msgSvc","test"),
   connectionString("sqlite://;schema=IOVDbConnTest.db;dbname=OFLP200"),
   log(msgSvc.get(), "IOVDbConn_Boost_test"),
   connection(connectionString, true, &log){
  }
};

//Basic tests that the service can be retrieved
BOOST_FIXTURE_TEST_SUITE(IOVDbConnTest , GaudiKernelFixture)
  BOOST_AUTO_TEST_CASE( SanityCheck ){
    BOOST_TEST(gaudiIsInitialised);
    const bool svcLocatorIsOk=(svcLoc != nullptr);
    BOOST_TEST(svcLocatorIsOk);
  }
  //tests connection to database
  BOOST_FIXTURE_TEST_SUITE(IOVDbConnection, IOVDbConnFixture)
    BOOST_AUTO_TEST_CASE(publicMethods){
      BOOST_TEST(!connection.isActive());//no db active at this point
      cool::IDatabasePtr db=connection.getCoolDb();//db connection or creation
      BOOST_TEST(connection.isActive());
      BOOST_TEST(connection.isReadOnly());
      BOOST_TEST(!connection.aborted());
      BOOST_TEST(connection.nFolders() == 0);
      connection.incUsage();
      BOOST_TEST(connection.nFolders() == 1);
      BOOST_TEST(connection.name() == connectionString);
    }
  BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

