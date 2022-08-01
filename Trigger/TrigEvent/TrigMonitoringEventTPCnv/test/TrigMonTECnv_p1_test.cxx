/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigMonTECnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigMonTECnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigMonTECnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigMonTE& p1,
              const TrigMonTE& p2)
{
  assert ( p1.getId()           == p2.getId() );
  assert ( p1.getIndex()        == p2.getIndex() );
  assert ( p1.getType()         == p2.getType() );
  assert ( p1.getActiveState()  == p2.getActiveState() );
  assert ( p1.getErrorState()   == p2.getErrorState() );
  assert ( p1.isTerminalNode()  == p2.isTerminalNode() );
  assert ( p1.isOutputL2Node()  == p2.isOutputL2Node() );
  assert ( p1.isOutputEFNode()  == p2.isOutputEFNode() );
  assert ( p1.isTopologicalTE() == p2.isTopologicalTE() );
  assert ( p1.getChildIndex()   == p2.getChildIndex() );
  assert ( p1.getParentIndex()  == p2.getParentIndex() );
  assert ( p1.getRoiId()        == p2.getRoiId() );
  assert ( p1.getClid()         == p2.getClid() );
  assert ( p1.getVarKey()       == p2.getVarKey() );
  assert ( p1.getVarVal()       == p2.getVarVal() );
}


void testit (const TrigMonTE& trans1)
{
  MsgStream log (0, "test");
  TrigMonTECnv_p1 cnv;
  TrigMonTE_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMonTE trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigMonTE trans1 (123, 345);
  trans1.setType (TrigMonTE::kROI);
  trans1.addState (TrigMonTE::activeState);
  trans1.addState (TrigMonTE::terminalNode);
  trans1.addState (TrigMonTE::topologicalTE);
  trans1.addChildIndex (321);
  trans1.addChildIndex (432);
  trans1.addParentIndex (543);
  trans1.addParentIndex (645);
  trans1.addParentIndex (765);
  trans1.addRoiId (143);
  trans1.addRoiId (93);
  trans1.addClid (23456);
  trans1.addClid (53453);
  trans1.addClid (123123);
  trans1.addVar (TrigMonVar (3, 5.5));
  trans1.addVar (TrigMonVar (8, 10.5));

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigMonTECnv_p1_test\n";
  test1();
  return 0;
}
