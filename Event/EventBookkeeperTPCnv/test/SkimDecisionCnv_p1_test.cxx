/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventBookkeeperTPCnv/test/SkimDecisionCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for SkimDecisionCnv_p1.
 */


#undef NDEBUG
#include "EventBookkeeperTPCnv/SkimDecisionCnv_p1.h"
#include "CxxUtils/StrFormat.h"
#include <cassert>
#include <iostream>


void compare (const SkimDecision& p1,
              const SkimDecision& p2)
{
  assert (p1.getName() == p2.getName());
  assert (p1.isAccepted() == p2.isAccepted());
}


void testit (const SkimDecision& trans1)
{
  MsgStream log (0, "test");
  SkimDecisionCnv_p1 cnv;
  SkimDecision_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  SkimDecision trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  SkimDecision trans1;
  trans1.setName ("foo");
  trans1.setIsAccepted (true);
  testit (trans1);

  SkimDecision trans2;
  trans1.setName ("bar");
  trans1.setIsAccepted (false);
  testit (trans2);
}


int main()
{
  test1();
  return 0;
}
