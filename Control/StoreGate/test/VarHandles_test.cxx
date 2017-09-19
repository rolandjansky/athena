/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TestTools/initGaudi.h"
#include "TestTools/SGassert.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/UpdateHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/DataProxy.h"
#include "SGTools/TestStore.h"

#include <cassert>
#include <iostream>
#include <list>

using SG::ReadHandle;
using SG::WriteHandle;
using SG::UpdateHandle;
using SG::DataProxy;
using SG::TransientAddress;
using std::list;
using std::cerr;
using std::cout;
using std::endl;


struct MyDataObj {
  MyDataObj(int i=3, double f=3.14) : m_i(i), m_f(f) {}
  int m_i;
  double m_f;
  int i() const { return m_i; }
  double f() const { return m_f; }
  typedef std::true_type thread_safe;
};
bool operator==(const MyDataObj& lhs, const MyDataObj& rhs) {
  return ((lhs.m_i==rhs.m_i) && (lhs.m_f==rhs.m_f));
}

typedef std::list<int> IntList;
/** @file DataHandle_test.cxx  unit test for DataHandle
 * @author ATLAS Collaboration
 * $Id: VarHandles_test.cxx 781577 2016-11-01 14:41:16Z ssnyder $
 ***************************************************************************/

#include "SGTools/CLASS_DEF.h"
CLASS_DEF(IntList, 8001, 3)
CLASS_DEF(MyDataObj, 8000, 1)

namespace Athena_test {
  template<class BYVALUE>
  BYVALUE sillyFunc(BYVALUE in) {
    return in;
  }

