/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigConfSigCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigConfSigCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigConfSigCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigConfSig& p1,
              const TrigConfSig& p2)
{
  assert ( p1.getCounter()   == p2.getCounter() );
  assert ( p1.getLogic()     == p2.getLogic() );
  assert ( p1.getLabel()     == p2.getLabel() );
  assert ( p1.getOutputTEs() == p2.getOutputTEs() );
}


void testit (const TrigConfSig& trans1)
{
  MsgStream log (0, "test");
  TrigConfSigCnv_p1 cnv;
  TrigConfSig_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigConfSig trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigConfSig trans1 (1, 2, "lab");
  trans1.addOutputTE (10);
  trans1.addOutputTE (11);
  trans1.addOutputTE (12);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigConfSigCnv_p1_test\n";
  test1();
  return 0;
}
