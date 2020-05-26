/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/test/ThinningHandle_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Tests for ThinningHandle.
 */


#undef NDEBUG
#include "StoreGate/ThinningHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/exceptions.h"
#include "AthContainers/DataVector.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <cassert>
#include <iostream>


class MyObj {};
CLASS_DEF (DataVector<MyObj>, 293889395, 1)


unsigned int thinning (const SG::ThinningDecisionBase& dec)
{
  unsigned int mask = 0;
  for (size_t i = 0; i < dec.size(); i++) {
    if (dec.thinned(i)) {
      mask |= (1<<i);
    }
  }
  return mask;
}


void test1()
{
  std::cout << "test1\n";

  ServiceHandle<StoreGateSvc> sg ("StoreGateSvc", "test");
  assert( sg.retrieve().isSuccess() );

  auto dv = std::make_unique<DataVector<MyObj> >();
  for (int i = 0; i < 16; i++) {
    dv->push_back (std::make_unique<MyObj>());
  }
  assert( sg->record (std::move(dv), "obj", false).isSuccess() );

  EventContext ctx;
  Atlas::setExtendedEventContext (ctx, Atlas::ExtendedEventContext(sg.get()) );

  SG::ThinningHandleKey<DataVector<MyObj> > tkey ("obj");
  assert( tkey.initialize("stream").isSuccess() );

  {
    SG::ThinningHandle<DataVector<MyObj> > h (tkey, ctx);
    h.thin (3);
    h.thin (5);
    h.thin (10);
    assert (thinning (h.decision()) == 0x428);
  }

  const SG::ThinningDecision* dout = nullptr;
  assert( sg->retrieve (dout, "obj_THINNED_stream").isSuccess() );
  assert (thinning (*dout) == 0x0428);
  assert (dout->index (5) == SG::ThinningDecision::RemovedIdx);
  assert (dout->index (6) == 4);
}


// makeHandle
void test2()
{
  std::cout << "test2\n";

  ServiceHandle<StoreGateSvc> sg ("StoreGateSvc", "test");
  assert( sg.retrieve().isSuccess() );

  EventContext ctx;
  Atlas::setExtendedEventContext (ctx, Atlas::ExtendedEventContext(sg.get()) );

  assert( sg->record (std::make_unique<DataVector<MyObj> >(),
                      "obj1", false).isSuccess() );

  {
    SG::ThinningHandleKey<DataVector<MyObj> > tkey1 ("obj1");
    assert (tkey1.initialize ("Stream").isSuccess());
    auto h1 = SG::makeHandle (tkey1, ctx);
    assert (h1.key() == "obj1");
    assert (h1.storeHandle().name() == "StoreGateSvc");
    assert (h1.mode() == Gaudi::DataHandle::Reader);
    h1.keepAll();
  }

  Gaudi::Hive::setCurrentContext (ctx);

  assert( sg->record (std::make_unique<DataVector<MyObj> >(),
                      "obj2", false).isSuccess() );

  {
    SG::ThinningHandleKey<DataVector<MyObj> > tkey2 ("obj2");
    assert (tkey2.initialize ("Stream").isSuccess());
    auto h2 = SG::makeHandle (tkey2);
    assert (h2.key() == "obj2");
    assert (h2.storeHandle().name() == "StoreGateSvc");
    assert (h2.mode() == Gaudi::DataHandle::Reader);
    h2.keepAll();
  }
}


int main()
{
  std::cout << "StoreGate/ThinningHandle_test\n";
  errorcheck::ReportMessage::hideErrorLocus();
  errorcheck::ReportMessage::hideFunctionNames();
  ISvcLocator* svcloc;
  if (!Athena_test::initGaudi("StoreGate/VarHandleBase_test.txt", svcloc)) {
    return 1;
  }

  test1();
  test2();
  return 0;
}
