/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar Ritsch <Elmar.Ritsch@cern.ch>
 * @date May, 2016
 * @brief Tests for ISF::CollectionMerger.
 */

#undef NDEBUG

// Framework
#include "TestTools/initGaudi.h"

// ATLAS C++
#include "CxxUtils/make_unique.h"

// Google Test
#include "gtest/gtest.h"
// Google Mock
// #include "gmock/gmock.h"

// Tested AthAlgorithm
#include "../src/CollectionMerger.h"



namespace ISFTesting {

// needed every time an AthAlgorithm, AthAlgTool or AthService is instantiated
ISvcLocator* g_svcLoc = nullptr;

// mock object for sensitive detector hits
class TestHits {
  public:
    TestHits(int value):m_value(value) { };
    int m_value;
};
// typedef for convenience
typedef AtlasHitsVector<ISFTesting::TestHits> TestHitCollection_t;


// global test environment takes care of setting up Gaudi
class GaudiEnvironment : public ::testing::Environment {
  protected:
    virtual void SetUp() override {
      Athena_test::initGaudi(ISFTesting::g_svcLoc);
    }
};

class CollectionMerger_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_alg = new ISF::CollectionMerger{"CollectionMerger", g_svcLoc};
      ASSERT_TRUE( m_alg->setProperties().isSuccess() );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
    }

    //
    // accessors for private methods
    // NB: This works because CollectionMerger_test is a friend of the tested
    //     CollectionMerger AthAlgorithm
    //
    template<typename... Args>
    StatusCode initializeVarHandleKey(Args&&... args) const {
      return m_alg->initializeVarHandleKey(std::forward<Args>(args)...);
    }

    template<typename... Args>
    StatusCode setupReadHandleKeyVector(Args&&... args) const {
      return m_alg->setupReadHandleKeyVector(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void mergeCollections(Args&&... args) const {
      m_alg->mergeCollections(std::forward<Args>(args)...);
    }

    ISF::CollectionMerger* m_alg;

};  // CollectionMerger_test fixture


TEST_F(CollectionMerger_test, empty_alg_execute) {
  ASSERT_TRUE( m_alg->execute().isSuccess() );
}


TEST_F(CollectionMerger_test, initializeVarHandleKey) {
  SG::VarHandleKey testKey{ 123456789 /* clid */,
                            std::string("testKey"),
                            Gaudi::DataHandle::Reader };
  ASSERT_TRUE( initializeVarHandleKey(testKey).isSuccess() );
  ASSERT_TRUE( testKey.storeHandle().isSet() );
}


TEST_F(CollectionMerger_test, initializeVarHandleKey_emptyKey) {
  SG::VarHandleKey emptyTestKey{ 123456789 /* clid */,
                                 std::string(),
                                 Gaudi::DataHandle::Reader };
  ASSERT_TRUE( initializeVarHandleKey(emptyTestKey).isSuccess() );
  ASSERT_TRUE( !emptyTestKey.storeHandle().isSet() );
}


TEST_F(CollectionMerger_test, setupReadHandleKeyVector) {
  std::vector<std::string>                            sgKeys{};
  std::vector<SG::ReadHandleKey<TestHitCollection_t>> readHandles{};

  ASSERT_TRUE( setupReadHandleKeyVector(sgKeys, readHandles).isSuccess() );
  ASSERT_TRUE( readHandles.empty() );

  sgKeys.emplace_back( "testKeyA" );
  sgKeys.emplace_back( "testKeyB" );
  sgKeys.emplace_back( "testKeyC" );

  ASSERT_TRUE( setupReadHandleKeyVector(sgKeys, readHandles).isSuccess() );
  ASSERT_EQ( readHandles.size(), 3 );
  ASSERT_EQ( readHandles.at(0).key(), "testKeyA" );
  ASSERT_TRUE( readHandles.at(0).clid() );
  ASSERT_EQ( readHandles.at(0).mode(), Gaudi::DataHandle::Reader );
  ASSERT_TRUE( readHandles.at(0).storeHandle().isSet() );
  ASSERT_EQ( readHandles.at(1).key(), "testKeyB" );
  ASSERT_TRUE( readHandles.at(1).clid() );
  ASSERT_EQ( readHandles.at(1).mode(), Gaudi::DataHandle::Reader );
  ASSERT_TRUE( readHandles.at(1).storeHandle().isSet() );
  ASSERT_EQ( readHandles.at(2).key(), "testKeyC" );
  ASSERT_TRUE( readHandles.at(2).clid() );
  ASSERT_EQ( readHandles.at(2).mode(), Gaudi::DataHandle::Reader );
  ASSERT_TRUE( readHandles.at(2).storeHandle().isSet() );
}


TEST_F(CollectionMerger_test, setupReadHandleKeyVector_emptyKey) {
  std::vector<std::string>                            sgKeys{"testKeyA", std::string(), "testKeyC"};
  std::vector<SG::ReadHandleKey<TestHitCollection_t>> readHandles{};

  // isFailure because of the empty string in the sgKeys vector
  ASSERT_TRUE( setupReadHandleKeyVector(sgKeys, readHandles).isFailure() );
}


TEST_F(CollectionMerger_test, mergeCollections) {
  std::vector<SG::ReadHandleKey<TestHitCollection_t>> inputKeys{};
  SG::WriteHandleKey<TestHitCollection_t>             outputKey{"outputCollectionMergeTest"};
  ASSERT_TRUE( outputKey.initialize().isSuccess() );

  // should not fail, just does nothing
  mergeCollections(inputKeys, outputKey);

  // create dummy input collections containing dummy data
  auto                                 inputTestDataA = CxxUtils::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleA{"inputCollectionA"};
  inputTestDataHandleA.record( std::move(inputTestDataA) );
  inputTestDataHandleA->Emplace(1);
  inputTestDataHandleA->Emplace(20);
  inputTestDataHandleA->Emplace(5);

  auto                                 inputTestDataB = CxxUtils::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleB{"inputCollectionB"};
  inputTestDataHandleB.record( std::move(inputTestDataB) );
  inputTestDataHandleB->Emplace(50);
  inputTestDataHandleB->Emplace(1);

  auto                                 inputTestDataC = CxxUtils::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleC{"inputCollectionC"};
  inputTestDataHandleC.record( std::move(inputTestDataC) );
  inputTestDataHandleC->Emplace(20);
  inputTestDataHandleC->Emplace(5);
  inputTestDataHandleC->Emplace(1);

  // add inputCollections with test data to inputKeys for later merging
  // ordering A, C, B is on purpose to test for unintended alphabetic ordering
  inputKeys.emplace_back( "inputCollectionA" );
  inputKeys.emplace_back( "inputCollectionC" );
  inputKeys.emplace_back( "inputCollectionB" );
  ASSERT_TRUE( inputKeys.at(0).initialize().isSuccess() );
  ASSERT_TRUE( inputKeys.at(1).initialize().isSuccess() );
  ASSERT_TRUE( inputKeys.at(2).initialize().isSuccess() );

  // access the inputTestData
  mergeCollections(inputKeys, outputKey);

  // test "outputCollectionMergeTest" contents
  SG::ReadHandleKey<TestHitCollection_t>  mergedCollectionKey{"outputCollectionMergeTest"};
  ASSERT_TRUE( mergedCollectionKey.initialize().isSuccess() );
  SG::ReadHandle<TestHitCollection_t>     mergedCollectionHandle{mergedCollectionKey};

  ASSERT_TRUE( mergedCollectionHandle.isValid() );
  ASSERT_EQ( mergedCollectionHandle->size(), 3+2+3 );

  auto& mergedCollectionVector = mergedCollectionHandle->getVector();
  ASSERT_EQ( mergedCollectionVector.at(0).m_value, 1  ); // inputCollectionA
  ASSERT_EQ( mergedCollectionVector.at(1).m_value, 20 ); // inputCollectionA
  ASSERT_EQ( mergedCollectionVector.at(2).m_value, 5  ); // inputCollectionA
  ASSERT_EQ( mergedCollectionVector.at(3).m_value, 20 ); // inputCollectionC
  ASSERT_EQ( mergedCollectionVector.at(4).m_value, 5  ); // inputCollectionC
  ASSERT_EQ( mergedCollectionVector.at(5).m_value, 1  ); // inputCollectionC
  ASSERT_EQ( mergedCollectionVector.at(6).m_value, 50 ); // inputCollectionB
  ASSERT_EQ( mergedCollectionVector.at(7).m_value, 1  ); // inputCollectionB
}


TEST_F(CollectionMerger_test, integration_with_data) {
  // ordering A, C, B is on purpose to test for unintended alphabetic ordering
  std::string inputPropertyValue = "['inputPixelCollectionIntegrationTestA',"
                                    "'inputPixelCollectionIntegrationTestC',"
                                    "'inputPixelCollectionIntegrationTestB']";
  std::string outputPropertyValue = "'outputPixelCollectionIntegrationTest'";
  ASSERT_TRUE( m_alg->setProperty( "InputPixelHits",  inputPropertyValue).isSuccess()  );
  ASSERT_TRUE( m_alg->setProperty( "OutputPixelHits", outputPropertyValue).isSuccess() );

  // create dummy input collections containing dummy data
  HepGeom::Point3D<double> pos(0.,0.,0.);
  auto                                 inputTestDataA = CxxUtils::make_unique<SiHitCollection>();
  SG::WriteHandle<SiHitCollection> inputTestDataHandleA{"inputPixelCollectionIntegrationTestA"};
  inputTestDataHandleA.record( std::move(inputTestDataA) );
  inputTestDataHandleA->Emplace( pos, pos, 1., 1.,  1, 0 );
  inputTestDataHandleA->Emplace( pos, pos, 1., 1., 20, 0 );
  inputTestDataHandleA->Emplace( pos, pos, 1., 1.,  5, 0 );

  auto                                 inputTestDataB = CxxUtils::make_unique<SiHitCollection>();
  SG::WriteHandle<SiHitCollection> inputTestDataHandleB{"inputPixelCollectionIntegrationTestB"};
  inputTestDataHandleB.record( std::move(inputTestDataB) );
  inputTestDataHandleB->Emplace( pos, pos, 1., 1., 50, 0 );
  inputTestDataHandleB->Emplace( pos, pos, 1., 1.,  1, 0 );

  auto                                 inputTestDataC = CxxUtils::make_unique<SiHitCollection>();
  SG::WriteHandle<SiHitCollection> inputTestDataHandleC{"inputPixelCollectionIntegrationTestC"};
  inputTestDataHandleC.record( std::move(inputTestDataC) );
  inputTestDataHandleC->Emplace( pos, pos, 1., 1., 20, 0 );
  inputTestDataHandleC->Emplace( pos, pos, 1., 1.,  5, 0 );
  inputTestDataHandleC->Emplace( pos, pos, 1., 1.,  1, 0 );

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  ASSERT_TRUE( m_alg->execute().isSuccess() );

  // test "outputPixelCollectionIntegrationTest" contents
  SG::ReadHandleKey<SiHitCollection>  mergedCollectionKey{"outputPixelCollectionIntegrationTest"};
  ASSERT_TRUE( mergedCollectionKey.initialize().isSuccess() );
  SG::ReadHandle<SiHitCollection>     mergedCollectionHandle{mergedCollectionKey};

  ASSERT_TRUE( mergedCollectionHandle.isValid() );
  ASSERT_EQ( mergedCollectionHandle->size(), 3+2+3 );

  auto& mergedCollectionVector = mergedCollectionHandle->getVector();
  ASSERT_EQ( mergedCollectionVector.at(0).trackNumber(), 1  ); // inputPixelCollectionIntegrationTestA
  ASSERT_EQ( mergedCollectionVector.at(1).trackNumber(), 20 ); // inputPixelCollectionIntegrationTestA
  ASSERT_EQ( mergedCollectionVector.at(2).trackNumber(), 5  ); // inputPixelCollectionIntegrationTestA
  ASSERT_EQ( mergedCollectionVector.at(3).trackNumber(), 20 ); // inputPixelCollectionIntegrationTestC
  ASSERT_EQ( mergedCollectionVector.at(4).trackNumber(), 5  ); // inputPixelCollectionIntegrationTestC
  ASSERT_EQ( mergedCollectionVector.at(5).trackNumber(), 1  ); // inputPixelCollectionIntegrationTestC
  ASSERT_EQ( mergedCollectionVector.at(6).trackNumber(), 50 ); // inputPixelCollectionIntegrationTestB
  ASSERT_EQ( mergedCollectionVector.at(7).trackNumber(), 1  ); // inputPixelCollectionIntegrationTestB
}


} // <-- namespace ISFTesting


CLASS_DEF (ISFTesting::TestHitCollection_t, 1234567890, 1)


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new ISFTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
