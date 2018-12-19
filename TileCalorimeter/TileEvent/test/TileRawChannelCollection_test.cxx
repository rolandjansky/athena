/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/test/TileRawChannelCollection_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileRawChannelCollection.
 */


#undef NDEBUG
#include "TileEvent/TileRawChannelCollection.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  TileRawChannelCollection coll (123);
  coll.setFragGlobalCRC (234);
  coll.setFragDSPBCID (345);
  coll.setFragBCID (456);
  coll.setFragMemoryPar (567);
  coll.setFragSstrobe (687);
  coll.setFragDstrobe (890);
  coll.setFragHeaderBit (1011);
  coll.setFragHeaderPar (1021);
  coll.setFragSampleBit (1031);
  coll.setFragSamplePar (1041);
  coll.setFragFEChipMask (1051);
  coll.setFragRODChipMask (1061);
  coll.setLvl1Id (1071);
  coll.setLvl1Type (1081);
  coll.setDetEvType (1091);
  coll.setRODBCID (1101);

  for (int i=0; i < 10; i++) {
    HWIdentifier hwid (10000 + i*1000);
    std::vector<float> amplitude { i*200.f, i*200.f+10 };
    std::vector<float> time { i*300.f+30, i*400.f+30, i*500.f+30 };
    std::vector<float> quality { i*500 + 0.5f, i*500 + 1.5f,
                                 i*500 + 2.5f, i*500 + 3.5f };
    float ped = i + 100;
    coll.push_back (std::make_unique<TileRawChannel> (hwid,
                                                      std::move (amplitude),
                                                      std::move (time),
                                                      std::move (quality),
                                                      ped));
  }

  TileRawChannelCollection newColl (coll);
  assert (newColl.identify() == 123);
  assert (newColl.getFragGlobalCRC() == 234);
  assert (newColl.getFragDSPBCID() == 345);
  assert (newColl.getFragBCID() == 456);
  assert (newColl.getFragMemoryPar() == 567);
  assert (newColl.getFragSstrobe() == 687);
  assert (newColl.getFragDstrobe() == 890);
  assert (newColl.getFragHeaderBit() == 1011);
  assert (newColl.getFragHeaderPar() == 1021);
  assert (newColl.getFragSampleBit() == 1031);
  assert (newColl.getFragSamplePar() == 1041);
  assert (newColl.getFragFEChipMask() == 1051);
  assert (newColl.getFragRODChipMask() == 1061);
  assert (newColl.getLvl1Id() == 1071);
  assert (newColl.getLvl1Type() == 1081);
  assert (newColl.getDetEvType() == 1091);
  assert (newColl.getRODBCID() == 1101);

  assert (newColl.size() == 10);
  for (int i=0; i < 10; i++) {
    const TileRawChannel* chan = newColl[i];
    assert (chan->adc_HWID() == HWIdentifier (10000 + i*1000));
    assert (chan->pedestal() == i + 100);

    assert (chan->size() == 2);
    assert (chan->amplitude(0) == i*200.f);
    assert (chan->amplitude(1) == i*200.f+10);

    assert (chan->sizeTime() == 3);
    assert (chan->time(0) == i*300.f+30);
    assert (chan->time(1) == i*400.f+30);
    assert (chan->time(2) == i*500.f+30);

    assert (chan->sizeQuality() == 4);
    assert (chan->quality(0) == i*500 + 0.5f);
    assert (chan->quality(1) == i*500 + 1.5f);
    assert (chan->quality(2) == i*500 + 2.5f);
    assert (chan->quality(3) == i*500 + 3.5f);
  }
}


int main()
{
  std::cout << "TileRawChannelCollection_test\n";
  test1();
  return 0;
}
