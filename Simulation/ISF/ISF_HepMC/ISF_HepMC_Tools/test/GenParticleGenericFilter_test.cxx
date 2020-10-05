/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date October, 2016
 * @brief Tests for ISF::GenParticleGenericFilter.
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
#include "../src/GenParticleGenericFilter.h"

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
class GenParticleGenericFilter_test: public ::testing::Test, public GaudiFixture {

protected:
  virtual void SetUp() override {
    // the tested tool
    IAlgTool* tool = nullptr;
    EXPECT_TRUE( m_toolSvc->retrieveTool("ISF::GenParticleGenericFilter/TestGenParticleGenericFilter", tool).isSuccess() );
    m_filterTool = dynamic_cast<ISF::GenParticleGenericFilter*>(tool);
  }

  virtual void TearDown() override {
    for (size_t refCount = m_filterTool->refCount(); refCount>0; refCount--) {
      StatusCode sc = m_toolSvc->releaseTool(m_filterTool);
      ASSERT_TRUE( sc.isSuccess() );
    }
  }

  // the tested AthAlgTool
  ISF::GenParticleGenericFilter* m_filterTool;

};  // GenParticleGenericFilter_test fixture


TEST_F(GenParticleGenericFilter_test, allPropertiesUnset_expectParticlePass) {
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

#ifdef HEPMC3
 HepMC::ConstGenParticlePtr part{};
#else
  const HepMC::GenParticle part{};
#endif
  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, minEtaGreaterThanParticleEta_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-1.3").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, minEtaSmallerThanParticleEta_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-.9").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(135.*M_PI/180.), 0.0, 1.0*cos(135.*M_PI/180.), 1.0); // rho=1, eta=-0.88
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, maxEtaSmallerThanParticleEta_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MaxEta", "1.3").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(30.*M_PI/180.), 0.0, 1.0*cos(30.*M_PI/180.), 1.0); // rho=1, eta=+1.32
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, maxEtaGreaterThanParticleEta_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MaxEta", "0.9").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(45.*M_PI/180.), 0.0, 1.0*cos(45.*M_PI/180.), 1.0); // rho=1, eta=+0.88
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, particlePositiveEtaWithinEtaRange_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-1.").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxEta", "0.89").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(45.*M_PI/180.), 0.0, 1.0*cos(45.*M_PI/180.), 1.0); // rho=1, eta=+0.88
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, particleNegativeEtaWithinEtaRange_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-0.89").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxEta", "1.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(135.*M_PI/180.), 0.0, 1.0*cos(135.*M_PI/180.), 1.0); // rho=1, eta=-0.88
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, particlePositiveEtaOutsideEtaRange_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-1.").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxEta", "0.87").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(45.*M_PI/180.), 0.0, 1.0*cos(45.*M_PI/180.), 1.0); // rho=1, eta=+0.88
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, particleNegativeEtaOutsideEtaRange_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-0.87").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxEta", "1.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(135.*M_PI/180.), 0.0, 1.0*cos(135.*M_PI/180.), 1.0); // rho=1, eta=-0.88
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, minPhiGreaterThanParticlePhi_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinPhi", "-1.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*cos(-1.1), 1.0*sin(-1.1), 0.0, 1.0); // rho=1, phi=-1.1
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, maxPhiSmallerThanParticlePhi_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MaxPhi", "1.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*cos(1.1), 1.0*sin(1.1), 0.0, 1.0); // rho=1, phi=1.1
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, pdgDoesntMatchParticle_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("ParticlePDG", "[123,34,5678]").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr();
#else
  const HepMC::GenParticle part{};
#endif
  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, pdgMatchesParticle_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("ParticlePDG", "[123,34,5678]").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom(0, 0, 0, 0);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom, /*pdg id=*/34);
#else
  const HepMC::GenParticle part(mom, /*pdg id=*/34);
#endif
  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, lastPdgMatchesParticle_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("ParticlePDG", "[123,34,5678]").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom(0, 0, 0, 0);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom, /*pdg id=*/5678);
#else
  const HepMC::GenParticle part(mom, /*pdg id=*/5678);
#endif
  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, minMomentumSmallerThanParticleMom_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinMom", "100.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(100.1, 0., 0., 0.);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, minMomentumGreaterThanParticleMom_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinMom", "100.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(99.9, 0., 0., 0.);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, maxMomentumGreaterThanParticleMom_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MaxMom", "100.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(0., 0., 99.9, 0.);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, maxMomentumSmallerThanParticleMom_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MaxMom", "100.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(0., 0., 100.1, 0.);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, particleMomentumWithinMomRange_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinMom", "100.").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxMom", "1000.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(0., 101., 0., 0.);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_TRUE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, particleMomentumBelowMomRange_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinMom", "100.").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxMom", "1000.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(0., 99.9, 0., 0.);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, particleMomentumAboveMomRange_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinMom", "100.").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxMom", "1000.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(0., 0., 1000.1, 0.);
