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
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "TestTools/initGaudi.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/ubsan_suppress.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>
#include <cmath>

// Can't link against these --- they're in a component library.
#include "../src/Trk2dDistanceSeeder.cxx"
#include "../src/PointOnTrack.cxx"
#include "../src/TwoTracks.cxx"

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
  std::cout << "TrkVertexSeedFinderUtils/Trk2dDistanceSeeder_test\n";
  CxxUtils::ubsan_suppress ([]() { TInterpreter::Instance(); });
  ISvcLocator* svcloc = nullptr;
  Athena_test::initGaudi ("TrkVertexSeedFinderUtils/TrkVertexSeedFinderUtils_tests.txt", svcloc);

  StoreGateSvc *cs=nullptr;
  assert (svcloc->service("StoreGateSvc/ConditionStore",cs).isSuccess());

  SGTest::TestStore dumstore;
  createNewtonTrkDistanceFinderCondData(dumstore);

  ToolHandle<Trk::Trk2dDistanceSeeder> tool ("Trk::Trk2dDistanceSeeder");
  assert( tool.retrieve().isSuccess() );

  test1 (*tool);

  return 0;
}
