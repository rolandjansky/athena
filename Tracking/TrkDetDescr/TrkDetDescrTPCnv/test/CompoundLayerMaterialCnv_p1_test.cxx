/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrkDetDescrTPCnv/test/CompoundLayerMaterialCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2015
 * @brief Unit test
 */

#undef NDEBUG
#include "TrkDetDescrTPCnv/TrkGeometry/CompoundLayerMaterialCnv_p1.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/FLOATassert.h"
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


void compare (const Trk::CompoundLayerMaterial& m1,
              const Trk::CompoundLayerMaterial& m2,
              size_t nbin0,
              size_t nbin1)
{
  compare (*m1.binUtility(), *m2.binUtility());
  for (size_t bin0 = 0; bin0 < nbin0; ++bin0) {
    for (size_t bin1 = 0; bin1 < nbin1; ++bin1) {
      const Trk::MaterialProperties* mat1 = m1.material (bin0, bin1);
      const Trk::MaterialProperties* mat2 = m2.material (bin0, bin1);
      compare (*mat1, *mat2);
    }
  }
}


void testit (const Trk::CompoundLayerMaterial& trans1,
             size_t nbin0, size_t nbin1)
{
  CompoundLayerMaterialCnv_p1 cnv;
  Trk::CompoundLayerMaterial_p1 pers;
  MsgStream log (0, "test");
  cnv.transToPers (&trans1, &pers, log);
  Trk::CompoundLayerMaterial trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2, nbin0, nbin1);
}


void test1()
{
  std::cout << "test1\n";
  Trk::BinUtility bin1 (10, 0, 10);
  Trk::ValueStore thickness {1.0, 0.1, {{1,2}, {3,4}}};
  Trk::ValueStore x0bins {1.1, 0.2, {{2,1}, {4,3}}};
  Trk::ValueStore l0bins {10.1, 1.0, {{3,4}, {1,2}}};
  Trk::ValueStore abins {20.1, 2.0, {{4,3}, {2,3}}};
  Trk::ValueStore zbins {15.1, 1.5, {{5,4}, {6,7}}};
  Trk::ValueStore rhobins {16.1, 1.2, {{4,5}, {7,6}}};
  std::vector<unsigned char> elements {1, 2, 3};
  std::vector<unsigned char> frac1 {10, 20, 30};
  std::vector<unsigned char> frac2 {20, 10, 15};
  std::vector<unsigned char> frac3 {15, 70, 50};
  std::vector<unsigned char> frac4 {50, 30, 10};
  std::vector< std::vector< Trk::MaterialComposition > > composition (2);
  composition[0].emplace_back (elements, frac1);
  composition[0].emplace_back (elements, frac2);
  composition[1].emplace_back (elements, frac3);
  composition[1].emplace_back (elements, frac4);
  Trk::CompoundLayerMaterial mat (bin1,
                                  thickness,
                                  x0bins,
                                  l0bins,
                                  abins,
                                  zbins,
                                  rhobins,
                                  composition);
  testit (mat, 2, 2);
}


int main()
{
  test1();
  return 0;
}
