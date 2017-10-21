/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ProxyProviderSvc_test.cxx
 * @brief unit test for the proxy provider mechanism
 * @author ATLAS Collaboration
 ***************************************************************************/

// $Id: ProxyProviderSvc_test.cxx,v 1.7 2008-07-10 00:29:24 calaf Exp $

#include "AthenaKernel/IProxyProviderSvc.h"

#include <cassert>
#include <iostream>
#include <string>

#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/SGassert.h"
#include "ToyConversion/FooBar.h"
#include "ToyConversion/ToyConversionSvc.h"

#include "SGTools/TransientAddress.h"
#include "SGTools/DataProxy.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/ClassID_traits.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/StoreID.h"
#include "CxxUtils/make_unique.h"

#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ClassID.h"


#include <string>

using namespace Athena_test;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using namespace SG;

class FooBar {
public:
  FooBar(): m_i(s_i++){}
  int i() const { return m_i; }
  ~FooBar() { 
    cout << "FooBar i=" << i() << " deleted" << endl;
  }
private:
  int m_i;
  static int s_i;
};
int FooBar::s_i=0;

#include "SGTools/CLASS_DEF.h"
CLASS_DEF(FooBar, 8109, 0)

template <typename PROXIED>
class TestProvider : public IAddressProvider {
public:
  virtual unsigned long addRef() override { std::abort(); }
  virtual unsigned long release() override { std::abort(); }
  virtual StatusCode queryInterface(const InterfaceID &/*ti*/, void** /*pp*/) override
  { std::abort(); }

  TestProvider(const std::string& key) :
    m_ID(ClassID_traits<PROXIED>::ID()), m_key(key)
  {  }
  ~TestProvider() override {}
  ///get all addresses that the provider wants to preload in SG maps

  StatusCode loadAddresses(StoreID::type /*id*/, tadList& /* tList */) override 
  {return StatusCode::SUCCESS;}

  StatusCode preLoadAddresses(StoreID::type /*id*/, tadList& tList) override 
  {
    TransientAddress* tad = new TransientAddress(m_ID, m_key, new GenericAddress(ToyConversionSvc::storageType(), m_ID,m_key));
    tList.push_back(tad);
    return StatusCode::SUCCESS;
  }

  ///get a specific address, plus all others  the provider wants to load in SG maps
  StatusCode updateAddress(StoreID::type /*sID*/, TransientAddress* tad,
                           const EventContext& /*ctx*/) override
  { 
    StatusCode sc;
    if ((tad->clID() != m_ID) || (tad->name() != m_key)) {
      //      cout <<"store id"<< sID << endl;
      //      cout << "m_ID: " << m_ID << " " << m_key << endl;
      //      cout << "id: " << tad->clID() << " " << tad->name() << endl;
      sc=StatusCode::FAILURE;
    } else {
      tad->setAddress(new GenericAddress(ToyConversionSvc::storageType(), m_ID,m_key));
      sc=StatusCode::SUCCESS;
    }
    return sc;
  }
      
private:
  CLID m_ID;
  std::string m_key;
};

void testRecordBeforeRead(StoreGateSvc& rSG, IProxyProviderSvc& rPPS) {
  cout << "*** ProxyProviderSvc_test RecordBeforeRead BEGINS ***" <<endl;
  rSG.clearStore().ignore();
  rPPS.addProvider(new TestProvider<Foo>("existingFoo"));
  const Foo *pFoo(nullptr);
  //NOT YET SGASSERTERROR(rSG.record(new Foo(6.28), "existingFoo").isSuccess());
  assert(rSG.record(new Foo(6.28), "existingFoo").isSuccess());
  assert(rSG.retrieve(pFoo, "existingFoo").isSuccess());
  cout << pFoo->a() << endl;
  cout << "*** ProxyProviderSvc_test RecordBeforeRead OK ***\n\n" <<endl;
}

