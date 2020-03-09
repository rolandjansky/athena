/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CoolLumiUtilities/test/BunchLumisCondAlg_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for BunchLumisCondAlg.
 */


#undef NDEBUG

// Disable checking due to use of AttributeList.
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

#include "../src/BunchLumisCondAlg.h"
#include "CoolLumiUtilities/BunchLumisCondData.h"
#include "CoolLumiUtilities/FillParamsCondData.h"
#include "AthenaKernel/DummyRCUSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "CoralBase/Blob.h"
#include <iostream>
#include <cassert>


const unsigned int TOTAL_LHC_BCIDS = 3564;

// BCID, lumi pairs
const std::pair<unsigned int, float> lumiData[] =
  {
   { 10, 10.5 },
   { 20, 20.5 },
   { 30, 12.5 },
   { 40, 13.5 },
   { 55, 15.5 },
};


EventIDBase timestamp (int t)
{
  return EventIDBase (EventIDBase::UNDEFNUM,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t);
}


// Push x onto data in little-endian format.
void push2 (uint16_t x, std::vector<uint8_t>& data)
{
  data.push_back (x & 0xff);
  data.push_back ((x>>8) & 0xff);
}


void push_float (float x, std::vector<uint8_t>& data)
{
  union {
    float f;
    uint32_t i;
  } cnv;
  cnv.f = x;
  data.push_back (cnv.i & 0xff);
  data.push_back ((cnv.i>>8) & 0xff);
  data.push_back ((cnv.i>>16) & 0xff);
  data.push_back ((cnv.i>>24) & 0xff);
}


void push_double (double x, std::vector<uint8_t>& data)
{
  union {
    double f;
    uint64_t i;
  } cnv;
  cnv.f = x;
  data.push_back (cnv.i & 0xff);
  data.push_back ((cnv.i>>8) & 0xff);
  data.push_back ((cnv.i>>16) & 0xff);
  data.push_back ((cnv.i>>24) & 0xff);
  data.push_back ((cnv.i>>32) & 0xff);
  data.push_back ((cnv.i>>40) & 0xff);
  data.push_back ((cnv.i>>48) & 0xff);
  data.push_back ((cnv.i>>56) & 0xff);
}


void push (unsigned int bss, float x, float avgRawLumi, std::vector<uint8_t>& data)
{
  switch (bss) {
  case 1: {
    uint8_t idat = x*100 / avgRawLumi;
    data.push_back (idat);
    break;
  }

  case 2: {
    uint16_t idat = x*100*100 / avgRawLumi;
    push2 (idat, data);
    break;
  }

  case 4:
    push_float (x, data);
    break;

  case 8:
    push_double (x, data);
    break;

  default:
    std::abort();
  }
}


coral::Blob makeBlob (unsigned int bss, unsigned int smod, float offs,
                      float& avgRawLumi)
{
  std::vector<uint8_t> data;
  data.push_back (bss*10 + smod);

  unsigned int nbcid = std::end(lumiData) - std::begin(lumiData);

  avgRawLumi = 0;
  for (const auto& p : lumiData) {
    avgRawLumi += p.second;
  }
  avgRawLumi /= nbcid;

  if (smod == 2) {
    push2 (nbcid, data);
    for (const auto& p : lumiData) {
      push2 (p.first, data);
    }
  }

  if (smod == 1) {
    unsigned int ilumi = 0;
    for (unsigned int bcid = 0; bcid < TOTAL_LHC_BCIDS; bcid++) {
      float lumi = 0;
      if (ilumi < nbcid && lumiData[ilumi].first == bcid) {
        lumi = lumiData[ilumi].second+offs;
        ++ilumi;
      }
      push (bss, lumi, avgRawLumi, data);
    }
  }
  else {
    for (const auto& p : lumiData) {
      push (bss, p.second+offs, avgRawLumi, data);
    }
  }

  coral::Blob blob (data.size());
  memcpy (blob.startingAddress(), data.data(), data.size());
  return blob;
}


coral::AttributeList makeAttrs(unsigned int bss, unsigned int smod, float offs)
{
  coral::AttributeList attr;
  attr.extend ("BunchRawInstLum", "blob");
  attr.extend ("AverageRawInstLum", "float");

  float avgRawLumi = 0;
  coral::Blob blob = makeBlob (bss, smod, offs, avgRawLumi);

  attr["BunchRawInstLum"].setValue (blob);
  attr["AverageRawInstLum"].setValue (avgRawLumi);

  return attr;
}


std::unique_ptr<FillParamsCondData> makeFillParams()
{
  std::vector<uint16_t> bcids;
  for (const auto& p : lumiData) {
    bcids.push_back (p.first);
  }

  auto fp = std::make_unique<FillParamsCondData>();
  fp->setLuminousBunches (bcids.data(), bcids.data() + bcids.size());
  return fp;
}


