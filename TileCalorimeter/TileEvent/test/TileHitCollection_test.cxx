/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileEvent/test/TileHitCollection_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Test for TileHitCollection.
 */


#undef NDEBUG
#include "TileEvent/TileHitCollection.h"
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  TileHitCollection coll (123);

  for (int i=0; i < 10; i++) {
    Identifier id (10000 + i*1000);
    auto hit = std::make_unique<TileHit> (id);
    hit->resize(2);
    hit->setEnergy (i*100, 0);
    hit->setEnergy (i*100+10, 1);
    hit->setTime (i*200, 0);
    hit->setTime (i*200+10, 1);
    coll.push_back (std::move(hit));
  }

  TileHitCollection newColl (coll);
  assert (newColl.identify() == 123);

  assert (newColl.size() == 10);
  for (int i=0; i < 10; i++) {
    const TileHit* hit = newColl[i];
    assert (hit->identify() == Identifier (10000 + i*1000));
    assert (hit->size() == 2);
    assert (hit->energy(0) == i*100);
    assert (hit->energy(1) == i*100+10);
    assert (hit->time(0) == i*200);
    assert (hit->time(1) == i*200+10);
  }
}


int main()
{
  std::cout << "TileHitCollection_test\n";
  test1();
  return 0;
}
