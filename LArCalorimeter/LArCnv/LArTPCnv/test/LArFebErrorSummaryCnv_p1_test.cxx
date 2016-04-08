/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArTPCnv/test/LArFebErrorSummaryCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2016
 * @brief Tests for LArFebErrorSummaryCnv_p1.
 */


#undef NDEBUG
#include "LArTPCnv/LArFebErrorSummaryCnv_p1.h"
#include <cassert>
#include <iostream>


void compare (const LArFebErrorSummary& p1,
              const LArFebErrorSummary& p2)
{
  assert (p1.get_all_febs() == p2.get_all_febs());
}


void testit (const LArFebErrorSummary& trans1)
{
  MsgStream log (0, "test");
  LArFebErrorSummaryCnv_p1 cnv;
  LArFebErrorSummary_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArFebErrorSummary trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  LArFebErrorSummary trans1;
  for (int i=0; i < 10; i++) {
    trans1.set_feb_error (i*100, i*10);
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
