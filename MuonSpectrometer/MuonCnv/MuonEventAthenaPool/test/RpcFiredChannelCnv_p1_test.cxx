/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/RpcFiredChannelCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/RpcFiredChannelCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const RpcFiredChannel& p1,
              const RpcFiredChannel& p2)
{
  assert (p1.bcid() == p2.bcid());
  assert (p1.time() == p2.time());
  assert (p1.ijk() == p2.ijk());
  assert (p1.channel() == p2.channel());
  assert (p1.ovl() == p2.ovl());
  assert (p1.thr() == p2.thr());
}


void testit (const RpcFiredChannel& trans1)
{
  MsgStream log (nullptr, "test");
  RpcFiredChannelCnv_p1 cnv;
  RpcFiredChannel_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  RpcFiredChannel trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;

  RpcFiredChannel trans1 (123, 234, 2, 345);
  testit (trans1);
  RpcFiredChannel trans2 (456, 567, 7, 678, 789);
  testit (trans2);
}


int main()
{
  test1();
  return 0;
}
