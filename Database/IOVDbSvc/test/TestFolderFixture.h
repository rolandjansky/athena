/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/TestFolderFixture.h
 * @author Shaun Roe
 * @date Feb, 2019
 * @brief Struct for creating a test local database
 */

#include "CoolKernel/Record.h" 
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/IFolder.h"
#include "CoolApplication/DatabaseSvcFactory.h"

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <stdexcept>

struct TestFolderFixtureBase{
  ServiceHandle<IIOVDbSvc> iovdbsvc;
  ServiceHandle<StoreGateSvc> detStore;
  const std::string fixtureFoldername;
  const std::string invalidFoldername;
  const std::string dbFileName;
  TestFolderFixtureBase(const std::string & dbName):iovdbsvc("IOVDbSvc", "test"),detStore("DetectorStore",""),
  fixtureFoldername("/key1"),invalidFoldername("nonsense"),dbFileName(dbName) {
    if (not iovdbsvc.retrieve().isSuccess()){
      throw (std::runtime_error("IOVDbSvc could not be retrieved in the TestFolderFixture"));
    }
    if (not detStore.retrieve().isSuccess()){
      throw (std::runtime_error("detStore could not be retrieved in the TestFolderFixture"));
    }
    unlink (dbFileName.c_str());
    cool::IDatabaseSvc& dbSvc=cool::DatabaseSvcFactory::databaseService();
    const std::string connectionString="sqlite://;schema="+dbFileName+";dbname=OFLP200";
    cool::IDatabasePtr coolDb = dbSvc.createDatabase(connectionString);
    cool::RecordSpecification spec;
    spec.extend ("int", cool::StorageType::Int32);
    cool::FolderSpecification fSpec (cool::FolderVersioning::SINGLE_VERSION,spec,cool::PayloadMode::INLINEPAYLOAD);
    std::string desc = "<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";
    {
      cool::IFolderPtr f = coolDb->createFolder (fixtureFoldername, fSpec, desc);
      cool::Record payload (f->payloadSpecification());
      payload[0].setValue (1);
      f->storeObject (0, 100, payload, 0);
      payload[0].setValue (2);
      f->storeObject (100, cool::ValidityKeyMax, payload, 0);
    }
    coolDb->closeDatabase();
  }
};