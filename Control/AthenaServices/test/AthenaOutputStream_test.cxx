/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaOutputStream_test.cxx
 * @brief  unit test for AtRndSvc
 *
 * @author Paolo Calafiura <pcalafiura@lbl.gov> -ATLAS Collaboration
 * $Id: AthenaOutputStream_test.cxx,v 1.5 2007-05-18 17:02:25 calaf Exp $
 **/

#undef NDEBUG

#include <cassert>
#include <iostream>
#include <vector>
#include "TestTools/initGaudi.h"
#include "FooBar.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "CxxUtils/ubsan_suppress.h"

#include "../src/AthenaOutputStream.h"
#include "TInterpreter.h"

using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using namespace Athena_test;

int main() {
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  errorcheck::ReportMessage::hideErrorLocus();
  const std::string appName = "AthenaOutputStream_test";
  cout << "*** " << appName << " starts ***" <<endl;
  ISvcLocator* pSvcLoc(0);
  if (!initGaudi("AthenaOutputStream_test.txt", pSvcLoc)) {
    cerr << "This test can not be run" << endl;
    return 0;
  }  
  assert(pSvcLoc);

  //locate necessary stuff
  ServiceHandle<IClassIDSvc> pCLIDSvc( "ClassIDSvc", appName );
  assert( pCLIDSvc.retrieve().isSuccess() );
  assert( pCLIDSvc );

  ServiceHandle<StoreGateSvc> pStore( "StoreGateSvc", appName );
  assert( pStore.retrieve().isSuccess() );
  assert( pStore );

#ifdef ATLAS_GAUDI_V21
  SmartIF<IAlgManager> algMan(pSvcLoc);
#else
  SmartIF<IAlgManager> algMan(IAlgManager::interfaceID(), pSvcLoc);
#endif
  assert( algMan.isValid() );
  IAlgorithm* pAlg(0);
  assert( (algMan->createAlgorithm( "AthenaOutputStream", "AthenaOutputStream", pAlg)).isSuccess() );

  assert( (pAlg->sysInitialize()).isSuccess() );

  assert( (pStore->record(new Foo(), "uno")).isSuccess());
  assert( (pStore->record(new Foo(), "due")).isSuccess());
  assert( (pStore->record(new Bar(), "uno")).isSuccess());
  assert( (pStore->record(new Bar(), "due")).isSuccess());

  assert( (pStore->record(new Bar(), "quattro")).isSuccess() );
  assert( (pStore->record(new Bar(), "cinque")).isSuccess() );
  assert( (pStore->symLink(8107, "quattro", 8108)).isSuccess() );
  assert( (pStore->symLink(8107, "cinque", 8108)).isSuccess() );
  
  AthenaOutputStream* pStream(dynamic_cast<AthenaOutputStream*>(pAlg));
  assert( pStream );

  //fill the vector of selected objects
  pStream->collectAllObjects();

  //  cout << pStream->selectedObjects()->end() - 
  //    pStream->selectedObjects()->begin() <<endl;
  // verify that we got the right objects in the list
  //  this of course depends on AthenaOutputStream_test.txt
  assert( 6 == (pStream->selectedObjects()->end() - 
  		pStream->selectedObjects()->begin()) );

  for (DataObject* obj : *pStream->selectedObjects()) {
    DataBucketBase* dbb = dynamic_cast<DataBucketBase*> (obj);
    const SG::DataProxy* proxy = pStore->proxy (dbb->object());
    std::cout << dbb->clID() << " " << proxy->name() << "\n";
  }
  
  pStream->clearSelection();
  assert( 0 == (pStream->selectedObjects()->end() - 
		pStream->selectedObjects()->begin()) );


  //all done
  cout << "*** " << appName << " OK ***" <<endl;
  return 0;
}
