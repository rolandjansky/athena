/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrigMonitoringEventTPCnv/test/TrigMonROBDataCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2019
 * @brief Tests for TrigMonROBDataCnv_p2.
 */


#undef NDEBUG
#include "TrigMonitoringEventTPCnv/TrigMonROBDataCnv_p2.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigMonROBData& p1,
              const TrigMonROBData& p2)
{
  assert ( p1.getROBId()        == p2.getROBId() );
  assert ( p1.getEncodedState() == p2.getEncodedState() );
  assert ( p1.getROBSize()      == p2.getROBSize() );
}


void testit (const TrigMonROBData& trans1)
{
  MsgStream log (0, "test");
  TrigMonROBDataCnv_p2 cnv;
  TrigMonROBData_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigMonROBData trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TrigMonROBData trans1 (123, 456);
  trans1.setHistory (static_cast<TrigMonROBData::History> (13));
  trans1.setStatusOK();
  //trans1.setStatusPREFETCH();

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "TrigMonitoringEventTPCnv/TrigMonROBDataCnv_p2_test\n";
  test1();
  return 0;
}
