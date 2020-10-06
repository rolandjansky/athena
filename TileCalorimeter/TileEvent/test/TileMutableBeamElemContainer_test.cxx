/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileEvent/test/TileMutableBeamElemContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileMutableBeamElemContainer.
 */


#undef NDEBUG
#include "TileEvent/TileMutableBeamElemContainer.h"
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


class TestTileMutableBeamElemContainer
  : public TileMutableBeamElemContainer
{
public:
  using TileMutableBeamElemContainer::TileMutableBeamElemContainer;
  using TileMutableBeamElemContainer::recycle;
};


void createCollection (TileMutableBeamElemContainer& cont, size_t hash)
{
  int frag = cont.hashFunc().identifier(hash);
  auto coll = std::make_unique<TileBeamElemCollection> (frag);

  coll->setLvl1Id (1071 + hash);
  coll->setLvl1Type (1081 + hash);
  coll->setDetEvType (1091 + hash);
  coll->setRODBCID (1101 + hash);

  assert( cont.addCollection (std::move (coll), hash).isSuccess() );
}


void addChans (TileMutableBeamElemContainer& cont, size_t hash)
{
  TileCablingService* cabling = TileCablingService::getInstance();
  int frag = cont.hashFunc().identifier (hash);
  HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (frag);
  for (size_t i = 0; i < NCHAN; i++) {
    HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);

    int offs = i + hash*NCHAN;
    std::vector<uint32_t> digits { offs*500u + 1, offs*500u + 2,
                                   offs*500u + 3, offs*500u + 4 };
    auto chan = std::make_unique<TileBeamElem> (hwid,
                                                std::move (digits));
    assert( cont.push_back (std::move (chan)).isSuccess() );
  }
}


std::unique_ptr<TileMutableBeamElemContainer>
create1 (bool createColl)
{
  auto cont = std::make_unique<TileMutableBeamElemContainer> (createColl);
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


void checkFind1 (TileMutableBeamElemContainer& cont_nc, bool created)
{
  const TileMutableBeamElemContainer& cont = cont_nc;
  for (size_t hash = 0; hash < NCOLL; hash++) {
    TileBeamElemCollection* coll_nc = cont_nc.indexFindPtr (hash);
    const TileBeamElemCollection* coll = cont.indexFindPtr (hash);
    assert (coll_nc == coll);
  }
  if (!created) {
    assert (cont_nc.indexFindPtr(NCOLL) == nullptr);
  }
}


void checkFind2 (TileMutableBeamElemContainer& cont_nc)
{
  for (size_t hash = 0; hash < NCOLL; hash++) {
    assert (cont_nc.indexFindPtr(hash) == nullptr);
  }
}


void modify (TileMutableBeamElemContainer& cont, int offs)
{
  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    TileBeamElemCollection* coll = cont.indexFindPtr (hash);
    for (TileBeamElem* chan : *coll) {
      std::vector<uint32_t> digits = chan->get_digits();
      for (size_t i = 0; i < digits.size(); i++) {
        digits[i] += offs;
      }
      TileBeamElem d (chan->adc_HWID(),
                      std::move (digits));
      *chan = d;
    }
  }
}


void check (const TileMutableBeamElemContainer& cont,
            bool created,
            int dig_offs)
{
  TileCablingService* cabling = TileCablingService::getInstance();

  assert (cont.get_type() == TileFragHash::Beam);
  assert (cont.get_unit() == TileRawChannelUnit::ADCcounts);
  assert (cont.get_bsflags() == 98765);

  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    const TileBeamElemCollection* coll = cont.indexFindPtr (hash);
    assert (coll->identify() == cont.hashFunc().identifier (hash));
    if (!created && dig_offs >= 0) {
      assert (coll->getLvl1Id() == 1071 + hash);
      assert (coll->getLvl1Type() == 1081 + hash);
      assert (coll->getDetEvType() == 1091 + hash);
      assert (coll->getRODBCID() == 1101 + hash);
    }

    if (dig_offs < 0) {
      assert (coll->size() == 0);
    }
    else {
      HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (coll->identify());
      int i = 0;
      for (const TileBeamElem* chan : *coll) {
        int offs = i + hash*NCHAN;
        HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);
        assert (chan->adc_HWID() == hwid);

        std::vector<uint32_t> digits
          { offs*500u + 1 + dig_offs, offs*500u + 2 + dig_offs,
            offs*500u + 3 + dig_offs, offs*500u + 4 + dig_offs };
      
        assert (chan->get_digits() == digits);

        i += 1;
      }
    }
  }
}


void test1()
{
  std::cout << "test1\n";

  {
    std::unique_ptr<TileMutableBeamElemContainer> cont1 = create1 (false);
    checkFind1 (*cont1, false);
    cont1->lock();
    checkFind2 (*cont1);
    check (*cont1, false, 0);

    TileBeamElemContainer& dc1 = *cont1;
    auto cont1a = std::make_unique<TestTileMutableBeamElemContainer> (dc1);
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
    std::unique_ptr<TileMutableBeamElemContainer> cont2 = create1 (true);
    checkFind1 (*cont2, true);
    modify (*cont2, 23);
    cont2->lock();
    checkFind2 (*cont2);
    check (*cont2, true, 23);

    TileBeamElemContainer& dc2 = *cont2;
    auto cont2a = std::make_unique<TileMutableBeamElemContainer> (dc2);
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
  std::cout << "TileMutableBeamElemContainer_test\n";
  TileCablingSvc cabling;
  test1();
  return 0;
}