  void varHandleTest() {
    //empty handles
    ReadHandle<MyDataObj> empty;
    ReadHandle<MyDataObj> copy(empty); 
    assert(copy==empty);

    //init with an empty proxy
    ReadHandle<IntList> emptyProxy("foo");
    assert(!emptyProxy.isInitialized());
    assert(emptyProxy.setState(new DataProxy()).isFailure()); //we are friends
    assert(!emptyProxy.isInitialized());
    assert(!emptyProxy.isSet());
    assert(!emptyProxy.isConst());
    assert(emptyProxy.cachedPtr() == nullptr);
    assert(!emptyProxy.isValid());

    //init with a valid proxy
    DataProxy* pMyProxy(new DataProxy(StoreGateSvc::asStorable(new MyDataObj),
				      new TransientAddress(CLID(8000), "foo")));
    pMyProxy->addRef();
    ReadHandle<MyDataObj> hMyR;
    assert(!hMyR.isInitialized());
    assert(hMyR.setState(pMyProxy).isSuccess());
    assert(hMyR.isInitialized());
    assert(hMyR.isSet());
    assert(!hMyR.isConst());
    assert(hMyR.isValid());
    assert(hMyR->m_i==3);
    assert(*hMyR.cachedPtr() == 3);
    assert(hMyR==empty); //this may be counterintutive but handles pointing to the same store/clid/key are ==

    UpdateHandle<MyDataObj> hMyU;
    assert(!hMyU.isInitialized());
    assert(hMyU.cachedPtr() == nullptr);
    assert(hMyU.setState(pMyProxy).isSuccess());
    assert(hMyU.isInitialized());
    assert(hMyU.isSet());
    assert(!hMyU.isConst());
    assert(hMyU.isValid());
    assert(hMyU->m_i==3);
    assert(*hMyU.cachedPtr() == 3);
    assert(hMyU==empty);

    WriteHandle<MyDataObj> hMy ("hMy");
    assert(!hMy.isInitialized());
    assert(hMy.cachedPtr() == nullptr);
    hMy.setProxyDict (&SGTest::store);
    hMy = std::make_unique<MyDataObj>(4);
    //assert(hMy.setState(pMyProxy).isSuccess());
    assert(hMy.isInitialized());
    assert(hMy.isSet());
    assert(!hMy.isConst());
    assert(hMy.isValid());
    assert(hMy->m_i==4);
    assert(*hMy.cachedPtr() == 4);
    assert(hMy!=empty);

    //copy it and test
    WriteHandle<MyDataObj> hMyCopy(hMy);
    assert(hMyCopy.isInitialized());
    assert(hMyCopy.isSet());
    assert(*hMyCopy.cachedPtr() == 4);
    assert(!hMyCopy.isConst());
    assert(hMyCopy.isValid());
    assert(hMyCopy->m_i==4);
    assert(hMy==hMyCopy);

    //assign it and test
    WriteHandle<MyDataObj> hMyCopy2;
    hMyCopy2 = hMyCopy;
    assert(hMyCopy2.isValid());
    assert(hMyCopy2->m_i==4);
    assert(hMy==hMyCopy2);

    //move it and test
    WriteHandle<MyDataObj> hMove(std::move(hMyCopy2));
    assert(hMove.isValid());
    assert(hMove->m_i==4);
    assert(hMy==hMove);

    //assign from a temp and test
    {
      ReadHandle<MyDataObj> hMove2;
      hMove2 = sillyFunc(std::move(hMyR));
      assert(hMove2.isValid());
      assert(hMove2->m_i==3);
      assert(hMyR==hMove2);
    }
    {
      WriteHandle<MyDataObj> hMove21;
      hMove21 = sillyFunc(std::move(hMyCopy));
      assert(hMove21.isValid());
      assert(hMove21->m_i==4);
      assert(hMy==hMove21);
    }
    {
      UpdateHandle<MyDataObj> hMove22;
      hMove22 = sillyFunc(std::move(hMyU));
      assert(hMove22.isValid());
      assert(hMove22->m_i==3);
      assert(hMyU==hMove22);
    }

    //init with another copy of the same object and compare
    DataProxy* pMyProxy2(new DataProxy(StoreGateSvc::asStorable(new MyDataObj),
				       new TransientAddress(CLID(8000), "foo2")));
    UpdateHandle<MyDataObj> hMy2;
    assert(!hMy2.isInitialized());
    assert(hMy2.setState(pMyProxy2).isSuccess());
    assert(hMy2.isInitialized());
    assert(hMy2.isSet());
    assert(!hMy2.isConst());
    assert(hMy2.isValid());
    assert(hMy2->m_i==3);
    assert(hMy!=hMy2);
    //assert(*hMy==*hMy2);

    const bool CONSTPROXY(true);
    DataProxy* pMyProxy3(new DataProxy(StoreGateSvc::asStorable(new MyDataObj),
				       new TransientAddress(CLID(8000), "foo3"), CONSTPROXY));
    ReadHandle<MyDataObj> hMy3;
    assert(hMy3.setState(pMyProxy3).isSuccess());
    assert(hMy3.isConst());
    assert(hMy3.isValid());
    assert(hMy3->m_i==3);
#ifdef CHECKCOMPILER
    assert(hMy3->f()==3.14); 
#endif
    assert(hMy!=hMy3);
    //assert(*hMy==*hMy3);


    //init with another proxy referring to the same object and compare
    WriteHandle<MyDataObj> hMySameProxy;
    assert(hMySameProxy.setState(pMyProxy).isSuccess());
    assert(!hMySameProxy.isValid());
    //assert(hMySameProxy->m_i==3);
    //assert(hMy==hMySameProxy);
    //assert(*hMy==*hMySameProxy);
    cout << "*** VarHandles_test static handle test OK ***" <<endl;
    return;
  }  

