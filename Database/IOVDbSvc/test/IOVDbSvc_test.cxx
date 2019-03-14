/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file IOVDbSvc/test/IOVDbSvc_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Some tests for IOVDbSvc.  Not complete!
 */


#undef NDEBUG
#include "TestTools/initGaudi.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IAddressProvider.h"
#include "CxxUtils/ubsan_suppress.h"
#include "CoolApplication/DatabaseSvcFactory.h"
#include "CoolKernel/pointers.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/IFolder.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TInterpreter.h"
#include <cassert>
#include <iostream>


void test1 (IIOVDbSvc& iovdbsvc)
{
  std::cout << "test1\n";

  IOVRange range;
  std::string tag;
  IOpaqueAddress* addr = nullptr;
  assert( iovdbsvc.getRange (1238547719, "/key1", IOVTime (10, 15),
                             range, tag, addr).isSuccess() );
  assert (std::string(range) == "{[10,10] - [10,20]}");

  assert( iovdbsvc.getRange (1238547719, "/key1", IOVTime (10, 35),
                             range, tag, addr).isSuccess() );
  assert (std::string(range) == "{[10,30] - [2147483647,4294967295]}");

  assert( iovdbsvc.getRange (1238547719, "/key2", IOVTime (20, 15),
                             range, tag, addr).isSuccess() );
  assert (std::string(range) == "{[20,10] - [20,20]}");

  assert( iovdbsvc.getRange (1238547719, "/key2", IOVTime (20, 35),
                             range, tag, addr).isSuccess() );
  assert (std::string(range) == "{[20,30] - [20,36]}");
}


int main()
{
  std::cout << "IOVDbSvc_test\n";

  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); } );
  ISvcLocator* svcLoc;
  //The following is to ensure I can run the test executable outside ctest
  const std::string here=__FILE__;
  const size_t lastSlash=here.find_last_of("/");
  const std::string dir=here.substr(0,lastSlash+1);
  const std::string searchPath=dir+"../share/";
  if (!Athena_test::initGaudi(searchPath+"IOVDbSvc_test.txt", svcLoc))
    return 1;

  unlink ("mytest.db");

  cool::IDatabaseSvc& dbSvc=cool::DatabaseSvcFactory::databaseService();
  cool::IDatabasePtr coolDb =
    dbSvc.createDatabase("sqlite://;schema=mytest.db;dbname=OFLP200");
  cool::RecordSpecification spec;
  spec.extend ("int", cool::StorageType::Int32);
  cool::FolderSpecification fSpec (cool::FolderVersioning::SINGLE_VERSION,
                                   spec,
                                   cool::PayloadMode::INLINEPAYLOAD);
  std::string desc = "<timeStamp>run-event</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";

  {
    cool::IFolderPtr f = coolDb->createFolder ("/key1", fSpec, desc);
    cool::Record payload (f->payloadSpecification());
    payload[0].setValue (1);
    f->storeObject ((10ull<<32) + 10, (10ull<<32) + 20, payload, 0);
    payload[0].setValue (2);
    f->storeObject ((10ull<<32) + 30, cool::ValidityKeyMax, payload, 0);
  }
  {
    cool::IFolderPtr f = coolDb->createFolder ("/key2", fSpec, desc);
    cool::Record payload (f->payloadSpecification());
    payload[0].setValue (11);
    f->storeObject ((20ull<<32) + 10, (20ull<<32) + 20, payload, 0);
    payload[0].setValue (12);
    f->storeObject ((20ull<<32) + 30, cool::ValidityKeyMax, payload, 0);
  }
  coolDb->closeDatabase();

  ServiceHandle<IIOVDbSvc> iovdbsvc ("IOVDbSvc", "test");
  if (iovdbsvc.retrieve().isFailure()) return 1;

  IAddressProvider* iovdbsvc_ap =  dynamic_cast<IAddressProvider*> (iovdbsvc.get());
  IAddressProvider::tadList tlist;
  assert( iovdbsvc_ap->preLoadAddresses (StoreID::DETECTOR_STORE, tlist).isSuccess() );

  test1 (*iovdbsvc);

  return 0;
}
