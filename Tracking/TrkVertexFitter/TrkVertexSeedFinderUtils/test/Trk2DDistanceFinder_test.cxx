/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderUtils/test/Trk2DDistanceFinder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Unit test for Trk2DDistanceFinder.
 */


#undef NDEBUG
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <cmath>


std::ostream& printVec3D (const Amg::Vector3D& a)
{
  std::cout << a.x() << " " << a.y() << " "  << a.z();
  return std::cout;
}


void assertVec3D (const Amg::Vector3D& a, const Amg::Vector3D& b)
{
  assert( Athena_test::isEqual (a.x(), b.x(), 1e-5) );
  assert( Athena_test::isEqual (a.y(), b.y(), 1e-5) );
  assert( Athena_test::isEqual (a.z(), b.z(), 1e-5) );
}


// Very basic test.
void test1 (Trk::ITrkDistanceFinder& tool)
{
  std::cout << "test1\n";

  Amg::Vector3D pos1a { 2, 1, 0 };
  Amg::Vector3D mom1a { 400, 600, 200 };
  Amg::Vector3D pos1b { 1, 2, 0 };
  Amg::Vector3D mom1b { 600, 400, -200 };

  Trk::Perigee p1a (pos1a, mom1a,  1, pos1a);
  Trk::Perigee p1b (pos1b, mom1b, -1, pos1b);

  std::optional<Trk::ITrkDistanceFinder::TwoPoints> op =
    tool.CalculateMinimumDistance (p1a, p1b);
  assert( op );
  std::pair<Amg::Vector3D,Amg::Vector3D> pp = op.value();

  assertVec3D (pp.first,  { 4.01976, 4.01976, 0 });
  assertVec3D (pp.second, { 4.01976, 4.01976, 0 });

  //assert( ! tool.CalculateMinimumDistance (p1b, p1b) );

  Amg::Vector3D pos2a { 2, 1, 0 };
  Amg::Vector3D mom2a { 500, 500, 0 };
  Amg::Vector3D pos2b { 1, 2, 0 };
  Amg::Vector3D mom2b { 500, 500, 0 };

  Trk::Perigee p2a (pos2a, mom2a, -1, pos2a);
  Trk::Perigee p2b (pos2b, mom2b,  1, pos2b);

  op = tool.CalculateMinimumDistance (p2a, p2b);
  assert( op );
  pp = op.value();

  assertVec3D (pp.first,  { -27.3934, -27.3934, 0 });
  assertVec3D (pp.second, { -27.3934, -27.3934, 0 });

  Amg::Vector3D pos3a { 10, 2, 2 };
  Amg::Vector3D mom3a { 10000, 30000, 50000 };
  Amg::Vector3D pos3b { 5, 5, -3 };
  Amg::Vector3D mom3b { 50000, 30000, -80000 };

  Trk::Perigee p3a (pos3a, mom3a,  1, pos3a);
  Trk::Perigee p3b (pos3b, mom3b, -1, pos3b);
  op = tool.CalculateMinimumDistance (p3a, p3b);
  assert( op );
  pp = op.value();
  assertVec3D (pp.first,  { 12.501, 9.50104, 0 });
  assertVec3D (pp.second, { 12.501, 9.50104, 0 });
}


int main()
{
  std::cout << "TrkVertexSeedFinderUtils/Trk2DDistanceFinder_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexSeedFinderUtils/TrkVertexSeedFinderUtils_tests.txt", svcloc);
  ServiceHandle<MagField::IMagFieldSvc> field ("MagField::AtlasFieldSvc/AtlasFieldSvc", "test");
  Incident inc_br ("test", IncidentType::BeginRun);
  dynamic_cast<IIncidentListener*>(&*field)->handle (inc_br);

  ToolHandle<Trk::ITrkDistanceFinder> tool ("Trk::Trk2DDistanceFinder");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
