/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file /Database/CoraCool/test/CoraCoolDatabaseSvcFactory_test.cxx
 * @author Shaun Roe
 * @date June, 2019
 * @brief A test for CoraCoolDatabaseSvcFactory in the Boost framework
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
//
#include "../CoraCool/CoraCoolDatabase.h"
#include "../CoraCool/CoraCoolFolder.h"
#include "../CoraCool/CoraCoolObjectIter.h"
#include "CoolApplication/Application.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/RecordSpecification.h"
#include "RelationalAccess/IConnectionService.h"
#include <unistd.h> //for unlink

BOOST_AUTO_TEST_SUITE(CoraCoolObjectIterTest)
  static cool::Application app;
  coral::IConnectionService& coralSvc{app.connectionSvc()};
  cool::IDatabaseSvc& coolDbSvc{app.databaseService()};
  cool::IDatabasePtr pCoolDb;
  const bool readonly(false);
  const std::string dbConnectionString{"sqlite://;schema=CoraCoolObjectIterTest.db;dbname=TESTCOOL"};
  //
  BOOST_AUTO_TEST_CASE(AllPublicMethods){
    unlink("CoraCoolObjectIterTest.db");
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
    
    auto pFolder = coraCoolDb.createFolder(folderPath,coralTable, fkspec, payloadspec,foreignKey, primaryKey,desc);   
    //write some data to the folder
    coral::AttributeList returnedAttrList = pFolder->emptyAttrList();

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
    cool::ChannelSelection selectedChannels(0,1);//from... to...
    {
      auto pCcIterator = pFolder->browseObjects(pointInTime,selectedChannels);
      /** finally have a valid iterator?
       Test following idiom:
       while (itr->hasNext()) {
         CoraCoolObjectPtr obj=itr->next()
      **/
      BOOST_TEST(pCcIterator->hasNext());
      BOOST_TEST(pCcIterator->next() != nullptr);
      pCcIterator->next();
      BOOST_TEST(!pCcIterator->hasNext());
      BOOST_CHECK_NO_THROW(pCcIterator->close());
    }
    {
      auto pCcIterator2 = pFolder->browseObjects(pointInTime,selectedChannels);
      //If the iterator is not null, at least one object is found and the iterator points to this
      unsigned int objectCount=1;
    
      /**
       while (itr->goToNext()) {
         CoraCoolObjectPtr obj=itr->currentRef()
      **/
      while(pCcIterator2->goToNext()) ++objectCount;
      BOOST_CHECK_NO_THROW(pCcIterator2->close());
      BOOST_TEST(objectCount == 3);//we stored three channels worth of data
    }
    pCoolDb->closeDatabase();
  }
BOOST_AUTO_TEST_SUITE_END()