void check (const std::vector<float>& v, float offs, float thresh)
{
  assert (v.size() == TOTAL_LHC_BCIDS);
  
  unsigned int nbcid = std::end(lumiData) - std::begin(lumiData);
  unsigned int ilumi = 0;
  for (unsigned int bcid = 0; bcid < TOTAL_LHC_BCIDS; bcid++) {
    float lumi = 0;
    if (ilumi < nbcid && lumiData[ilumi].first == bcid) {
      lumi = lumiData[ilumi].second+offs;
      ++ilumi;
    }
    if (lumi == 0) {
      assert (v[bcid] == 0);
    }
    else {
      assert (Athena_test::isEqual (lumi, v[bcid], thresh));
    }
  }
}


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  EventIDBase eid (0, 0, 0, 0);
  ctx.setEventID (eid);

  BunchLumisCondAlg alg ("BunchLumisCondAlg", svcloc);
  alg.addRef();
  assert( alg.sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  DataObjID id1 ("testLumi");
  auto cc1 = std::make_unique<CondCont<CondAttrListCollection> > (rcu, id1);

  auto attrs = std::make_unique<CondAttrListCollection> (false);
  attrs->add (0, makeAttrs (1, 0,  0));
  attrs->add (1, makeAttrs (2, 1, 10));
  attrs->add (2, makeAttrs (4, 2, 20));
  attrs->add (3, makeAttrs (8, 0, 30));

  const EventIDRange range (timestamp (0), timestamp (100));
  assert( cc1->insert (range, std::move(attrs), ctx).isSuccess() );

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");
  assert( conditionStore->record (std::move (cc1), "testLumi") );

  DataObjID id2 ("FillParamsCondData");
  auto cc2 = std::make_unique<CondCont<FillParamsCondData> > (rcu, id2);
  const EventIDRange range2 (timestamp (0), timestamp (80));
  assert( cc2->insert (range2, makeFillParams(), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc2), "FillParamsCondData") );

  assert( alg.execute (ctx).isSuccess() );

  CondCont<BunchLumisCondData>* cc3 = nullptr;
  assert( conditionStore->retrieve (cc3, "BunchLumisCondData").isSuccess() );
  const BunchLumisCondData* data = 0;
  const EventIDRange* range3p = nullptr;
  assert (cc3->find (eid, data, &range3p));
  assert (range3p->start().time_stamp() == timestamp(0).time_stamp());
  assert (range3p->stop().time_stamp() == timestamp(80).time_stamp());

  check (data->rawLuminosity(0),  0, 1e-2);
  check (data->rawLuminosity(1), 10, 1e-4);
  check (data->rawLuminosity(2), 20, 1e-6);
  check (data->rawLuminosity(3), 30, 1e-6);
  assert (data->rawLuminosity(5).empty());

  //====================================================================

  BunchLumisCondAlg alg5 ("BunchLumisCondAlg5", svcloc);
  alg5.addRef();
  assert( alg5.sysInitialize().isSuccess() );
  
  DataObjID id5 ("testLumi5");
  auto cc5 = std::make_unique<CondCont<CondAttrListCollection> > (rcu, id5);
  auto attrs5 = std::make_unique<CondAttrListCollection> (false);
  attrs5->add (1, makeAttrs (2, 1, 10));
  attrs5->add (2, makeAttrs (4, 2, 20));
  assert( cc5->insert (range, std::move(attrs5), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc5), "testLumi5") );

  assert( alg5.execute (ctx).isSuccess() );

  CondCont<BunchLumisCondData>* cc6 = nullptr;
  assert( conditionStore->retrieve (cc6, "BunchLumisCondData5").isSuccess() );
  const BunchLumisCondData* data6 = 0;
  const EventIDRange* range6p = nullptr;
  assert (cc6->find (eid, data6, &range6p));
  assert (range6p->start().time_stamp() == timestamp(0).time_stamp());
  assert (range6p->stop().time_stamp() == timestamp(100).time_stamp());

  check (data6->rawLuminosity(1), 10, 1e-4);
  check (data6->rawLuminosity(2), 20, 1e-6);
  assert (data6->rawLuminosity(0).empty());

  //====================================================================

  BunchLumisCondAlg alg7 ("BunchLumisCondAlg7", svcloc);
  alg7.addRef();
  assert( alg7.sysInitialize().isSuccess() );
  
  assert( alg7.execute (ctx).isSuccess() );
}


int main()
{
  std::cout << "CoolLumiUtilities/BunchLumisCondAlg\n";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("CoolLumiUtilities/BunchLumisCondAlg_test.txt", svcloc)) {
    return 1;
  }

  test1 (svcloc);
  return 0;
}
