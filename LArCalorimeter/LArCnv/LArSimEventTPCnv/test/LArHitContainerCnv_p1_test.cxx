/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArSimEventTPCnv/test/LArHitContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for LArHitContainerCnv_p1.
 */


#undef NDEBUG
#include "LArSimEventTPCnv/LArHitContainerCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const LArHit& p1,
              const LArHit& p2)
{
  assert (p1.cellID() == p2.cellID());
  assert (p1.energy() == p2.energy());
  assert (p1.time() == p2.time());
}


void compare (const LArHitContainer& p1,
              const LArHitContainer& p2)
{
  assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const LArHitContainer& trans1)
{
  MsgStream log (0, "test");
  LArHitContainerCnv_p1 cnv;
  LArHitContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArHitContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LArHitContainer trans1 ("coll");
  for (int i=0; i < 10; i++) {
    int o = i*100;
    trans1.push_back (new LArHit (Identifier (0x1234+o), 10.5+o, 12.5+o));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
