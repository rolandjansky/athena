/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockComps/test/TrigLiveFractionCondAlg_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jum, 2019
 * @brief Unit test for TrigLiveFractionCondAlg.
 */


#undef NDEBUG
#include "../src/TrigLiveFractionCondAlg.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "LumiBlockData/TrigLiveFractionCondData.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/DummyRCUSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include <iostream>
#include <cassert>


static const unsigned int totalTurns = 100000;
static const unsigned int TOTAL_LHC_BCIDS = LuminosityCondData::TOTAL_LHC_BCIDS;


// BCID, busy, weight
typedef std::tuple<unsigned int, unsigned int, float> Data;
const Data busyDataLo[] =
  {
   { 10,   100, 1.1 },
   { 20, 90000, 0.5 },
   { 30, 12345, 2.0 },
   { 40, 54321, 0.7 },
   { 50, 10101, 2.5 },
};


const Data busyDataHi[] =
  {
   { 15,   501, 3.5 },
   { 25, 80003, 0.3 },
   { 35, 32341, 0.7 },
   { 45, 14326, 1.7 },
   { 55, 60106, 1.1 },
};


EventIDBase timestamp (int t)
{
  return EventIDBase (EventIDBase::UNDEFNUM,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t);
}


EventIDBase mixed (int run, int lbn, int t)
{
  return EventIDBase (run,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t,
                      0, // ns offset
                      lbn);
}


template <class ITERATOR>
coral::Blob make_busyblob (ITERATOR beg, ITERATOR end)
{
  std::vector<uint8_t> data;
  for (unsigned int bcid = 0; bcid < TOTAL_LHC_BCIDS; ++bcid)
  {
    unsigned int idata = 0;
    if (beg != end && std::get<0>(*beg) == bcid) {
      idata = std::get<1>(*beg);
      ++beg;
    }
    data.push_back (idata & 0xff);
    data.push_back ((idata>>8) & 0xff);
    data.push_back ((idata>>16) & 0xff);
  }

  coral::Blob blob (data.size());
  memcpy (blob.startingAddress(), data.data(), data.size());
  return blob;
}


std::unique_ptr<AthenaAttributeList> make_attrlist()
{
  auto attrs = std::make_unique<AthenaAttributeList>();
  attrs->extend ("TurnCounter", "unsigned int");
  attrs->extend ("LowPriority", "blob");
  attrs->extend ("HighPriority", "blob");
  (*attrs)["TurnCounter"].setValue (totalTurns);

  (*attrs)["LowPriority"].setValue (make_busyblob (std::begin (busyDataLo),
                                                   std::end (busyDataLo)));
  (*attrs)["HighPriority"].setValue (make_busyblob (std::begin (busyDataHi),
                                                    std::end (busyDataHi)));
  
  return attrs;
}


template <class ITERATOR>
float fill_lumiweights  (std::vector<float>& w, ITERATOR beg, ITERATOR end)
{
  float totW = 0;
  for (; beg != end; ++beg) {
    w[std::get<0> (*beg)] = std::get<2> (*beg);
    totW += std::get<2> (*beg);
  }
  return totW;
}


std::unique_ptr<LuminosityCondData> make_lumi (float& totW)
{
  auto lumi = std::make_unique<LuminosityCondData>();
  std::vector<float> w (TOTAL_LHC_BCIDS, 0.0);
  totW = 0;
  totW += fill_lumiweights (w, std::begin(busyDataLo), std::end(busyDataLo));
  totW += fill_lumiweights (w, std::begin(busyDataHi), std::end(busyDataHi));
  lumi->setLbLuminosityPerBCIDVector (std::move (w));
  return lumi;
}


template <class ITERATOR>
void checkVec (const std::vector<float>& vec, ITERATOR beg, ITERATOR end)
{
  assert (vec.size() == TOTAL_LHC_BCIDS);
  for (unsigned int bcid = 0; bcid < TOTAL_LHC_BCIDS; ++bcid) {
    unsigned int idata = 0;
    if (beg != end && std::get<0>(*beg) == bcid) {
      idata = std::get<1>(*beg);
      ++beg;
    }
    assert( Athena_test::isEqual (vec[bcid],
                                  static_cast<float> (totalTurns - idata) / totalTurns) );
  }
}


template <class ITERATOR>
float calc_weighted (float totW, ITERATOR beg, ITERATOR end)
{
  float num = 0;
  float thistot = 0;
  for (; beg != end; ++beg) {
    unsigned int ibusy = std::get<1> (*beg);
    float livefrac = static_cast<float>(totalTurns - ibusy) / totalTurns;
    num += livefrac * std::get<2> (*beg);
    thistot += std::get<2> (*beg);
  }
  return (num + (totW-thistot)) / totW;
}


void test1 (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  EventIDBase eid (1, 0, 0, 0, 20);
  ctx.setEventID (eid);

  TrigLiveFractionCondAlg alg ("TrigLiveFractionCondAlg", svcloc);
  alg.addRef();
  assert( alg.sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  
  DataObjID id1 ("testDeadtime");
  auto cc1 = std::make_unique<CondCont<AthenaAttributeList> > (rcu, id1);
  const EventIDRange range1 (timestamp (0), timestamp (100));
  assert( cc1->insert (range1, make_attrlist(), ctx).isSuccess() );

  DataObjID id2 ("testLumi");
  auto cc2 = std::make_unique<CondCont<LuminosityCondData> > (rcu, id2);
  const EventIDRange range2 (mixed (1, 10, 0), mixed (1, 100, 90));
  float totW = 0;
  assert( cc2->insert (range2, make_lumi(totW), ctx).isSuccess() );

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");
  assert( conditionStore->record (std::move (cc1), id1.key()) );
  assert( conditionStore->record (std::move (cc2), id2.key()) );

  assert( alg.execute (ctx).isSuccess() );

  CondCont<TrigLiveFractionCondData>* ccout = nullptr;
  assert( conditionStore->retrieve (ccout, "TrigLiveFractionCondData").isSuccess() );
  const TrigLiveFractionCondData* data = nullptr;
  const EventIDRange* rangeout = nullptr;
  assert (ccout->find (eid, data, &rangeout));
  assert (rangeout->start().time_stamp() == timestamp(0).time_stamp());
  assert (rangeout->stop().time_stamp() == timestamp(90).time_stamp());

  assert (data->lhcTurnCounter() == totalTurns);
  checkVec (data->l1LiveFractionVector (false),
            std::begin(busyDataLo), std::end(busyDataLo));
  checkVec (data->l1LiveFractionVector (true),
            std::begin(busyDataHi), std::end(busyDataHi));

  assert( Athena_test::isEqual (data->lbAverageLiveFraction (false),
                                calc_weighted (totW,
                                               std::begin(busyDataLo),
                                               std::end(busyDataLo))) );
  assert( Athena_test::isEqual (data->lbAverageLiveFraction (true),
                                calc_weighted (totW,
                                               std::begin(busyDataHi),
                                               std::end(busyDataHi))) );
}


int main()
{
  std::cout << "LumiBlockComps/TrigLiveFractionCondAlg_test\n";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("LumiBlockComps/TrigLiveFractionCondAlg_test.txt", svcloc)) {
    return 1;
  }

  test1 (svcloc);
  return 0;
}

