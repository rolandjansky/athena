/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexSeedFinderUtils/test/Trk2dDistanceSeeder_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2019
 * @brief Unit test for Trk2dDistanceSeeder
 */


#undef NDEBUG
#include "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder.h"
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
#include "../src/Trk2dDistanceSeeder.cxx"
#include "../src/PointOnTrack.cxx"
#include "../src/TwoTracks.cxx"


void assertVec3D (const Amg::Vector3D& a, const Amg::Vector3D& b)
{
  assert( Athena_test::isEqual (a.x(), b.x(), 1e-5) );
  assert( Athena_test::isEqual (a.y(), b.y(), 1e-5) );
  assert( Athena_test::isEqual (a.z(), b.z(), 1e-5) );
}


#if 0
std::ostream& printVec3D (const Amg::Vector3D& a)
{
  std::cout << a.x() << " " << a.y() << " "  << a.z();
  return std::cout;
}
#endif


// Very basic test.
void test1 (Trk::Trk2dDistanceSeeder& tool)
{
  std::cout << "test1\n";

  Amg::Vector3D pos1a { 2, 1, 0 };
  Amg::Vector3D mom1a { 400, 600, 200 };
  Amg::Vector3D pos1b { 1, 2, 0 };
  Amg::Vector3D mom1b { 600, 400, -200 };

  Trk::Perigee p1a (pos1a, mom1a,  1, pos1a);
  Trk::Perigee p1b (pos1b, mom1b, -1, pos1b);

  Trk::TwoPoints pp;
  Trk::TwoPointOnTrack pot = tool.GetSeed (Trk::TwoTracks (p1a, p1b), &pp);

  assertVec3D (pot.first.getPerigee().position(),  pos1a);
  assertVec3D (pot.first.getPerigee().momentum(),  mom1a);
  assert( Athena_test::isEqual (pot.first.getPerigee().charge(), 1) );
  assert( Athena_test::isEqual (pot.first.getPhiPoint(), 0.979777) );
  assertVec3D (pot.second.getPerigee().position(), pos1b);
  assertVec3D (pot.second.getPerigee().momentum(), mom1b);
  assert( Athena_test::isEqual (pot.second.getPerigee().charge(), -1) );
  assert( Athena_test::isEqual (pot.second.getPhiPoint(), 0.591019) );

  assertVec3D (pp.first,  { 4.01976, 4.01976, 0 });
  assertVec3D (pp.second, { 4.01976, 4.01976, 0 });

  Amg::Vector3D pos3a { 10, 2, 2 };
  Amg::Vector3D mom3a { 10000, 30000, 50000 };
  Amg::Vector3D pos3b { 5, 5, -3 };
  Amg::Vector3D mom3b { 50000, 30000, -80000 };

  Trk::Perigee p3a (pos3a, mom3a,  1, pos3a);
  Trk::Perigee p3b (pos3b, mom3b, -1, pos3b);

  pot = tool.GetSeed (Trk::TwoTracks (p3a, p3b), &pp);

  assertVec3D (pot.first.getPerigee().position(),  pos3a);
  assertVec3D (pot.first.getPerigee().momentum(),  mom3a);
  assert( Athena_test::isEqual (pot.first.getPerigee().charge(), 1) );
  assert( Athena_test::isEqual (pot.first.getPhiPoint(), 1.2489, 1e-5) );
  assertVec3D (pot.second.getPerigee().position(), pos3b);
  assertVec3D (pot.second.getPerigee().momentum(), mom3b);
  assert( Athena_test::isEqual (pot.second.getPerigee().charge(), -1) );
  assert( Athena_test::isEqual (pot.second.getPhiPoint(), 0.540509, 1e-5) );

  assertVec3D (pp.first,  { 12.501, 9.50104, 0 });
  assertVec3D (pp.second, { 12.501, 9.50104, 0 });

#if 0
  printVec3D (pp.first) << "\n";
  printVec3D (pp.second) << "\n";

  printVec3D (pot.first.getPerigee().position()) << " / ";
  printVec3D (pot.first.getPerigee().momentum()) << " " <<
    pot.first.getPerigee().charge() << " " << pot.first.getPhiPoint() << "\n";
  printVec3D (pot.second.getPerigee().position()) << " / ";
  printVec3D (pot.second.getPerigee().momentum()) << " " <<
    pot.second.getPerigee().charge() << " " << pot.second.getPhiPoint() << "\n";
#endif
}


int main()
{
  std::cout << "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexSeedFinderUtils/TrkVertexSeedFinderUtils_tests.txt", svcloc);
  ServiceHandle<MagField::IMagFieldSvc> field ("MagField::AtlasFieldSvc/AtlasFieldSvc", "test");
  Incident inc_br ("test", IncidentType::BeginRun);
  dynamic_cast<IIncidentListener*>(&*field)->handle (inc_br);

  ToolHandle<Trk::Trk2dDistanceSeeder> tool ("Trk::Trk2dDistanceSeeder");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