  void resetableTest() {
    const bool CONSTPROXY(true);
    const bool RESETONLY(true);
    
    DataProxy* pNR(new DataProxy(StoreGateSvc::asStorable(new MyDataObj(33)),
				 new TransientAddress(CLID(8000), "noReset"),
				 CONSTPROXY, !RESETONLY) );
    ReadHandle<MyDataObj> hNR ("noReset");
    assert(hNR.setState(pNR).isSuccess());
    assert(hNR.isValid());
    assert(33 == hNR->i());
    hNR.reset(true);
    assert(!hNR.isInitialized());
    assert(!hNR.isValid());

    pNR = new DataProxy(StoreGateSvc::asStorable(new MyDataObj(33)),
                        new TransientAddress(CLID(8000), "noReset"),
                        CONSTPROXY, !RESETONLY);
    assert(hNR.setState(pNR).isSuccess());
    assert(hNR.isValid());
    assert(33 == hNR->i());
    hNR.reset(false);
    assert(!hNR.isInitialized());
    assert(!hNR.isValid());

    //now with the RESETONLY
    DataProxy* pRO(new DataProxy(StoreGateSvc::asStorable(new MyDataObj(44)),
				 new TransientAddress(CLID(8000), "noReset"),
				 !CONSTPROXY, RESETONLY) );  
    pRO->addRef();
    UpdateHandle<MyDataObj> hRO ("noReset");
    assert(hRO.setState(pRO).isSuccess());
    assert(hRO.isValid());
    assert(44 == hRO->i());
    hRO.reset(true);
    assert(!hRO.isInitialized());
    assert(hRO.setState(pRO).isSuccess());
    assert(hRO.isValid());
    assert(44 == hRO->i());
    hRO.reset(false);
    assert(hRO.isInitialized());

    assert(hRO.setState(pRO).isSuccess());
    assert(hRO.isValid());
    assert(44 == hRO->i());
    delete pRO;
    assert(!hRO.isInitialized());
    assert(!hRO.isValid());
    //kaboom    assert(44 == hRO->i());

    std::cout << "*** VarHandles_test resetable test OK ***" <<std::endl;
    return;
  }

  void refCountTest () {
    std::vector<DataProxy*> vp;
    for (int i=0; i < 4; i++) {
      DataProxy* dp = new DataProxy(StoreGateSvc::asStorable(new MyDataObj),
				    new TransientAddress(CLID(8000), "foo"));
      vp.push_back (dp);
    }
    for (unsigned int i=0; i < vp.size(); ++i) {
      //bump up the ref count to check the effects of handle destructor
      assert (1 == vp[i]->addRef()); 
    }
    
    {
      WriteHandle<MyDataObj> dh;
      assert(dh.setState(vp[0]).isSuccess());
      assert (vp[0]->refCount() == 2);
      assert(dh.setState(vp[0]).isSuccess());
      assert (vp[0]->refCount() == 2);
      WriteHandle<MyDataObj> dh2 (dh);
      assert (vp[0]->refCount() == 3);
      WriteHandle<MyDataObj> dh3;
      dh3=dh2;
      assert (vp[0]->refCount() == 4);

      for (int i=1; i < 4; i++)
	assert (vp[i]->refCount() == 1);
      
      dh2 = dh3;
      assert (vp[0]->refCount() == 4);
      for (int i=1; i < 4; i++)
	assert (vp[i]->refCount() == 1);
      
      dh2 = dh;
      assert (vp[0]->refCount() == 4);
      for (int i=1; i < 4; i++)
	assert (vp[i]->refCount() == 1);
    } //invoke handles destructor

    for (unsigned int i=0; i < vp.size(); ++i) {
      assert (0==vp[i]->release());
    }

    std::cout << "*** VarHandles_test ref count test OK ***" <<std::endl;
    return;
  }
}

using namespace Athena_test;

//#include "Reflex/PluginService.h"

int main() {
  SGTest::initTestStore();
  ISvcLocator* pDum;
  initGaudi(pDum); //need MessageSvc
  cout << "*** VarHandles_test starts ***" <<endl;
  varHandleTest();
  resetableTest();
  refCountTest();
  cout << "*** VarHandles_test OK ***" <<endl;
  return 0; 
}
