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

// ATLAS C++
#include "CxxUtils/make_unique.h"

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

  virtual ~MockInputConverter() {};

  // needed to resolve ambiguity when assigning instance to SmartIF
  static const InterfaceID& interfaceID() { return IID_IInputConverter; }

  StatusCode  initialize() override { return StatusCode::SUCCESS;}
  StatusCode  finalize() override { return StatusCode::SUCCESS;}

  // needed to make this AthService implementation work with Athena
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override {
      if (IID_IInputConverter == riid ) {
        *ppvInterface = (IInputConverter*)this;
      } else  {
        // Interface is not directly available: try out a base class
        return Service::queryInterface(riid, ppvInterface);
      }
      addRef();
      return StatusCode::SUCCESS;
  }

  StatusCode convert(const McEventCollection&, ISF::ISFParticleContainer&, bool) const override { return StatusCode::SUCCESS; }
  //MOCK_CONST_METHOD3(convert, StatusCode(const McEventCollection&,
  //                                       ISF::ISFParticleContainer&,
  //                                       bool));
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

    // retrieve mock athena components
    //m_mockInputConverter = m_svcLoc->service("ISF::InputConverter/AInputConverter");
    //m_mockInputConverter = m_svcLoc->service("ISF::InputConverter/AInputConverter");
    //m_mockInputConverter = m_svcLoc->service(mockInputConverterName);
//    m_mockInputConverter = m_svcLoc->service(mockInputConverterName);
//    m_mockInputConverter = m_svcLoc->service(mockInputConverterName);
//    m_mockInputConverter->addRef();
//    SmartIF<IService> svc = m_svcLoc->service(mockInputConverterName);
//    svc.addRef();
//    m_mockInputConverter = svc;
    //m_mockInputConverter = dynamic_cast<MockInputConverter*>(svc.get());
    //std::cout << "out=" << m_mockInputConverter << std::endl;
//    ASSERT_TRUE( m_mockInputConverter.isValid() );
//    ASSERT_TRUE( m_mockInputConverter->setProperties().isSuccess() );
//    ASSERT_TRUE( m_mockInputConverter->configure().isSuccess() );
  }

  virtual void TearDown() override {
    // release tested AthAlgorithm
    delete m_alg;

    // release the mock athena components (if it's there)
    //if (m_mockInputConverter.isValid()) {
    //  m_svcMgr->removeService(m_mockInputConverter);
    //  ASSERT_TRUE( m_mockInputConverter->finalize().isSuccess() );
    //  ASSERT_TRUE( m_mockInputConverter->terminate().isSuccess() );
    //  ReleaseSmartIFComponent(m_mockInputConverter);
    //}
  }

  // the tested AthAlgorithm
  ISF::SimKernelMT* m_alg;

  // mocked Athena components
  SmartIF<ISFTesting::MockInputConverter> m_mockInputConverter;
  //ISFTesting::MockInputConverter* m_mockInputConverter;

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

  auto inputEvgen = CxxUtils::make_unique<McEventCollection>();
  SG::WriteHandle<McEventCollection> testInputEvgenHandle{"testInputEvgenCollection"};
  testInputEvgenHandle.record( std::move(inputEvgen) );

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  ASSERT_TRUE( m_alg->execute().isSuccess() );
  SG::ReadHandle<McEventCollection> testOutputTruthHandle("testOutputTruthCollection");
  ASSERT_TRUE( testOutputTruthHandle.isValid() );
}

//TEST_F(SimKernelMT_test, filledInputCollection_expectFullConversion) {
//  auto inputEvgen = CxxUtils::make_unique<McEventCollection>();
//  auto* genEvent = new HepMC::GenEvent{};
//
//  inputEvgen->push_back(genEvent);
//  SG::WriteHandle<McEventCollection> inputEvgenHandle{"testInputEvgenCollection"};
//  inputEvgenHandle.record( std::move(inputEvgen) );
//
//  m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection");
//  m_alg->setProperty("OutputTruthCollection", "testOutputTruthCollection");
//  m_alg->setProperty("InputConverter", mockInputConverterName);
//  EXPECT_TRUE( m_alg->initialize().isSuccess() );
//
//  //EXPECT_CALL(*m_mockInputConverter, convert(::testing::_, ::testing::_, ::testing::_))
//  //    .Times(1)
//  //    .WillOnce(::testing::Return(StatusCode::SUCCESS));
//
//  ASSERT_TRUE( m_alg->execute().isSuccess() );
//}
//
//  //HepMC::GenParticle* genPart = new HepMC::GenParticle{};
//  //HepMC::FourVector mom{12.3, 45.6, 78.9, 0.12};
//  //HepMC::GenParticle* genPart2 = new HepMC::GenParticle{mom,
//  //                                                      11, // pdg id (e-)
//  //                                                      1 // status
//  //                                                      };

//TEST_F(SimKernelMT_test, emptyInputEvgenCollection_expectSuccess) {
//  auto inputEvgen = CxxUtils::make_unique<McEventCollection>();
//  SG::WriteHandle<McEventCollection> inputEvgenHandle{"testInputEvgenCollection"};
//  inputEvgenHandle.record( std::move(inputEvgen) );
//
//  m_alg->setProperty("InputEvgenCollection", "testInputEvgenCollection");
//  ASSERT_TRUE( m_alg->execute().isSuccess() );
//}


} // namespace ISFTesting


int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
  // if the above gets stuck forever while trying to finalize Boost stuff
  // inside SGTools, try to use that:
  //  skips proper finalization:
  //std::quick_exit( RUN_ALL_TESTS() );
}
