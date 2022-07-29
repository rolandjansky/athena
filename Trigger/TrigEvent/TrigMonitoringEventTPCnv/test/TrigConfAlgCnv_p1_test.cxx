/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigConfAlgCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigConfAlgCnv_p1.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigConfAlgCnv_p1.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigConfAlg& p1,
              const TrigConfAlg& p2)
{
  assert ( p1.getIndex()    == p2.getIndex() );
  assert ( p1.getPosition() == p2.getPosition() );
  assert ( p1.getNameId()   == p2.getNameId() );
  assert ( p1.getTypeId()   == p2.getTypeId() );
  assert ( p1.getName()     == p2.getName() );
  assert ( p1.getType()     == p2.getType() );
}


void testit (const TrigConfAlg& trans1)
{
  MsgStream log (0, "test");
  TrigConfAlgCnv_p1 cnv;
  TrigConfAlg_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigConfAlg trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigConfAlg trans1 (1, 2, "nm", "typ", 3, 4);

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigConfAlgCnv_p1_test\n";
  test1();
  return 0;
}
