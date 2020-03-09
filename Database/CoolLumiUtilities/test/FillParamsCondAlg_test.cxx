/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/test/FillParamsCondAlg_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for FillParamsCondAlg.
 */


#undef NDEBUG
#include "../src/FillParamsCondAlg.h"
#include "CoolLumiUtilities/FillParamsCondData.h"
#include "AthenaKernel/DummyRCUSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "TestTools/initGaudi.h"
#include "CoralBase/Blob.h"
#include "CoolKernel/IObject.h"
#include <iostream>
#include <cassert>


EventIDBase timestamp (int t)
{
  return EventIDBase (EventIDBase::UNDEFNUM,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t);
}


coral::Blob makeBlob (const std::vector<uint16_t>& v)
{
  coral::Blob blob (v.size() * sizeof(uint16_t));
  uint16_t* data = static_cast<uint16_t*> (blob.startingAddress());
  for (size_t i = 0; i < v.size(); i++) {
    data[i] = v[i];
  }
  return blob;
}


void setBlob (AthenaAttributeList& attrs,
              const char* name,
              const std::vector<uint16_t>& v)
{
  attrs.extend (name, "blob");
  attrs[name].setValue (makeBlob (v));
}


void setCount (AthenaAttributeList& attrs,
               std::vector<uint16_t>& masks,
               const char* name,
               const std::vector<uint16_t>& v)
{
  masks.insert (masks.end(), v.begin(), v.end());

  attrs.extend (name, "unsigned int");
  attrs[name].setValue (static_cast<unsigned int>(v.size()));
}


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  EventIDBase eid (0, 0, 0, 0);
  ctx.setEventID (eid);

  FillParamsCondAlg alg ("FillParamsCondAlg", svcloc);
  alg.addRef();
  assert( alg.sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  DataObjID id1 ("testFP");
  auto cc1 = std::make_unique<CondCont<AthenaAttributeList> > (rcu, id1);
  DataObjID id2 ("FillParamsCondData");
  
  auto attrs = std::make_unique<AthenaAttributeList>();
  std::vector<uint16_t> masks;
  setCount (*attrs, masks, "Beam1Bunches", {1, 2, 3, 4});
  setCount (*attrs, masks, "Beam2Bunches", {4, 10, 15});
  setCount (*attrs, masks, "LuminousBunches", {13, 12, 11, 10, 9});
  setBlob (*attrs, "BCIDmasks", masks);

  const EventIDRange range (timestamp (0), timestamp (100));
  assert( cc1->insert (range, std::move(attrs), ctx).isSuccess() );

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");
  assert( conditionStore->record (std::move (cc1), "testFP") );

  assert( alg.execute (ctx).isSuccess() );

  CondCont<FillParamsCondData>* cc2 = nullptr;
  assert( conditionStore->retrieve (cc2, "FillParamsCondData").isSuccess() );
  const FillParamsCondData* data = 0;
  const EventIDRange* range2p = nullptr;
  assert (cc2->find (eid, data, &range2p));
  assert (range2p->start().time_stamp() == timestamp(0).time_stamp());
  assert (range2p->stop().time_stamp() == timestamp(100).time_stamp());

  assert (data->beam1Bunches() == (std::vector<unsigned int> {1, 2, 3, 4}));
  assert (data->beam2Bunches() == (std::vector<unsigned int> {4, 10, 15}));
  assert (data->luminousBunches() == (std::vector<unsigned int> {13, 12, 11, 10, 9}));
}


int main()
{
  std::cout << "CoolLumiUtilities/FillParamsCondAlg\n";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("CoolLumiUtilities/FillParamsCondAlg_test.txt", svcloc)) {
    return 1;
  }


  test1 (svcloc);
  return 0;
}
