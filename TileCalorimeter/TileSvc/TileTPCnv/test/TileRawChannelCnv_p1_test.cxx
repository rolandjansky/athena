/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileTPCnv/test/TileRawChannelCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileRawChannelCnv_p1.
 */


#undef NDEBUG
#include "TileTPCnv/TileRawChannelCnv_p1.h"
#include "TileConditions/TileCablingService.h"
#include "TileIdentifier/TileHWID.h"
#include "IdDictParser/IdDictParser.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


TileHWID hwid;
TileTBID tbid;
TileID   tileid;

class TileCablingSvc
{
public:
  static
  void init_idhelpers ATLAS_NOT_THREAD_SAFE (IdDictParser& parser)
  {
    tileid.set_do_neighbours (false);
    IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
    assert (hwid.initialize_from_dictionary (idd) == 0);
    assert (tbid.initialize_from_dictionary (idd) == 0);
    assert (tileid.initialize_from_dictionary (idd) == 0);
    TileCablingService* svc = TileCablingService::getInstance_nc();
    svc->setTileHWID (&hwid);
    svc->setTileTBID (&tbid);
    svc->setTileID (&tileid);
  }
};


void compare (const TileRawChannel& p1,
              const TileRawChannel& p2)
{
  assert (p1.identify() == p2.identify());

  assert (p1.size() == p2.size());
  for (int i=0; i < p1.size(); i++)
    assert (p1.amplitude(i) == p2.amplitude(i));
  assert (p1.sizeTime() == p2.sizeTime());
  for (int i=0; i < p1.sizeTime(); i++)
    assert (p1.time(i) == p2.time(i));
  assert (p1.sizeQuality() == p2.sizeQuality());
  for (int i=0; i < p1.sizeQuality(); i++)
    assert (p1.quality(i) == p2.quality(i));
  assert (p1.pedestal() == p2.pedestal());
}


void testit (const TileRawChannel& trans1)
{
  MsgStream log (0, "test");
  TileRawChannelCnv_p1 cnv;
  TileRawChannel_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileRawChannel trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TileRawChannel trans1 (hwid.adc_id (1, 2, 3, 0),
                         std::vector<float> {1.5},
                         std::vector<float> {2.5, 3.5},
                         std::vector<float> {4.5, 5.5, 6.5},
                         7.5);

  testit (trans1);
}


int main()
{
  IdDictParser parser;
  TileCablingSvc::init_idhelpers(parser);
  test1();
  return 0;
}
