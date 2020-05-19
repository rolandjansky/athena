/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/test/ThinningHandleBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2019
 * @brief Tests for ThinningHandleBase.
 */


#undef NDEBUG
#include "StoreGate/ThinningHandleBase.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/exceptions.h"
#include "AthContainers/DataVector.h"
#include "TestTools/initGaudi.h"
#include "TestTools/expect_exception.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "GaudiKernel/ServiceHandle.h"
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

  SG::WriteHandleKey<SG::ThinningDecision> wkey ("dec");
  assert( wkey.initialize().isSuccess() );

  EventContext ctx;
  Atlas::setExtendedEventContext (ctx, Atlas::ExtendedEventContext(sg.get()) );

  {
    SG::ThinningHandleBase h (wkey, "obj", ctx);

    h.thin (3);
    h.thin (5);
    h.thin (10);
    assert (thinning (h.decision()) == 0x428);

    h.keep (5);
    assert (thinning (h.decision()) == 0x408);

    std::vector<bool> v (16);
    v[4] = true;
    v[12] = true;
    v[1] = true;
    h.thin (v, SG::ThinningHandleBase::Op::Or);
    assert (thinning (h.decision()) == 0x141a);

    v[0] = true;
    v[7] = true;
    h.keep (v);
    assert (thinning (h.decision()) == 0xef6c);

    SG::ThinningDecisionBase dd (16);
    dd.thin (v);
    h.thin (dd);
    assert (thinning (h.decision()) == 0x1093);

    h.thinAll();
    assert (thinning (h.decision()) == 0xffff);
    h.keepAll();
    assert (thinning (h.decision()) == 0x0000);

    h.keep (dd);
    assert (thinning (h.decision()) == 0xef6c);
  }

  const SG::ThinningDecision* dout = nullptr;
  assert( sg->retrieve (dout, "dec").isSuccess() );
  assert (thinning (*dout) == 0xef6c);
  assert (dout->index (6) == SG::ThinningDecision::RemovedIdx);
  assert (dout->index (7) == 3);

  // Should fail: duplicate.
  {
    SG::ThinningHandleBase h (wkey, "obj", ctx);
  }
}


int main()
{
  std::cout << "StoreGate/ThinningHandleBase_test\n";
  errorcheck::ReportMessage::hideErrorLocus();
  errorcheck::ReportMessage::hideFunctionNames();
  ISvcLocator* svcloc;
  if (!Athena_test::initGaudi("StoreGate/VarHandleBase_test.txt", svcloc)) {
    return 1;
  }

  test1();
  return 0;
}
