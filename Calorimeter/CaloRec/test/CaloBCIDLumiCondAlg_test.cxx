/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/test/CaloBCIDLumiCondAlg_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2020
 * @brief Unit test for CaloBCIDLumiCondAlg.
 */


#undef NDEBUG
#include "../src/CaloBCIDLumiCondAlg.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArMinBiasAverage.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LumiBlockData/BunchCrossingCondData.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "IdDictParser/IdDictParser.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/DummyRCUSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "TestTools/initGaudi.h"
#include "TestTools/random.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/ServiceHandle.h"
#include <iostream>
#include <cassert>


static constexpr size_t NCELL = 1833;


EventIDBase timestamp (int t)
{
  return EventIDBase (EventIDBase::UNDEFNUM,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t);
}


EventIDBase runlbn (int run,
                    int lbn)
{
  return EventIDBase (run,  // run
                      EventIDBase::UNDEFEVT,  // event
                      EventIDBase::UNDEFNUM,  // timestamp
                      0,                      // ns offset
                      lbn);
}


EventIDBase mixed (int lbn,
                   int t)
{
  return EventIDBase (10,  // run
                      EventIDBase::UNDEFEVT,  // event
                      t,                      // timestamp
                      0,                      // ns offset
                      lbn);
}


//************************************************************************


class LArOnlineIDTest
{
public:
  LArOnlineIDTest();
  ~LArOnlineIDTest();

  LArOnlineIDTest (const LArOnlineIDTest&) = delete;
  LArOnlineIDTest& operator= (const LArOnlineIDTest&) = delete;

  const LArOnlineID&  onlineID() const { return *m_helper; }


private:
  std::unique_ptr<IdDictParser> m_parser;
  LArOnlineID*                  m_helper;
};


LArOnlineIDTest::LArOnlineIDTest()
  : m_parser (std::make_unique<IdDictParser>())
{
  m_parser->register_external_entity("LArCalorimeter", "IdDictLArCalorimeter_DC3-05.xml");
  IdDictMgr& idd = m_parser->parse("IdDictParser/ATLAS_IDS.xml");
  m_helper = new LArOnlineID;
  m_helper->set_quiet (true);
  if (m_helper->initialize_from_dictionary(idd) != 0) {
    std::abort();
  }

  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "test");
  assert( detStore->record (m_helper, "LArOnlineID").isSuccess() );
}


LArOnlineIDTest::~LArOnlineIDTest()
{
}


//************************************************************************


class TestData
{
public:
  TestData (const uint32_t seed_in,
            const size_t ndata,
            const std::vector<HWIdentifier>& hwids,
            float maxval = 1,
            float offset = 0);
  LArVectorProxy data (const HWIdentifier& id) const;

  std::unordered_map<HWIdentifier, std::vector<float> > m_data;
};


TestData::TestData (uint32_t seed_in,
                    const size_t ndata,
                    const std::vector<HWIdentifier>& hwids,
                    float maxval /*= 1*/,
                    float offset /*= 0*/)
{
  for (HWIdentifier id : hwids) {
    std::vector<float>& data = m_data[id];
    data.resize (ndata);
    uint32_t seed = id.get_identifier32().get_compact() + seed_in;
    for (size_t i = 0; i < ndata; ++i) {
      data[i] = Athena_test::randf_seed (seed, maxval) - offset;
    }
  }
}


LArVectorProxy TestData::data (const HWIdentifier& id) const
{
  auto it = m_data.find (id);
  if (it != m_data.end()) {
    return LArVectorProxy (it->second.data(), it->second.data()+it->second.size());
  }
  return LArVectorProxy();
}


//************************************************************************


class TestOFC : public ILArOFC
{
public:
  using OFCRef_t = LArVectorProxy;

  TestOFC (size_t nofc, const std::vector<HWIdentifier>& hwids)
    : m_data (1234, nofc, hwids)
  {
  }

  virtual OFCRef_t OFC_a (const HWIdentifier& id,
                          int,
                          int=0) const override
  {
    return m_data.data (id);
  }
  
