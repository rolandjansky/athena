/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileEvent/test/TileMutableRawChannelContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileMutableRawChannelContainer.
 */


#undef NDEBUG
#include "TileEvent/TileMutableRawChannelContainer.h"
#include "TileConditions/TileCablingService.h"
#include "TileIdentifier/TileHWID.h"
#include "IdDictParser/IdDictParser.h"
#include <iostream>
#include <cassert>


static const size_t NCOLL = 2;
static const size_t NCHAN = 10;


class TileCablingSvc
{
public:
  IdDictParser parser;
  TileHWID hwid;
  TileTBID tbid;
  TileID   tileid;

  TileCablingSvc()
  {
    init_idhelpers();
  }

  void init_idhelpers()
  {
    tileid.set_do_neighbours (false);
    parser.register_external_entity("TileCalorimeter", "IdDictTileCalorimeter.xml");
    IdDictMgr& idd = parser.parse ("IdDictParser/ATLAS_IDS.xml");
    hwid.set_quiet (true);
    tbid.set_quiet (true);
    tileid.set_quiet (true);
    assert (hwid.initialize_from_dictionary (idd) == 0);
    assert (tbid.initialize_from_dictionary (idd) == 0);
    assert (tileid.initialize_from_dictionary (idd) == 0);
    TileCablingService* svc = TileCablingService::getInstance();
    svc->setTileHWID (&hwid);
    svc->setTileTBID (&tbid);
    svc->setTileID (&tileid);
  }
};


class TestTileMutableRawChannelContainer
  : public TileMutableRawChannelContainer
{
public:
  using TileMutableRawChannelContainer::TileMutableRawChannelContainer;
  using TileMutableRawChannelContainer::recycle;
};


void createCollection (TileMutableRawChannelContainer& cont, size_t hash)
{
  int frag = cont.hashFunc().identifier(hash);
  auto coll = std::make_unique<TileRawChannelCollection> (frag);

  coll->setFragGlobalCRC (234 + hash);
  coll->setFragDSPBCID (345 + hash);
  coll->setFragBCID (456 + hash);
  coll->setFragMemoryPar (567 + hash);
  coll->setFragSstrobe (687 + hash);
  coll->setFragDstrobe (890 + hash);
  coll->setFragHeaderBit (1011 + hash);
  coll->setFragHeaderPar (1021 + hash);
  coll->setFragSampleBit (1031 + hash);
  coll->setFragSamplePar (1041 + hash);
  coll->setFragFEChipMask (1051 + hash);
  coll->setFragRODChipMask (1061 + hash);
  coll->setLvl1Id (1071 + hash);
  coll->setLvl1Type (1081 + hash);
  coll->setDetEvType (1091 + hash);
  coll->setRODBCID (1101 + hash);

  assert( cont.addCollection (std::move (coll), hash).isSuccess() );
}


void addChans (TileMutableRawChannelContainer& cont, size_t hash)
{
  TileCablingService* cabling = TileCablingService::getInstance();
  int frag = cont.hashFunc().identifier (hash);
  HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (frag);
  for (size_t i = 0; i < NCHAN; i++) {
    HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);

    int offs = i + hash*NCHAN;
    std::vector<float> amplitude { offs*200.f, offs*200.f+10 };
    std::vector<float> time { offs*300.f+30, offs*400.f+30, offs*500.f+30 };
    std::vector<float> quality { offs*500 + 0.5f, offs*500 + 1.5f,
                                 offs*500 + 2.5f, offs*500 + 3.5f };
    float ped = offs + 100;

    auto chan = std::make_unique<TileRawChannel> (hwid,
                                                  std::move (amplitude),
                                                  std::move (time),
                                                  std::move (quality),
                                                  ped);
    assert( cont.push_back (std::move (chan)).isSuccess() );
  }
}


std::unique_ptr<TileMutableRawChannelContainer>
create1 (bool createColl)
{
  auto cont = std::make_unique<TileMutableRawChannelContainer> (createColl);
  assert( cont->status().isSuccess() );
  cont->set_bsflags (98765);

  if (!createColl) {
    for (size_t hash = 0; hash < NCOLL; hash++) {
      createCollection (*cont, hash);
    }
  }

  for (size_t hash = 0; hash < NCOLL; hash++) {
    addChans (*cont, hash);
  }

  return cont;
}


void checkFind1 (TileMutableRawChannelContainer& cont_nc, bool created)
{
  const TileMutableRawChannelContainer& cont = cont_nc;
  for (size_t hash = 0; hash < NCOLL; hash++) {
    TileRawChannelCollection* coll_nc = cont_nc.indexFindPtr (hash);
    const TileRawChannelCollection* coll = cont.indexFindPtr (hash);
    assert (coll_nc == coll);
  }
  if (!created) {
    assert (cont_nc.indexFindPtr(NCOLL) == nullptr);
  }
}


