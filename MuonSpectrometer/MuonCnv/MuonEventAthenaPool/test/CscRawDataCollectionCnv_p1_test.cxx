/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/CscRawDataCollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/CscRawDataCollectionCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const CscRawData& p1,
              const CscRawData& p2)
{
  assert (p1.samples() == p2.samples());
  assert (p1.address() == p2.address());
  assert (p1.identify() == p2.identify());
  //assert (p1.time() == p2.time());
  assert (p1.rpuID() == p2.rpuID());
  //assert (p1.width() == p2.width());
  assert (1 == p2.width());
  //assert (p1.isTimeComputed() == p2.isTimeComputed());
  //assert (p1.hashId() == p2.hashId());
}


void compare (const CscRawDataCollection& p1,
              const CscRawDataCollection& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.identifyHash() == p2.identifyHash());
  assert (p1.rodId() == p2.rodId());
  assert (p1.subDetectorId() == p2.subDetectorId());
  //assert (p1.numRPU() == p2.numRPU());
  //assert (p1.samplingPhase() == p2.samplingPhase());
  //assert (p1.triggerType() == p2.triggerType());
  //assert (p1.firstBitSummary() == p2.firstBitSummary());
  assert (p1.eventType() == p2.eventType());
  //assert (p1.rpuID() == p2.rpuID());
  //assert (p1.dataType() == p2.dataType());
  //assert (p1.spuCount() == p2.spuCount());
  //assert (p1.scaAddress() == p2.scaAddress());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const CscRawDataCollection& trans1)
{
  MsgStream log (nullptr, "test");
  CscRawDataCollectionCnv_p1 cnv;
  CscRawDataCollection_p1 pers;
  //cnv.transToPers (&trans1, &pers, log);

  pers.m_rate = 20;
  pers.m_numSamples = trans1.numSamples();
  pers.m_latency = trans1.latency();
  pers.m_numDPU = trans1.numRPU();
  pers.m_id = trans1.identify();
  pers.m_subDetectorId = trans1.subDetectorId();
  switch (trans1.rodId()) {
  case 0x0005:  pers.m_rodId = 0;  break;
  case 0x0007:  pers.m_rodId = 1;  break;
  case 0x0009:  pers.m_rodId = 2;  break;
  case 0x0011:  pers.m_rodId = 3;  break;
  case 0x0015:  pers.m_rodId = 4;  break;
  case 0x0017:  pers.m_rodId = 5;  break;
  case 0x0019:  pers.m_rodId = 6;  break;
  case 0x0021:  pers.m_rodId = 7;  break;
  default: std::abort();
  }

  pers.resize (trans1.size());
  for (size_t i=0; i < trans1.size(); i++) {
    const CscRawData& telt = *trans1[i];
    CscRawData_p1& pelt = pers[i];

    pelt.m_id = telt.identify();
    for (uint32_t amp : telt.samples())
      pelt.m_amps.push_back (amp - 2048);
    int strip = telt.address() & 0xff;
    pelt.m_address = strip;
  }

  CscRawDataCollection trans2;
  trans2.setIdentifyHash(trans1.identifyHash());
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  CscRawDataCollection trans1 (123);
  trans1.setIdentifyHash (123);
  trans1.setRodId (0x11);
  trans1.setSubDetectorId (456);
  trans1.set_samplingPhase (true);
  trans1.set_triggerType (true);
  trans1.set_firstBitSummary (56);
  trans1.set_eventType (78);
  trans1.setRPUID (std::vector<uint16_t> {321, 432, 53});
  trans1.addDataType (32);
  trans1.addDataType (43);
  for (int i=0; i < 10; i++)
    trans1.set_spuCount (i, i+11);
  trans1.set_scaAddress (789);

  for (int i=0; i < 4; i++) {
    int ioffs = i*20;
    std::unique_ptr<CscRawData> csc;
    // nb see comment in CscRawDataCnv_p1.cxx on chamberIndex
    if ((i&1) != 0) {
      csc = std::make_unique<CscRawData> (std::vector<uint16_t>(3001+ioffs, 3002+ioffs),
                                          2048+3+ioffs, 4+ioffs, 0, 6+ioffs);
    }
    else {
      csc = std::make_unique<CscRawData> (std::vector<uint16_t>(3011+ioffs, 3012+ioffs),
                                          2048+13+ioffs, 14+ioffs, 15+ioffs, 0, 17+ioffs, 18+ioffs);
    }
    trans1.push_back (std::move(csc));
  }

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