void testReadPrivate(StoreGateSvc& rSG) {
  cout << "*** ProxyProviderSvc_test readPrivate BEGINS ***" <<endl;
  
  std::unique_ptr<Foo> apFoo;
  SGASSERTERROR((rSG.readPrivateCopy<Foo>("NotThere")).get() != nullptr);
  
  apFoo=rSG.readUniquePrivateCopy<Foo>("diskFoo");
  assert(nullptr != apFoo.get());
  assert(floatEQ(0, static_cast<float>(apFoo->a()))); //check that our Foo is the def constr one

  //now test the situation in which we have a transient obj in the way
  //record must fail because we already have a provider
  //not yet SGASSERTERROR(rSG.record(new Foo(6.28), "privFoo").isSuccess());
  assert(rSG.record(new Foo(6.28), "privFoo").isSuccess());
  assert(rSG.overwrite(CxxUtils::make_unique<Foo>(6.28), "privFoo").isSuccess());
  
  apFoo=rSG.readUniquePrivateCopy<Foo>("privFoo");
  assert(nullptr != apFoo.get());
  assert(floatNEQ(6.28f, static_cast<float>(apFoo->a()))); //check that our Foo is a different one
  apFoo->setA(3.14);
  std::unique_ptr<Foo> bpFoo(rSG.readUniquePrivateCopy<Foo>("privFoo"));
  assert(nullptr != bpFoo.get());
  assert(&*bpFoo != &*apFoo); //two independent instances
  assert(floatNEQ(6.28f, static_cast<float>(bpFoo->a()))); 
  assert(floatNEQ(3.14f, static_cast<float>(bpFoo->a()))); 
  assert(floatEQ(3.14f, static_cast<float>(apFoo->a()))); 
  const Foo* plainFoo(nullptr);
  assert(plainFoo = rSG.retrieve<Foo>("privFoo"));
  assert(floatEQ(6.28f, static_cast<float>(plainFoo->a()))); //this is the old guy!
  assert(plainFoo != &*apFoo); //yet another instance
  assert(plainFoo != &*bpFoo); //yet another instance
  //  cout << "---------ap " << &*apFoo << " bp " << &*bpFoo  <<endl;
  //cout << " pFoo33 " << pFoo33 << endl;

  const Foo* pFoo33Orig = apFoo.get();
  assert(rSG.record(std::move(apFoo), "silly33").isSuccess());
  const Foo *pFoo33(rSG.retrieve<Foo>("silly33"));
  assert(nullptr != pFoo33);
  assert(floatEQ(3.14f, static_cast<float>(pFoo33->a())));
  assert(pFoo33 == pFoo33Orig); //the private copy we recorded.
  assert(pFoo33 != &*bpFoo); //not one of the private copies
  SGASSERTERROR((rSG.readPrivateCopy<Foo>("silly33")).get() != nullptr);
  assert(rSG.retrieve<Foo>("silly33"));
  std::unique_ptr<Foo> aptrFoo33(rSG.retrieveUniquePrivateCopy<Foo>("silly33"));
  assert(aptrFoo33.get() == pFoo33);
  assert(floatEQ(3.14f, static_cast<float>(aptrFoo33->a())));
  SGASSERTERROR((pFoo33 = rSG.retrieve<Foo>("silly33")) != nullptr);
  
  
  cout << "*** ProxyProviderSvc_test readPrivate OK ***\n\n" <<endl;
}




void testHLTAutoKeyReset(StoreGateSvc& rSG, IProxyProviderSvc& rPPS) {
  cout << "*** ProxyProviderSvc_test HLTAutoKeyReset BEGINS ***" <<endl;
  assert(rSG.clearStore(true).isSuccess());
  std::list<DataProxy*> pl;
  assert(rSG.proxies().empty());
  rPPS.addProvider(new TestProvider<Foo>("HLTAutoKey_1"));
  rPPS.addProvider(new TestProvider<Foo>("HLTAutoKey_2"));
  rPPS.addProvider(new TestProvider<Foo>("HLTAutoKey_3"));
  rPPS.addProvider(new TestProvider<Foo>("NOT_HLTAutoKey_3"));
  assert(rSG.contains<Foo>("HLTAutoKey_1"));
  assert(rSG.contains<Foo>("HLTAutoKey_2"));
  assert(rSG.contains<Foo>("HLTAutoKey_3"));
  assert(rSG.contains<Foo>("NOT_HLTAutoKey_3"));
  pl.clear();
  assert(rSG.proxies().size() == 4);
  assert(rSG.clearStore().isSuccess());
  pl.clear();
  assert(rSG.proxies().size() == 1);
  assert(rSG.contains<Foo>("HLTAutoKey_1"));
  assert(rSG.contains<Foo>("NOT_HLTAutoKey_3"));
  pl.clear();
  assert(rSG.proxies().size() == 2);
  
  cout << "*** ProxyProviderSvc_test HLTAutoKeyReset OK ***\n\n" <<endl;
}


