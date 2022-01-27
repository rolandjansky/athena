/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventAthenaPool/test/RpcSectorLogicContainerCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2016
 * @brief Regression tests.
 */

#undef NDEBUG
#include "../src/RpcSectorLogicContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>

void compare (const RpcSLTriggerHit& p1,
              const RpcSLTriggerHit& p2)
{
  assert (p1.rowinBcid() == p2.rowinBcid());
  assert (p1.padId() == p2.padId());
  assert (p1.ptId() == p2.ptId());
  assert (p1.roi() == p2.roi());

  assert (p1.outerPlane() == p2.outerPlane());
  assert (p1.overlapPhi() == p2.overlapPhi());
  assert (p1.overlapEta() == p2.overlapEta());

  assert (p1.triggerBcid() == p2.triggerBcid());
  assert (p1.isInput() == p2.isInput());
}


void compare (const RpcSectorLogic& p1,
              const RpcSectorLogic& p2)
{
  assert (p1.sectorId() == p2.sectorId());
  assert (p1.fel1Id() == p2.fel1Id());
  assert (p1.bcid() == p2.bcid());
  assert (p1.errorCode() == p2.errorCode());
  assert (p1.crc() == p2.crc());
  assert (p1.hasMoreThan2TriggerCand() == p2.hasMoreThan2TriggerCand());
  assert (p1.counters() == p2.counters());
  assert (p1.triggerRates() == p2.triggerRates());

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void compare (const RpcSectorLogicContainer& p1,
              const RpcSectorLogicContainer& p2)
{
  assert (p1.sectorList()[0] == p2.sectorList()[0]);
  assert (p1.sectorList()[1] == p2.sectorList()[1]);

  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const RpcSectorLogicContainer& trans1)
{
  MsgStream log (nullptr, "test");
  RpcSectorLogicContainerCnv_p1 cnv;
  RpcSectorLogicContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  RpcSectorLogicContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  Athena_test::Leakcheck check;
  RpcSectorLogicContainer trans1;
  RpcSectorLogicContainer::SectorList_t sectorList { 123, 345 };
  trans1.setSectorList (sectorList);
  for (int i=0; i < 3; i++) {
    int ioffs = 10*i;
    auto sl = std::make_unique<RpcSectorLogic> (321+ioffs, 432+ioffs, 543+ioffs, 654+ioffs, 765+ioffs);
    sl->setHasMoreThan2TriggerCand ((i&1) != 0);
    sl->addCounter (ioffs);
    sl->addCounter (ioffs+1);
    sl->addTriggerRate (ioffs + 55.5);
    sl->addTriggerRate (ioffs + 56.5);

    for (int j=0; j < 5; j++) {
      int joffs = j*100 + ioffs;
      auto hit = std::make_unique<RpcSLTriggerHit>(1+joffs, 2+joffs, 3+joffs, 4+joffs, 5+joffs, 6+joffs, 7+joffs, 8+joffs);
      hit->setIsInput ((j&1) == 0);
      sl->push_back (std::move(hit));
    }
    
    trans1.push_back (std::move (sl));
  }

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
