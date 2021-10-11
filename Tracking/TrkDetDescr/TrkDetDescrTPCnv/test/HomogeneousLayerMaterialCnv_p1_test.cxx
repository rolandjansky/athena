/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkDetDescrTPCnv/test/HomogeneousLayerMaterialCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "TrkDetDescrTPCnv/TrkGeometry/HomogeneousLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/LayerMaterialMapCnv_tlp1.h"
#include "TestTools/leakcheck.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>


using Athena_test::isEqual;


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


void compare (const Trk::MaterialProperties& m1,
              const Trk::MaterialProperties& m2)
{
  compare (m1.material(), m2.material());
  assert (isEqual (m1.thicknessInX0(), m2.thicknessInX0()));
  assert (isEqual (m1.thicknessInL0(), m2.thicknessInL0()));
  assert (isEqual (m1.zOverAtimesRhoTimesD(), m2.zOverAtimesRhoTimesD()));
}


void compare (const Trk::HomogeneousLayerMaterial& m1,
              const Trk::HomogeneousLayerMaterial& m2)
{
  compare (*m1.material(0,0), *m2.material(0,0));
  assert (m1.alongPostFactor() == m2.alongPostFactor());
}


void testit (const Trk::HomogeneousLayerMaterial& trans1)
{
  HomogeneousLayerMaterialCnv_p1 cnv;
  LayerMaterialMapCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::HomogeneousLayerMaterial_p1 pers;
  MsgStream log (nullptr, "test");
  cnv.transToPers (&trans1, &pers, log);
  Trk::HomogeneousLayerMaterial trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Trk::Material mat1 (1.5, 2.5, 3.5, 4.5, 5.5);
  Trk::MaterialProperties matprop1 (mat1, 2);
  testit (Trk::HomogeneousLayerMaterial (matprop1, 3.5));
}


int main()
{
  test1();
  return 0;
}
