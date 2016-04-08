/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigCaloEventTPCnv/test/TrigT2MbtsBitsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for TrigT2MbtsBitsCnv_p1.
 */


#undef NDEBUG
#include "TrigCaloEventTPCnv/TrigT2MbtsBitsCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigT2MbtsBits& p1,
              const TrigT2MbtsBits& p2)
{
  for (float x : p2.triggerEnergies())
    assert (x == 0);
  assert (p1.triggerTimes() == p2.triggerTimes());
}


void testit (const TrigT2MbtsBits& trans1)
{
  MsgStream log (0, "test");
  TrigT2MbtsBitsCnv_p1 cnv;
  TrigT2MbtsBits_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigT2MbtsBits trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigT2MbtsBits trans1 (std::vector<float> {1.5, 2.5, 3.5},
                         std::vector<float> {4.5, 5.5});
  
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