  virtual OFCRef_t OFC_b(const HWIdentifier&,
                         int,
                         int=0) const override
  { std::abort(); }
  
  virtual float timeOffset(const HWIdentifier&, int) const override
  { std::abort(); }

  virtual unsigned nTimeBins(const HWIdentifier&, int) const override
  { std::abort(); }
 
  virtual float timeBinWidth(const HWIdentifier&, int) const override
  { std::abort(); }

  TestData m_data;
};


//************************************************************************


// I'm kind of pulling a fast one here.
// The actual shapes have both positive and negative entries,
// averaging to zero.  That means that we're summing a series
// where the terms have varying sign, and so the result will depend
// on the order in which the sums are done.  Thus, in general, we won't
// get exactly the same result if we calculate the offset using the
// original method or the vectorized method.  However, here we choose
// the entries to be entirely non-negative.  In that case,
// the order of summation won't matter, so we should get the same result.
class TestShape : public ILArShape
{
public:
  TestShape (const std::vector<HWIdentifier>& hwids)
    : m_data (4321, 32, hwids)
  {
  }

  virtual ShapeRef_t Shape   (const HWIdentifier& id,
                              int,
                              int = 0,
                              int = 0 )  const override
  {
    return m_data.data (id);
  }

  
  virtual ShapeRef_t ShapeDer(const HWIdentifier&,
                              int,
                              int = 0,
                              int = 0 )  const override
  { std::abort(); }

  TestData m_data;
};


//************************************************************************


class TestMinBiasAverage : public ILArMinBiasAverage
{
public:
  TestMinBiasAverage (const std::vector<HWIdentifier>& hwids)
    : m_data (563424, 1, hwids, 100, 10)
  {
  }

  virtual const float& minBiasAverage(const HWIdentifier& id)  const override
  {
    return *m_data.data(id).data();
  }

  TestData m_data;
};


//************************************************************************


class BunchCrossingCondAlg
{
public:
  static void fill (BunchCrossingCondData& bccd);
};


void BunchCrossingCondAlg::fill (BunchCrossingCondData& bccd)
{
  static constexpr int MAX_BCID = BunchCrossingCondData::m_MAX_BCID;
  uint32_t seed = 12734;
  for (size_t i = 0; i < MAX_BCID; i++) {
    bccd.m_luminous[i] = Athena_test::rng_seed (seed) & 0x10000;
  }
}


std::unique_ptr<BunchCrossingCondData> get_bccd()
{
  auto bccd = std::make_unique<BunchCrossingCondData>();
  BunchCrossingCondAlg::fill (*bccd);
  return bccd;
}


std::vector<float> get_lumivec()
{
  static constexpr int MAX_BCID = BunchCrossingCondData::m_MAX_BCID;
  uint32_t seed = 42345;
  std::vector<float> lumivec;
  lumivec.reserve (200);
  for (size_t i = 0; i < MAX_BCID; i++) {
    lumivec.push_back (Athena_test::randf_seed (seed, 10));
  }
  return lumivec;
}


//************************************************************************


std::vector<HWIdentifier> get_hwids (const LArOnlineID& online_id)
{
  std::vector<HWIdentifier> hwids;

  size_t nhec_left = 100;
  for (HWIdentifier hwid : online_id.channel_range()) {
    if (hwids.size() == NCELL) {
      break;
    }
    else if (online_id.isHECchannel (hwid) && nhec_left > 0) {
      hwids.push_back (hwid);
      --nhec_left;
    }
    else if ((NCELL - hwids.size()) > nhec_left) {
      hwids.push_back (hwid);
    }
  }
  
  return hwids;
}


//************************************************************************


