/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file TrkDetDescrTPCnv/test/BinnedLayerMaterialCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "TrkDetDescrTPCnv/TrkGeometry/BinnedLayerMaterialCnv_p1.h"
#include "TrkDetDescrTPCnv/LayerMaterialMapCnv_tlp1.h"
#include "TestTools/leakcheck.h"
#include "TestTools/FLOATassert.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <cassert>



using Athena_test::isEqual;


void compare (const Trk::BinningData& b1, const Trk::BinningData& b2)
{
  assert (b1.type == b2.type);
  assert (b1.option == b2.option);
  assert (b1.binvalue == b2.binvalue);
  assert (b1.bins == b2.bins);
  assert (b1.min == b2.min);
  assert (b1.max == b2.max);
  assert (b1.step == b2.step);
  assert (b1.subStep == b2.subStep);
  assert (b1.boundaries == b2.boundaries);
  assert (b1.refphi == b2.refphi);
  assert (b1.hbounds == b2.hbounds);
}


void compare (const Trk::BinUtility& b1, const Trk::BinUtility& b2)
{
  size_t sz = b1.binningData().size();
  assert (b2.binningData().size() == sz);
  for (size_t i = 0; i < sz; i++)
    compare (b1.binningData()[i], b2.binningData()[i]);
}


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


void compare (const Trk::BinnedLayerMaterial& m1,
              const Trk::BinnedLayerMaterial& m2)
{
  compare (*m1.binUtility(), *m2.binUtility());
  assert (m1.alongPostFactor() == m2.alongPostFactor());

  const Trk::MaterialPropertiesMatrix& mm1 = m1.fullMaterial();
  const Trk::MaterialPropertiesMatrix& mm2 = m2.fullMaterial();
  assert (mm1.size() == mm2.size());
  for (size_t i=0; i < mm1.size(); i++) {
    const Trk::MaterialPropertiesVector& mv1 = mm1[i];
    const Trk::MaterialPropertiesVector& mv2 = mm2[i];
    assert (mv1.size() == mv2.size());
    for (size_t j = 0; j < mv1.size(); j++) {
      compare (*mv1[j], *mv2[j]);
    }
  }
}


void testit (const Trk::BinnedLayerMaterial& trans1)
{
  BinnedLayerMaterialCnv_p1 cnv;
  LayerMaterialMapCnv_tlp1 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  Trk::BinnedLayerMaterial_p1 pers;
  MsgStream log (nullptr, "test");
  cnv.transToPers (&trans1, &pers, log);
  Trk::BinnedLayerMaterial trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;
  Trk::MaterialPropertiesMatrix mm;
  mm.resize(2);
  Trk::Material mat1 ( 1.5,  2.5,  3.5,  4.5,  5.5);
  Trk::Material mat2 (11.5, 12.5, 13.5, 14.5, 15.5);
  Trk::Material mat3 (21.5, 22.5, 23.5, 24.5, 25.5);
  Trk::Material mat4 (31.5, 32.5, 33.5, 34.5, 35.5);
  mm[0].push_back (new Trk::MaterialProperties (mat1, 1));
  mm[0].push_back (new Trk::MaterialProperties (mat2, 2));
  mm[1].push_back (new Trk::MaterialProperties (mat3, 3));
  mm[1].push_back (new Trk::MaterialProperties (mat4, 4));

  Trk::BinUtility binning (2, 0, 10);
  Trk::BinUtility binning2 (2, 0, 20);
  binning += binning2;
  
  Trk::BinnedLayerMaterial bl (binning, mm, 1.5);
  testit (bl);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  test1();
  return 0;
}
