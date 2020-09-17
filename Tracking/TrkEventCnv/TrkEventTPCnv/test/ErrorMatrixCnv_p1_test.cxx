/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkEventTPCnv/test/ErrorMatrixCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include "TestTools/leakcheck.h"
#include "CxxUtils/checker_macros.h"
#include <cassert>
#include <iostream>


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  MsgStream log (0, "test");
  Trk::ErrorMatrix trans1;
  trans1.values.push_back (1.5);
  trans1.values.push_back (2.5);
  trans1.values.push_back (3.5);
  ErrorMatrixCnv_p1 cnv;
  Trk::HepSymMatrix_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Trk::ErrorMatrix trans2;
  cnv.persToTrans (&pers, &trans2, log);

  assert (trans1.values == trans2.values);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