void testOverwrite(StoreGateSvc& rSG, IProxyProviderSvc& rPPS) {
  cout << "*** ProxyProviderSvc_test Overwrite starts ***\n\n" <<endl;
  
  assert(rSG.clearStore(true).isSuccess());
  rPPS.addProvider(new TestProvider<Foo>("toOverwrite"));
  //key
  const std::string KEY("toOverwrite");
  //our "data members"
  WriteHandle<FooBar> wFB(KEY);
  ReadHandle<FooBar> rFB(KEY);
  //simulate an event loop
  for (int i=0;i<3;++i) {
    cout << "=============Event #" << i << " starts" << std::endl;
    //check we can not retrieve a non const pointer from the PPS
    assert(!(wFB.isValid()));
    //overwrite an object coming from the PPS
    assert(rSG.overwrite(new FooBar(), KEY));
    //check contents of the overwritten object
    assert(rFB.isValid());
    cout << "Overwritten FooBar i="<< rFB->i() << endl;
    assert(i == rFB->i());
    //clear the store
    assert(rSG.clearStore().isSuccess());
  }
  cout << "*** ProxyProviderSvc_test Overwrite OK ***\n\n" <<endl;
}

int main() {
  ISvcLocator* pSvcLoc(nullptr);
  if (!initGaudi("ProxyProviderSvc_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert( pSvcLoc );

  StoreGateSvc* pStore(nullptr);
  static const bool CREATE(true);
  assert( (pSvcLoc->service("StoreGateSvc", pStore, CREATE)).isSuccess() );
  assert( pStore );
  IProxyProviderSvc* pIPPSvc;
  assert( (pSvcLoc->service("ProxyProviderSvc", pIPPSvc, CREATE)).isSuccess() );
  assert( pIPPSvc );

  pIPPSvc->addProvider(new TestProvider<Foo>("aFoo"));
  pIPPSvc->addProvider(new TestProvider<Foo>("diskFoo"));
  pIPPSvc->addProvider(new TestProvider<Foo>("privFoo"));
  pIPPSvc->addProvider(new TestProvider<Bar>("aBar"));
  pIPPSvc->addProvider(new TestProvider<FooBar>("aFooBar"));


  DataHandle<Bar> hBar;
  assert( (pStore->bind(hBar, "aBar")).isSuccess() );
  
  assert( hBar.ID() == "aBar" );

  assert( hBar.cptr() );

  cout << pStore->dump() << endl;

  assert( !(pStore->transientContains<Foo>("aFoo")) );
  assert( pStore->contains<Foo>("aFoo") );

  const Foo* pFoo(nullptr);
  assert( (pStore->retrieve(pFoo, "aFoo")).isSuccess() );
  assert( pFoo );


  const FooBar* pFooBar(nullptr);
  SGASSERTERROR( (pStore->retrieve(pFooBar, "aFooBar")).isSuccess() );
  assert( nullptr == pFooBar );

  cout << "*** ProxyProviderSvc_test OK ***" <<endl;


  testRecordBeforeRead(*pStore, *pIPPSvc);
  testReadPrivate(*pStore);
  testHLTAutoKeyReset(*pStore, *pIPPSvc);
  testOverwrite(*pStore, *pIPPSvc);
  assert(pStore->clearStore(true).isSuccess());
  return 0;
}

