/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Tests for ISF::GenParticleLifetimeFilter.
 */

#undef NDEBUG

// Google Test
#include "gtest/gtest.h"

// Framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"

// tested AthAlgTool
#include "../src/GenParticleLifetimeFilter.h"

// Truth related includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"


namespace ISFTesting {

// Gaudi Test fixture that provides a clean Gaudi environment for
// each individual test case
class GaudiFixture {

protected:
  GaudiFixture() {
    SetUpGaudi();
  }

  ~GaudiFixture() {
    TearDownGaudi();
  }

  void SetUpGaudi() {
    m_appMgr = Gaudi::createApplicationMgr();
    ASSERT_TRUE( m_appMgr!=nullptr );

    m_svcLoc = m_appMgr;
    ASSERT_TRUE( m_svcLoc.isValid() );

    m_svcMgr = m_appMgr;
    ASSERT_TRUE( m_svcMgr.isValid() );

    m_propMgr = m_appMgr;
    ASSERT_TRUE( m_propMgr.isValid() );
    ASSERT_TRUE( m_propMgr->setProperty("EvtSel", "NONE").isSuccess() );
    ASSERT_TRUE( m_propMgr->setProperty("JobOptionsType", "NONE").isSuccess() );

    m_toolSvc = m_svcLoc->service("ToolSvc");
    ASSERT_TRUE( m_toolSvc.isValid() );

    ASSERT_TRUE( m_appMgr->configure().isSuccess() );
    ASSERT_TRUE( m_appMgr->initialize().isSuccess() );
  }

  void TearDownGaudi() {
    ASSERT_TRUE( m_appMgr->finalize().isSuccess() );
    ASSERT_TRUE( m_appMgr->terminate().isSuccess() );
    Gaudi::setInstance( static_cast<IAppMgrUI*>(nullptr) );
  }

  // protected member variables for Core Gaudi components
  IAppMgrUI*             m_appMgr = nullptr;
  SmartIF<ISvcLocator>   m_svcLoc;
  SmartIF<ISvcManager>   m_svcMgr;
  SmartIF<IToolSvc>      m_toolSvc;
  SmartIF<IProperty>     m_propMgr;
};


// Test fixture specifically for SimKernelMT AthAlgorithm
class GenParticleLifetimeFilter_test: public ::testing::Test, public GaudiFixture {

protected:
  virtual void SetUp() override {
    // the tested tool
    IAlgTool* tool = nullptr;
    EXPECT_TRUE( m_toolSvc->retrieveTool("ISF::GenParticleLifetimeFilter/TestGenParticleLifetimeFilter", tool).isSuccess() );
    m_filterTool = dynamic_cast<ISF::GenParticleLifetimeFilter*>(tool);
  }

  virtual void TearDown() override {
    for (size_t refCount = m_filterTool->refCount(); refCount>0; refCount--) {
      StatusCode sc = m_toolSvc->releaseTool(m_filterTool);
      ASSERT_TRUE( sc.isSuccess() );
    }
  }

  // the tested AthAlgTool
  ISF::GenParticleLifetimeFilter* m_filterTool;

};  // GenParticleLifetimeFilter_test fixture


TEST_F(GenParticleLifetimeFilter_test, allPropertiesUnset_expectPass) {
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::GenParticle part{};
  ASSERT_TRUE( m_filterTool->pass(part) ); // will pass as no end vertex
}


TEST_F(GenParticleLifetimeFilter_test, setMinimumLifetime_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinimumLifetime", "1.3").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::GenParticle part{};
  ASSERT_TRUE( m_filterTool->pass(part) ); // will pass as no end vertex
}


TEST_F(GenParticleLifetimeFilter_test, addProdVtx_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinimumLifetime", "1.3").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("OutputLevel", "3").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector prodPos(0., 0., 0., 0.);
  HepMC::GenVertex prodVtx(prodPos);
  auto part = HepMC::newGenParticlePtr(); // need dynamic allocation as GenVertex takes ownership
  prodVtx.add_particle_out(part);
  ASSERT_TRUE( m_filterTool->pass(*part) ); // will pass as no end vertex
}


TEST_F(GenParticleLifetimeFilter_test, minLifetimeGreaterThanParticleLifetime_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinimumLifetime", "1.3").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector prodPos(0., 0., 0., 0.);
  HepMC::GenVertex prodVtx(prodPos);
  auto part = HepMC::newGenParticlePtr(); // need dynamic allocation as GenVertex takes ownership
  prodVtx.add_particle_out(part);
  const HepMC::FourVector endPos(0., 0., 0., 1.);
  HepMC::GenVertex endVtx(endPos);
  endVtx.add_particle_in(part);
  ASSERT_FALSE( m_filterTool->pass(*part) ); // will fail as particle lifetime is only 1.0
}


TEST_F(GenParticleLifetimeFilter_test, minLifetimeLessThanParticleLifetime_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinimumLifetime", "1.3").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector prodPos(0., 0., 0., 0.);
  HepMC::GenVertex prodVtx(prodPos);
  auto part = HepMC::newGenParticlePtr(); // need dynamic allocation as GenVertex takes ownership
  prodVtx.add_particle_out(part);
  const HepMC::FourVector endPos(0., 0., 0., 2.);
  HepMC::GenVertex endVtx(endPos);
  endVtx.add_particle_in(part);
  ASSERT_TRUE( m_filterTool->pass(*part) ); // will pass as particle lifetime is 2.0
}


TEST_F(GenParticleLifetimeFilter_test, endVtxButNoProdVtx_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinimumLifetime", "1.3").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  auto part = HepMC::newGenParticlePtr(); // need dynamic allocation as GenVertex takes ownership
  const HepMC::FourVector endPos(0., 0., 0., 2.);
  HepMC::GenVertex endVtx(endPos);
  endVtx.add_particle_in(part);
  ASSERT_FALSE( m_filterTool->pass(*part) ); // will fail as prodVtx undefined
}

} // namespace ISFTesting


int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
  // if the above gets stuck forever while trying to finalize Boost stuff
  // inside SGTools, try to use that:
  //  skips proper finalization:
  //std::quick_exit( RUN_ALL_TESTS() );
}
