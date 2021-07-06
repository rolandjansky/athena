/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file MuonEventAthenaPool/test/RpcPadContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/RpcPadContainerCnv_p1.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/initGaudi.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>

#include "make_dd.icc"

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


void compare (const RpcPadContainer& p1,
              const RpcPadContainer& p2)
{
  RpcPadContainer::const_iterator it1 = p1.begin();
  RpcPadContainer::const_iterator end1 = p1.end();
  RpcPadContainer::const_iterator it2 = p2.begin();
  RpcPadContainer::const_iterator end2 = p2.end();
  for (; it1 != end1; ++it1, ++it2) {
    assert (it2 != end2);
    assert (it1.hashId() == it2.hashId());
    compare (**it1, **it2);
  }
  assert (it2 == end2);
}


void testit (const RpcPadContainer& trans1)
{
  MsgStream log (0, "test");
  RpcPadContainerCnv_p1 cnv;
  assert(cnv.initialize (log));
  RpcPadContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  std::unique_ptr<RpcPadContainer> trans2 (cnv.createTransient (&pers, log));
  compare (trans1, *trans2);
}


void test1 (const std::vector<Identifier>& ids)
{
  std::cout << "test1\n";

  // Go through initialization once before we enable leak checking,
  // to get all framework stuff built.
  {
    MsgStream log (0, "test");
    RpcPadContainerCnv_p1 cnv;
    assert( cnv.initialize (log).isSuccess() );
  }

  Athena_test::Leakcheck check;
  RpcPadContainer trans1 (ids.size());
  for (size_t k=0; k < ids.size(); k++) {
    int koffs = k*50;
    auto pad = std::make_unique<RpcPad> (ids[k],
                                         k,
                                         985+koffs,
                                         984+koffs,
                                         983+koffs,
                                         982+koffs,
                                         981+koffs,
                                         980+koffs);

    for (int j=0; j < 5; j++) {
      int joffs = j*100;
      auto cm = std::make_unique<RpcCoinMatrix> (Identifier (123+joffs),
                                                 234+joffs, 345+joffs,
                                                 456+joffs, 576+joffs);

      for (int i=0; i < 10; i++) {
        int ioffs = 1000*i;
        if (i < 7)
          cm->push_back (new RpcFiredChannel (321+ioffs, 432+ioffs,
                                              i, 534+ioffs));
        else
          cm->push_back (new RpcFiredChannel (213+ioffs, 324+ioffs, 7,
                                              435+ioffs, 546+ioffs));
      }

      pad->push_back (std::move (cm));
    }
    assert (trans1.addCollection (pad.release(), k).isSuccess());
  }

  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("MuonEventAthenaPool/MuonEventAthenaPool_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  TestRCUSvc rcusvc;

  std::vector<Identifier> ids;
  ids.emplace_back (0x6024c400);
  ids.emplace_back (0x603b6800);
  ids.emplace_back (0x603e5400);

  make_dd();
  make_cond(rcusvc, ids);

  test1(ids);
  return 0;
}
