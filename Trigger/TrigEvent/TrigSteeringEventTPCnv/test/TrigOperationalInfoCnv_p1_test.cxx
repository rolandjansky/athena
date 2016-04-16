/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigSteeringEventTPCnv/test/TrigOperationalInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigOperationalInfoCnv_p1.
 */


#undef NDEBUG
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigOperationalInfo& p1,
              const TrigOperationalInfo& p2)
{
  assert (p1.infos() == p2.infos());
}


void testit (const TrigOperationalInfo& trans1)
{
  MsgStream log (0, "test");
  TrigOperationalInfoCnv_p1 cnv;
  TrigOperationalInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigOperationalInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigOperationalInfo trans1;
  trans1.set ("aaa", 4.5);
  trans1.set ("bbb", 5.5);
  trans1.set ("ccc", 6.5);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
