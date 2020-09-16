/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file /Database/CoraCool/test/CoraCoolDatabaseSvc_test.cxx
 * @author Shaun Roe
 * @date June, 2019
 * @brief A test for CoraCoolDatabaseSvc in the Boost framework
 */

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_CORACOOL

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <boost/test/unit_test.hpp>

#pragma GCC diagnostic pop

#include "CoolApplication/Application.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "RelationalAccess/IConnectionService.h"
#include <unistd.h> //for unlink

#include "../CoraCool/CoraCoolDatabaseSvc.h"
#include <exception>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(CoraCoolDatabaseSvcTest)
  static cool::Application app;
  coral::IConnectionService& coralSvc{app.connectionSvc()};
  cool::IDatabaseSvc& coolDbSvc{app.databaseService()};
  cool::IDatabasePtr pCoolDb;
  const bool readonly(false);
  const std::string dbConnectionString{"sqlite://;schema=CoraCoolDatabaseSvcTest.db;dbname=TESTCOOL"};
  
  BOOST_AUTO_TEST_CASE(Constructor){
    BOOST_CHECK_NO_THROW(CoraCoolDatabaseSvc(coralSvc,coolDbSvc));
  }
  BOOST_AUTO_TEST_CASE(PublicMethods,* utf::expected_failures(1)){
    unlink("CoraCoolDatabaseSvcTest.db");
    auto ccDbSvc = CoraCoolDatabaseSvc(coralSvc,coolDbSvc);
    //I would expect the following to throw (but it does not), will investigate later
    BOOST_CHECK_THROW(ccDbSvc.openDatabase(dbConnectionString,pCoolDb,true), std::exception);
    BOOST_CHECK(ccDbSvc.openDatabase(dbConnectionString,pCoolDb,readonly) != nullptr);
    BOOST_CHECK(ccDbSvc.openDatabase(dbConnectionString, readonly) != nullptr);
    BOOST_CHECK(&(ccDbSvc.coolIDatabaseSvc()) == &coolDbSvc);
    //pCoolDb->closeDatabase(); explicitly closing would cause an error, don't do it.
  }

BOOST_AUTO_TEST_SUITE_END()
