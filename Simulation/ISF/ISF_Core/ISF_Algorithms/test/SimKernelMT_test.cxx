/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date October, 2016
 * @brief Tests for ISF::SimKernelMT.
 */

#undef NDEBUG

// Tested AthAlgorithm
#include "../src/SimKernelMT.h"

// ISF includes
#include "ISF_Event/ISFParticle.h"
#include "ISF_Interfaces/BaseSimulationSvc.h"

// Amg classes
#include "GeoPrimitives/GeoPrimitives.h"

// Framework includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

// Google Test and Google Mock
#include "gtest/gtest.h"
#include "gmock/gmock.h"


namespace ISFTesting {

  // Globally used services and tools
  //
  const std::string particleKillerSimulatorToolName = "ISF::ParticleKillerSimTool/ISF__ParticleKillerSimTool";
  const std::string fullInputConverter = "ISF::InputConverter/InputConverter";

  // Athena Service to mock an InputConverter
  //
  const std::string mockInputConverterName = "ISFTesting::MockInputConverter/MyTestInputConverter";

  class MockInputConverter : public extends<AthService, ISF::IInputConverter> {

  public:
    MockInputConverter(const std::string& name, ISvcLocator* svc)
      : base_class(name,svc)
    {  };
    virtual ~MockInputConverter() { };

    MOCK_METHOD0(finalize, StatusCode());
    MOCK_CONST_METHOD3(convert, StatusCode(const McEventCollection&,
                                           ISF::ISFParticleContainer&,
                                           bool));
    MOCK_CONST_METHOD3(convertHepMCToG4Event, StatusCode(McEventCollection&,
                                                         G4Event*&,
                                                         bool));
    MOCK_CONST_METHOD2(ISF_to_G4Event, G4Event*(const std::vector<const ISF::ISFParticle*>&,
                                                HepMC::GenEvent*));

  }; // MockInputConverter class

  DECLARE_COMPONENT( MockInputConverter )


  // Athena Service to mock a GeoIDSvc
  //
  const std::string mockGeoIDSvcName = "ISFTesting::MockGeoIDSvc/MyMockGeoIDSvc";

  class MockGeoIDSvc : public extends<AthService, ISF::IGeoIDSvc> {

  public:
    MockGeoIDSvc(const std::string& name, ISvcLocator* svc)
      : base_class(name,svc)
    {  };
    MOCK_METHOD0(initialize, StatusCode());
    MOCK_METHOD0(finalize, StatusCode());

    MOCK_CONST_METHOD2(inside, ISF::InsideType(const Amg::Vector3D &, AtlasDetDescr::AtlasRegion));
    MOCK_CONST_METHOD1(identifyGeoID, AtlasDetDescr::AtlasRegion(const Amg::Vector3D &));
    MOCK_CONST_METHOD2(identifyNextGeoID, AtlasDetDescr::AtlasRegion(const Amg::Vector3D &, const Amg::Vector3D &));
  };

  DECLARE_COMPONENT( MockGeoIDSvc ) // MockGeoIDSvc class


// Athena Tool to mock a SimulationSelector
//
const std::string mockSimulationSelectorName = "ISFTesting::MockSimulationSelector/MyTestSimulationSelector";

class MockSimulationSelector : public extends<AthAlgTool, ISF::ISimulationSelector> { //public ISF::BaseSimulationSelector {

public:
  MockSimulationSelector(const std::string& type, const std::string& name, const IInterface* svclocator)
    : base_class(type, name, svclocator)
  {
  };
  virtual ~MockSimulationSelector() { };

  MOCK_METHOD0(finalize, StatusCode());
  MOCK_CONST_METHOD1(passSelectorCuts, bool(const ISF::ISFParticle&));

