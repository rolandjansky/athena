/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderUtils/test/Mode3dTo1dFinder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Simple unit test for Mode3dTo1dFinder.
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
#include "TrkVertexSeedFinderUtils/Mode3dTo1dFinder.h"
#include <cassert>
#include <cmath>

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


void test1 (Trk::Mode3dTo1dFinder& tool)
{
  std::cout << "test1\n";

  {
    std::vector<Trk::PositionAndWeight> points;
    points.emplace_back (Amg::Vector3D(1.1, 1.7, 2.8), 0.9);
    points.emplace_back (Amg::Vector3D(0.9, 1.8, 3.1), 1.1);
    points.emplace_back (Amg::Vector3D(0.8, 2.3, 3.5), 0.7);
    points.emplace_back (Amg::Vector3D(1.3, 2.1, 2.5), 1.8);

    const Amg::Vector3D mode = tool.getMode (0, 0, points);
    assertVec3D (mode, {0.861111, 1.755, 2.6,});
  }

  {
    std::vector<Amg::Vector3D> points;
    points.emplace_back (1.1, 1.7, 2.8);
    points.emplace_back (0.9, 1.8, 3.1);
    points.emplace_back (0.8, 2.3, 3.5);
    points.emplace_back (1.3, 2.1, 2.5);

    const Amg::Vector3D mode = tool.getMode (0, 0, points);
    assertVec3D (mode, {0.85, 1.75, 2.65});
  }
}


int main()
{
  std::cout << "TrkVertexSeedFinderUtils/Mode3dTo1dFinder_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexSeedFinderUtils/TrkVertexSeedFinderUtils_tests.txt", svcloc);

  ToolHandle<Trk::Mode3dTo1dFinder> tool ("Trk::Mode3dTo1dFinder");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
