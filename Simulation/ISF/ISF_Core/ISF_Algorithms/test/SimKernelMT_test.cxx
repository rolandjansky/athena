/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date October, 2016
 * @brief Tests for ISF::SimKernelMT.
 */

#undef NDEBUG

// Google Test and Google Mock
#include "gtest/gtest.h"
#include "gmock/gmock.h"

// Framework includes
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SmartIF.h"
#include "AthenaBaseComps/AthService.h"

// Tested AthAlgorithm
#include "../src/SimKernelMT.h"

namespace ISFTesting {

std::string mockInputConverterName = "ISFTesting::MockInputConverter/MyTestInputConverter";

// Athena Service to mock an InputConverter
class MockInputConverter : public AthService,
                           public ISF::IInputConverter {

public:
  MockInputConverter(const std::string& name, ISvcLocator* svc)
    : AthService(name,svc)
  { };

  virtual ~MockInputConverter() { };

  // needed to resolve ambiguity when assigning instance to SmartIF
  static const InterfaceID& interfaceID() { return IID_IInputConverter; }

  StatusCode  initialize() override { return StatusCode::SUCCESS;}
  StatusCode  finalize() override { return StatusCode::SUCCESS;}

  // needed to make this AthService implementation work with Athena
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override {
      if (IID_IInputConverter != riid) {
        // Interface is not directly available: try out a base class
        return AthService::queryInterface(riid, ppvInterface);
      }
      *ppvInterface = (IInputConverter*)this;
      addRef();
      return StatusCode::SUCCESS;
  }

  MOCK_CONST_METHOD3(convert, StatusCode(const McEventCollection&,
                                         ISF::ISFParticleContainer&,
                                         bool));
}; // MockInputConverter class

DECLARE_SERVICE_FACTORY( MockInputConverter )


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
    ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsType", "NONE" ).isSuccess() );

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

  void ReleaseSmartIFComponent(IInterface* comp) {
    size_t finalRefCount = 1; // keep one reference for the SmartIF destructor
    for (size_t refCount = comp->refCount(); refCount>finalRefCount; refCount--) {
      comp->release();
    }
  }

  // protected member variables for Core Gaudi components
  IAppMgrUI*             m_appMgr = nullptr;
  SmartIF<ISvcLocator>   m_svcLoc;
  SmartIF<ISvcManager>   m_svcMgr;
  SmartIF<IToolSvc>      m_toolSvc;
  SmartIF<IProperty>     m_propMgr;
};


// Test fixture specifically for SimKernelMT AthAlgorithm
class SimKernelMT_test: public ::testing::Test, public GaudiFixture {

protected:
  virtual void SetUp() override {
    // the tested AthAlgorithm
    m_alg = new ISF::SimKernelMT{"SimKernelMT", m_svcLoc};

    // retrieve mocked Athena components
    SmartIF<IService> svc = m_svcLoc->service(mockInputConverterName);

    m_mockInputConverter = dynamic_cast<MockInputConverter*>(svc.get());
    ASSERT_NE(nullptr, m_mockInputConverter);

    ASSERT_TRUE( m_mockInputConverter->setProperties().isSuccess() );
    ASSERT_TRUE( m_mockInputConverter->configure().isSuccess() );
  }

  virtual void TearDown() override {
    // release tested AthAlgorithm
    delete m_alg;

    // release the mock Athena components
    m_svcMgr->removeService(m_mockInputConverter);
    ASSERT_TRUE( m_mockInputConverter->finalize().isSuccess() );
    ASSERT_TRUE( m_mockInputConverter->terminate().isSuccess() );
    ReleaseSmartIFComponent(m_mockInputConverter);
    delete m_mockInputConverter;
  }

  // the tested AthAlgorithm
  ISF::SimKernelMT* m_alg;

  // mocked Athena components
  ISFTesting::MockInputConverter* m_mockInputConverter;

};  // SimKernelMT_test fixture


TEST_F(SimKernelMT_test, unsetInputOutputCollections_expectInitializeFailure) {
  ASSERT_TRUE( m_alg->initialize().isFailure() );
}


TEST_F(SimKernelMT_test, unsetInputCollection_expectInitializeFailure) {
  m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection");
  m_alg->setProperty("InputConverter", mockInputConverterName);

  ASSERT_TRUE( m_alg->initialize().isFailure() );
}


TEST_F(SimKernelMT_test, unsetOutputCollection_expectInitializeFailure) {
  m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection");
  m_alg->setProperty("InputConverter", mockInputConverterName);

  ASSERT_TRUE( m_alg->initialize().isFailure() );
}


TEST_F(SimKernelMT_test, unsetInputConverter_expectInitializeFailure) {
  m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection");
  m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection");

  ASSERT_TRUE( m_alg->initialize().isFailure() );
}


TEST_F(SimKernelMT_test, allPropertiesSet_expectInitializeSuccess) {
  m_alg->setProperty("InputEvgenCollection", "SomeRandomInputCollectionKey");
  m_alg->setProperty("OutputTruthCollection", "SomeRandomOutputCollectionKey");
  m_alg->setProperty("InputConverter", mockInputConverterName);

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
}


TEST_F(SimKernelMT_test, nonexistingInputOutputCollections_expectFailure) {
  m_alg->setProperty("InputEvgenCollection", "DoesntExistInputCollection");
  m_alg->setProperty("OutputTruthCollection", "DoesntExistOutputCollection");
  m_alg->setProperty("InputConverter", mockInputConverterName);

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  ASSERT_TRUE( m_alg->execute().isFailure() );
}


TEST_F(SimKernelMT_test, nonexistingOutputCollection_expectCreationOfOutputCollection) {
  m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection");
  m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection");
  m_alg->setProperty("InputConverter", mockInputConverterName);

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

  m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection");
  m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection");
  m_alg->setProperty("InputConverter", mockInputConverterName);
  EXPECT_TRUE( m_alg->initialize().isSuccess() );

  ASSERT_TRUE( m_alg->execute().isSuccess() );
}

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

  m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection");
  m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection");
  m_alg->setProperty("InputConverter", mockInputConverterName);
  EXPECT_TRUE( m_alg->initialize().isSuccess() );

  EXPECT_CALL(*m_mockInputConverter, convert(ContainsOneGenEventEq(*genEvent),
                                             ::testing::_,
                                             ::testing::_))
      .WillOnce(::testing::Return(StatusCode::SUCCESS));

  ASSERT_TRUE( m_alg->execute().isSuccess() );
}


} // namespace ISFTesting


int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  //return RUN_ALL_TESTS();
  // if the above gets stuck forever while trying to finalize Boost stuff
  // inside SGTools, try to use that:
  //  skips proper finalization:
  std::quick_exit( RUN_ALL_TESTS() );
}