  // dummy methods implementing in pure virtual interface methods (to make class non-abstract)
  StatusCode initialize() {
    ATH_MSG_INFO ("initializing MockSimulationSelector: " << name());
    return StatusCode::SUCCESS; };
  ServiceHandle<ISF::ISimulationSvc>* simulator() { return nullptr; };
  bool isDynamic() { return false; };
  ISF::SimSvcID simSvcID() { return 0; };
  virtual ISF::SimulationFlavor simFlavor() { return ISF::Fatras; }
  void initializeSelector() { };
  void beginEvent() { };
  void endEvent() { };
  void update(const ISF::ISFParticle& ) { };
  bool selfSelect(const ISF::ISFParticle& particle) { return passSelectorCuts(particle); };

}; // MockSimulationSelector class

DECLARE_COMPONENT( MockSimulationSelector )


// Athena Tool to mock a SimulatorTool
//
const std::string mockSimulatorToolName = "ISFTesting::MockSimulatorTool/MyTestSimulatorTool";

class MockSimulatorTool : public extends<AthAlgTool, ISF::ISimulatorTool> {

public:
  MockSimulatorTool(const std::string& type, const std::string& name, const IInterface* svclocator)
    : base_class(type, name, svclocator)
  {
  };
  virtual ~MockSimulatorTool() { };

  MOCK_METHOD0(finalize, StatusCode());
  MOCK_METHOD0(setupEvent, StatusCode());
  MOCK_METHOD3(simulate, StatusCode(const ISF::ISFParticle&, ISF::ISFParticleContainer&, McEventCollection*));
  MOCK_METHOD3(simulateVector, StatusCode(const ISF::ConstISFParticleVector&, ISF::ISFParticleContainer&, McEventCollection*));
  MOCK_METHOD0(releaseEvent, StatusCode());
  MOCK_CONST_METHOD1(bid, int(const ISF::ISFParticle&));

  // dummy methods implementing in pure virtual interface methods (to make class non-abstract)
  virtual StatusCode initialize() override {
    ATH_MSG_INFO ("initializing MockSimulatorTool: " << name());
    return StatusCode::SUCCESS;
  };
  virtual StatusCode setupEventST() override { return StatusCode::FAILURE; };
  virtual StatusCode releaseEventST() override { return StatusCode::FAILURE; };
  virtual ISF::SimulationFlavor simFlavor() const override { return ISF::Fatras; };

}; // MockSimulatorTool

DECLARE_COMPONENT( MockSimulatorTool )


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
    ASSERT_TRUE( m_propMgr->setProperty( "EvtSel",         "NONE" ).isSuccess() );
    ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsType", "FILE" ).isSuccess() );
    ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsPath", "SimKernelMT_test.txt" ).isSuccess() );

    m_toolSvc = m_svcLoc->service("ToolSvc");
    ASSERT_TRUE( m_toolSvc.isValid() );

    ASSERT_TRUE( m_appMgr->configure().isSuccess() );
    ASSERT_TRUE( m_appMgr->initialize().isSuccess() );

