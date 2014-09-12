/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ProxyProviderSvc_test.cxx
 * @brief unit test for the proxy provider mechanism
 * @author ATLAS Collaboration
 ***************************************************************************/

// $Id: ProxyProviderSvc_test.cxx,v 1.7 2008-07-10 00:29:24 calaf Exp $

//<<<<<< INCLUDES                                                       >>>>>>

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

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/ClassID_traits.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/StoreID.h"

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
using SG::DataProxy;
using SG::TransientAddress;

class FooBar {};
#include "SGTools/CLASS_DEF.h"
CLASS_DEF(FooBar, 8109, 0)

template <typename PROXIED>
class TestProvider : public IAddressProvider {
public:
  TestProvider(string key) :
    m_ID(ClassID_traits<PROXIED>::ID()), m_key(key)
  {  }
  virtual ~TestProvider() {}
  ///get all addresses that the provider wants to preload in SG maps

  virtual StatusCode loadAddresses(StoreID::type /*id*/, tadList& /* tList */) 
  {return StatusCode::SUCCESS;}

  virtual StatusCode preLoadAddresses(StoreID::type /*id*/, tadList& tList) 
  {
    TransientAddress* tad = new TransientAddress(m_ID, m_key, new GenericAddress(ToyConversionSvc::storageType(), m_ID,m_key));
    tList.push_back(tad);
    return StatusCode::SUCCESS;
  }

  ///get a specific address, plus all others  the provider wants to load in SG maps
  virtual StatusCode updateAddress(StoreID::type /*sID*/, TransientAddress* tad)
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
  rSG.clearStore().isSuccess();
  rPPS.addProvider(new TestProvider<Foo>("existingFoo"));
  const Foo *pFoo(0);
  //NOT YET SGASSERTERROR(rSG.record(new Foo(6.28), "existingFoo").isSuccess());
  assert(rSG.record(new Foo(6.28), "existingFoo").isSuccess());
  assert(rSG.retrieve(pFoo, "existingFoo").isSuccess());
  cout << pFoo->a() << endl;
  cout << "*** ProxyProviderSvc_test RecordBeforeRead OK ***\n\n" <<endl;
}

void testReadPrivate(StoreGateSvc& rSG) {
  cout << "*** ProxyProviderSvc_test readPrivate BEGINS ***" <<endl;
  
  std::auto_ptr<Foo> apFoo;
  SGASSERTERROR((rSG.readPrivateCopy<Foo>("NotThere")).get() != 0);
  
  apFoo=rSG.readPrivateCopy<Foo>("diskFoo");
  assert(0 != &*apFoo);
  assert(floatEQ(0, static_cast<float>(apFoo->a()))); //check that our Foo is the def constr one

  //now test the situation in which we have a transient obj in the way
  //record must fail because we already have a provider
  //not yet SGASSERTERROR(rSG.record(new Foo(6.28), "privFoo").isSuccess());
  assert(rSG.record(new Foo(6.28), "privFoo").isSuccess());
  assert(rSG.overwrite(std::auto_ptr<Foo>(new Foo(6.28)), "privFoo").isSuccess());
  
  apFoo=rSG.readPrivateCopy<Foo>("privFoo");
  assert(0 != &*apFoo);
  assert(floatNEQ(6.28f, static_cast<float>(apFoo->a()))); //check that our Foo is a different one
  apFoo->setA(3.14);
  std::auto_ptr<Foo> bpFoo(rSG.readPrivateCopy<Foo>("privFoo"));
  assert(0 != &*bpFoo);
  assert(&*bpFoo != &*apFoo); //two independent instances
  assert(floatNEQ(6.28f, static_cast<float>(bpFoo->a()))); 
  assert(floatNEQ(3.14f, static_cast<float>(bpFoo->a()))); 
  assert(floatEQ(3.14f, static_cast<float>(apFoo->a()))); 
  const Foo* plainFoo(0);
  assert(plainFoo = rSG.retrieve<Foo>("privFoo"));
  assert(floatEQ(6.28f, static_cast<float>(plainFoo->a()))); //this is the old guy!
  assert(plainFoo != &*apFoo); //yet another instance
  assert(plainFoo != &*bpFoo); //yet another instance
  //  cout << "---------ap " << &*apFoo << " bp " << &*bpFoo  <<endl;
  //cout << " pFoo33 " << pFoo33 << endl;
  
  assert(rSG.record(apFoo, "silly33").isSuccess());
  const Foo *pFoo33(rSG.retrieve<Foo>("silly33"));
  assert(0 != pFoo33);
  assert(floatEQ(3.14f, static_cast<float>(pFoo33->a())));
  assert(pFoo33 != &*apFoo); //not one of the private copies
  assert(pFoo33 != &*bpFoo); //not one of the private copies
  SGASSERTERROR((rSG.readPrivateCopy<Foo>("silly33")).get() != 0);
  assert(rSG.retrieve<Foo>("silly33"));
  std::auto_ptr<Foo> aptrFoo33(rSG.retrievePrivateCopy<Foo>("silly33"));
  assert(aptrFoo33.get() == pFoo33);
  assert(floatEQ(3.14f, static_cast<float>(aptrFoo33->a())));
  SGASSERTERROR((pFoo33 = rSG.retrieve<Foo>("silly33")) != 0);
  
  
  cout << "*** ProxyProviderSvc_test readPrivate OK ***\n\n" <<endl;
}




