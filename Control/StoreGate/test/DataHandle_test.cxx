/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// DataHandle<> is not thread-safe.
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "TestTools/initGaudi.h"
#include "TestTools/SGassert.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/DataProxy.h"

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
  return ((lhs.m_i==rhs.m_i) && (lhs.m_f==rhs.m_f));
}

typedef std::list<int> IntList;
/** @file DataHandle_test.cxx  unit test for DataHandle
 * @author ATLAS Collaboration
 * $Id: DataHandle_test.cxx,v 1.15 2008-05-22 22:52:12 calaf Exp $
 ***************************************************************************/

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/StorableConversions.h"
CLASS_DEF(IntList, 8001, 3)
CLASS_DEF(MyDataObj, 8000, 1)

namespace Athena_test {
  void handleTest() {
    //empty handles
    const DataHandle<MyDataObj> empty;
    DataHandle<MyDataObj> copy(empty); //FIXME this should not compile
    assert(copy==empty);

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

    //copy it and test
    DataHandle<MyDataObj> hMyCopy(hMy);
    assert(hMyCopy.isValid());
    assert(hMyCopy->m_i==3);
    assert(hMy==hMyCopy);

    //assign it and test
    DataHandle<MyDataObj> hMyCopy2;
    hMyCopy2 = hMyCopy;
    assert(hMyCopy2.isValid());
    assert(hMyCopy2->m_i==3);
    assert(hMy==hMyCopy2);

    //init with another copy of the same object and compare
    DataProxy* pMyProxy2(new DataProxy(SG::asStorable(new MyDataObj),
				       new TransientAddress(CLID(8000), "foo2")));
    DataHandle<MyDataObj> hMy2(pMyProxy2);
    assert(hMy2.isValid());
    assert(hMy2->m_i==3);
    assert(hMy!=hMy2);
    assert(*hMy==*hMy2);

    const DataHandle<MyDataObj> hMy3(pMyProxy2);
    assert(hMy3.isValid());
    assert(hMy3->m_i==3);
#ifdef CHECKCOMPILER
    assert(hMy3->f()==3.14); 
#endif
    assert(hMy!=hMy3);
    assert(*hMy==*hMy3);


    //init with another proxy referring to the same object and compare
    DataHandle<MyDataObj> hMySameProxy(pMyProxy);
    assert(hMySameProxy.isValid());
    assert(hMySameProxy->m_i==3);
    assert(hMy==hMySameProxy);
    assert(*hMy==*hMySameProxy);
    cout << "*** DataHandle_test static handle test OK ***" <<endl;
  }  


  void refcount_test (ISvcLocator* /*svcloc*/)
{
  std::vector<DataProxy*> vp;
  SG::ProxyMap mp;
  for (int i=0; i < 4; i++) {
    DataProxy* dp = new DataProxy(SG::asStorable(new MyDataObj),
                                  new TransientAddress(CLID(8000), "foo"));
    vp.push_back (dp);
    mp[std::string (1, 'A' + i)] = dp;
  }

  {
    DataHandle<MyDataObj> dh (vp[0]);
    assert (vp[0]->refCount() == 1);
    dh.setState (vp[0]);
    assert (vp[0]->refCount() == 1);
    DataHandle<MyDataObj> dh2 (dh);
    assert (vp[0]->refCount() == 2);
    DataHandle<MyDataObj> dh3 (mp.begin(), mp.end());
    assert (vp[0]->refCount() == 3);
    for (int i=1; i < 4; i++)
      assert (vp[i]->refCount() == 0);

    dh2 = dh3;
    assert (vp[0]->refCount() == 3);
    for (int i=1; i < 4; i++)
      assert (vp[i]->refCount() == 0);

    dh2 = dh;
    assert (vp[0]->refCount() == 3);
    for (int i=1; i < 4; i++)
      assert (vp[i]->refCount() == 0);

    ++dh;
    assert (vp[0]->refCount() == 2);
    dh2++;
    assert (vp[0]->refCount() == 1);

    dh.setState (vp[0]);
    assert (vp[0]->refCount() == 2);
    SG::ConstProxyIterator it = mp.begin();
    dh3.setState (it, mp.end());
    assert (vp[0]->refCount() == 2);
    for (int i=1; i < 4; i++)
      assert (vp[i]->refCount() == 0);
    dh3++;
    assert (vp[0]->refCount() == 1);
    assert (vp[1]->refCount() == 1);
    dh2.setState (vp[1]);
    ++dh3;
    for (int i=0; i < 3; i++)
      assert (vp[i]->refCount() == 1);

    for (int i=0; i < 4; i++)
      vp[i]->addRef();

    dh.setState (vp[1]);
    assert (vp[0]->refCount() == 1);
    assert (vp[1]->refCount() == 3);
    assert (vp[2]->refCount() == 2);
    assert (vp[3]->refCount() == 1);
    dh.setState (vp[0]);
    assert (vp[3]->refCount() == 1);
    for (int i=0; i < 3; i++)
      assert (vp[i]->refCount() == 2);
    it++; it++;
    dh.setState (it, mp.end());
    assert (vp[0]->refCount() == 1);
    assert (vp[1]->refCount() == 2);
    assert (vp[2]->refCount() == 3);
    assert (vp[3]->refCount() == 1);
    --it;
    dh.setState (it, mp.end());
    assert (vp[0]->refCount() == 1);
    assert (vp[1]->refCount() == 3);
    assert (vp[2]->refCount() == 2);
    assert (vp[3]->refCount() == 1);
    dh.setState (vp[0]);
    assert (vp[3]->refCount() == 1);
    for (int i=0; i < 3; i++)
      assert (vp[i]->refCount() == 2);

    const DataHandle<MyDataObj> dh4;
    dh4.setState (vp[0]);
    assert (vp[0]->refCount() == 3);
    assert (vp[1]->refCount() == 2);
    assert (vp[2]->refCount() == 2);
    assert (vp[3]->refCount() == 1);
    dh4.setState (it, mp.end());
    assert (vp[0]->refCount() == 2);
    assert (vp[1]->refCount() == 3);
    assert (vp[2]->refCount() == 2);
    assert (vp[3]->refCount() == 1);
    dh4.setState (vp[0]);
    assert (vp[0]->refCount() == 3);
    assert (vp[1]->refCount() == 2);
    assert (vp[2]->refCount() == 2);
    assert (vp[3]->refCount() == 1);
  }

  for (unsigned int i=0; i < vp.size(); ++i) {
    assert (vp[i]->refCount() == 1);
    delete vp[i];
  }

}

}

using namespace Athena_test;

//#include "Reflex/PluginService.h"

int main() { 
  //  ROOT::Reflex::PluginService::SetDebug(8);
  ISvcLocator* pDum;
  initGaudi(pDum); //need MessageSvc
  handleTest();
  refcount_test(pDum);
  cout << "*** DataHandle_test OK ***" <<endl;
  return 0; 
}
