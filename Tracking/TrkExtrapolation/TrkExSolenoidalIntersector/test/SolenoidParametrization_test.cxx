/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file TrkExSolenoidalIntersector/test/SolenoidParametrization_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2019
 * @brief Basic regression test for SolenoidParametrization
 */

#undef NDEBUG
#include "TrkExSolenoidalIntersector/SolenoidParametrization.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "AthenaKernel/Units.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include <iostream>
#include <cassert>
#include <cmath>

#include "../src/SolenoidParametrization.cxx"


using Athena::Units::meter;
using Athena::Units::cm;


void test1 (Trk::SolenoidParametrization& sol)
{
  std::cout << "test1\n";

  Trk::SolenoidParametrization::Parameters parms (sol, /*r=*/30*cm, /*z=*/123*cm, /*cotTheta=*/1);

  assert( sol.validOrigin (Amg::Vector3D ({10, 10, 10})) );
  assert( !sol.validOrigin (Amg::Vector3D ({50, 10, 10})) );
  assert( !sol.validOrigin (Amg::Vector3D ({10, 10, 300})) );
  
  assert( Athena_test::isEqual (sol.maximumR(), 570) );
  assert( Athena_test::isEqual (sol.maximumZ(), 2150) );
  assert( Athena_test::isEqual (sol.centralField(), -0.598746) );

  assert( Athena_test::isEqual (sol.fieldComponent (123*cm, parms), -0.592178) );
  assert( Athena_test::isEqual (sol.fieldComponent (30*cm, 123*cm, 1),
                                -0.551903) );

  double firstIntegral = 0;
  double secondIntegral = 0;
  sol.fieldIntegrals (firstIntegral, secondIntegral,
                      100*cm, 150*cm, parms);
  assert( Athena_test::isEqual (firstIntegral, -295.526) );
  assert( Athena_test::isEqual (secondIntegral, -296.768) );
}


int main()
{
  std::cout << "SolenoidParametrization_test\n";
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("SolenoidalIntersector_test.txt", svcloc);
  ServiceHandle<MagField::IMagFieldSvc> field ("MagField::AtlasFieldSvc/AtlasFieldSvc", "test");
  Incident inc_br ("test", IncidentType::BeginRun);
  dynamic_cast<IIncidentListener*>(&*field)->handle (inc_br);

  Trk::SolenoidParametrization sol (&*field);
  test1 (sol);
  
  return 0;
}
