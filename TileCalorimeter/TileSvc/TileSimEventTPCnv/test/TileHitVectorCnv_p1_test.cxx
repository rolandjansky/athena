/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TileSimEventTPCnv/test/TileHitVectorCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TileHitVectorCnv_p1.
 */


#undef NDEBUG
#include "TileSimEventTPCnv/TileHitVectorCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TileHit& p1,
              const TileHit& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.size() == p2.size());
  for (int i = 0; i < p1.size(); i++) {
    assert (p1.energy(i) == p2.energy(i));
    assert (p1.time(i) == p2.time(i));
  }
}


void compare (const TileHitVector& p1,
              const TileHitVector& p2)
{
  assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (p1[i], p2[i]);
  }
}


void testit (const TileHitVector& trans1)
{
  MsgStream log (0, "test");
  TileHitVectorCnv_p1 cnv;
  TileHitVector_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TileHitVector trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TileHitVector trans1 ("vec");
  for (int i = 0; i < 10; i++) {
    int offs = i*100;
    TileHit hit (Identifier (0x1234+offs), 3.5+offs, 13.5+offs);
    hit.add (4.5+offs, 14.5+offs);
    hit.add (5.5+offs, 15.5+offs);
    trans1.push_back (hit);
  }

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
