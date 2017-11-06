/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date June, 2016
 * @brief Tests for ISF::InputConverter.
 */

#undef NDEBUG

// Tested service
#include "../src/InputConverter.h"

// Framework
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SmartIF.h"
#include "AthenaBaseComps/AthAlgTool.h"

// Framework testing
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"
// Google Mock
#include "gmock/gmock.h"

// ISF
#include "ISF_Event/ISFParticle.h"
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

// Amg
#include "GeoPrimitives/GeoPrimitives.h"

// HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// STL includes
#include <cstdlib> // quick_exit


namespace ISFTesting {

// Athena Tool to emulate a GenParticleFilter
class MockFilterTool : public AthAlgTool,
                       public ISF::IGenParticleFilter { 
    
 public: 
  MockFilterTool(const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type,name,parent)
  { declareInterface<ISF::IGenParticleFilter>(this); };

  virtual ~MockFilterTool() {};

  // mock method which will be called by tested code
  MOCK_CONST_METHOD1(pass, bool(const HepMC::GenParticle&));
}; 

DECLARE_TOOL_FACTORY( MockFilterTool )


class InputConverter_test: public ::testing::Test {

 protected:
  virtual void SetUp() override {
    m_appMgr = Gaudi::createApplicationMgr();
    ASSERT_TRUE( m_appMgr!=nullptr );

    m_svcLoc = m_appMgr;
    ASSERT_TRUE( m_svcLoc.isValid() );

    m_svcMgr = m_appMgr;
    ASSERT_TRUE( m_svcMgr.isValid() );

    m_propMgr = m_appMgr;
    ASSERT_TRUE( m_propMgr.isValid() );
    ASSERT_TRUE( m_propMgr->setProperty( "EvtSel", "NONE" ).isSuccess() );
    ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsType", "FILE" ).isSuccess() );
    ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsPath", "InputConverter_test.txt" ).isSuccess() );

    m_toolSvc = m_svcLoc->service("ToolSvc");
    ASSERT_TRUE( m_toolSvc.isValid() );

    ASSERT_TRUE( m_appMgr->configure().isSuccess() );
    ASSERT_TRUE( m_appMgr->initialize().isSuccess() );

    // the tested AthenaService
    const auto& inputConverterTypeAndName = "ISF::InputConverter/InputConverter";
    SmartIF<IService> svc = m_svcLoc->service(inputConverterTypeAndName);
    m_svc = dynamic_cast<ISF::InputConverter*>(svc.get());
    ASSERT_NE(nullptr, m_svc);

    ASSERT_TRUE( m_svc->setProperties().isSuccess() );
    ASSERT_TRUE( m_svc->configure().isSuccess() );
  }

  virtual void TearDown() override {
    m_svcMgr->removeService(m_svc);
    ASSERT_TRUE( m_svc->finalize().isSuccess() );
    ASSERT_TRUE( m_svc->terminate().isSuccess() );
    ReleaseSmartIFComponent(m_svc);

    ASSERT_TRUE( m_appMgr->finalize().isSuccess() );
    ASSERT_TRUE( m_appMgr->terminate().isSuccess() );
    Gaudi::setInstance( static_cast<IAppMgrUI*>(nullptr)) ;
  }

  void ReleaseSmartIFComponent(IInterface* comp) {
    size_t finalRefCount = 1; // keep one reference for the SmartIF destructor
    for (size_t refCount = comp->refCount(); refCount>finalRefCount; refCount--) {
      comp->release();
    }
  }

  //
  // accessors for private methods
  // NB: This works because InputConverter_test is a friend
  //     of the tested InputConverter service
  //
  template<typename... Args>
  ISF::ISFParticle* convertParticle(Args&&... args) const {
    return m_svc->convertParticle(std::forward<Args>(args)...);
  }

  template<typename... Args>
  bool passesFilters(Args&&... args) const {
    return m_svc->passesFilters(std::forward<Args>(args)...);
  }

