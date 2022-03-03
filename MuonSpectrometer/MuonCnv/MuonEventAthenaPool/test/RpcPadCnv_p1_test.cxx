/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/RpcPad_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/RpcPadCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
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


void compare (const RpcCoinMatrix& p1,
              const RpcCoinMatrix& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.onlineId() == p2.onlineId());
  assert (p1.crc() == p2.crc());
  assert (p1.fel1Id() == p2.fel1Id());
  assert (p1.febcId() == p2.febcId());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void compare (const RpcPad& p1,
              const RpcPad& p2)
{
  assert (p1.identify() == p2.identify());
  assert (p1.identifyHash() == p2.identifyHash());
  assert (p1.onlineId() == p2.onlineId());
  //assert (p1.lvl1Id() == p2.lvl1Id());
  //assert (p1.bcId() == p2.bcId());
  assert (p1.status() == p2.status());
  assert (p1.errorCode() == p2.errorCode());
  assert (p1.sector() == p2.sector());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const RpcPad& trans1)
{
  MsgStream log (nullptr, "test");
  RpcPadCnv_p1 cnv;
  RpcPad_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  RpcPad trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;
  RpcPad trans1 (Identifier(987),
                 986,
                 985,
                 984,
                 983,
                 982,
                 981,
                 980);

  for (int j=0; j < 5; j++) {
    int offs = j*100;
    auto cm = std::make_unique<RpcCoinMatrix> (Identifier (123+offs),
                                               234+offs, 345+offs,
                                               456+offs, 576+offs);
    
    for (int i=0; i < 10; i++) {
      int offs = 1000*i;
      if (i < 7)
        cm->push_back (new RpcFiredChannel (321+offs, 432+offs, i, 534+offs));
      else
        cm->push_back (new RpcFiredChannel (213+offs, 324+offs, 7,
                                               435+offs, 546+offs));
    }
    trans1.push_back (std::move (cm));
  }

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
