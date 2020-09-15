/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderUtils/test/Mode3dFromFsmw1dFinder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Simple unit test for Mode3dFromFsmw1dFinder.
 */


#undef NDEBUG
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"
#include "TInterpreter.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/expect_exception.h"
#include "TestTools/initGaudi.h"
#include "TestTools/random.h"
#include "TrkVertexSeedFinderUtils/Mode3dFromFsmw1dFinder.h"
#include <cassert>
#include <cmath>
#include <iostream>




using Gaudi::Units::mm;
using Gaudi::Units::MeV;
using Gaudi::Units::GeV;


std::ostream& printVec3D (const Amg::Vector3D& a)
{
  std::cout << a.x() << " " << a.y() << " " << a.z();
  return std::cout;
}
void assertVec3D (const Amg::Vector3D& a, const Amg::Vector3D& b)
{
  assert( Athena_test::isEqual (a.x(), b.x(), 1e-5) );
  assert( Athena_test::isEqual (a.y(), b.y(), 1e-5) );
  assert( Athena_test::isEqual (a.z(), b.z(), 1e-5) );
}
void assertFVec (const std::vector<float>& a, const std::vector<float>& b)
{
  assert (a.size() == b.size());
  for (unsigned int i=0; i < a.size(); i++) {
    assert( Athena_test::isEqual (a[i], b[i], 1e-5) );
  }
}


void test1 (Trk::Mode3dFromFsmw1dFinder& tool)
{
  std::cout << "test1\n";
  std::vector<Trk::PositionAndWeight> points;

  points.emplace_back (Amg::Vector3D(1.1, 1.7, 2.8), 0.9);
  points.emplace_back (Amg::Vector3D(0.9, 1.8, 3.1), 1.1);
  points.emplace_back (Amg::Vector3D(0.8, 2.3, 3.5), 0.7);
  points.emplace_back (Amg::Vector3D(1.3, 2.1, 2.5), 1.8);

  std::unique_ptr<Trk::IMode3dInfo> info;
  const Amg::Vector3D mode = tool.getMode (0.3, 0.2, points, info);
  assertVec3D (mode, {1.13684, 1.91842, 2.74474});

  std::vector<int> indices = info->AcceptedCrossingPointsIndices();
  assert (indices == (std::vector<int> {1, 0, 3}));

  std::vector<float> phi;
  std::vector<float> radi;
  std::vector<float> z;
  std::vector<float> w;

  unsigned int m = info->Modes1d (phi, radi, z, w);
  assert (m == 3);

  assertFVec (phi,  (std::vector<float> {0.996491, 1.01649, 1.10715}));
  assertFVec (radi, (std::vector<float> {2.02485,  2.46982, 2.01246}));
  assertFVec (z,    (std::vector<float> {2.8, 2.5, 3.1}));
  assertFVec (w,    (std::vector<float> {0.9, 1.8, 1.1}));
}


int main()
{
  std::cout << "TrkVertexSeedFinderUtils/Mode3dFromFsmw1dFinder_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexSeedFinderUtils/TrkVertexSeedFinderUtils_tests.txt", svcloc);

  ToolHandle<Trk::Mode3dFromFsmw1dFinder> tool ("Trk::Mode3dFromFsmw1dFinder");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
