/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file LumiBlockComps/test/LuminosityAlg_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Unit test for LuminosityCondAlg.
 */


#undef NDEBUG
#include "../src/LuminosityCondAlg.h"
#include "LumiBlockData/LuminosityCondData.h"
#include "CoolLumiUtilities/BunchLumisCondData.h"
#include "CoolLumiUtilities/BunchGroupCondData.h"
#include "CoolLumiUtilities/FillParamsCondData.h"
#include "CoolLumiUtilities/OnlineLumiCalibrationCondData.h"
#include "AthenaKernel/DummyRCUSvc.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "CoolKernel/IObject.h"
#include "CxxUtils/checker_macros.h"
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


// Run1 results
const std::pair<unsigned int, float> lumiCalibData[] =
  {
   { 10, 0.171479 },
   { 20, 0.622183 },
   { 30, 0.239085 },
   { 40, 0.277113 },
   { 55, 0.361620 },
};


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


coral::Blob makeBlob (float offs)
{
  unsigned int bss = 4;
  unsigned int smod = 1;

  std::vector<uint8_t> data;
  data.push_back (bss*10 + smod);

  unsigned int nbcid = std::end(lumiData) - std::begin(lumiData);

  unsigned int ilumi = 0;
  for (unsigned int bcid = 0; bcid < TOTAL_LHC_BCIDS; bcid++) {
    float lumi = 0;
    if (ilumi < nbcid && lumiData[ilumi].first == bcid) {
      lumi = lumiData[ilumi].second+offs;
      ++ilumi;
    }
    push_float (lumi, data);
  }

  coral::Blob blob (data.size());
  memcpy (blob.startingAddress(), data.data(), data.size());
  return blob;
}


std::unique_ptr<CondAttrListCollection> make_run2_attrlist (float instLumi = 1.5)
{
  auto attrs = std::make_unique<CondAttrListCollection> (false);
  coral::AttributeList al;

  al.extend ("LBAvInstLumi", "float");
  al.extend ("LBAvEvtsPerBX", "float");
  al.extend ("Valid", "unsigned int");
  al.extend ("AlgorithmID", "unsigned int");
  al.extend ("BunchInstLumi", "blob");
  al["LBAvInstLumi"].setValue (instLumi);
  al["LBAvEvtsPerBX"].setValue (10.5f);
  al["Valid"].setValue (0u);
  al["AlgorithmID"].setValue (42u);
  al["BunchInstLumi"].setValue (makeBlob (0));
  attrs->add (0, al);
  return attrs;
}


std::unique_ptr<CondAttrListCollection> make_run1_attrlist()
{
  auto attrs = std::make_unique<CondAttrListCollection> (false);
  coral::AttributeList al;

  al.extend ("LBAvInstLumi", "float");
  al.extend ("LBAvEvtsPerBX", "float");
  al.extend ("Valid", "unsigned int");
  al["LBAvInstLumi"].setValue (1.5f);
  al["LBAvEvtsPerBX"].setValue (10.5f);
  unsigned int valid = (42) << 22;
  al["Valid"].setValue (valid);
  attrs->add (0, al);
  return attrs;
}


std::unique_ptr<OnlineLumiCalibrationCondData> make_onlineLumiCalib()
{
  auto calib = std::make_unique<OnlineLumiCalibrationCondData>();

  float muToLumi = 2.5;
  float p0 = 1;
  float p1 = 1;
  float p2 = 1;

  coral::AttributeList attr;
  attr.extend ("NumOfParameters", "unsigned int");
  attr.extend ("Function", "string");
  attr.extend ("MuToLumi", "float");
  attr.extend ("Parameters", "blob");

  attr["MuToLumi"].setValue (muToLumi);
  attr["Function"].setValue (std::string ("Polynomial"));

  coral::Blob blob (9 * sizeof(float));
  float* p = static_cast<float*> (blob.startingAddress());
  p[0] = 1;
  p[1] = 0;
  p[2] = 100;
  p[3] = p0;
  p[4] = p1;
  p[5] = p2;
  p[6] = 0;
  p[7] = 0;
  p[8] = 0;
  attr["Parameters"].setValue (blob);
  attr["NumOfParameters"].setValue (9u);

  OnlineLumiCalibrator lc;
  assert (lc.setCalibration (attr));
  calib->set (42, std::move (lc));
  
  return calib;
}


