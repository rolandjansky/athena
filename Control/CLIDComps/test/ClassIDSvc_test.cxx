/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ClassIDSvc_test.cxx  
 * @brief unit test for ClassIDSvc
 * @author ATLAS Collaboration
 * $Id: ClassIDSvc_test.cxx,v 1.5 2009-01-19 17:02:55 binet Exp $
 ***************************************************************************/

#undef NDEBUG

//<<<<<< INCLUDES                                                       >>>>>>

#include <cassert>
#include <iostream>
#include "TestTools/initGaudi.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ModuleIncident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "../src/ClassIDSvc.h"

#include "SGTools/CLASS_DEF.h"
#include "SGTools/CLIDRegistry.h"
class Foo{};
CLASS_DEF( Foo, 8101, 0) 
class Bar{};
CLASS_DEF( Bar, 8107, 0) 
template <typename T>
struct Bla {
  T bla;
};
CLASS_DEF( Bla<unsigned int>, 8108, 0)

#define ASSERTERROR( FALSEEXPR )   \
    std::cerr << "Now we expect to see an error message:" << std::endl \
              << "----Error Message Starts--->>" << std::endl; \
    assert(!FALSEEXPR); \
    std::cerr<< "<<---Error Message Ends-------" << std::endl

using namespace std;
using namespace Athena_test;
void incident_test(ISvcLocator* pSvcLoc) {
  cout << "*** ClassIDSvc incident test starts ***" <<endl;
  assert(pSvcLoc);

  IClassIDSvc* pClassIDSvc(0);
  assert((pSvcLoc->service("ClassIDSvc", pClassIDSvc, true)).isSuccess());
  assert(pClassIDSvc);
  ModuleLoadedIncident fooInc("test", "Foo");
  IIncidentListener* il = dynamic_cast<IIncidentListener*>(pClassIDSvc);
  if (!il) std::abort();
  il->handle(fooInc);
  ModuleLoadedIncident barInc("test", "Bar");
  il->handle(barInc);
  cout << "*** ClassIDSvc incident test OK ***" <<endl;
}
void basic_test(ISvcLocator* pSvcLoc) {
  cout << "*** ClassIDSvc basic test starts ***" <<endl;
  assert(pSvcLoc);

  IClassIDSvc* pClassIDSvc(0);
  assert((pSvcLoc->service("ClassIDSvc", pClassIDSvc, true)).isSuccess());
  assert(pClassIDSvc);

  assert(pClassIDSvc->nextAvailableID() == CLIDdetail::MINCLID);
  Athena::PackageInfo info(PACKAGE_VERSION);
  Athena::PackageInfo info2("APack-00-39-98");
  assert(pClassIDSvc->setTypePackageForID(7890, "Bla", info2).isSuccess());
  ASSERTERROR(pClassIDSvc->setTypePackageForID(CLIDdetail::MINCLID/2, "No", 
					       info).isSuccess());
  ASSERTERROR(pClassIDSvc->setTypePackageForID(CLIDdetail::MAXCLID*2, "Nah", 
					       info).isSuccess());
  //FIXME is this an issue? Should we do the reverse check type -> id?
  assert(pClassIDSvc->setTypePackageForID(9942, "Bli", info).isSuccess());
  assert(pClassIDSvc->setTypePackageForID(9943, "Blu", info).isSuccess());
  ASSERTERROR(pClassIDSvc->setTypePackageForID(7890, "Ble", info).isSuccess());
  //check disabled  ASSERTERROR(pClassIDSvc->setTypePackageForID(9942, "Bli", info2).isSuccess());
  ASSERTERROR(pClassIDSvc->setTypePackageForID(9945, "Bli", info).isSuccess());
  assert(pClassIDSvc->setTypePackageForID(7890, "Bla", info2).isSuccess());
  assert(pClassIDSvc->setTypePackageForID(7890, " Bla", info2).isSuccess());
  ASSERTERROR(pClassIDSvc->setTypePackageForID(7890, "   Bl a ", info2).isSuccess());
  ASSERTERROR(pClassIDSvc->setTypePackageForID(7890, " B  l a ", info2).isSuccess());
  assert(pClassIDSvc->setTypePackageForID(7890, "Bla   ", info2).isSuccess());
  assert(pClassIDSvc->setTypePackageForID(7890, " Bla  ", info2).isSuccess());


  string name; 
  assert(pClassIDSvc->getTypeNameOfID(8101, name).isSuccess());
  assert(name == "Foo");
  assert(pClassIDSvc->getTypeNameOfID(8107, name).isSuccess());
  assert(name == "Bar");
  assert(pClassIDSvc->getTypeNameOfID(7890, name).isSuccess());
  assert(name == "Bla");
  assert(pClassIDSvc->getTypeNameOfID(9942, name).isSuccess());
  assert(name == "Bli");
  assert(pClassIDSvc->getTypeNameOfID(9943, name).isSuccess());
  assert(name == "Blu");
  assert(pClassIDSvc->getTypeNameOfID(2, name).isFailure());

  CLID id;
  assert(pClassIDSvc->getIDOfTypeName("Blu", id).isSuccess());
  assert(id == 9943);
  assert(pClassIDSvc->getIDOfTypeName("NotExist", id).isFailure());

  assert(pClassIDSvc->isIDInUse(9942));
  //  assert(!pClassIDSvc->isIDInUse(9945));
  assert(!pClassIDSvc->isIDInUse((unsigned)-2243341));

  assert(pClassIDSvc->isNameInUse("Blu"));
  assert(!pClassIDSvc->isNameInUse("NotExist"));
  assert(!pClassIDSvc->isNameInUse(""));

  //test handling of spaces (savannah 39528)
  assert( pClassIDSvc->isNameInUse("Bla<unsigned int>") );
  assert( !pClassIDSvc->isNameInUse("Bla<unsignedint>") );
  assert(pClassIDSvc->getTypeNameOfID(8108, name).isSuccess());
  assert(name == "Bla<unsigned int>");
  //  dynamic_cast<ClassIDSvc*>(pClassIDSvc)->dump();

  assert((dynamic_cast<IService*>(pClassIDSvc)->finalize()).isSuccess());
  cout << "*** ClassIDSvc basic test OK ***" <<endl;
}

int main() {
  ISvcLocator* pSvcLoc(0);
  if (!initGaudi("ClassIDSvc_test.txt", pSvcLoc)) {
    cerr << "ClassIDSvc_test can not be run" << endl;
    return 0;
  }  
  basic_test(pSvcLoc);
  incident_test(pSvcLoc);
  return 0;
}
