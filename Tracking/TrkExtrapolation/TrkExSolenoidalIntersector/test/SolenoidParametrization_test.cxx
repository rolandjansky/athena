/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file TrkExSolenoidalIntersector/test/SolenoidParametrization_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2019
 * @brief Basic regression test for SolenoidParametrization
 */

#undef NDEBUG
#include "TrkExSolenoidalIntersector/SolenoidParametrization.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
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

// for the field map
#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TTree.h"

using Athena::Units::meter;
using Athena::Units::cm;


void test1 (Trk::SolenoidParametrization& sol, const AtlasFieldCacheCondObj &fieldCondObj)
{
  MagField::AtlasFieldCache fieldCache;
  fieldCondObj.getInitializedCache (fieldCache);
  std::cout << "test1\n";

  Trk::SolenoidParametrization::Parameters parms (sol, /*r=*/30*cm, /*z=*/123*cm, /*cotTheta=*/1);

  assert( sol.validOrigin (Amg::Vector3D ({10, 10, 10})) );
  assert( !sol.validOrigin (Amg::Vector3D ({50, 10, 10})) );
  assert( !sol.validOrigin (Amg::Vector3D ({10, 10, 300})) );
  
  assert( Athena_test::isEqual (sol.maximumR(), 570) );
  assert( Athena_test::isEqual (sol.maximumZ(), 2150) );
  assert( Athena_test::isEqual (sol.centralField(), -0.598746) );

  assert( Athena_test::isEqual (sol.fieldComponent (123*cm, parms), -0.592178) );
  assert( Athena_test::isEqual (sol.fieldComponent (30*cm, 123*cm, 1, fieldCache),
                                -0.551903) );

  double firstIntegral = 0;
  double secondIntegral = 0;
  sol.fieldIntegrals (firstIntegral, secondIntegral,
                      100*cm, 150*cm, parms);
  assert( Athena_test::isEqual (firstIntegral, -295.526) );
  assert( Athena_test::isEqual (secondIntegral, -296.768) );
}

std::unique_ptr<MagField::AtlasFieldMap> getFieldMap(const std::string mapFile, double sol_current, double tor_current) {
       // find the path to the map file
    std::string resolvedMapFile = PathResolver::find_file( mapFile.c_str(), "DATAPATH" );
    assert ( !resolvedMapFile.empty() );
    // Do checks and extract root file to initialize the map
    assert ( resolvedMapFile.find(".root") != std::string::npos );

    std::unique_ptr<TFile> rootfile( std::make_unique<TFile>(resolvedMapFile.c_str(), "OLD") );
    assert ( rootfile );
    assert ( rootfile->cd() );
    // open the tree
    TTree* tree = (TTree*)rootfile->Get("BFieldMap");
    assert(tree);

    // create map
    std::unique_ptr<MagField::AtlasFieldMap> field_map=std::make_unique<MagField::AtlasFieldMap>();

    // initialize map
    assert (field_map->initializeMap( rootfile.get(), sol_current, tor_current ));
    return field_map;

}


int main()
{
  std::cout << "SolenoidParametrization_test\n";

  std::unique_ptr<MagField::AtlasFieldMap> fieldMap=getFieldMap("MagneticFieldMaps/bfieldmap_7730_20400_14m.root",7730,20400);
  auto fieldCondObj = std::make_unique<AtlasFieldCacheCondObj>();

    // initialize cond obj with current scale factors and the field svc (needed to setup cache)
  fieldCondObj->initialize(1. /*solenoid current scale factor*/, 1. /*toroid current scale factor*/, fieldMap.get());

  Trk::SolenoidParametrization sol (*fieldCondObj);
  test1 (sol, *fieldCondObj);
  
  return 0;
}