  ToolHandleArray<ISF::IGenParticleFilter>& getGenParticleFilters() const {
    return m_svc->m_genParticleFilters;
  }
  //
  // protected member variables 
  //

  // Core Gaudi components
  IAppMgrUI*             m_appMgr = nullptr;
  SmartIF<ISvcLocator>   m_svcLoc;
  SmartIF<ISvcManager>   m_svcMgr;
  SmartIF<IToolSvc>      m_toolSvc;
  SmartIF<IProperty>     m_propMgr;

  ISF::InputConverter*   m_svc; // the tested AthenaService

};  // InputConverter_test fixture


TEST_F(InputConverter_test, initialize_empty) {
  ASSERT_TRUE( m_svc->initialize().isSuccess() );
}


TEST_F(InputConverter_test, convertParticle_nullptr) {
  ISF::ISFParticle* expected = nullptr;
  int bcid = 1;
  ASSERT_EQ( expected, convertParticle(nullptr, bcid) );
}


TEST_F(InputConverter_test, convertParticle_without_production_vertex) {
  HepMC::FourVector mom(12.3, 45.6, 78.9, 0.12);
  HepMC::GenParticle* genPart = new HepMC::GenParticle(mom,
                                                       123, // pdg
                                                       1 // status
                                                      );
  ISF::ISFParticle* expected = nullptr;
  int bcid = 99;
  ASSERT_EQ( expected, convertParticle(genPart, bcid) );
  delete genPart;
}


TEST_F(InputConverter_test, convertParticle_using_generated_mass) {
  m_svc->setProperty("UseGeneratedParticleMass", "True");
  ASSERT_TRUE( m_svc->initialize().isSuccess() );

  HepMC::FourVector mom(12.3, 45.6, 78.9, 0.12);
  // dynamic allocation necessary as particle ownership is
  // handed over to a HepMC::GenVertex later
  HepMC::GenParticle* genPart = new HepMC::GenParticle(mom,
                                                       11, // pdg id (e-)
                                                       1 // status
                                                      );
  genPart->set_generated_mass(1234.56);
  genPart->suggest_barcode(9876352);

  HepMC::FourVector pos(9.8, 7.65, 4.3, 0.321); // NB: 4th component is time*c
  int vtx_id = -123;
  HepMC::GenVertex prodVtx(pos, vtx_id);
  prodVtx.add_particle_out(genPart);

  Amg::Vector3D expectedPos(9.8, 7.65, 4.3);
  Amg::Vector3D expectedMom(12.3, 45.6, 78.9);
  ISF::DetRegionSvcIDPair expectedHistory(AtlasDetDescr::fUndefinedAtlasRegion, ISF::fEventGeneratorSimID);
  auto* expectedTruthBinding = new ISF::TruthBinding(genPart);
  ISF::ISFParticle expected(expectedPos,
                            expectedMom,
                            1234.56,
                            -1., // charge
                            11, // pdg id
                            0.321/Gaudi::Units::c_light, // time
                            expectedHistory,
                            563, // bcid
                            9876352, // barcode
                            expectedTruthBinding
                            );

  int bcid = 563;
  // call the InputConverter's private method
  ISF::ISFParticle* returned = convertParticle(genPart, bcid);
  ASSERT_TRUE( returned );

  ASSERT_EQ( expected, *returned );

  delete returned;
}


TEST_F(InputConverter_test, convertParticle_using_particleDataTable_photon) {
  m_svc->setProperty("UseGeneratedParticleMass", "False");
  ASSERT_TRUE( m_svc->initialize().isSuccess() );

  HepMC::FourVector mom(12.3, 45.6, 78.9, 0.12);
  // dynamic allocation necessary as particle ownership is
  // handed over to a HepMC::GenVertex later
  HepMC::GenParticle* genPart = new HepMC::GenParticle(mom,
                                                       22, // pdg id (gamma)
                                                       1 // status
                                                      );
  genPart->set_generated_mass(1234.56); // should be ignored later on
  genPart->suggest_barcode(9876352);

  HepMC::FourVector pos(9.8, 7.65, 4.3, 0.321); // NB: 4th component is time*c
  int vtx_id = -123;
  HepMC::GenVertex prodVtx(pos, vtx_id);
  prodVtx.add_particle_out(genPart);

  Amg::Vector3D expectedPos(9.8, 7.65, 4.3);
  Amg::Vector3D expectedMom(12.3, 45.6, 78.9);
  ISF::DetRegionSvcIDPair expectedHistory(AtlasDetDescr::fUndefinedAtlasRegion, ISF::fEventGeneratorSimID);
  auto* expectedTruthBinding = new ISF::TruthBinding(genPart);
  ISF::ISFParticle expected(expectedPos,
                            expectedMom,
                            0., // mass from ParticleDataTable
                            0., // charge
                            22, // pdg id
                            0.321/Gaudi::Units::c_light, // time
                            expectedHistory,
                            0, // bcid
                            9876352, // barcode
                            expectedTruthBinding
                            );

  int bcid = 0;
  // call the InputConverter's private method
  ISF::ISFParticle* returned = convertParticle(genPart, bcid);
  ASSERT_TRUE( returned );

  ASSERT_EQ( expected, *returned );

  delete returned;
}


TEST_F(InputConverter_test, convertParticle_using_particleDataTable_electron) {
  m_svc->setProperty("UseGeneratedParticleMass", "False");
  ASSERT_TRUE( m_svc->initialize().isSuccess() );

  HepMC::FourVector mom(12.3, 45.6, 78.9, 0.12);
  // dynamic allocation necessary as particle ownership is
  // handed over to a HepMC::GenVertex later
  HepMC::GenParticle* genPart = new HepMC::GenParticle(mom,
                                                       11, // pdg id (e-)
                                                       1 // status
                                                      );
  genPart->set_generated_mass(1234.56); // should be ignored later on
  genPart->suggest_barcode(9876352);

  HepMC::FourVector pos(9.8, 7.65, 4.3, 0.321); // NB: 4th component is time*c
  int vtx_id = -123;
  HepMC::GenVertex prodVtx(pos, vtx_id);
  prodVtx.add_particle_out(genPart);

  Amg::Vector3D expectedPos(9.8, 7.65, 4.3);
  Amg::Vector3D expectedMom(12.3, 45.6, 78.9);
  ISF::DetRegionSvcIDPair expectedHistory(AtlasDetDescr::fUndefinedAtlasRegion, ISF::fEventGeneratorSimID);
  auto* expectedTruthBinding = new ISF::TruthBinding(genPart);
  ISF::ISFParticle expected(expectedPos,
                            expectedMom,
                            0.51099891/Gaudi::Units::MeV, // from particle
                            -1., // charge
                            11, // pdg id
                            0.321/Gaudi::Units::c_light, // time
                            expectedHistory,
                            11, // bcid
                            9876352, // barcode
                            expectedTruthBinding
                            );

  int bcid = 11;
  // call the InputConverter's private method
  ISF::ISFParticle* returned = convertParticle(genPart, bcid);
  ASSERT_TRUE( returned );

  ASSERT_EQ( expected, *returned );

  delete returned;
}


TEST_F(InputConverter_test, passesFilters_empty_filters_defaultconstructed_genpart) {
  ASSERT_TRUE( m_svc->initialize().isSuccess() );

  const HepMC::GenParticle genPart{};
  ASSERT_TRUE( passesFilters(genPart) );
}


TEST_F(InputConverter_test, passesFilters_empty_filters) {
  ASSERT_TRUE( m_svc->initialize().isSuccess() );

  HepMC::FourVector mom(12.3, 45.6, 78.9, 0.12);
  HepMC::GenParticle genPart(mom,
                              11, // pdg id (e-)
                              1 // status
                             );
  genPart.set_generated_mass(1234.56);
  genPart.suggest_barcode(9876352);
  const HepMC::GenParticle constGenPart(std::move(genPart));

  ASSERT_TRUE( passesFilters(constGenPart) );
}


TEST_F(InputConverter_test, passesFilters_one_pass_filter) {
  // retrieve mockable GenParticleFilter tool and point InputConverter to the same instance
  m_svc->setProperty("GenParticleFilters", "['ISFTesting::MockFilterTool/DummyFilter']");
  ASSERT_TRUE( m_svc->initialize().isSuccess() );
  ToolHandleArray<ISF::IGenParticleFilter>& genParticleFilters = getGenParticleFilters();
  ASSERT_EQ (genParticleFilters.size(), 1);
  MockFilterTool* filterTool = dynamic_cast<MockFilterTool*>(&*(genParticleFilters[0]));
  ASSERT_TRUE( filterTool );
  const HepMC::GenParticle genPart{};
  HepMC::FourVector mom(12.3, 45.6, 78.9, 0.12);
  HepMC::GenParticle genPart2(mom,
                              11, // pdg id (e-)
                              1 // status
                              );

  EXPECT_CALL(*filterTool, pass(genPart))
             .Times(1)
             .WillOnce(::testing::Return(true));

  ASSERT_TRUE( passesFilters(genPart) );
}


TEST_F(InputConverter_test, passesFilters_one_nonpass_filter) {
  // retrieve mockable GenParticleFilter tool and point InputConverter to the same instance
  m_svc->setProperty("GenParticleFilters", "['ISFTesting::MockFilterTool/DummyFilter']");
  ASSERT_TRUE( m_svc->initialize().isSuccess() );
  ToolHandleArray<ISF::IGenParticleFilter>& genParticleFilters = getGenParticleFilters();
  ASSERT_EQ (genParticleFilters.size(), 1);
  MockFilterTool* filterTool = dynamic_cast<MockFilterTool*>(&*(genParticleFilters[0]));
  ASSERT_TRUE( filterTool );

  const HepMC::GenParticle genPart{};
  HepMC::FourVector mom(12.3, 45.6, 78.9, 0.12);
  HepMC::GenParticle genPart2(mom,
                              11, // pdg id (e-)
                              1 // status
                              );

  EXPECT_CALL(*filterTool, pass(genPart))
      .Times(1)
      .WillOnce(::testing::Return(false));

  ASSERT_FALSE( passesFilters(genPart) );
}


TEST_F(InputConverter_test, passesFilters_two_filters) {
  // retrieve mockable GenParticleFilter tool and point InputConverter to the same instance
  m_svc->setProperty("GenParticleFilters", "['ISFTesting::MockFilterTool/DummyFilterZ', 'ISFTesting::MockFilterTool/DummyFilterY']");
  ASSERT_TRUE( m_svc->initialize().isSuccess() );
  ToolHandleArray<ISF::IGenParticleFilter>& genParticleFilters = getGenParticleFilters();
  ASSERT_EQ (genParticleFilters.size(), 2);
  MockFilterTool* filterTool1 = dynamic_cast<MockFilterTool*>(&*(genParticleFilters[0]));
  ASSERT_TRUE( filterTool1 );
  MockFilterTool* filterTool2 = dynamic_cast<MockFilterTool*>(&*(genParticleFilters[1]));
  ASSERT_TRUE( filterTool2 );

  const HepMC::GenParticle genPart{};
  HepMC::FourVector mom(12.3, 45.6, 78.9, 0.12);
  HepMC::GenParticle genPart2(mom,
                              11, // pdg id (e-)
                              1 // status
                              );

  EXPECT_CALL(*filterTool1, pass(genPart))
      .Times(1)
      .WillOnce(::testing::Return(true));

  EXPECT_CALL(*filterTool2, pass(genPart))
      .Times(1)
      .WillOnce(::testing::Return(true));

  ASSERT_TRUE( passesFilters(genPart) );

}

} // <-- namespace ISFTesting

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest( &argc, argv );

  // gets stuck forever while trying to finalize boost ***REMOVED*** SGTools:
  //return RUN_ALL_TESTS();
  // skips proper finalization:
  std::quick_exit( RUN_ALL_TESTS() );
}
