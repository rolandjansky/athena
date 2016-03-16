/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkEventTPCnv/test/ErrorMatrixCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";

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


int main()
{
  test1();
  return 0;
}
