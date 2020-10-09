/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileEvent/test/TileMutableHitContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileMutableHitContainer.
 */


#undef NDEBUG
#include "TileEvent/TileMutableHitContainer.h"
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


class TestTileMutableHitContainer
  : public TileMutableHitContainer
{
public:
  using TileMutableHitContainer::TileMutableHitContainer;
  using TileMutableHitContainer::recycle;
};


void createCollection (TileMutableHitContainer& cont, size_t hash)
{
  int frag = cont.hashFunc().identifier(hash);
  auto coll = std::make_unique<TileHitCollection> (frag);

  assert( cont.addCollection (std::move (coll), hash).isSuccess() );
}


void addChans (TileMutableHitContainer& cont, size_t hash)
{
  TileCablingService* cabling = TileCablingService::getInstance();
  int frag = cont.hashFunc().identifier (hash);
  HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (frag);
  for (size_t i = 0; i < NCHAN; i++) {
    HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);
    Identifier id = cabling->h2s_pmt_id (hwid);
    int offs = i + hash*NCHAN;

    auto hit = std::make_unique<TileHit> (id);
    hit->resize(2);
    hit->setEnergy (offs*100, 0);
    hit->setEnergy (offs*100+10, 1);
    hit->setTime (offs*200, 0);
    hit->setTime (offs*200+10, 1);
    assert( cont.push_back (std::move (hit)).isSuccess() );
  }
}


std::unique_ptr<TileMutableHitContainer>
create1 (bool createColl)
{
  auto cont = std::make_unique<TileMutableHitContainer> (createColl);
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


void checkFind1 (TileMutableHitContainer& cont_nc, bool created)
{
  const TileMutableHitContainer& cont = cont_nc;
  for (size_t hash = 0; hash < NCOLL; hash++) {
    TileHitCollection* coll_nc = cont_nc.indexFindPtr (hash);
    const TileHitCollection* coll = cont.indexFindPtr (hash);
    assert (coll_nc == coll);
  }
  if (!created) {
    assert (cont_nc.indexFindPtr(NCOLL) == nullptr);
  }
}


void checkFind2 (TileMutableHitContainer& cont_nc)
{
  for (size_t hash = 0; hash < NCOLL; hash++) {
    assert (cont_nc.indexFindPtr(hash) == nullptr);
  }
}


void modify (TileMutableHitContainer& cont, float offs)
{
  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    TileHitCollection* coll = cont.indexFindPtr (hash);
    for (TileHit* hit : *coll) {
      size_t sz = hit->size();
      for (size_t i = 0; i < sz; i++) {
        hit->setEnergy (hit->energy(i)+offs, i);
        hit->setTime (hit->time(i)+offs, i);
      }
    }
  }
}


void check (const TileMutableHitContainer& cont, bool /*created*/, float hit_offs)
{
  TileCablingService* cabling = TileCablingService::getInstance();

  assert (cont.get_type() == TileFragHash::Default);
  assert (cont.get_unit() == TileRawChannelUnit::ADCcounts);
  assert (cont.get_bsflags() == 98765);

  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    const TileHitCollection* coll = cont.indexFindPtr (hash);
    assert (coll->identify() == cont.hashFunc().identifier (hash));
    HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (coll->identify());

    if (hit_offs < 0) {
      assert (coll->size() == 0);
    }
    else {
      int i = 0;
      for (const TileHit* hit : *coll) {
        int offs = i + hash*NCHAN;
        HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);
        Identifier id = cabling->h2s_pmt_id (hwid);
        assert (hit->identify() == id);

        assert (hit->size() == 2);
        assert (hit->energy(0) == offs*100 + hit_offs);
        assert (hit->energy(1) == offs*100+10 + hit_offs);
        assert (hit->time(0) == offs*200 + hit_offs);
        assert (hit->time(1) == offs*200+10 + hit_offs);

        i += 1;
      }
    }
  }
}


void test1()
{
  std::cout << "test1\n";

  {
    std::unique_ptr<TileMutableHitContainer> cont1 = create1 (false);
    checkFind1 (*cont1, false);
    cont1->lock();
    checkFind2 (*cont1);
    check (*cont1, false, 0);

    TileHitContainer& hc1 = *cont1;
    auto cont1a = std::make_unique<TestTileMutableHitContainer> (hc1);
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
    std::unique_ptr<TileMutableHitContainer> cont2 = create1 (true);
    checkFind1 (*cont2, true);
    modify (*cont2, 23);
    cont2->lock();
    checkFind2 (*cont2);
    check (*cont2, true, 23);

    TileHitContainer& hc2 = *cont2;
    auto cont2a = std::make_unique<TileMutableHitContainer> (hc2);
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
  std::cout << "TileMutableHitContainer_test\n";
  TileCablingSvc cabling;
  test1();
  return 0;
}

