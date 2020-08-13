/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaOutputStream_test.cxx
 * @brief  unit test for AthenaOutputStream
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
#include "../src/SelectionVetoes.h"
#include "../src/CompressionInfo.h"
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
  ISvcLocator* pSvcLoc(nullptr);
  if (!initGaudi("AthenaServices/AthenaOutputStream_test.txt", pSvcLoc)) {
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

  SmartIF<IAlgManager> algMan(pSvcLoc);
  assert( algMan.isValid() );
  IAlgorithm* pAlg(nullptr);
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

  auto baz = std::make_unique<Baz>();
  auto bazaux = std::make_unique<BazAuxContainer>();
  baz->setStore (bazaux.get());
  SG::AuxElement::Accessor<int> aaa ("aaa");
  SG::AuxElement::Accessor<int> bbb ("bbb");
  SG::AuxElement::Accessor<int> ccc ("ccc");
  aaa (*baz);
  bbb (*baz);
  ccc (*baz);
  assert( (pStore->record(std::move(baz), "sei")).isSuccess() );
  assert( (pStore->record(std::move(bazaux), "seiAux.")).isSuccess() );
  
  auto baz4lfc = std::make_unique<Baz>();
  auto bazaux4lfc = std::make_unique<BazAuxContainer>();
  baz4lfc->setStore (bazaux4lfc.get());
  SG::AuxElement::Accessor<float> foo ("foo");
  SG::AuxElement::Accessor<double> bar ("bar");
  SG::AuxElement::Accessor<std::vector<float>> zzz ("zzz");
  foo (*baz4lfc);
  bar (*baz4lfc);
  zzz (*baz4lfc);
  assert( (pStore->record(std::move(baz4lfc), "comp")).isSuccess() );
  assert( (pStore->record(std::move(bazaux4lfc), "compAux.")).isSuccess() );

  AthenaOutputStream* pStream(dynamic_cast<AthenaOutputStream*>(pAlg));
  assert( pStream );

  //fill the vector of selected objects
  assert( pStream->collectAllObjects().isSuccess() );

  //  cout << pStream->selectedObjects()->end() - 
  //    pStream->selectedObjects()->begin() <<endl;
  // verify that we got the right objects in the list
  //  this of course depends on AthenaOutputStream_test.txt
  assert( 10 == (pStream->selectedObjects()->end() -
  		pStream->selectedObjects()->begin()) );

  for (DataObject* obj : *pStream->selectedObjects()) {
    DataBucketBase* dbb = dynamic_cast<DataBucketBase*> (obj);
    if (!dbb) std::abort();
    const SG::DataProxy* proxy = pStore->proxy (dbb->object());
    std::cout << dbb->clID() << " " << proxy->name() << "\n";
  }

  const SG::SelectionVetoes* selvetoes = nullptr;
  assert (pStore->retrieve (selvetoes, "SelectionVetoes_AthenaOutputStream").isSuccess());
  assert (selvetoes->size() == 2);
  auto it = selvetoes->find("sei");
  assert (it != selvetoes->end());
  assert (!it->second.test (aaa.auxid()));
  assert ( it->second.test (bbb.auxid()));
  assert (!it->second.test (ccc.auxid()));

  const SG::CompressionInfo* compInfo = nullptr;
  assert (pStore->retrieve (compInfo, "CompressionInfo_AthenaOutputStream").isSuccess());
  assert (compInfo->size() == 2); // 2 levels of compression as high/low
  auto val = compInfo->find("comp");
  assert (val != compInfo->end());
  assert (val->second.at(10).test(foo.auxid()));  // compress foo high
  assert (!val->second.at(10).test(bar.auxid())); // don't compress bar since it's double
  assert (val->second.at(16).test(zzz.auxid()));  // compress zzz low

  pStream->clearSelection();
  assert( 0 == (pStream->selectedObjects()->end() - 
		pStream->selectedObjects()->begin()) );


  //all done
  cout << "*** " << appName << " OK ***" <<endl;
  return 0;
}
