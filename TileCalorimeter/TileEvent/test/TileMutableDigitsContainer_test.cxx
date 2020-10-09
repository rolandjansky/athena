/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileEvent/test/TileMutableDigitsContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileMutableDigitsContainer.
 */


#undef NDEBUG
#include "TileEvent/TileMutableDigitsContainer.h"
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


class TestTileMutableDigitsContainer
  : public TileMutableDigitsContainer
{
public:
  using TileMutableDigitsContainer::TileMutableDigitsContainer;
  using TileMutableDigitsContainer::recycle;
};


void createCollection (TileMutableDigitsContainer& cont, size_t hash)
{
  int frag = cont.hashFunc().identifier(hash);
  auto coll = std::make_unique<TileDigitsCollection> (frag);

  unsigned int offs = hash;
  coll->setFragChipCRCWords (std::vector<uint32_t> { 100+offs });
  coll->setFragChipCRCWordsHigh (std::vector<uint32_t> { 200+offs, 300+offs });
  coll->setFragChipHeaderWords (std::vector<uint32_t> { 300+offs, 400+offs, 500+offs });
  coll->setFragChipHeaderWordsHigh (std::vector<uint32_t> { 700+offs, 600+offs });
  coll->setFragExtraWords (std::vector<uint32_t> { 900+offs, 800+offs });
  coll->setFragSize (23+offs);
  coll->setFragBCID (53+offs);

  coll->setLvl1Id (1071 + offs);
  coll->setLvl1Type (1081 + offs);
  coll->setDetEvType (1091 + offs);
  coll->setRODBCID (1101 + offs);

  assert( cont.addCollection (std::move (coll), hash).isSuccess() );
}


void addChans (TileMutableDigitsContainer& cont, size_t hash)
{
  TileCablingService* cabling = TileCablingService::getInstance();
  int frag = cont.hashFunc().identifier (hash);
  HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (frag);
  for (size_t i = 0; i < NCHAN; i++) {
    HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);

    int offs = i + hash*NCHAN;
    std::vector<float> digits { offs*500 + 0.5f, offs*500 + 1.5f,
                                offs*500 + 2.5f, offs*500 + 3.5f };
    auto chan = std::make_unique<TileDigits> (hwid,
                                              std::move (digits));
    assert( cont.push_back (std::move (chan)).isSuccess() );
  }
}


std::unique_ptr<TileMutableDigitsContainer>
create1 (bool createColl)
{
  auto cont = std::make_unique<TileMutableDigitsContainer> (createColl);
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


void checkFind1 (TileMutableDigitsContainer& cont_nc, bool created)
{
  const TileMutableDigitsContainer& cont = cont_nc;
  for (size_t hash = 0; hash < NCOLL; hash++) {
    TileDigitsCollection* coll_nc = cont_nc.indexFindPtr (hash);
    const TileDigitsCollection* coll = cont.indexFindPtr (hash);
    assert (coll_nc == coll);
  }
  if (!created) {
    assert (cont_nc.indexFindPtr(NCOLL) == nullptr);
  }
}


void checkFind2 (TileMutableDigitsContainer& cont_nc)
{
  for (size_t hash = 0; hash < NCOLL; hash++) {
    assert (cont_nc.indexFindPtr(hash) == nullptr);
  }
}


void modify (TileMutableDigitsContainer& cont, float offs)
{
  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    TileDigitsCollection* coll = cont.indexFindPtr (hash);
    for (TileDigits* chan : *coll) {
      std::vector<float> digits = chan->samples();
      for (size_t i = 0; i < digits.size(); i++) {
        digits[i] += offs;
      }
      TileDigits d (chan->adc_HWID(),
                    std::move (digits));
      *chan = d;
    }
  }
}


void check (const TileMutableDigitsContainer& cont,
            bool created,
            float dig_offs)
{
  TileCablingService* cabling = TileCablingService::getInstance();

  assert (cont.get_type() == TileFragHash::Default);
  assert (cont.get_unit() == TileRawChannelUnit::ADCcounts);
  assert (cont.get_bsflags() == 98765);

  for (IdentifierHash hash : cont.GetAllCurrentHashes()) {
    const TileDigitsCollection* coll = cont.indexFindPtr (hash);
    assert (coll->identify() == cont.hashFunc().identifier (hash));
    if (!created && dig_offs >= 0) {
      unsigned int offs = hash;
      assert (coll->getFragChipCRCWords() ==  (std::vector<uint32_t> { 100+offs }));
      assert (coll->getFragChipCRCWordsHigh() ==  (std::vector<uint32_t> { 200+offs, 300+offs }));
      assert (coll->getFragChipHeaderWords() == (std::vector<uint32_t> { 300+offs, 400+offs, 500+offs }));
      assert (coll->getFragChipHeaderWordsHigh() ==  (std::vector<uint32_t> { 700+offs, 600+offs }));
      assert (coll->getFragExtraWords() == (std::vector<uint32_t> { 900+offs, 800+offs }));
      assert (coll->getFragSize() == 23+offs);
      assert (coll->getFragBCID() == 53+offs);

      assert (coll->getLvl1Id() == 1071 + offs);
      assert (coll->getLvl1Type() == 1081 + offs);
      assert (coll->getDetEvType() == 1091 + offs);
      assert (coll->getRODBCID() == 1101 + offs);
    }

    if (dig_offs < 0) {
      assert (coll->size() == 0);
    }
    else {
      HWIdentifier drawer_id = cabling->getTileHWID()->drawer_id (coll->identify());
      int i = 0;
      for (const TileDigits* chan : *coll) {
        int offs = i + hash*NCHAN;
        HWIdentifier hwid = cabling->getTileHWID()-> channel_id (drawer_id, i);
        assert (chan->adc_HWID() == hwid);

        std::vector<float> digits
          { offs*500 + 0.5f + dig_offs, offs*500 + 1.5f + dig_offs,
            offs*500 + 2.5f + dig_offs, offs*500 + 3.5f + dig_offs };
      
        assert (chan->samples() == digits);

        i += 1;
      }
    }
  }
}


void test1()
{
  std::cout << "test1\n";

  {
    std::unique_ptr<TileMutableDigitsContainer> cont1 = create1 (false);
    checkFind1 (*cont1, false);
    cont1->lock();
    checkFind2 (*cont1);
    check (*cont1, false, 0);

    TileDigitsContainer& dc1 = *cont1;
    auto cont1a = std::make_unique<TestTileMutableDigitsContainer> (dc1);
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
    std::unique_ptr<TileMutableDigitsContainer> cont2 = create1 (true);
    checkFind1 (*cont2, true);
    modify (*cont2, 23);
    cont2->lock();
    checkFind2 (*cont2);
    check (*cont2, true, 23);

    TileDigitsContainer& dc2 = *cont2;
    auto cont2a = std::make_unique<TileMutableDigitsContainer> (dc2);
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
  std::cout << "TileMutableDigitsContainer_test\n";
  TileCablingSvc cabling;
  test1();
  return 0;
}

