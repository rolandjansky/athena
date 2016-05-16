/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/SGassert.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/SGIterator.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/DataProxy.h"
#include "SGTools/StorableConversions.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>

using SG::DataProxy;
using SG::TransientAddress;
using std::list;
using std::cerr;
using std::cout;
using std::endl;


struct MyDataObj {
  MyDataObj() : m_i(3), m_f(3.14) {}
  int m_i;
  double m_f;
  double f() { return m_f; }
};
bool operator==(const MyDataObj& lhs, const MyDataObj& rhs) {
  return ( (lhs.m_i == rhs.m_i) && Athena_test::floatEQ(lhs.m_f,rhs.m_f) );
}

typedef std::list<int> IntList;
/** @file DataHandle_test.cxx  unit test for DataHandle
 * @author ATLAS Collaboration
 * $Id: SGIterator_test.cxx,v 1.1 2008-09-26 23:37:53 calaf Exp $
 ***************************************************************************/

#include "SGTools/CLASS_DEF.h"
CLASS_DEF(IntList, 8001, 3)
CLASS_DEF(MyDataObj, 8000, 1)

namespace Athena_test {
  void iteratorTest() {
    //empty handles
    DataHandle<MyDataObj> empty;
    SG::Iterator<MyDataObj> emptyIter(empty);

    //init with an empty proxy
    DataProxy* emProxy(new DataProxy());
    const DataHandle<IntList> emptyProxy(emProxy);
    SGASSERTERROR(emptyProxy.isValid());
    //delete emProxy;
    //init with a valid proxy
    DataProxy* pMyProxy(new DataProxy(SG::asStorable(new MyDataObj),
				      new TransientAddress(CLID(8000), "foo")));
    DataHandle<MyDataObj> hMy(pMyProxy);
    assert(hMy.isValid());
    assert(hMy->m_i==3);
    assert(hMy!=empty);
    SG::Iterator<MyDataObj> iterMy(hMy);
    assert(iterMy->m_i==3);
    assert(iterMy!=emptyIter);



    //copy it and test
    SG::Iterator<MyDataObj> iterMyCopy(iterMy);
    assert(iterMyCopy->m_i==3);
    assert(iterMy==iterMyCopy);

    //assign it and test
    SG::Iterator<MyDataObj> iterMyCopy2;
    iterMyCopy2 = iterMy;
    assert(iterMyCopy2->m_i==3);
    assert(iterMy==iterMyCopy2);

    //try a couple of stl algorithms to check the typedefs
    std::distance(iterMy, iterMyCopy); 
    cout << "*** SGIterator test OK ***" <<endl;
  }  
  void constIteratorTest() {
    //empty handles
    DataHandle<MyDataObj> empty;
    SG::ConstIterator<MyDataObj> emptyIter(empty);

    //init with an empty proxy
    DataProxy* emProxy(new DataProxy());
    const DataHandle<IntList> emptyProxy(emProxy);
    SGASSERTERROR(emptyProxy.isValid());
    //delete emProxy;
    //init with a valid proxy
    DataProxy* pMyProxy(new DataProxy(SG::asStorable(new MyDataObj),
				      new TransientAddress(CLID(8000), "foo")));
    DataHandle<MyDataObj> hMy(pMyProxy);
    assert(hMy.isValid());
    assert(hMy->m_i==3);
    assert(hMy!=empty);
    SG::ConstIterator<MyDataObj> iterMy(hMy);
    assert(iterMy->m_i==3);
    assert(iterMy!=emptyIter);



    //copy it and test
    SG::ConstIterator<MyDataObj> iterMyCopy(iterMy);
    assert(iterMyCopy->m_i==3);
    assert(iterMy==iterMyCopy);

    //assign it and test
    SG::ConstIterator<MyDataObj> iterMyCopy2;
    iterMyCopy2 = iterMy;
    assert(iterMyCopy2->m_i==3);
    assert(iterMy==iterMyCopy2);

    //try a couple of stl algorithms to check the typedefs
    std::distance(iterMy, iterMyCopy); 
    //needs ordering predicate    std::sort(iterMy, iterMyCopy); 
    cout << "*** SGConstIterator test OK ***" <<endl;
  }  

}

using namespace Athena_test;

//#include "Reflex/PluginService.h"

int main() { 
  //  ROOT::Reflex::PluginService::SetDebug(8);
  cout << "*** SGIterator_test starts ***" <<endl;
  ISvcLocator* pDum;
  initGaudi(pDum); //need MessageSvc
  iteratorTest();
  constIteratorTest();
  cout << "*** SGIterator_test OK ***" <<endl;
  return 0; 
}
