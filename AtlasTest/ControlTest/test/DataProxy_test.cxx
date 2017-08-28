/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include "SGTools/DataProxy.h"

#include "TestTools/initGaudi.h"
#include "ToyConversion/FooBar.h"
#include "ToyConversion/ToyConversionSvc.h"
#include "GaudiKernel/GenericAddress.h"
#include "AthenaKernel/IProxyProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "SGTools/TransientAddress.h"
#include "SGTools/StorableConversions.h"
#include "SGTools/T2pMap.h"
#include "SGTools/CLASS_DEF.h"
#include <iostream>

#include <cassert>

const bool CREATEIF(true);

struct Bla{
  Bla(): i(0) {}
  Bla(int m): i(m) {}
  int i;
  void doNothing() const {};
};
CLASS_DEF( Bla, 7890, 0) 

using std::cerr;
using std::cout;
using std::endl;

using SG::DataProxy_cast;
using SG::DataProxy;
using SG::TransientAddress;

using namespace Athena_test;

int main() {
  cout << "*** DataProxy_test BEGINS ***" <<endl;

  ISvcLocator* pSvcLoc;
  if (!initGaudi("DataProxy_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  

  assert(pSvcLoc);

  DataProxy emptyProxy;
  assert( !emptyProxy.isValid() );
  assert( emptyProxy.transientAddress() );

  //cerr << "Now we expect to see a warning message:" << endl
  //     << "----Warning Message Starts--->>" << endl; 
  assert( !emptyProxy.accessData() );
  //cerr << "<<---Warning Message Ends-------" << endl;
  assert( emptyProxy.errNo() == DataProxy::NOCNVSVC );
  assert( !emptyProxy.object() );
  assert( !emptyProxy.address() );
  assert( emptyProxy.name().empty() );
  assert( emptyProxy.identifier().empty() );
  assert( 0 == emptyProxy.clID() );
  assert( 0 == emptyProxy.transientAddress()->transientID().size() );
  assert( emptyProxy.isResetOnly() );
  assert( !emptyProxy.isConst() );
  try {
    cerr << "Now we expect to see a warning message:" << endl
	 << "----Warning Message Starts--->>" << endl; 
    const Bla& bRef(DataProxy_cast<Bla>(emptyProxy));
    bRef.doNothing(); //remove warning
  } catch (...) {
    cerr << "<<---Warning Message Ends-------" << endl;
  }
  

  Bla *pBla(new Bla(77));
  DataProxy transientProxy(SG::asStorable(pBla),
			   new TransientAddress(ClassID_traits<Bla>::ID(), "bla"));
  assert( transientProxy.isValid() );
  assert( transientProxy.accessData() );
  assert( transientProxy.object() );
  assert( !transientProxy.address() );
  assert( transientProxy.name() == "bla" );
  assert( transientProxy.identifier() == "bla" );
  assert( ClassID_traits<Bla>::ID() == transientProxy.clID() );
  assert( transientProxy.transientAddress()->transientID().size() == 1);
  assert( transientProxy.isResetOnly() );
  assert( !transientProxy.isConst() );
  assert( (DataProxy_cast<Bla>(transientProxy)).i == 77 );


  IConversionSvc* pIConvSvc(nullptr);
  StatusCode cs((pSvcLoc->service("EventPersistencySvc", pIConvSvc, CREATEIF)));
  assert(cs.isSuccess());
  assert(pIConvSvc);
  // create a transient address with IOA.
  TransientAddress* tGen = new TransientAddress(ClassID_traits<Foo>::ID(),
						"foo",
						new GenericAddress(ToyConversionSvc::storageType(), ClassID_traits<Foo>::ID(), "foo"));

  SG::T2pMap t2p;
  DataProxy addressProxy(tGen, pIConvSvc);
  cout << "FIXME: in opt 32 bits we need to print the address of addressProxy to avoid a crash when the jobs ends!?!  @" << &addressProxy << endl;
  addressProxy.setT2p(&t2p);

  assert( addressProxy.isValid() );
  assert( !addressProxy.object() );
  assert( addressProxy.address() );
  //the proxy provider mechanism is tested in ProxyProviderSvc_test
  assert( addressProxy.accessData() );
  assert( addressProxy.object() );
  assert( addressProxy.name() == "foo" );
  assert( addressProxy.identifier() == "foo" );
  assert( ClassID_traits<Foo>::ID() == addressProxy.clID() );
  assert( addressProxy.transientAddress()->transientID().size() == 1);
  assert( addressProxy.isResetOnly() );
  assert( !addressProxy.isConst() );
  const Foo& fRef(DataProxy_cast<Foo>(addressProxy));
  fRef.doNothing(); //remove warning


  IProxyProviderSvc* pIPPSvc(nullptr);
  StatusCode psc(pSvcLoc->service("ProxyProviderSvc", pIPPSvc, CREATEIF));
  assert( psc.isSuccess() );
  assert( pIPPSvc );

  TransientAddress* tad = new TransientAddress(ClassID_traits<Bla>::ID(), "bla");

  DataProxy identifiedProxy(tad, pIConvSvc);

  assert( !identifiedProxy.isValid() );
  assert( !identifiedProxy.object() );
  assert( !identifiedProxy.address() );
  //the proxy provider mechanism is tested in ProxyProviderSvc_test
  cerr << "Now we expect to see a warning message:" << endl
       << "----Warning Message Starts--->>" << endl; 
  assert( !identifiedProxy.accessData() ); 
  assert( identifiedProxy.errNo() == DataProxy::NOIOA );
  cerr << "<<---Warning Message Ends-------" << endl;
  assert( identifiedProxy.name() == "bla" );
  assert( identifiedProxy.identifier() == "bla" );
  assert( ClassID_traits<Bla>::ID() == identifiedProxy.clID() );
  assert( identifiedProxy.transientAddress()->transientID().size() == 1);
  assert( identifiedProxy.isResetOnly() );
  assert( !identifiedProxy.isConst() );
  try {
    cerr << "Now we expect to see an warning message:" << endl
	 << "----Warning Message Starts--->>" << endl; 
    const Bla& bRef(DataProxy_cast<Bla>(identifiedProxy));
    bRef.doNothing(); //remove warning
  } catch (...) {
    cerr << "<<---Warning Message Ends-------" << endl;
  }


  cout << "*** DataProxy_test OK ***" <<endl;
  return 0;
}












