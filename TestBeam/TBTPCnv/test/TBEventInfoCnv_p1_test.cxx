/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TBTPCnv/test/TBEventInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2019
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TBTPCnv/TBEventInfoCnv_p1.h"
#include "TBEvent/TBEventInfo.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TBEventInfo& p1,
              const TBEventInfo& p2)
{
  assert (p1.getEventNum() == p2.getEventNum());
  assert (p1.getEventClock() == p2.getEventClock());
  assert (p1.getEventType() == p2.getEventType());
  assert (p1.getRunNum() == p2.getRunNum());
  assert (p1.getBeamMomentum() == p2.getBeamMomentum());
  assert (p1.getCryoX() == p2.getCryoX());
  assert (p1.getCryoAngle() == p2.getCryoAngle());
  assert (p1.getTableY() == p2.getTableY());
}


void testit (const TBEventInfo& trans1)
{
  MsgStream log (0, "test");
  TBEventInfoCnv_p1 cnv;
  TBEventInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TBEventInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  TBEventInfo trans1 (123, 234, 345, 456,
                      10.5, "pp", 11.5, 12.5, 13.5);
                                  
  testit (trans1);
}


int main()
{
  std::cout << "TBTPCnv/test/TBEventInfoCnv_p1_test\n";
  test1();
  return 0;
}
