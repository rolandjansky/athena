/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkDetDescrTPCnv/test/MaterialCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "TrkDetDescrTPCnv/TrkGeometry/MaterialCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>


void compare (const Trk::Material& m1, const Trk::Material& m2)
{
  assert (m1.X0 == m2.X0);
  assert (m1.L0 == m2.L0);
  assert (m1.A == m2.A);
  assert (m1.Z == m2.Z);
  assert (m1.rho == m2.rho);
  assert (m1.dEdX == m2.dEdX);
  assert (m1.zOaTr == m2.zOaTr);
  if (m1.composition)
    assert (*m1.composition == *m2.composition);
  else
    assert (m2.composition == nullptr);
}


void testit (const Trk::Material& trans1)
{
  MaterialCnv_p1 cnv;
  Trk::Material_p1 pers;
  MsgStream log (nullptr, "test");
  cnv.transToPers (&trans1, &pers, log);
  Trk::Material trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  Trk::Material mat1 (1.5, 2.5, 3.5, 4.5, 5.5);
  testit (mat1);

  Trk::Material mat2 (1.5, 2.5, 3.5, 4.5, 5.5, 6.5);
  testit (mat2);

  auto mc = std::make_unique<Trk::MaterialComposition>();
  mc->emplace_back (1, 2);
  mc->emplace_back (3, 4);
  mc->emplace_back (5, 6);
  Trk::Material mat3 (1.5, 2.5, 3.5, 4.5, 5.5, 6.5, mc.release());
  testit (mat3);
}


int main()
{
  test1();
  return 0;
}
