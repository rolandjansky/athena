/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TileTPCnv/test/TileBeamElemContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Tests for TileBeamElemContainerCnv_p1.
 */


#undef NDEBUG
#include "TileTPCnv/TileBeamElemContainerCnv_p1.h"
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
    hwid.set_quiet (true);
    tbid.set_quiet (true);
    tileid.set_quiet (true);
    assert (hwid.initialize_from_dictionary (idd) == 0);
    assert (tbid.initialize_from_dictionary (idd) == 0);
    assert (tileid.initialize_from_dictionary (idd) == 0);
    TileCablingService* svc = TileCablingService::getInstance_nc();
    svc->setTileHWID (&hwid);
    svc->setTileTBID (&tbid);
    svc->setTileID (&tileid);
  }
};


void compare (const TileBeamElem& p1,
              const TileBeamElem& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.get_digits() == p2.get_digits());
}


void compare (const TileBeamElemCollection& p1,
              const TileBeamElemCollection& p2)
{
  assert (p1.identify() == p2.identify());
  // These aren't saved.
  //assert (p1.getLvl1Id() == p2.getLvl1Id());
  //assert (p1.getLvl1Type() == p2.getLvl1Type());
  //assert (p1.getDetEvType() == p2.getDetEvType());
  //assert (p1.getRODBCID() == p2.getRODBCID());
  assert (p1.size() == p1.size());
  for (size_t j = 0; j < p1.size(); j++) {
    compare (*p1[j], *p2[j]);
  }
}


void compare (const TileBeamElemContainer& p1,
              const TileBeamElemContainer& p2)
{
  assert (p1.get_hashType() == p2.get_hashType());
  assert (p1.get_unit() == p2.get_unit());

  TileBeamElemContainer::const_iterator it1 = p1.begin();
  TileBeamElemContainer::const_iterator it1e = p1.end();
  TileBeamElemContainer::const_iterator it2 = p2.begin();
  TileBeamElemContainer::const_iterator it2e = p2.end();
  while (it1 != it1e && it2 != it2e) {
    assert (it1.hashId() == it2.hashId());
    compare (**it1, **it2);
    ++it1;
    ++it2;
  }
  assert (it1 == it1e && it2 == it2e);
}


void testit (const TileBeamElemContainer& trans1)
{
  MsgStream log (0, "test");
  TileBeamElemContainerCnv_p1 cnv;
  TileBeamElemContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileBeamElemContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


std::unique_ptr<const TileBeamElemContainer>
makecont()
{
  auto cont = std::make_unique<TileBeamElemContainer> (false,
                                                       TileFragHash::Default,
                                                       TileRawChannelUnit::ADCcounts);

  std::vector<std::unique_ptr<TileBeamElemCollection> > colls (100);
  for (int hi=2; hi <= 3; hi++) {
    auto coll = std::make_unique<TileBeamElemCollection>(IdentifierHash(hi));
    coll->setLvl1Id (hi + 10);
    coll->setLvl1Type (hi + 20);
    coll->setDetEvType (hi + 30);
    coll->setRODBCID (hi + 40);

    for (int i=0; i < 10; i++) {
      int offs = i*10 + hi*100;

      std::vector<uint32_t> digits;
      for (int j = 0; j < i+1; j++) {
        digits.push_back (offs + j);
      }

      Identifier id = tileid.adc_id (1, hi*2 - 5, 2, i, 0, 0, 0, true);

      auto elt = std::make_unique<TileBeamElem>
        (id, std::move (digits));
      int hash = cont->hashFunc().hash(elt->frag_ID());
      if (hash >= static_cast<int>(colls.size())) {
        colls.resize (hash+1);
      }
      if (!colls[hash]) {
        auto coll = std::make_unique<TileBeamElemCollection>(IdentifierHash(elt->frag_ID()));
        coll->setLvl1Id (hash + 10);
        coll->setLvl1Type (hash + 20);
        coll->setDetEvType (hash + 30);
        coll->setRODBCID (hash + 40);
        colls[hash] = std::move(coll);
      }

      colls[hash]->push_back (std::move (elt));
    }
  }

  for (size_t hash = 0; hash < colls.size(); hash++) {
    if (colls[hash]) {
      cont->addCollection (colls[hash].release(), hash).ignore();
    }
  }
  return cont;
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  std::unique_ptr<const TileBeamElemContainer> trans1 = makecont();

  testit (*trans1);
}


int main()
{
  std::cout << "TileTPCnv/TileBeamElemContainerCnv_p1_test\n";
  IdDictParser parser;
  TileCablingSvc::init_idhelpers(parser);
  test1();
  return 0;
}
