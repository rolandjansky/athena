/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file /Database/CoraCool/test/CoraCoolFolder_test.cxx
 * @author Shaun Roe
 * @date June, 2019
 * @brief A test for CoraCoolFolder in the Boost framework
 */

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_CORACOOL
/*
LCG_95/Boost/1.69.0/x86_64-centos7-gcc8-opt/include/boost/test/tree/observer.hpp:73:21: 
warning: 'virtual void boost::unit_test::test_observer::test_unit_skipped(const boost::
unit_test::test_unit&)' was hidden [-Woverloaded-virtual] 
... (etc)
*/
//...so use a pragma around the include:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <boost/test/unit_test.hpp>

#pragma GCC diagnostic pop
//
#include "../CoraCool/CoraCoolDatabase.h"
#include "../CoraCool/CoraCoolFolder.h"

#include "CoolApplication/Application.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/RecordSpecification.h"
#include "RelationalAccess/IConnectionService.h"
#include <unistd.h> //for unlink
#include <sstream>



BOOST_AUTO_TEST_SUITE(CoraCoolFolderTest)
  static cool::Application app;
  coral::IConnectionService& coralSvc{app.connectionSvc()};
  cool::IDatabaseSvc& coolDbSvc{app.databaseService()};
  cool::IDatabasePtr pCoolDb;
  const bool readonly(false);
  const std::string dbConnectionString{"sqlite://;schema=CoraCoolFolderTest.db;dbname=TESTCOOL"};
  
  //
  BOOST_AUTO_TEST_CASE(ConstructorAndPublicMethods){
    unlink("CoraCoolFolderTest.db");
    //instantiate first
    pCoolDb=coolDbSvc.createDatabase(dbConnectionString);
    auto coraCoolDb = CoraCoolDatabase(dbConnectionString,pCoolDb,coralSvc,readonly);
     // create a folder, need some pre-requisites
    const std::string folderPath="/MyFolder";
    const std::string coralTable="MyFolder";
    cool::RecordSpecification fkspec;
    fkspec.extend("CoolKey",cool::StorageType::Int32);
    cool::RecordSpecification payloadspec;
    // create the AttributeListSpecification for the payload
    // primary / foreign keys
    const std::string foreignKey("FgKey");
    const std::string primaryKey("PrimKey");
    payloadspec.extend(primaryKey,cool::StorageType::Int32);
    payloadspec.extend(foreignKey,cool::StorageType::Int32);
    //data
    payloadspec.extend("IntObj",cool::StorageType::Int32);
    payloadspec.extend("String1",cool::StorageType::String4k);
    const std::string desc="<timeStamp>run-event</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"55403898\" /></addrHeader><typeName>CondAttrListVec</typeName><coracool>MyFolder:CoolKey:FgKey:PrimKey</coracool>";
    /**
    Under the hood, this exercises:
    CoraCoolFolder(const std::string& coolfolder,coral::ISessionProxy* proxy, 
        cool::IDatabasePtr cooldb,CoraCoolDatabase* coradb, coral::MessageStream& log);
    **/
    auto pFolder = coraCoolDb.createFolder(folderPath,coralTable, fkspec, payloadspec,foreignKey, primaryKey,desc);
    /**
    INTERESTING TO NOTE: any attempt to dereference the pointer and call the following 
    methods directly on the object result in all manner of memory access mayhem
    **/
    //now we have a folder to play with...
    BOOST_TEST(pFolder->coralTableName() == "MyFolder");
    BOOST_TEST(pFolder->coralFKey() == "FgKey");
    BOOST_TEST(pFolder->coralPKey() == "PrimKey");
    BOOST_TEST((pFolder->fkSpecification() == fkspec));
    BOOST_TEST((pFolder->payloadSpecification() == payloadspec));
    coral::AttributeList returnedAttrList = pFolder->emptyAttrList();
    std::stringstream attrListRepresentation;
    attrListRepresentation<<returnedAttrList;
    BOOST_TEST(attrListRepresentation.str() == "[PrimKey (int) : 0], [FgKey (int) : 0], [IntObj (int) : 0], [String1 (string) : ]");
    BOOST_TEST(pFolder->coolFolder() != nullptr);
    //exercise writing to the folder
    returnedAttrList["PrimKey"].data<int>()=0;
    returnedAttrList["FgKey"].data<int>()=0;
    returnedAttrList["IntObj"].data<int>()=4;
    returnedAttrList["String1"].data<std::string>()="A test string of data";
    std::vector<coral::AttributeList> payload{10, returnedAttrList}; //make 10 the same
    for (unsigned int ichan{};ichan!=3;++ichan){ //store in 3 channels
      pFolder->storeObject(0,10,payload.begin(),payload.end(),ichan);
    }
    /**
    methods referenceObject(...) and addPayload(...) appear to be little-used
    **/
    const cool::ValidityKey pointInTime=5;
    int channel=2;
    BOOST_TEST(pFolder->findObject(pointInTime,channel) != nullptr);
    cool::ChannelSelection selectedChannels(0,1);//from... to...
    BOOST_TEST(pFolder->browseObjects(pointInTime,selectedChannels) != nullptr);
    BOOST_TEST(pFolder->browseObjects(pointInTime, pointInTime+1, selectedChannels) != nullptr);
    /**
    methods only used internally:
    bool setAttrKey(coral::Attribute& attr,const int keyval);
    bool setFieldKey(cool::IField& attr,const int keyval);
    bool setFieldAttr(cool::IField& attr,const coral::Attribute& keyval);
    int getAttrKey(const coral::Attribute& attr);
    **/
    pCoolDb->closeDatabase();
  }
BOOST_AUTO_TEST_SUITE_END()
