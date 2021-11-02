/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file InDetConversionFinderTools/test/VertexPointEstimator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Simple unit test for VertexPointEstimator.
 */


#undef NDEBUG
#include "InDetConversionFinderTools/VertexPointEstimator.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <cmath>


void assertVec3D (const char* which,
                  const Amg::Vector3D& a,
                  const Amg::Vector3D& b)
{
  if ( ! Athena_test::isEqual (a.x(), b.x(), 2e-5) ||
       ! Athena_test::isEqual (a.y(), b.y(), 2e-5) ||
       ! Athena_test::isEqual (a.z(), b.z(), 2e-5) )
  {
    std::cerr << "assertVec3D mismatch " << which
              << ": ["
              << a.x() << ", "
              << a.y() << ", "
              << a.z() << "] / ["
              << b.x() << ", "
              << b.y() << ", "
              << b.z() << "]\n";
    std::abort();
  }
}


void test1 (InDet::VertexPointEstimator& tool)
{
  std::cout << "test1\n";

  Amg::Vector3D pos0 { 0, 0, 0 };

  Amg::Vector3D pos1a { 1, 0.3, 2 };
  Amg::Vector3D mom1a { 10000, 200, 100 };
  Amg::Vector3D pos1b { 2, 0.4, 1.5 };
  Amg::Vector3D mom1b { 10000, -100, -100 };

  Trk::Perigee p1a (pos1a, mom1a,  1, pos0);
  Trk::Perigee p1b (pos1b, mom1b, -1, pos0);


  int err = 0;
  std::map<std::string, float> m1;
  Amg::Vector3D v1 = tool.getCirclesIntersectionPoint (&p1a, &p1b, 0, err, m1);
  assert (err == 0);
  assertVec3D ("v1", v1, {35.8597, 1.72122, 1.74997});
  assert (m1.size() == 2);
  assert ( Athena_test::isEqual (m1["deltaPhiTracks"], 0.0255183, 1e-5 ) );
  assert ( Athena_test::isEqual (m1["DR1R2"], -2.60722, 1e-5 ) );

  Trk::Perigee p2a (pos1a, mom1a, -1, pos0);
  Trk::Perigee p2b (pos1b, mom1b,  1, pos0);

  err = 0;
  std::map<std::string, float> m2;
  Amg::Vector3D v2 = tool.getCirclesIntersectionPoint (&p2a, &p2b, 0, err, m2);
  assert (err == 0);
  assertVec3D ("v2", v2, {31.1606, 1.69782, 1.74998});
  assert (m2.size() == 2);
  assert ( Athena_test::isEqual (m2["deltaPhiTracks"], 0.0338889, 1e-5 ) );
  assert ( Athena_test::isEqual (m2["DR1R2"], -4.59815, 1e-5 ) );

  Amg::Vector3D pos3a { 1, 0.3, 2 };
  Amg::Vector3D mom3a { 10000, 200, 200 };
  Amg::Vector3D pos3b { 2, -0.4, -1 };
  Amg::Vector3D mom3b { 10000, -500, -300 };

  Trk::Perigee p3a (pos3a, mom3a, -1, pos0);
  Trk::Perigee p3b (pos3b, mom3b,  1, pos0);

  err = 0;
  std::map<std::string, float> m3;
  Amg::Vector3D v3 = tool.getCirclesIntersectionPoint (&p3a, &p3b, 0, err, m3);
#if 0
  std::cout << v3.x() << ", " << v3.y() << ", " << v3.z() << "\n";
  std::cout << err << "\n";
  std::cout << "dphi " << tool.getLastValues()["deltaPhiTracks"] << " "
            << "DR1R2 " << tool.getLastValues()["DR1R2"] << "\n";
#endif
  assert (err == 8);
  assertVec3D ("v3", v3, {-45.9745, 0.190709, 0.729039});
  assert (m3.size() == 2);
  assert ( Athena_test::isEqual (m3["deltaPhiTracks"], 0.0642217, 1e-5 ) );
  assert ( Athena_test::isEqual (m3["DR1R2"], -16.5221, 1e-5 ) );
}


int main()
{
  std::cout << "InDetConversionFinderTools/VertexPointEstimator_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi (svcloc);

  ToolHandle<InDet::VertexPointEstimator> tool ("InDet::VertexPointEstimator");
  assert( tool.retrieve().isSuccess() );
  tool->setProperty("ReturnOnError", false).ignore();
  test1 (*tool);

  return 0;
}
