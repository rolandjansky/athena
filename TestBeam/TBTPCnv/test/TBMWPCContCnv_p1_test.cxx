/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBMWPCContCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBMWPCContCnv_p1.h"
#include "TBEvent/TBMWPCCont.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBMWPC& p1,
              const TBMWPC& p2)
{
  assert (p1.isOverflow() == p2.isOverflow());
  assert (p1.getDetectorName() == p2.getDetectorName());
  assert (p1.getCPos() == p2.getCPos());
  assert (p1.getCErr() == p2.getCErr());
  assert (p1.isX() == p2.isX());
  assert (p1.getClusterSizeC() == p2.getClusterSizeC());
  assert (p1.isCPosOverflow() == p2.isCPosOverflow());
}


void compare (const TBMWPCCont& p1,
              const TBMWPCCont& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*p1[i], *p2[i]);
  }
}


void testit (const TBMWPCCont& trans1)
{
  MsgStream log (0, "test");
  TBMWPCContCnv_p1 cnv;
  TBMWPCCont_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBMWPCCont trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


std::unique_ptr<TBMWPC> makeone (const std::string& name, int n)
{
  auto p = std::make_unique<TBMWPC> (name);
  p->setOverflow ((n&1) == 0);
  p->setCPos (std::vector<float> { n*1000+1.5f, n*1000+2.5f });
  p->setCErr (std::vector<float> { n*1000+3.5f, n*1000+4.5f, n*1000+5.5f });
  p->setXchambers ((n&1) != 0);
  p->setClusterSizeC (std::vector<float> { n*1000+9.5f, n*1000+8.5f, n*1000+7.5f });
  p->setCPosOverflow (std::vector<bool> { true, false } );

  return p;
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBMWPCCont trans1;
  trans1.push_back (makeone ("aaa", 1));
  trans1.push_back (makeone ("bbb", 2));
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBMWPCContCnv_p1_test\n";
  test1();
  return 0;
}
