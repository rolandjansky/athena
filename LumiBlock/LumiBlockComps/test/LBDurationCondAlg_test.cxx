/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockComps/test/LBDurationCondAlg_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for LBDurationCondAlg.
 */


#undef NDEBUG
#include "../src/LBDurationCondAlg.h"
#include "LumiBlockData/LBDurationCondData.h"
#include "AthenaKernel/DummyRCUSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "TestTools/initGaudi.h"
#include "CoolKernel/IObject.h"
#include <iostream>
#include <cassert>


EventIDBase timestamp (int t)
{
  return EventIDBase (EventIDBase::UNDEFNUM,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t);
}


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  EventIDBase eid (0, 0, 0, 0);
  ctx.setEventID (eid);

  LBDurationCondAlg alg ("LBDurationCondAlg", svcloc);
  alg.addRef();
  assert( alg.sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  DataObjID id1 ("testLBLB");
  auto cc1 = std::make_unique<CondCont<AthenaAttributeList> > (rcu, id1);
  DataObjID id2 ("LBDurationCondData");
  
  auto attrs = std::make_unique<AthenaAttributeList>();
  attrs->extend ("StartTime", "unsigned long long");
  attrs->extend ("EndTime",   "unsigned long long");
  (*attrs)["StartTime"].setValue (static_cast<cool::UInt63> (123*1e9));
  (*attrs)["EndTime"].setValue   (static_cast<cool::UInt63> (125.5*1e9));

  const EventIDRange range (timestamp (0), timestamp (100));
  assert( cc1->insert (range, std::move(attrs), ctx).isSuccess() );

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");
  assert( conditionStore->record (std::move (cc1), "testLBLB") );

  assert( alg.execute (ctx).isSuccess() );

  CondCont<LBDurationCondData>* cc2 = nullptr;
  assert( conditionStore->retrieve (cc2, "LBDurationCondData").isSuccess() );
  const LBDurationCondData* data = nullptr;
  const EventIDRange* range2p = nullptr;
  assert (cc2->find (eid, data, &range2p));
  assert (range2p->start().time_stamp() == timestamp(0).time_stamp());
  assert (range2p->stop().time_stamp() == timestamp(100).time_stamp());

  assert (data->lbDuration() == 2.5);
}


int main()
{
  std::cout << "LumiBlockComps/LBDurationCondAlg_test\n";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("LumiBlockComps/LBDurationCondAlg_test.txt", svcloc)) {
    return 1;
  }


  test1 (svcloc);
  return 0;
}
