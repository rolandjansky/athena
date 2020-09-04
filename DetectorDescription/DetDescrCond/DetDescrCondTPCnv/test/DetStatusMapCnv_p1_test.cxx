/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file DetDescrCondTPCnv/test/DetStatusMapCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "DetDescrCondTPCnv/DetStatusMapCnv_p1.h"
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
  DetStatusMap trans1;
  trans1.add ("det1", DetStatus(10, 1.5, 2.5));
  trans1.add ("det2", DetStatus(11, 3.5, 4.5));
  trans1.add ("det3", DetStatus(12, 5.5, 6.5));

  DetStatusMapCnv_p1 cnv;
  DetStatusMap_p1 pers;
  cnv.transToPers (&trans1, &pers, log);

  DetStatusMap trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans1.size() == 3);
  assert (trans2.size() == 3);

  DetStatusMap::const_iterator it;
  it = trans2.find ("det2");
  assert (it->second.fullCode() == 11);
  assert (it->second.deadFrac() == 3.5);
  assert (it->second.deadThrust() == 4.5);

  it = trans2.find ("det1");
  assert (it->second.fullCode() == 10);
  assert (it->second.deadFrac() == 1.5);
  assert (it->second.deadThrust() == 2.5);

  it = trans2.find ("detx");
  assert (it == trans2.end());
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