    m_jobOptionsSvc = m_svcLoc->service("JobOptionsSvc");
    ASSERT_TRUE( m_jobOptionsSvc.isValid() );
  }

  void TearDownGaudi() {
    ASSERT_TRUE( m_svcMgr->finalize().isSuccess() );
    ASSERT_TRUE( m_appMgr->finalize().isSuccess() );
    ASSERT_TRUE( m_appMgr->terminate().isSuccess() );
    m_svcLoc->release();
    m_svcMgr->release();
    Gaudi::setInstance( static_cast<IAppMgrUI*>(nullptr) );
  }

  // protected member variables for Core Gaudi components
  IAppMgrUI*               m_appMgr = nullptr;
  SmartIF<ISvcLocator>     m_svcLoc;
  SmartIF<ISvcManager>     m_svcMgr;
  SmartIF<IJobOptionsSvc>  m_jobOptionsSvc;
  SmartIF<IToolSvc>        m_toolSvc;
  SmartIF<IProperty>       m_propMgr;
};


  // Test fixture specifically for SimKernelMT AthAlgorithm
  class SimKernelMT_test: public ::testing::Test, public GaudiFixture {

  protected:
    virtual void SetUp() override {
      // the tested AthAlgorithm
      m_alg = new ISF::SimKernelMT{"SimKernelMT", m_svcLoc};
      EXPECT_TRUE( m_alg->setProperty("ParticleKillerTool", particleKillerSimulatorToolName).isSuccess() );
      EXPECT_TRUE( m_alg->setProperty("GeoIDSvc", mockGeoIDSvcName).isSuccess() );

      // retrieve mocked Athena components
      m_mockGeoIDSvc = retrieveService<MockGeoIDSvc>(mockGeoIDSvcName);
      m_mockInputConverter = retrieveService<MockInputConverter>(mockInputConverterName);
      m_mockSimulatorTool = retrieveTool<MockSimulatorTool>(mockSimulatorToolName);
      m_mockSimulationSelector = retrieveTool<MockSimulationSelector>(mockSimulationSelectorName);
    }

    virtual void TearDown() override {
      // let the Gaudi ServiceManager finalize all services
      ASSERT_TRUE( m_svcMgr->finalize().isSuccess() );

      // release tested AthAlgorithm
      delete m_alg;
      // release various service instances
      delete m_mockGeoIDSvc;
      delete m_mockInputConverter;
    }

    template<typename T>
    T* retrieveService(const std::string& name) {
      T* service = nullptr;
      SmartIF<IService>& serviceSmartPointer = m_svcLoc->service(name);
      service = dynamic_cast<T*>(serviceSmartPointer.get());
      EXPECT_NE(nullptr, service);
      if(!service) {
        return nullptr;
      }
      EXPECT_TRUE( service->setProperties().isSuccess() );
      EXPECT_TRUE( service->configure().isSuccess() );
      EXPECT_TRUE( m_svcMgr->addService(service).isSuccess() );
      // assert that finalize() gets called once per test case
      EXPECT_CALL( *service, finalize() )
        .Times(1)
        .WillOnce(::testing::Return(StatusCode::SUCCESS));

      return service;
    }

    template<typename T>
    T* retrieveTool(const std::string& name) {
      IAlgTool* toolInterface = nullptr;
      EXPECT_TRUE( m_toolSvc->retrieveTool(name, toolInterface).isSuccess() );
      EXPECT_NE(nullptr, toolInterface);

      T* tool = dynamic_cast<T*>(toolInterface);
      EXPECT_NE(nullptr, tool);
      if(!tool) {
        return nullptr;
      }

      EXPECT_TRUE( tool->setProperties().isSuccess() );
      EXPECT_TRUE( tool->configure().isSuccess() );

      // assert that finalize() gets called once per test case
      EXPECT_CALL( *tool, finalize() )
        .Times(1)
        .WillOnce(::testing::Return(StatusCode::SUCCESS));

      return tool;
    }

    void setEmptyInputOutputCollections() {
      auto inputEvgen = std::make_unique<McEventCollection>();
      SG::WriteHandle<McEventCollection> inputEvgenHandle{"emptyTestInputEvgenCollection"};
      inputEvgenHandle.record( std::move(inputEvgen) );

      EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "emptyTestInputEvgenCollection").isSuccess() );
      EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection").isSuccess() );
      EXPECT_TRUE( m_alg->setProperty("InputConverter", mockInputConverterName).isSuccess() );
    }

    // wrappers for private methods
    // NB: This works because CollectionMerger_test is a friend of the tested
    //     CollectionMerger class
    //
    template<typename ...Args>
    ISF::ISimulatorTool& identifySimulator(Args&... args) {
      return m_alg->identifySimulator(std::forward<Args>(args)...);
    }

    // the tested AthAlgorithm
    ISF::SimKernelMT* m_alg;

    // mocked Athena components
    ISFTesting::MockGeoIDSvc* m_mockGeoIDSvc = nullptr;
    ISFTesting::MockInputConverter* m_mockInputConverter = nullptr;
    ISFTesting::MockSimulatorTool* m_mockSimulatorTool = nullptr;
    ISFTesting::MockSimulationSelector* m_mockSimulationSelector = nullptr;

  };  // SimKernelMT_test fixture


  // checks if the two given HepMC::GenEvent instances are equal.
  // returns true if they are equal, false otherwise
  bool GenEventsEq(const HepMC::GenEvent& a, const HepMC::GenEvent& b) {
    HepMC::GenEvent::vertex_const_iterator aVertexIterator = a.vertices_begin();
    HepMC::GenEvent::vertex_const_iterator bVertexIterator = b.vertices_begin();
    const auto& aVertexIteratorEnd = a.vertices_end();
    const auto& bVertexIteratorEnd = b.vertices_end();

    bool eventsAreEqual = true;

    do {
      bool endReached = (aVertexIterator == aVertexIteratorEnd) ||
        (bVertexIterator == bVertexIteratorEnd);
      if (endReached) {
        break;
      }

      eventsAreEqual = **aVertexIterator == **bVertexIterator;

      ++aVertexIterator;
      ++bVertexIterator;
    } while(eventsAreEqual);

    return eventsAreEqual;
  }


  // matcher to check if the given McEventCollection contains one HepMC::GenEvent that's
  // equal to the given expectedGenEvent
  MATCHER_P(ContainsOneGenEventEq, expectedGenEvent, "is equal to expected HepMC::GenEvent") {
    const auto& actualMcEventCollection = arg;

    size_t expectedCollectionSize = 1;
    auto actualCollectionSize = actualMcEventCollection.size();
    if (expectedCollectionSize!=actualCollectionSize) {
      return false;
    }

    size_t firstEventNum = 0;
    const auto* actualGenEvent = actualMcEventCollection[firstEventNum];
    return GenEventsEq(expectedGenEvent, *actualGenEvent);
  }


  TEST_F(SimKernelMT_test, unsetInputOutputCollections_expectInitializeFailure) {
    ASSERT_TRUE( m_alg->initialize().isFailure() );
  }


  TEST_F(SimKernelMT_test, unsetInputCollection_expectInitializeFailure) {
    EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("InputConverter", mockInputConverterName).isSuccess() );

    ASSERT_TRUE( m_alg->initialize().isFailure() );
  }


  TEST_F(SimKernelMT_test, unsetOutputCollection_expectInitializeFailure) {
    EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("InputConverter", mockInputConverterName).isSuccess() );

    ASSERT_TRUE( m_alg->initialize().isFailure() );
  }


  TEST_F(SimKernelMT_test, unsetInputConverter_expectInitializeFailure) {
    EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection").isSuccess() );

    ASSERT_TRUE( m_alg->initialize().isFailure() );
  }


  TEST_F(SimKernelMT_test, allPropertiesSet_expectInitializeSuccess) {
    EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "SomeRandomInputCollectionKey").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "SomeRandomOutputCollectionKey").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("InputConverter", mockInputConverterName).isSuccess() );

    ASSERT_TRUE( m_alg->initialize().isSuccess() );
  }


  TEST_F(SimKernelMT_test, nonexistingInputOutputCollections_expectFailure) {
    EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "DoesntExistInputCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "DoesntExistOutputCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("InputConverter", mockInputConverterName).isSuccess() );

    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isFailure() );
  }


  TEST_F(SimKernelMT_test, nonexistingOutputCollection_expectCreationOfOutputCollection) {
    EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("InputConverter", mockInputConverterName).isSuccess() );

    auto inputEvgen = std::make_unique<McEventCollection>();
    SG::WriteHandle<McEventCollection> testInputEvgenHandle{"testInputEvgenCollection"};
    testInputEvgenHandle.record( std::move(inputEvgen) );

    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    SG::ReadHandle<McEventCollection> testOutputTruthHandle("testOutputTruthCollection");
    ASSERT_TRUE( testOutputTruthHandle.isValid() );
  }


  TEST_F(SimKernelMT_test, emptyInputCollection_expectSuccess) {
    auto inputEvgen = std::make_unique<McEventCollection>();
    auto* genEvent = new HepMC::GenEvent{};

    inputEvgen->push_back(genEvent);
    SG::WriteHandle<McEventCollection> inputEvgenHandle{"testInputEvgenCollection"};
    inputEvgenHandle.record( std::move(inputEvgen) );

    EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("InputConverter", mockInputConverterName).isSuccess() );
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    ASSERT_TRUE( m_alg->execute().isSuccess() );
  }


  TEST_F(SimKernelMT_test, filledInputCollection_expectFullConversion) {
    auto* genEvent = new HepMC::GenEvent{};
    HepMC::GenParticle* genPart = new HepMC::GenParticle{};
    HepMC::FourVector mom{12.3, 45.6, 78.9, 0.12};
    HepMC::GenParticle* genPart2 = new HepMC::GenParticle{mom,
                                                          11,  // pdg id (e-)
                                                          1  // status
    };
    auto* genVertex = new HepMC::GenVertex{};
    genVertex->add_particle_out(genPart);
    genVertex->add_particle_out(genPart2);
    genEvent->add_vertex(genVertex);

    auto inputEvgen = std::make_unique<McEventCollection>();
    inputEvgen->push_back(genEvent);
    SG::WriteHandle<McEventCollection> inputEvgenHandle{"testInputEvgenCollection"};
    inputEvgenHandle.record( std::move(inputEvgen) );

    EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("InputConverter", mockInputConverterName).isSuccess() );
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    ASSERT_NE( m_mockInputConverter, nullptr );
    EXPECT_CALL( *m_mockInputConverter, convert(ContainsOneGenEventEq(*genEvent),
                                                ::testing::_,
                                                ::testing::_) )
      .WillOnce(::testing::Return(StatusCode::SUCCESS));

    ASSERT_TRUE( m_alg->execute().isSuccess() );
  }


  TEST_F(SimKernelMT_test, emptyInputEvgenCollection_expectInitializeSuccess) {
    this->setEmptyInputOutputCollections();
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    ASSERT_TRUE( m_alg->execute().isSuccess() );
  }


  TEST_F(SimKernelMT_test, specifyASimulationTool_expectInitializeSuccess) {
    EXPECT_TRUE( m_alg->setProperty("SimulationTools", "['"+mockSimulatorToolName+"']").isSuccess() );
    this->setEmptyInputOutputCollections();
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    ASSERT_TRUE( m_alg->execute().isSuccess() );
  }


  TEST_F(SimKernelMT_test, emptySimulationSelectors_expectInitializeSuccess) {
    EXPECT_TRUE( m_alg->setProperty("BeamPipeSimulationSelectors", "[]").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("IDSimulationSelectors", "[]").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("CaloSimulationSelectors", "[]").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("MSSimulationSelectors", "[]").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("CavernSimulationSelectors", "[]").isSuccess() );
    this->setEmptyInputOutputCollections();

    ASSERT_TRUE( m_alg->initialize().isSuccess() );
  }


  TEST_F(SimKernelMT_test, filledSimulationSelectors_expectInitializeFailure) {
    EXPECT_TRUE( m_alg->setProperty("BeamPipeSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() ); //mockSimulationSelectorName);
    EXPECT_TRUE( m_alg->setProperty("IDSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("CaloSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("MSSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("CavernSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    this->setEmptyInputOutputCollections();

    ASSERT_TRUE( m_alg->initialize().isFailure() );
  }


  TEST_F(SimKernelMT_test, filledSimulationSelectors_expectInitializeSuccess) {
    EXPECT_TRUE( m_alg->setProperty("ParticleKillerTool", mockSimulatorToolName).isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("BeamPipeSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() ); //mockSimulationSelectorName);
    EXPECT_TRUE( m_alg->setProperty("IDSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("CaloSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("MSSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("CavernSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    this->setEmptyInputOutputCollections();

    ASSERT_TRUE( m_alg->initialize().isSuccess() );
  }


  TEST_F(SimKernelMT_test, identifySimulator_particleInsideInnerDetectorAndInnerDetectorSimulationSelectorAcceptingParticle_expectInnerDetectorSimulatorReturned) {
    EXPECT_TRUE( m_alg->setProperty("SimulationTools", "['"+mockSimulatorToolName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("IDSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    this->setEmptyInputOutputCollections();
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    Amg::Vector3D position(0., 1., 0.);  // close to the origin
    Amg::Vector3D momentum(1., 2., 3.);
    ISF::DetRegionSvcIDPair particleOrigin(AtlasDetDescr::fAtlasID, ISF::fUndefinedSimID);
    const ISF::ISFParticle particle(position,
                                    momentum,
                                    123.*Gaudi::Units::GeV,  // mass
                                    -1.,  // charge
                                    11,  // PDG code
                                    0.2*Gaudi::Units::second,  // time
                                    particleOrigin,
                                    123  // BCID
                                    );

    ASSERT_NE( m_mockSimulationSelector, nullptr );
    EXPECT_CALL( *m_mockSimulationSelector, passSelectorCuts(particle) )
      .WillOnce(::testing::Return(true));

    const auto* actualSimulatorToolPtr = &this->identifySimulator(particle);

    const auto* expectedSimulatorToolPtr = m_mockSimulatorTool;
    ASSERT_EQ(expectedSimulatorToolPtr, actualSimulatorToolPtr);
  }


  TEST_F(SimKernelMT_test, identifySimulator_particleInsideInnerDetectorAndInnerDetectorSimulationSelectorNotAcceptingParticle_expectParticleKillerSimulatorReturned) {
    EXPECT_TRUE( m_alg->setProperty("SimulationTools", "['"+mockSimulatorToolName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("IDSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    this->setEmptyInputOutputCollections();
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    Amg::Vector3D position(0., 1., 0.);  // close to the origin
    Amg::Vector3D momentum(1., 2., 3.);
    ISF::DetRegionSvcIDPair particleOrigin(AtlasDetDescr::fAtlasID, ISF::fUndefinedSimID);
    const ISF::ISFParticle particle(position,
                                    momentum,
                                    123.*Gaudi::Units::GeV,  // mass
                                    -1.,  // charge
                                    11,  // PDG code
                                    0.2*Gaudi::Units::second,  // time
                                    particleOrigin,
                                    123  // BCID
                                    );

    ASSERT_NE( m_mockSimulationSelector, nullptr );
    EXPECT_CALL( *m_mockSimulationSelector, passSelectorCuts(particle) )
      .WillOnce(::testing::Return(false));

    const auto* actualSimulatorToolPtr = &this->identifySimulator(particle);
    ToolHandle<ISF::ISimulatorTool> particleKillerSimulatorTool(particleKillerSimulatorToolName);
    const ISF::ISimulatorTool* expectedSimulatorToolPtr = &*particleKillerSimulatorTool;

    ASSERT_EQ(expectedSimulatorToolPtr, actualSimulatorToolPtr);
  }


  TEST_F(SimKernelMT_test, identifySimulator_particleInsideCaloAndOnlyInnerDetectorSimulationSelectorPresent_expectSimulationSelectorNotCalledAndParticleKillerSimulatorReturned) {
    EXPECT_TRUE( m_alg->setProperty("SimulationTools", "['"+mockSimulatorToolName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("IDSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    this->setEmptyInputOutputCollections();
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    Amg::Vector3D position(0., 1., 0.);  // close to the origin
    Amg::Vector3D momentum(1., 2., 3.);
    ISF::DetRegionSvcIDPair particleOrigin(AtlasDetDescr::fAtlasCalo, ISF::fUndefinedSimID);
    const ISF::ISFParticle particle(position,
                                    momentum,
                                    123.*Gaudi::Units::GeV,  // mass
                                    -1.,  // charge
                                    11,  // PDG code
                                    0.2*Gaudi::Units::second,  // time
                                    particleOrigin,
                                    123  // BCID
                                    );

    ASSERT_NE( m_mockSimulationSelector, nullptr );
    EXPECT_CALL( *m_mockSimulationSelector, passSelectorCuts(::testing::_) )
      .Times(0);

    const auto* actualSimulatorToolPtr = &this->identifySimulator(particle);
    ToolHandle<ISF::ISimulatorTool> particleKillerSimulatorTool(particleKillerSimulatorToolName);
    const ISF::ISimulatorTool* expectedSimulatorToolPtr = &*particleKillerSimulatorTool;

    ASSERT_EQ(expectedSimulatorToolPtr, actualSimulatorToolPtr);
  }


  TEST_F(SimKernelMT_test, identifySimulator_particleInsideCaloAndCaloSimulationSelectorSelectingParticle_expectSimulatorReturned) {
    EXPECT_TRUE( m_alg->setProperty("SimulationTools", "['"+mockSimulatorToolName+"']").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("CaloSimulationSelectors", "['"+mockSimulationSelectorName+"']").isSuccess() );
    this->setEmptyInputOutputCollections();
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    Amg::Vector3D position(0., 1., 0.);  // close to the origin
    Amg::Vector3D momentum(1., 2., 3.);
    ISF::DetRegionSvcIDPair particleOrigin(AtlasDetDescr::fAtlasCalo, ISF::fUndefinedSimID);
    const ISF::ISFParticle particle(position,
                                    momentum,
                                    123.*Gaudi::Units::GeV,  // mass
                                    -1.,  // charge
                                    11,  // PDG code
                                    0.2*Gaudi::Units::second,  // time
                                    particleOrigin,
                                    123  // BCID
                                    );

    ASSERT_NE( m_mockSimulationSelector, nullptr );
    EXPECT_CALL( *m_mockSimulationSelector, passSelectorCuts(particle) )
      .WillOnce(::testing::Return(true));

    const auto* actualSimulatorToolPtr = &this->identifySimulator(particle);
    const auto* expectedSimulatorToolPtr = m_mockSimulatorTool;

    ASSERT_EQ(expectedSimulatorToolPtr, actualSimulatorToolPtr);
  }


  TEST_F(SimKernelMT_test, filledInputCollectionAndEmptySimulationTools_expectConvertedParticleSentToParticleKiller) {
    auto* genEvent = new HepMC::GenEvent{};
    HepMC::FourVector mom{12.3, 45.6, 78.9, 1234.5};
    HepMC::GenParticle* genPart = new HepMC::GenParticle{mom,
                                                         11,  // pdg id (e-)
                                                         1  // status
    };
    HepMC::FourVector pos{9., 8., 7., 678.9};
    auto* genVertex = new HepMC::GenVertex{pos};
    genVertex->add_particle_out(genPart);
    genEvent->add_vertex(genVertex);

    auto inputEvgen = std::make_unique<McEventCollection>();
    inputEvgen->push_back(genEvent);
    SG::WriteHandle<McEventCollection> inputEvgenHandle{"testInputEvgenCollection"};
    inputEvgenHandle.record( std::move(inputEvgen) );

    EXPECT_TRUE( m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection").isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("ParticleKillerTool", mockSimulatorToolName).isSuccess() );
    EXPECT_TRUE( m_alg->setProperty("InputConverter", fullInputConverter).isSuccess() );
    EXPECT_TRUE( m_alg->initialize().isSuccess() );

    Amg::Vector3D position(9., 8., 7.);
    Amg::Vector3D momentum(12.3, 45.6, 78.9);
    ISF::DetRegionSvcIDPair particleOrigin(AtlasDetDescr::fUndefinedAtlasRegion, ISF::fEventGeneratorSimID);
    ISF::TruthBinding* truthBinding = new ISF::TruthBinding(genPart);

    ISF::ISFParticle convertedParticle(position,
                                       momentum,
                                       0.510999*Gaudi::Units::MeV,  // e- mass
                                       -1.,  // charge
                                       11,  // e- PDG code
                                       678.9/Gaudi::Units::c_light*Gaudi::Units::ns,  // time
                                       particleOrigin,
                                       0,  // BCID
                                       10001,  // barcode
                                     truthBinding
                                       );

    ASSERT_NE( m_mockSimulatorTool, nullptr );
    EXPECT_CALL( *m_mockSimulatorTool, simulateVector(::testing::_,::testing::_,::testing::_) )
      .Times(1)
      .WillOnce(::testing::Return(StatusCode::SUCCESS));

    ASSERT_NE( m_mockGeoIDSvc, nullptr );
    EXPECT_CALL( *m_mockGeoIDSvc, identifyNextGeoID(::testing::_,::testing::_) )
      .Times(1)
      .WillOnce(::testing::Return(AtlasDetDescr::fAtlasID));

    ASSERT_TRUE( m_alg->execute().isSuccess() );
  }


} // namespace ISFTesting


int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  //return RUN_ALL_TESTS();
  // if the above gets stuck forever while trying to finalize Boost stuff
  // inside SGTools, try to use the following:
  // skips proper finalization:
  std::quick_exit( RUN_ALL_TESTS() );
}