void checkFind2 (TileMutableRawChannelContainer& cont_nc)
{
  for (size_t hash = 0; hash < NCOLL; hash++) {
    assert (cont_nc.indexFindPtr(hash) == nullptr);
  }
}


void modify (TileMutableRawChannelContainer& cont, float offs)
{
  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    TileRawChannelCollection* coll = cont.indexFindPtr (hash);
    for (TileRawChannel* chan : *coll) {
      size_t sz = chan->size();
      for (size_t i = 0; i < sz; i++) {
        chan->setAmplitude (chan->amplitude(i) + offs, i);
      }
    }
  }
}


void check (const TileMutableRawChannelContainer& cont, bool created, float amp_offs)
{
  TileCablingService* cabling = TileCablingService::getInstance();

  assert (cont.get_type() == TileFragHash::Default);
  assert (cont.get_unit() == TileRawChannelUnit::ADCcounts);
  assert (cont.get_bsflags() == 98765);

  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    const TileRawChannelCollection* coll = cont.indexFindPtr (hash);
    assert (coll->identify() == cont.hashFunc().identifier (hash));
    if (!created && amp_offs >= 0) {
      assert (coll->getFragGlobalCRC() == 234 + hash);
      assert (coll->getFragDSPBCID() == 345 + hash);
      assert (coll->getFragBCID() == 456 + hash);
      assert (coll->getFragMemoryPar() == 567 + hash);
      assert (coll->getFragSstrobe() == 687 + hash);
      assert (coll->getFragDstrobe() == 890 + hash);
      assert (coll->getFragHeaderBit() == 1011 + hash);
      assert (coll->getFragHeaderPar() == 1021 + hash);
      assert (coll->getFragSampleBit() == 1031 + hash);
      assert (coll->getFragSamplePar() == 1041 + hash);
      assert (coll->getFragFEChipMask() == 1051 + hash);
      assert (coll->getFragRODChipMask() == 1061 + hash);
      assert (coll->getLvl1Id() == 1071 + hash);
      assert (coll->getLvl1Type() == 1081 + hash);
      assert (coll->getDetEvType() == 1091 + hash);
      assert (coll->getRODBCID() == 1101 + hash);
    }

    if (amp_offs < 0) {
      assert (coll->size() == 0);
    }
    else {
      HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (coll->identify());
      int i = 0;
      for (const TileRawChannel* chan : *coll) {
        int offs = i + hash*NCHAN;
        HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);
        assert (chan->adc_HWID() == hwid);
        assert (chan->pedestal() == offs + 100);

        assert (chan->size() == 2);
        assert (chan->amplitude(0) == offs*200.f    + amp_offs);
        assert (chan->amplitude(1) == offs*200.f+10 + amp_offs);
      
        assert (chan->sizeTime() == 3);
        assert (chan->time(0) == offs*300.f+30);
        assert (chan->time(1) == offs*400.f+30);
        assert (chan->time(2) == offs*500.f+30);

        assert (chan->sizeQuality() == 4);
        assert (chan->quality(0) == offs*500 + 0.5f);
        assert (chan->quality(1) == offs*500 + 1.5f);
        assert (chan->quality(2) == offs*500 + 2.5f);
        assert (chan->quality(3) == offs*500 + 3.5f);

        i += 1;
      }
    }
  }
}


void test1()
{
  std::cout << "test1\n";

  {
    std::unique_ptr<TileMutableRawChannelContainer> cont1 = create1 (false);
    checkFind1 (*cont1, false);
    cont1->lock();
    checkFind2 (*cont1);
    check (*cont1, false, 0);

    TileRawChannelContainer& rc1 = *cont1;
    auto cont1a = std::make_unique<TestTileMutableRawChannelContainer> (rc1);
    checkFind1 (*cont1a, false);
    check (*cont1a, false, 0);
    modify (*cont1a, 11);
    cont1a->lock();
    checkFind2 (*cont1a);
    check (*cont1a, false, 11);
    cont1a->recycle();
    assert (cont1a->get_bsflags() == 0);
    cont1a->set_bsflags (98765);
    checkFind1 (*cont1a, false);
    check (*cont1a, false, -1);
  }

  {
    std::unique_ptr<TileMutableRawChannelContainer> cont2 = create1 (true);
    checkFind1 (*cont2, true);
    modify (*cont2, 23);
    cont2->lock();
    checkFind2 (*cont2);
    check (*cont2, true, 23);

    TileRawChannelContainer& rc2 = *cont2;
    auto cont2a = std::make_unique<TileMutableRawChannelContainer> (rc2);
    checkFind1 (*cont2a, true);
    check (*cont2a, true, 23);
    modify (*cont2a, 11);
    cont2a->lock();
    checkFind2 (*cont2a);
    check (*cont2a, true, 34);
  }
}


int main()
{
  std::cout << "TileMutableRawChannelContainer_test\n";
  TileCablingSvc cabling;
  test1();
  return 0;
}

