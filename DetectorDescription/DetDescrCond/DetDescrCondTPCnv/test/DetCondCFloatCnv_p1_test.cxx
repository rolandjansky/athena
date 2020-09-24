/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DetDescrCondTPCnv/test/DetCondCFloatCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "DetDescrCondTPCnv/DetCondCFloatCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  MsgStream log (0, "test");
  DetCondCFloat trans1 (3, "tag");

  float data1[] = {1.5, 2.5, 3.5};
  float data2[] = {4.5, 5.5, 6.5};
  float data3[] = {7.5, 8.5, 9.5};
  trans1.add (Identifier(1234), data1);
  trans1.add (Identifier(2345), data2);
  trans1.add (Identifier(3456), data3);

  DetCondCFloatCnv_p1 cnv;
  DetCondCFloat_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  DetCondCFloat trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans2.size() == 3);
  assert (trans2.tag() == "tag");

  assert (std::equal (data2, data2+3, trans2.find(Identifier(2345))));
  assert (std::equal (data1, data1+3, trans2.find(Identifier(1234))));
  assert (trans2.find(Identifier(9876)) == nullptr);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
