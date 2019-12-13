/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBScintillatorContCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBScintillatorContCnv_p1.h"
#include "TBEvent/TBScintillatorCont.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBScintillator& p1,
              const TBScintillator& p2)
{
  assert (p1.isOverflow() == p2.isOverflow());
  assert (p1.getDetectorName() == p2.getDetectorName());
  assert (p1.getSignal() == p2.getSignal());
  assert (p1.getTimeSignal() == p2.getTimeSignal());
  assert (p1.isSignalOverflow() == p2.isSignalOverflow());
  assert (p1.isTimeOverflow() == p2.isTimeOverflow());
}


void compare (const TBScintillatorCont& p1,
              const TBScintillatorCont& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const TBScintillatorCont& trans1)
{
  MsgStream log (0, "test");
  TBScintillatorContCnv_p1 cnv;
  TBScintillatorCont_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBScintillatorCont trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


std::unique_ptr<TBScintillator> makeone (const std::string& name, int n)
{
  auto p = std::make_unique<TBScintillator> (name);
  p->setOverflow ((n&1) == 0);
  p->setSignal (n*1000 + 1.5 );
  p->setTimeSignal (n*1000 + 2.5 );
  p->setSignalOverflow ((n&1) != 0);
  p->setTimeOverflow ((n&1) != 0);

  return p;
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBScintillatorCont trans1;
  trans1.push_back (makeone ("aaa", 1));
  trans1.push_back (makeone ("bbb", 2));
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBScintillatorContCnv_p1_test\n";
  test1();
  return 0;
}
