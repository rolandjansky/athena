/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file /Database/CoraCool/test/CoraCoolDatabase_test.cxx
 * @author Shaun Roe
 * @date June, 2019
 * @brief A test for CoraCoolDatabase in the Boost framework
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

#include "../CoraCool/CoraCoolDatabase.h"

#include "CoolApplication/Application.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/RecordSpecification.h"
#include "RelationalAccess/IConnectionService.h"
#include <unistd.h> //for unlink

BOOST_AUTO_TEST_SUITE(CoraCoolDatabaseTest)
  static cool::Application app;
  coral::IConnectionService& coralSvc{app.connectionSvc()};
  cool::IDatabaseSvc& coolDbSvc{app.databaseService()};
  cool::IDatabasePtr pCoolDb;
  const bool readonly(false);
  const std::string dbConnectionString{"sqlite://;schema=CoraCoolDatabaseTest.db;dbname=TESTCOOL"};
  //
  BOOST_AUTO_TEST_CASE(Instantiation){
    unlink("CoraCoolDatabaseTest.db");
    pCoolDb=coolDbSvc.createDatabase(dbConnectionString);
    BOOST_CHECK_NO_THROW(CoraCoolDatabase(dbConnectionString,pCoolDb,coralSvc,readonly));
    pCoolDb->closeDatabase();
  }
  BOOST_AUTO_TEST_CASE(PublicMethods){
    //instantiate first
    pCoolDb=coolDbSvc.openDatabase(dbConnectionString, readonly);
    auto coraCoolDb = CoraCoolDatabase(dbConnectionString,pCoolDb,coralSvc,readonly);
    BOOST_CHECK(coraCoolDb.connect());
    BOOST_CHECK(coraCoolDb.dbname() == "TESTCOOL");
    BOOST_CHECK(coraCoolDb.coolDatabase() != nullptr);
    BOOST_CHECK(coraCoolDb.existsFolder("MyFolder") == false);
    BOOST_CHECK(!coraCoolDb.disconnect()); //does this seem right?
    // create a folder, need some pre-requisites
    const std::string folderPath="/MyFolder";
    const std::string coralTable="MyFolder";
    cool::RecordSpecification fkspec;
    fkspec.extend("CoolKey",cool::StorageType::Int32);
    cool::RecordSpecification payloadspec;
    // create the AttributeListSpecification for the payload
    // primary / foreign keys
    const std::string foreignKey("ForeignKey");
    const std::string primaryKey("PrimKey");
    payloadspec.extend(primaryKey,cool::StorageType::Int32);
    payloadspec.extend(foreignKey,cool::StorageType::Int32);
    //data
    payloadspec.extend("IntObj",cool::StorageType::Int32);
    payloadspec.extend("String1",cool::StorageType::String4k);
    const std::string desc="<timeStamp>run-event</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"55403898\" /></addrHeader><typeName>CondAttrListVec</typeName><coracool>MyFolder:CoolKey:ForeignKey:PrimKey</coracool>";
    //
    BOOST_CHECK(coraCoolDb.createFolder(folderPath,coralTable, fkspec, payloadspec,foreignKey, primaryKey,desc)!=nullptr);
    BOOST_CHECK(coraCoolDb.existsFolder(folderPath) == true);
    BOOST_CHECK(coraCoolDb.getFolder(folderPath) != nullptr);
    pCoolDb->closeDatabase();
    //
    std::string tablename;
		std::string keycolcool;
		std::string fkeycolcoral;
		std::string pkeycolcoral;
    pCoolDb=coolDbSvc.openDatabase(dbConnectionString, readonly);
    auto reConnect = CoraCoolDatabase(dbConnectionString,pCoolDb,coralSvc,readonly);
    BOOST_CHECK(reConnect.parseFolderDescription(desc,tablename,keycolcool,fkeycolcoral,pkeycolcoral));
    BOOST_CHECK(reConnect.deleteFolder(folderPath));
    BOOST_CHECK(reConnect.existsFolder(folderPath) == false);
    pCoolDb->closeDatabase();
  }

BOOST_AUTO_TEST_SUITE_END()