std::unique_ptr<BunchLumisCondData> make_bunchLumis()
{
  auto bl = std::make_unique<BunchLumisCondData>();
  std::vector<float> rawLumi (LuminosityCondData::TOTAL_LHC_BCIDS);
  for (const auto& p : lumiData) {
    rawLumi[p.first] = p.second;
  }
  bl->addChannel (42, std::move (rawLumi));
  return bl;
}


std::unique_ptr<BunchGroupCondData> make_bunchGroup()
{
  auto bg = std::make_unique<BunchGroupCondData>();
  for (const auto& p : lumiData) {
    bg->addBCID (p.first, 2);
  }
  return bg;
}


std::unique_ptr<FillParamsCondData> make_fillParams()
{
  auto params = std::make_unique<FillParamsCondData>();
  std::vector<uint16_t> bunches;
  for (const auto& p : lumiData) {
    if (p.first == 10) continue;
    bunches.push_back (p.first);
  }
  params->setLuminousBunches (bunches.data(), bunches.data()+bunches.size());
  return params;
}

// need to keep the Algs around until the end or the CondSvc will complain
std::vector<LuminosityCondAlg*> theAlgs;

// run2
void test1 ATLAS_NOT_REENTRANT (ISvcLocator* svcloc)
{
  std::cout << "test1\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  EventIDBase eid (1, 0, 0, 0, 20);
  ctx.setEventID (eid);

  LuminosityCondAlg* alg = new LuminosityCondAlg("LuminosityCondAlg", svcloc);
  theAlgs.push_back(alg);
  alg->addRef();
  assert( alg->sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  
  DataObjID id1 ("testLumi");
  auto cc1 = std::make_unique<CondCont<CondAttrListCollection> > (rcu, id1);
  const EventIDRange range1 (runlbn (1, 10), runlbn (1, 100));
  assert( cc1->insert (range1, make_run2_attrlist(), ctx).isSuccess() );

  DataObjID id2 ("testCalib");
  auto cc2 = std::make_unique<CondCont<OnlineLumiCalibrationCondData> > (rcu, id2);
  const EventIDRange range2 (timestamp (0), timestamp (90));
  assert( cc2->insert (range2, make_onlineLumiCalib(), ctx).isSuccess() );

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");
  assert( conditionStore->record (std::move (cc1), "testLumi") );
  assert( conditionStore->record (std::move (cc2), "testCalib") );

  assert( alg->execute (ctx).isSuccess() );

  CondCont<LuminosityCondData>* ccout = nullptr;
  assert( conditionStore->retrieve (ccout, "LuminosityCondData").isSuccess() );
  const LuminosityCondData* data = 0;
  const EventIDRange* rangeout = nullptr;
  assert (ccout->find (eid, data, &rangeout));
  assert (rangeout->start().time_stamp() == 0);
  assert (rangeout->start().run_number() == 1);
  assert (rangeout->start().lumi_block() == 10);
  assert (rangeout->stop().time_stamp() == 90);
  assert (rangeout->stop().run_number() == 1);
  assert (rangeout->stop().lumi_block() == 100);

  assert( data->lbAverageLuminosity() == 1.5 );
  assert( data->lbAverageInteractionsPerCrossing() == 10.5 );
  assert( data->lbAverageValid() == 0 );
  assert( data->muToLumi() == 2.5 );

  std::vector<float> vec = data->lbLuminosityPerBCIDVector();
  assert (vec.size() == LuminosityCondData::TOTAL_LHC_BCIDS);
  for (const auto& p : lumiData) {
    assert (vec[p.first] == p.second);
    vec[p.first] = 0;
  }

  for (float f : vec) {
    assert (f == 0);
  }
}


// run1
void test2 ATLAS_NOT_REENTRANT (ISvcLocator* svcloc)
{
  std::cout << "test2\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  EventIDBase eid (1, 0, 0, 0, 20);
  ctx.setEventID (eid);

  LuminosityCondAlg *alg = new LuminosityCondAlg ("LuminosityCondAlgRun1", svcloc);
  theAlgs.push_back(alg);
  alg->addRef();
  assert( alg->sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  
  DataObjID id1 ("testLumiRun1");
  auto cc1 = std::make_unique<CondCont<CondAttrListCollection> > (rcu, id1);
  const EventIDRange range1 (runlbn (1, 10), runlbn (1, 100));
  assert( cc1->insert (range1, make_run1_attrlist(), ctx).isSuccess() );

  DataObjID id2 ("testCalibRun1");
  auto cc2 = std::make_unique<CondCont<OnlineLumiCalibrationCondData> > (rcu, id2);
  const EventIDRange range2 (timestamp (0), timestamp (90));
  assert( cc2->insert (range2, make_onlineLumiCalib(), ctx).isSuccess() );

  DataObjID id3 ("testBunchLumisRun1");
  auto cc3 = std::make_unique<CondCont<BunchLumisCondData> > (rcu, id3);
  const EventIDRange range3 (timestamp (0), timestamp (85));
  assert( cc3->insert (range3, make_bunchLumis(), ctx).isSuccess() );

  DataObjID id4 ("testBunchGroupRun1");
  auto cc4 = std::make_unique<CondCont<BunchGroupCondData> > (rcu, id4);
  const EventIDRange range4 (timestamp (0), timestamp (95));
  assert( cc4->insert (range4, make_bunchGroup(), ctx).isSuccess() );

  DataObjID id5 ("testFillParamsRun1");
  auto cc5 = std::make_unique<CondCont<FillParamsCondData> > (rcu, id5);
  const EventIDRange range5 (timestamp (0), timestamp (80));
  assert( cc5->insert (range5, make_fillParams(), ctx).isSuccess() );

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");
  assert( conditionStore->record (std::move (cc1), "testLumiRun1") );
  assert( conditionStore->record (std::move (cc2), "testCalibRun1") );
  assert( conditionStore->record (std::move (cc3), "testBunchLumisRun1") );
  assert( conditionStore->record (std::move (cc4), "testBunchGroupRun1") );
  assert( conditionStore->record (std::move (cc5), "testFillParamsRun1") );

  assert( alg->execute (ctx).isSuccess() );

  CondCont<LuminosityCondData>* ccout = nullptr;
  assert( conditionStore->retrieve (ccout, "LuminosityCondDataRun1").isSuccess() );
  const LuminosityCondData* data = 0;
  const EventIDRange* rangeout = nullptr;
  assert (ccout->find (eid, data, &rangeout));
  assert (rangeout->start().time_stamp() == 0);
  assert (rangeout->start().run_number() == 1);
  assert (rangeout->start().lumi_block() == 10);
  assert (rangeout->stop().time_stamp() == 80);
  assert (rangeout->stop().run_number() == 1);
  assert (rangeout->stop().lumi_block() == 100);

  assert( data->lbAverageLuminosity() == 1.5 );
  assert( data->lbAverageInteractionsPerCrossing() == 10.5 );
  assert( (data->lbAverageValid() >> 22) == 42 );
  assert( ((data->lbAverageValid()&0x3ff) % 100) == 0 );
  assert( data->muToLumi() == 2.5 );

  std::vector<float> vec = data->lbLuminosityPerBCIDVector();
  assert (vec.size() == LuminosityCondData::TOTAL_LHC_BCIDS);
  for (const auto& p : lumiCalibData) {
    assert( Athena_test::isEqual (vec[p.first], p.second, 1e-5) );
    vec[p.first] = 0;
  }

  for (float f : vec) {
    assert (f == 0);
  }
}


// MC
void test3 ATLAS_NOT_REENTRANT (ISvcLocator* svcloc)
{
  std::cout << "test3\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  EventIDBase eid (1, 0, 0, 0, 20, 0);
  ctx.setEventID (eid);

  LuminosityCondAlg *alg = new LuminosityCondAlg("LuminosityCondAlgMC", svcloc);
  theAlgs.push_back(alg);
  alg->addRef();
  assert( alg->sysInitialize().isSuccess() );

  assert( alg->execute (ctx).isSuccess() );

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");
  CondCont<LuminosityCondData>* ccout = nullptr;
  assert( conditionStore->retrieve (ccout, "LuminosityCondDataMC").isSuccess() );
  const LuminosityCondData* data = 0;
  const EventIDRange* rangeout = nullptr;
  assert (ccout->find (eid, data, &rangeout));
  assert (rangeout->start().time_stamp() == timestamp(0).time_stamp());

  assert( data->lbAverageLuminosity() == 0 );
  assert( data->lbAverageInteractionsPerCrossing() == 0 );
  assert( data->lbAverageValid() == 0xffffffff );
  assert( data->muToLumi() == 0 );

  std::vector<float> vec = data->lbLuminosityPerBCIDVector();
  assert (vec.size() == LuminosityCondData::TOTAL_LHC_BCIDS);
  for (float f : vec) {
    assert (f == 0);
  }
}


// Missing lumi
void test4 ATLAS_NOT_REENTRANT (ISvcLocator* svcloc)
{
  std::cout << "test4\n";

  EventContext ctx;
  ctx.setExtension (Atlas::ExtendedEventContext());
  EventIDBase eid (1, 0, 0, 0, 20);
  ctx.setEventID (eid);

  LuminosityCondAlg *alg = new LuminosityCondAlg ("LuminosityCondAlgMiss", svcloc);
  theAlgs.push_back(alg);
  alg->addRef();
  assert( alg->sysInitialize().isSuccess() );

  Athena_test::DummyRCUSvc rcu;
  
  DataObjID id1 ("testLumiMiss");
  auto cc1 = std::make_unique<CondCont<CondAttrListCollection> > (rcu, id1);
  const EventIDRange range1 (runlbn (1, 10), runlbn (1, 100));
  assert( cc1->insert (range1, make_run2_attrlist(0), ctx).isSuccess() );

  DataObjID id2 ("testCalibMiss");
  auto cc2 = std::make_unique<CondCont<OnlineLumiCalibrationCondData> > (rcu, id2);
  const EventIDRange range2 (timestamp (0), timestamp (90));
  assert( cc2->insert (range2, make_onlineLumiCalib(), ctx).isSuccess() );

  ServiceHandle<StoreGateSvc> conditionStore ("ConditionStore", "test");
  assert( conditionStore->record (std::move (cc1), "testLumiMiss") );
  assert( conditionStore->record (std::move (cc2), "testCalibMiss") );

  assert( alg->execute (ctx).isSuccess() );

  CondCont<LuminosityCondData>* ccout = nullptr;
  assert( conditionStore->retrieve (ccout, "LuminosityCondDataMiss").isSuccess() );
  const LuminosityCondData* data = 0;
  const EventIDRange* rangeout = nullptr;
  assert (ccout->find (eid, data, &rangeout));
  assert (rangeout->start().time_stamp() == 0);
  assert (rangeout->start().run_number() == 1);
  assert (rangeout->start().lumi_block() == 10);
  assert (rangeout->stop().time_stamp() == EventIDBase::UNDEFNUM-1);
  assert (rangeout->stop().run_number() == 1);
  assert (rangeout->stop().lumi_block() == 100);
 
  assert( data->lbAverageLuminosity() == 0 );
  assert( data->lbAverageInteractionsPerCrossing() == 10.5 );
  assert( data->lbAverageValid() == 0 );
  assert( data->muToLumi() == 0 );

  std::vector<float> vec = data->lbLuminosityPerBCIDVector();
  assert (vec.size() == LuminosityCondData::TOTAL_LHC_BCIDS);
  for (const auto& p : lumiData) {
    assert (vec[p.first] == 0);
  }

  for (float f : vec) {
    assert (f == 0);
  }
}


int main ATLAS_NOT_REENTRANT ()
{
  std::cout << "LumiBlockComps/LuminosityCondAlg_test\n";

  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("LumiBlockComps/LuminosityCondAlg_test.txt", svcloc)) {
    return 1;
  }

  test1 (svcloc);
  test2 (svcloc);
  test3 (svcloc);
  test4 (svcloc);

  for ( auto &a : theAlgs) {
    delete a;
  }
  
  return 0;
}
