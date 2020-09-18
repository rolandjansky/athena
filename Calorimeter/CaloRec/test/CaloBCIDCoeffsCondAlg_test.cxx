/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloRec/test/CaloBCIDCoeffsCondAlg_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2020
 * @brief Unit test for CaloBCIDCoeffsCondAlg.
 */


#undef NDEBUG
#include "../src/CaloBCIDCoeffsCondAlg.h"
#include "LArRawConditions/LArMCSym.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArMinBiasAverage.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloHelpersTest.h"
#include "IdDictParser/IdDictParser.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/DummyRCUSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "TestTools/initGaudi.h"
#include "TestTools/random.h"
#include "GaudiKernel/ServiceHandle.h"
#include <iostream>
#include <cassert>


static constexpr size_t NCELL = 1833;


EventIDBase runlbn (int run,
                    int lbn)
{
  return EventIDBase (run,  // run
                      EventIDBase::UNDEFEVT,  // event
                      EventIDBase::UNDEFNUM,  // timestamp
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
            const CaloCell_ID& calo_id,
            const LArOnlineID& online_id)
{
  std::cout << "test1\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  ctx.setEventID (runlbn (10, 50));

  CaloBCIDCoeffsCondAlg alg ("CaloBCIDCoeffsCondAlg", svcloc);
  alg.addRef();
  assert( alg.sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  std::vector<HWIdentifier> hwids = get_hwids (online_id);

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");

  DataObjID id_mcsym ("LArMCSym");
  auto cc_mcsym = std::make_unique<CondCont<LArMCSym> > (rcu, id_mcsym);
  auto mcsym = std::make_unique<LArMCSym> (&online_id,
                                           &calo_id,
                                           std::vector<HWIdentifier>(),
                                           std::vector<HWIdentifier>(),
                                           std::vector<HWIdentifier>(hwids));
  const EventIDRange range_mcsym (runlbn (10, 0), runlbn (10, 100));
  assert( cc_mcsym->insert (range_mcsym, std::move(mcsym), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc_mcsym), id_mcsym.key()) );

  DataObjID id_ofcs ("LArOFC");
  auto cc_ofcs = std::make_unique<CondCont<ILArOFC> > (rcu, id_ofcs);
  auto ofcs = std::make_unique<TestOFC> (5, hwids);
  const EventIDRange range_ofcs (runlbn (10, 20), runlbn (10, 120));
  assert( cc_ofcs->insert (range_ofcs, std::move(ofcs), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc_ofcs), id_ofcs.key()) );

  DataObjID id_shapes ("LArShape32");
  auto cc_shapes = std::make_unique<CondCont<ILArShape> > (rcu, id_shapes);
  auto shapes = std::make_unique<TestShape> (hwids);
  const EventIDRange range_shapes (runlbn (10, 10), runlbn (10, 80));
  assert( cc_shapes->insert (range_shapes, std::move(shapes), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc_shapes), id_shapes.key()) );

  DataObjID id_mb ("LArPileupAverageSym");
  auto cc_mb = std::make_unique<CondCont<ILArMinBiasAverage> > (rcu, id_mb);
  auto mb = std::make_unique<TestMinBiasAverage> (hwids);
  const EventIDRange range_mb (runlbn (10, 15), runlbn (10, 75));
  assert( cc_mb->insert (range_mb, std::move(mb), ctx).isSuccess() );
  assert( conditionStore->record (std::move (cc_mb), id_mb.key()) );

  assert( alg.execute (ctx).isSuccess() );

  CondCont<CaloBCIDCoeffs>* cc_coeffs = nullptr;
  assert( conditionStore->retrieve (cc_coeffs, "CaloBCIDCoeffs").isSuccess() );
  const CaloBCIDCoeffs* coeffs = nullptr;
  const EventIDRange* range_coeffs = nullptr;
  assert (cc_coeffs->find (runlbn (10, 50), coeffs, &range_coeffs));
  assert (range_coeffs->start().lumi_block() == 20);
  assert (range_coeffs->stop().lumi_block() == 75);

  assert (coeffs->nshapes() == 32);
  assert (coeffs->nsamples_coeff() == 5);
}


int main()
{
  std::cout << "CaloRec/CaloBCIDCoeffsCondAlg_test\n";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi ("CaloRec/CaloBCIDTests.txt", svcloc)) {
    return 1;
  }

  CaloHelpersTest caloHelpers;
  LArOnlineIDTest larhelpers;

  test1 (svcloc, caloHelpers.caloID(), larhelpers.onlineID());
  return 0;
}