void testHLTAutoKeyReset(StoreGateSvc& rSG, IProxyProviderSvc& rPPS) {
  cout << "*** ProxyProviderSvc_test HLTAutoKeyReset BEGINS ***" <<endl;
  assert(rSG.clearStore(true).isSuccess());
  std::list<DataProxy*> pl;
  assert(rSG.store()->proxyList(pl).isSuccess());
  assert(0 == pl.size());
  rPPS.addProvider(new TestProvider<Foo>("HLTAutoKey_1"));
  rPPS.addProvider(new TestProvider<Foo>("HLTAutoKey_2"));
  rPPS.addProvider(new TestProvider<Foo>("HLTAutoKey_3"));
  rPPS.addProvider(new TestProvider<Foo>("NOT_HLTAutoKey_3"));
  assert(rSG.contains<Foo>("HLTAutoKey_1"));
  assert(rSG.contains<Foo>("HLTAutoKey_2"));
  assert(rSG.contains<Foo>("HLTAutoKey_3"));
  assert(rSG.contains<Foo>("NOT_HLTAutoKey_3"));
  pl.clear();
  assert(rSG.store()->proxyList(pl).isSuccess());
  assert(4 == pl.size());
  assert(rSG.clearStore().isSuccess());
  pl.clear();
  assert(rSG.store()->proxyList(pl).isSuccess());
  assert(1 == pl.size());
  assert(rSG.contains<Foo>("HLTAutoKey_1"));
  assert(rSG.contains<Foo>("NOT_HLTAutoKey_3"));
  pl.clear();
  assert(rSG.store()->proxyList(pl).isSuccess());
  assert(2 == pl.size());
  
  cout << "*** ProxyProviderSvc_test HLTAutoKeyReset OK ***\n\n" <<endl;
}

int main() {
  ISvcLocator* pSvcLoc(0);
  if (!initGaudi("ProxyProviderSvc_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert( pSvcLoc );

  StoreGateSvc* pStore(0);
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

  const Foo* pFoo(0);
  assert( (pStore->retrieve(pFoo, "aFoo")).isSuccess() );
  assert( pFoo );


  const FooBar* pFooBar(0);
  SGASSERTERROR( (pStore->retrieve(pFooBar, "aFooBar")).isSuccess() );
  assert( 0 == pFooBar );

  cout << "*** ProxyProviderSvc_test OK ***" <<endl;


  testRecordBeforeRead(*pStore, *pIPPSvc);
  testReadPrivate(*pStore);
  testHLTAutoKeyReset(*pStore, *pIPPSvc);
  assert(pStore->clearStore(true).isSuccess());
  return 0;
}

