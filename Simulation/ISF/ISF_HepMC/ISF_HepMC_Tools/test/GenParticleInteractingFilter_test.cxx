/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date October, 2016
 * @brief Tests for ISF::GenParticleInteractingFilter.
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
#include "../src/GenParticleInteractingFilter.h"

// Truth related includes
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/SimpleVector.h"


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
class GenParticleInteractingFilter_test: public ::testing::Test, public GaudiFixture {

protected:
  virtual void SetUp() override {
    // the tested tool
    IAlgTool* tool = nullptr;
    EXPECT_TRUE( m_toolSvc->retrieveTool("ISF::GenParticleInteractingFilter/TestGenParticleInteractingFilter", tool).isSuccess() );
    m_filterTool = dynamic_cast<ISF::GenParticleInteractingFilter*>(tool);
  }

  virtual void TearDown() override {
    for (size_t refCount = m_filterTool->refCount(); refCount>0; refCount--) {
      StatusCode sc = m_toolSvc->releaseTool(m_filterTool);
      ASSERT_TRUE( sc.isSuccess() );
    }
  }

  // the tested AthAlgTool
  ISF::GenParticleInteractingFilter* m_filterTool;

};  // GenParticleInteractingFilter_test fixture


TEST_F(GenParticleInteractingFilter_test, allPropertiesUnset_stdParticle_expectPass) {
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif
  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleInteractingFilter_test, allPropertiesUnset_stdParticle_expectNoPass) {
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/12);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/12);
#endif
  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleInteractingFilter_test, allPropertiesUnset_exoticParticle_expectNoPass) {
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/4110000);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/4110000);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleInteractingFilter_test, stdParticle_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("AdditionalInteractingParticleTypes", "[4110000]").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif
  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleInteractingFilter_test, stdParticle_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("AdditionalInteractingParticleTypes", "[4110000]").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/12);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/12);
#endif
  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleInteractingFilter_test, exoticParticle_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("AdditionalInteractingParticleTypes", "[4110000]").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/4110000);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/4110000);
#endif

  ASSERT_TRUE( m_filterTool->pass(part) );
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