void test1 (ISvcLocator* svcloc,
            const LArOnlineID& online_id)
{
  std::cout << "test1\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  ctx.setEventID (mixed (50, 50));

  CaloBCIDLumiCondAlg alg_mc ("CaloBCIDLumiCondAlgMC", svcloc);
  alg_mc.addRef();
  assert( alg_mc.sysInitialize().isSuccess() );
  CaloBCIDLumiCondAlg alg_data ("CaloBCIDLumiCondAlgData", svcloc);
  alg_data.addRef();
  assert( alg_data.sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  std::vector<HWIdentifier> hwids = get_hwids (online_id);
  TestOFC ofcs (5, hwids);
  TestShape shapes (hwids);
  TestMinBiasAverage minbias (hwids);

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");

  DataObjID id_coeffs ("CaloBCIDCoeffs");
  auto cc_coeffs = std::make_unique<CondCont<CaloBCIDCoeffs> > (rcu, id_coeffs);
  auto coeffs = std::make_unique<CaloBCIDCoeffs> (hwids,
                                                  online_id,
                                                  ofcs,
                                                  shapes,
                                                  minbias);
  const EventIDRange range_coeffs (runlbn (10, 20), runlbn (10, 75));
  assert( cc_coeffs->insert (range_coeffs, std::move(coeffs), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc_coeffs), id_coeffs.key()) );

  DataObjID id_bccd ("BunchCrossingData");
  auto cc_bccd = std::make_unique<CondCont<BunchCrossingCondData> > (rcu, id_bccd);
  const EventIDRange range_bccd (timestamp (0), timestamp (100));
  assert( cc_bccd->insert (range_bccd, get_bccd(), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc_bccd), id_bccd.key()) );

  DataObjID id_lumi ("LuminosityCondData");
  auto cc_lumi = std::make_unique<CondCont<LuminosityCondData> > (rcu, id_lumi);
  auto lumi = std::make_unique<LuminosityCondData>();
  lumi->setLbLuminosityPerBCIDVector (get_lumivec());
  const EventIDRange range_lumi (mixed (15, 20), mixed (80, 80));
  assert( cc_lumi->insert (range_lumi, std::move(lumi), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc_lumi), id_lumi.key()) );

  assert( alg_mc.execute (ctx).isSuccess() );

  CondCont<CaloBCIDLumi>* cc_lumimc = nullptr;
  assert( conditionStore->retrieve (cc_lumimc, "CaloBCIDLumiMC").isSuccess() );
  const CaloBCIDLumi* lumimc = nullptr;
  const EventIDRange* range_lumimc = nullptr;
  assert (cc_lumimc->find (mixed (50, 50), lumimc, &range_lumimc));
  assert (range_lumimc->start().lumi_block() == 20);
  assert (range_lumimc->start().time_stamp() == 0);
  assert (range_lumimc->stop().lumi_block() == 75);
  assert (range_lumimc->stop().time_stamp() == 100);

  CxxUtils::vec_aligned_vector<float> out;
  lumimc->calc (1, 1, out);
  assert (out.size() == NCELL);
  assert (Athena_test::isEqual (out[10], 110.33034));
  assert (Athena_test::isEqual (out[20], 101.59116));

  assert( alg_data.execute (ctx).isSuccess() );

  CondCont<CaloBCIDLumi>* cc_lumidata = nullptr;
  assert( conditionStore->retrieve (cc_lumidata, "CaloBCIDLumiData").isSuccess() );
  const CaloBCIDLumi* lumidata = nullptr;
  const EventIDRange* range_lumidata = nullptr;
  assert (cc_lumidata->find (mixed (50, 50), lumidata, &range_lumidata));
  assert (range_lumidata->start().lumi_block() == 20);
  assert (range_lumidata->start().time_stamp() == 20);
  assert (range_lumidata->stop().lumi_block() == 75);
  assert (range_lumidata->stop().time_stamp() == 80);

  out.clear();
  lumidata->calc (1, 1, out);
  assert (out.size() == NCELL);
  assert (Athena_test::isEqual (out[10], 6697.305));
  assert (Athena_test::isEqual (out[20], 5756.6152));
}


int main()
{
  std::cout << "CaloRec/CaloBCIDLumiCondAlg_test\n";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi ("CaloRec/CaloBCIDTests.txt", svcloc)) {
    return 1;
  }

  LArOnlineIDTest larhelpers;

  test1 (svcloc, larhelpers.onlineID());
  return 0;
}
