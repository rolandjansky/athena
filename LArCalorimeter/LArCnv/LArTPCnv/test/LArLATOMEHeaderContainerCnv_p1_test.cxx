/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file LArTPCnv/test/LArLATOMEHeaderContainerCnv_p1_test.cxx
 * @author Pavol Strizenec
 * @brief based on test LArDigitContainerCnv_p1_test.cxx by Scott
 */


#undef NDEBUG
#include "LArTPCnv/LArLATOMEHeaderContainerCnv_p1.h"
#include "LArRawEvent/LArLATOMEHeader.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const LArLATOMEHeader& p1,
              const LArLATOMEHeader& p2)
{
  assert (p1.SourceId() == p2.SourceId());
  assert (p1.LatomeId() == p2.LatomeId());
  assert (p1.NChans() == p2.NChans());
  assert (p1.BCId() == p2.BCId());
  assert (p1.L1Id() == p2.L1Id());
}


void compare (const LArLATOMEHeaderContainer& p1,
              const LArLATOMEHeaderContainer& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i=0; i < p1.size(); i++)
    compare (*p1[i], *p2[i]);
}


void testit (const LArLATOMEHeaderContainer& trans1)
{
  MsgStream log (0, "test");
  LArLATOMEHeaderContainerCnv_p1 cnv;
  LArLATOMEHeaderContainer_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  LArLATOMEHeaderContainer trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  LArLATOMEHeaderContainer trans1;
  for (int i=0; i < 10; i++) {
    short o = i*100;
    trans1.push_back (new LArLATOMEHeader (1234+o,1235+o, 24+i, 1122+i,2211+i));
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
