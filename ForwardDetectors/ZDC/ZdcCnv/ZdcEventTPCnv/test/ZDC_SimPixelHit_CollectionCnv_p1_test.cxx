/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file ZdcEventTPCnv/test/ZDC_SimPixelHit_CollectionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for ZDC_SimPixelHit_CollectionCnv_p1.
 */


#undef NDEBUG
#include "ZdcEventTPCnv/ZDC_SimPixelHit_CollectionCnv_p1.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>


void compare (const ZDC_SimPixelHit& p1,
              const ZDC_SimPixelHit& p2)
{
  assert (p1.GetSide() == p2.GetSide());
  assert (p1.GetMod() == p2.GetMod());
  assert (p1.GetPix() == p2.GetPix());
  assert (p1.GetEdep() == p2.GetEdep());
  assert (p1.GetNPhotons() == p2.GetNPhotons());
}


void compare (const ZDC_SimPixelHit_Collection& p1,
              const ZDC_SimPixelHit_Collection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const ZDC_SimPixelHit_Collection& trans1)
{
  MsgStream log (0, "test");
  ZDC_SimPixelHit_CollectionCnv_p1 cnv;
  ZDC_SimPixelHit_Collection_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  ZDC_SimPixelHit_Collection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  ZDC_SimPixelHit_Collection dum1 ("coll");
  Athena_test::Leakcheck check;

  ZDC_SimPixelHit_Collection trans1 ("coll");
  for (int i = 0; i < 10; i++) {
    int o = i*100;
    trans1.Emplace (123+o, 234+o, 345+o, 21+o, 12345.5+o);
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
