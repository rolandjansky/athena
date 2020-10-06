/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileEvent/testTileMutableDataContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileMutableDataContainer.
 */


#undef NDEBUG
#include "TileEvent/TileMutableDataContainer.h"
#include "TileEvent/TileRawDataContainer.h"
#include "TileEvent/TileRawDataCollection.h"
#include "TileEvent/TileRawData.h"
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


class TestElement
  : public TileRawData
{
public:
  using TileRawData::TileRawData;

  unsigned m_x;
};


class TestCollection
  : public TileRawDataCollection<TestElement>
{
public:
  typedef TestElement TElement;
  using TileRawDataCollection<TestElement>::TileRawDataCollection;
};


typedef TileRawDataContainer<TestCollection> TestContainer;
typedef TileMutableDataContainer<TestContainer> MutableTestContainer;


class TestMutableTestContainer
  : public MutableTestContainer
{
public:
  using MutableTestContainer::MutableTestContainer;
  using MutableTestContainer::recycle;
};


void createCollection (MutableTestContainer& cont, size_t hash)
{
  int frag = cont.hashFunc().identifier(hash);
  auto coll = std::make_unique<TestCollection> (frag);
  assert( cont.addCollection (std::move (coll), hash).isSuccess() );
}


void addChans (MutableTestContainer& cont, size_t hash)
{
  TileCablingService* cabling = TileCablingService::getInstance();
  int frag = cont.hashFunc().identifier (hash);
  HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (frag);
  for (size_t i = 0; i < NCHAN; i++) {
    HWIdentifier hwid = cabling->getTileHWID()->channel_id (drawer_id, i);

    int offs = i + hash*NCHAN;
    auto chan = std::make_unique<TestElement> (hwid);
    chan->m_x = offs;
    assert( cont.push_back (std::move (chan)).isSuccess() );
  }
}


std::unique_ptr<MutableTestContainer>
create1 (bool createColl)
{
  auto cont = std::make_unique<MutableTestContainer> (createColl);
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


void checkFind1 (MutableTestContainer& cont_nc, bool created)
{
  const MutableTestContainer& cont = cont_nc;
  for (size_t hash = 0; hash < NCOLL; hash++) {
    TestCollection* coll_nc = cont_nc.indexFindPtr (hash);
    const TestCollection* coll = cont.indexFindPtr (hash);
    assert (coll_nc == coll);
  }
  if (!created) {
    assert (cont_nc.indexFindPtr(NCOLL) == nullptr);
  }
}


void checkFind2 (MutableTestContainer& cont_nc)
{
  for (size_t hash = 0; hash < NCOLL; hash++) {
    assert (cont_nc.indexFindPtr(hash) == nullptr);
  }
}


void modify (MutableTestContainer& cont, float offs)
{
  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    TestCollection* coll = cont.indexFindPtr (hash);
    for (TestElement* chan : *coll) {
      chan->m_x += offs;
    }
  }
}


void check (const MutableTestContainer& cont, bool /*created*/, float x_offs)
{
  TileCablingService* cabling = TileCablingService::getInstance();

  assert (cont.get_type() == TileFragHash::Default);
  assert (cont.get_unit() == TileRawChannelUnit::ADCcounts);
  assert (cont.get_bsflags() == 98765);

  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    const TestCollection* coll = cont.indexFindPtr (hash);
    assert (coll->identify() == cont.hashFunc().identifier (hash));

    if (x_offs < 0) {
      assert (coll->size() == 0);
    }
    else {
      HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (coll->identify());
      int i = 0;
      for (const TestElement* chan : *coll) {
        int offs = i + hash*NCHAN;
        HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);
        assert (chan->adc_HWID() == hwid);

        assert (chan->m_x == offs + x_offs);

        i += 1;
      }
    }
  }
}


void test1()
{
  std::cout << "test1\n";

  {
    std::unique_ptr<MutableTestContainer> cont1 = create1 (false);
    checkFind1 (*cont1, false);
    cont1->lock();
    checkFind2 (*cont1);
    check (*cont1, false, 0);

    TestContainer& tc1 = *cont1;
    auto cont1a = std::make_unique<TestMutableTestContainer> (tc1);
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
    std::unique_ptr<MutableTestContainer> cont2 = create1 (true);
    checkFind1 (*cont2, true);
    modify (*cont2, 23);
    cont2->lock();
    checkFind2 (*cont2);
    check (*cont2, true, 23);

    TestContainer& tc2 = *cont2;
    auto cont2a = std::make_unique<MutableTestContainer> (tc2);
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
  std::cout << "TileMutableDataContainer_test\n";
  TileCablingSvc cabling;
  test1();
  return 0;
}

