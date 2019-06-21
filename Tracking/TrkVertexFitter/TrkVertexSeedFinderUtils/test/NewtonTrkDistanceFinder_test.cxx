/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderUtils/test/NewtonTrkDistanceFinder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Unit test for NewtonTrkDistanceFinder.
 */


#undef NDEBUG
#include "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder.h"
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


// Can't link against these --- they're in a component library.
#include "../src/NewtonTrkDistanceFinder.cxx"
#include "../src/PointOnTrack.cxx"


void assertVec3D (const Amg::Vector3D& a, const Amg::Vector3D& b)
{
  assert( Athena_test::isEqual (a.x(), b.x(), 1e-5) );
  assert( Athena_test::isEqual (a.y(), b.y(), 1e-5) );
  assert( Athena_test::isEqual (a.z(), b.z(), 1e-5) );
}


// Very basic test.
void test1 (Trk::NewtonTrkDistanceFinder& tool)
{
  std::cout << "test1\n";

  Amg::Vector3D pos1a { 2, 1, 0 };
  Amg::Vector3D mom1a { 400, 600, 200 };
  Amg::Vector3D pos1b { 1, 2, 0 };
  Amg::Vector3D mom1b { 600, 400, -200 };

  Trk::Perigee p1a (pos1a, mom1a,  1, pos1a);
  Trk::Perigee p1b (pos1b, mom1b, -1, pos1b);

  Trk::TwoPointOnTrack pot = tool.GetClosestPoints (p1a, p1b);
  Trk::TwoPoints pp = tool.GetClosestPoints();

  assertVec3D (pp.first,  { -2.6655,  1.99717, 0.3325  } );
  assertVec3D (pp.second, { -1.99717, 2.6655, -0.3325  } );

  Amg::Vector3D pos2a { 2, 1, 0 };
  Amg::Vector3D mom2a { 500, 500, 0 };
  Amg::Vector3D pos2b { 1, 2, 0 };
  Amg::Vector3D mom2b { 500, 500, 0 };

  Trk::Perigee p2a (pos2a, mom2a, -1, pos2a);
  Trk::Perigee p2b (pos2b, mom2b,  1, pos2b);

  EXPECT_EXCEPTION( Trk::Error::NewtonProblem, tool.GetClosestPoints (p2a, p2b));

  Amg::Vector3D pos3a { 10, 2, 2 };
  Amg::Vector3D mom3a { 10000, 30000, 50000 };
  Amg::Vector3D pos3b { 5, 5, -3 };
  Amg::Vector3D mom3b { 50000, 30000, -80000 };

  Trk::Perigee p3a (pos3a, mom3a,  1, pos3a);
  Trk::Perigee p3b (pos3b, mom3b, -1, pos3b);
  pot = tool.GetClosestPoints (p3a, p3b);
  pp = tool.GetClosestPoints();
  assertVec3D (pp.first,  { -9.47935, 0.437956, -0.603394 });
  assertVec3D (pp.second, { -4.46622, 4.67974,  -2.14596  });
}


int main()
{
  std::cout << "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexSeedFinderUtils/TrkVertexSeedFinderUtils_tests.txt", svcloc);
  ServiceHandle<MagField::IMagFieldSvc> field ("MagField::AtlasFieldSvc/AtlasFieldSvc", "test");
  Incident inc_br ("test", IncidentType::BeginRun);
  dynamic_cast<IIncidentListener*>(&*field)->handle (inc_br);

  ToolHandle<Trk::NewtonTrkDistanceFinder> tool ("Trk::NewtonTrkDistanceFinder");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
