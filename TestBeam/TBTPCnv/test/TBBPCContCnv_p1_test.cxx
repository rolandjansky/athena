/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBBPCContCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBBPCContCnv_p1.h"
#include "TBEvent/TBBPCCont.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBBPC& p1,
              const TBBPC& p2)
{
  assert (p1.isOverflow() == p2.isOverflow());
  assert (p1.getDetectorName() == p2.getDetectorName());
  assert (p1.getXPos() == p2.getXPos());
  assert (p1.getYPos() == p2.getYPos());
  assert (p1.getXErr() == p2.getXErr());
  assert (p1.getYErr() == p2.getYErr());
  assert (p1.getXPulse() == p2.getXPulse());
  assert (p1.getYPulse() == p2.getYPulse());
  assert (p1.getHitNbr() == p2.getHitNbr());
  assert (p1.isXPosOverflow() == p2.isXPosOverflow());
  assert (p1.isYPosOverflow() == p2.isYPosOverflow());
  assert (p1.isXPulseOverflow() == p2.isXPulseOverflow());
  assert (p1.isYPulseOverflow() == p2.isYPulseOverflow());
}


void compare (const TBBPCCont& p1,
              const TBBPCCont& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const TBBPCCont& trans1)
{
  MsgStream log (0, "test");
  TBBPCContCnv_p1 cnv;
  TBBPCCont_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBBPCCont trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


std::unique_ptr<TBBPC> makeone (const std::string& name, int n)
{
  auto p = std::make_unique<TBBPC> (name);
  p->setOverflow ((n&1) == 0);
  p->setXPos (n*1000 + 1);
  p->setYPos (n*1000 + 2);
  p->setXErr (n*1000 + 3);
  p->setXErr (n*1000 + 3);
  p->setXPulse (n*1000 + 4);
  p->setYPulse (n*1000 + 5);
  p->setHitNbr (n*1000 + 6);

  p->setXPosOverflow ((n&1) == 0);
  p->setYPosOverflow ((n&1) != 0);
  p->setXPulseOverflow ((n&1) != 0);
  p->setYPulseOverflow ((n&1) == 0);

  return p;
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBBPCCont trans1;
  trans1.push_back (makeone ("aaa", 1));
  trans1.push_back (makeone ("bbb", 2));
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBBPCContCnv_p1_test\n";
  test1();
  return 0;
}
