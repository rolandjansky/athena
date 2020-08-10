/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrkVertexFitterUtils/test/ImpactPoint3dEstimator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2019
 * @brief Simple unit test for ImpactPoint3dEstimator.
 */


#undef NDEBUG
#include "TrkVertexFitterInterfaces/IImpactPoint3dEstimator.h"
#include "TestTools/initGaudi.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <cmath>

// for the field map
#include "PathResolver/PathResolver.h"
#include "TFile.h"
#include "TTree.h"

// for populating conditions store
#include "SGTools/TestStore.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/WriteCondHandle.h"

// for the conditions data
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

using Gaudi::Units::mm;
using Gaudi::Units::MeV;
using Gaudi::Units::GeV;


std::unique_ptr<AmgSymMatrix(5)> cov5()
{
  auto m = std::make_unique<AmgSymMatrix(5)>();
  m->setIdentity();
  return m;
}


[[maybe_unused]]
void dumpPlaneSurface (const Trk::PlaneSurface& s)
{
  std::cout << "{" << s.center().x() << ", "
            << s.center().y() << ", "
            << s.center().z() << "} ";
  std::cout << "{" << s.normal().x() << ", "
            << s.normal().y() << ", "
            << s.normal().z() << "}\n";
}


void assertVec3D (const char* which,
                  const Amg::Vector3D& a,
                  const Amg::Vector3D& b)
{
  if ( ! Athena_test::isEqual (a.x(), b.x(), 2e-5) ||
       ! Athena_test::isEqual (a.y(), b.y(), 2e-5) ||
       ! Athena_test::isEqual (a.z(), b.z(), 2e-5) )
  {
    std::cerr << "TrkVKalVrtFitterTestAlg::assertVec3D mismatch " << which
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


void checkPlaneSurface (const Trk::PlaneSurface& s,
                        const Amg::Vector3D& pos,
                        const Amg::Vector3D& norm)
{
  assertVec3D ("pos", s.center(), pos);
  assertVec3D ("normal", s.normal(), norm);
}


void test1 (Trk::IImpactPoint3dEstimator& tool)
{
  std::cout << "test1\n";

  Amg::Vector3D pos1 { 2*mm, 1*mm, -10*mm };
  Amg::Vector3D mom1 { 400*MeV, 600*MeV, 200*MeV };
  Amg::Vector3D vert { 1.2*mm, 0.8*mm, -7*mm };

  double distance = 0;
  Trk::Perigee p1 (pos1, mom1, 1, pos1, cov5().release());
  std::unique_ptr<Trk::PlaneSurface> s1 (tool.Estimate3dIP (&p1, &vert, distance));
  checkPlaneSurface (*s1, vert, {0.53466, 0.801692, 0.267261});
  assert( Athena_test::isEqual (distance, 3.10391, 1e-5) );

  distance = 0;
  Trk::NeutralPerigee p2 (pos1, mom1, 1, pos1, cov5().release());
  std::unique_ptr<Trk::PlaneSurface> s2 (tool.Estimate3dIP (&p2, &vert, distance));
  checkPlaneSurface (*s2, vert, {0.534522, 0.801784, 0.267261});
  assert( Athena_test::isEqual (distance, 3.10391, 1e-5) );

  // Other methods require full Extrapolator / TrackingGeometry machinery;
  // not tested here.
}

std::unique_ptr<MagField::AtlasFieldMap> getFieldMap(const std::string& mapFile, double sol_current, double tor_current) {
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

void createNewtonTrkDistanceFinderCondData(SGTest::TestStore &store) {
   SG::WriteCondHandleKey<AtlasFieldCacheCondObj> fieldKey {"fieldCondObj"};
   assert( fieldKey.initialize().isSuccess());

   // from StoreGate/test/WriteCondHandle_test.cxx
   EventIDBase now(0, EventIDBase::UNDEFEVT, 1);
   EventContext ctx(1, 1);
   ctx.setEventID( now );
   ctx.setExtension( Atlas::ExtendedEventContext(&store) );
   Gaudi::Hive::setCurrentContext(ctx);

   EventIDBase s1_1(0, EventIDBase::UNDEFEVT, 0);
   EventIDBase e1_1(0, EventIDBase::UNDEFEVT, 3);
   EventIDRange r1_1 (s1_1,e1_1);


   SG::WriteCondHandle<AtlasFieldCacheCondObj> fieldHandle {fieldKey};
   {
      std::unique_ptr<MagField::AtlasFieldMap> fieldMap=getFieldMap("MagneticFieldMaps/bfieldmap_7730_20400_14m.root",7730,20400);
      auto fieldCondObj = std::make_unique<AtlasFieldCacheCondObj>();
      fieldCondObj->initialize(1. /*solenoid current scale factor*/, 1. /*toroid current scale factor*/, fieldMap.release());
      assert( fieldHandle.record(r1_1, std::move(fieldCondObj)).isSuccess());
   }
}

int main()
{
  std::cout << "TrkVertexFitterUtils/ImpactPoint3dEstimator_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexFitterUtils/TrkVertexFitterUtils_tests.txt", svcloc);

  StoreGateSvc *cs=nullptr;
  assert (svcloc->service("StoreGateSvc/ConditionStore",cs).isSuccess());

  SGTest::TestStore dumstore;
  createNewtonTrkDistanceFinderCondData(dumstore);

  ToolHandle<Trk::IImpactPoint3dEstimator> tool ("Trk::ImpactPoint3dEstimator");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