#ifdef HEPMC3
  HepMC::GenParticlePtr part=HepMC::newGenParticlePtr(mom4, /*pdg id=*/11);
#else
  const HepMC::GenParticle part(mom4, /*pdg id=*/11);
#endif

  ASSERT_FALSE( m_filterTool->pass(part) );
}


TEST_F(GenParticleGenericFilter_test, productionVertexInsideApplicableRadius_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MaxApplicableRadius", "10.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector pos(0., 0., 0., 0.);
#ifdef HEPMC3
  HepMC3::GenVertex vtx(pos);
  auto part = HepMC::newGenParticlePtr(); 
  vtx.add_particle_out(part);

  ASSERT_TRUE( m_filterTool->pass(part) );
#else
  HepMC::GenVertex vtx(pos);
  auto part = HepMC::newGenParticlePtr(); // need dynamic allocation as GenVertex takes ownership
  vtx.add_particle_out(part);

  ASSERT_TRUE( m_filterTool->pass(*part) );
#endif
}


TEST_F(GenParticleGenericFilter_test, productionVertexOutsideApplicableRadiusAndUnmetEtaCut_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-1.31").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxApplicableRadius", "10.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
  auto part = HepMC::newGenParticlePtr(mom4, /*pdg id=*/11); // need dynamic allocation as GenVertex takes ownership

  const HepMC::FourVector pos(0., 100., 0., 0.);
#ifdef HEPMC3
  HepMC3::GenVertex vtx(pos);
  vtx.add_particle_out(part);

  ASSERT_TRUE( m_filterTool->pass(part) );
#else
  HepMC::GenVertex vtx(pos);
  vtx.add_particle_out(part);

  ASSERT_TRUE( m_filterTool->pass(*part) );
#endif
}


TEST_F(GenParticleGenericFilter_test, productionVertexWithinApplicableRadiusAndUnmetEtaCut_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-1.31").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxApplicableRadius", "10.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(1.0*sin(150.*M_PI/180.), 0.0, 1.0*cos(150.*M_PI/180.), 1.0); // rho=1, eta=-1.32
  auto part = HepMC::newGenParticlePtr(mom4, /*pdg id=*/11); // need dynamic allocation as GenVertex takes ownership

  const HepMC::FourVector pos(0., 9.9, 0., 0.);
#ifdef HEPMC3
  HepMC3::GenVertex vtx(pos);
  vtx.add_particle_out(part);

  ASSERT_FALSE( m_filterTool->pass(part) );
#else
  HepMC::GenVertex vtx(pos);
  vtx.add_particle_out(part);

  ASSERT_FALSE( m_filterTool->pass(*part) );
#endif
}


TEST_F(GenParticleGenericFilter_test, productionVertexFarForwardInsideApplicableRadiusWithUnmetEtaCut_expectNoPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-2.5").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxEta", "2.5").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxApplicableRadius", "1.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(0., 0., 1.0, 1.0); // rho=1, eta=inf
  auto part = HepMC::newGenParticlePtr(mom4, /*pdg id=*/11); // need dynamic allocation as GenVertex takes ownership

  const HepMC::FourVector pos(0., 0., 9999., 0.);
#ifdef HEPMC3
  HepMC3::GenVertex vtx(pos);
  vtx.add_particle_out(part);
  ASSERT_FALSE( m_filterTool->pass(part) );
#else
  HepMC::GenVertex vtx(pos);
  vtx.add_particle_out(part);


  ASSERT_FALSE( m_filterTool->pass(*part) );
#endif
}


TEST_F(GenParticleGenericFilter_test, productionVertexFarForwardOutsideApplicableRadiusWithUnmetEtaCut_expectPass) {
  EXPECT_TRUE( m_filterTool->setProperty("MinEta", "-2.5").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxEta", "2.5").isSuccess() );
  EXPECT_TRUE( m_filterTool->setProperty("MaxApplicableRadius", "1.").isSuccess() );
  EXPECT_TRUE( m_filterTool->initialize().isSuccess() );

  const HepMC::FourVector mom4(0., 0., 1.0, 1.0); // rho=1, eta=inf
  auto part = HepMC::newGenParticlePtr(mom4, /*pdg id=*/11); // need dynamic allocation as GenVertex takes ownership

  const HepMC::FourVector pos(1.1, 0., 9999., 0.);
#ifdef HEPMC3
  HepMC3::GenVertex vtx(pos);
  vtx.add_particle_out(part);

  ASSERT_TRUE( m_filterTool->pass(part) );
#else
  HepMC::GenVertex vtx(pos);
  vtx.add_particle_out(part);

  ASSERT_TRUE( m_filterTool->pass(*part) );
#endif
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
