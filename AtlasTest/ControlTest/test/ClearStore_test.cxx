/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 test the data store clear
 -------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: ClearStore_test.cxx,v 1.1 2007-07-10 01:14:31 calaf Exp $

#include <iostream>

#undef NDEBUG

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGtests.h"


#ifndef NOGAUDI
#include "TestTools/initGaudi.h"
using namespace Athena_test;
using namespace std;

int main() {
  cout << "*** ClearStore_test BEGINS ***" <<endl;
  ISvcLocator* pSvcLoc;
  if (!initGaudi("StoreGateTestCommon.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);

  StoreGateSvc* pStore(0);
  static const bool CREATE(true);
  assert((pSvcLoc->service("StoreGateSvc", pStore, CREATE)).isSuccess());
  assert(pStore);
  
  cout << "*** ClearStore_test run standard testRecord a first time ***" << endl;
  testRecord(*pStore);
  cout << "*** ClearStore_test clear the store ***" << endl;
  assert(pStore->clearStore().isSuccess());
  cout << "Testing dump: store should contain no data now \n -------->>\n" 
       << pStore->dump() 
       << "\n<<--------" << endl;

  cout << "\n\n\n*** ClearStore_test run standard testRecord a second time ***" << endl;
  testRecord(*pStore);
  cout << "*** ClearStore_test clear the store again ***" << endl;
  assert(pStore->clearStore().isSuccess());

  cout << "Testing dump: store should contain no data now \n -------->>\n" 
       << pStore->dump() 
       << "\n<<--------" << endl;

  cout << "*** ClearStore_test clear the store one last time forcing proxy removal (the way we do in finalize()) ***" << endl;
  const bool FORCEREMOVE(true);
  assert(pStore->clearStore(FORCEREMOVE).isSuccess());

  cout << "Testing dump: store should contain no proxy now \n -------->>\n" 
       << pStore->dump() 
       << "\n<<--------" << endl;


  cout << "*** ClearStore_test OK ***" <<endl;
  return 0;
}
#endif /*NOGAUDI*/













