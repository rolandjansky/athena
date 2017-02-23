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
    StatusCode setupReadHandleVector(Args&&... args) const {
      return m_alg->setupReadHandleVector(std::forward<Args>(args)...);
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


TEST_F(CollectionMerger_test, setupReadHandleVector__emptySGKeysVector__expectEmptyReadHandleVector) {
  std::vector<std::string> sgKeys{};
  std::vector<SG::ReadHandle<TestHitCollection_t>> readHandles{};

  ASSERT_TRUE( setupReadHandleVector(sgKeys, readHandles).isSuccess() );
  ASSERT_TRUE( readHandles.empty() );
}


TEST_F(CollectionMerger_test, setupReadHandleVector__oneNonExistingSGCollectionInSGKeysVector__expectFailure) {
  // populating test collections on StoreGate
  SG::WriteHandle<TestHitCollection_t> testCollectionA1("testKeyA0");
  testCollectionA1 = CxxUtils::make_unique<TestHitCollection_t>();
  // -> skipping testKeyB2
  SG::WriteHandle<TestHitCollection_t> testCollectionC1("testKeyC0");
  testCollectionC1 = CxxUtils::make_unique<TestHitCollection_t>();

  std::vector<std::string> sgKeys{"testKeyA0", "testKeyB0", "testKeyC0"};
  std::vector<SG::ReadHandle<TestHitCollection_t>> readHandles{};

  ASSERT_TRUE( setupReadHandleVector(sgKeys, readHandles).isFailure() );
}


TEST_F(CollectionMerger_test, setupReadHandleVector__filledSGKeysVectorFilledCollections__expectValidReadHandles) {
  // populating test collections on StoreGate
  SG::WriteHandle<TestHitCollection_t> testCollectionA1("testKeyA1");
  testCollectionA1 = CxxUtils::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> testCollectionB1("testKeyB1");
  testCollectionB1 = CxxUtils::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> testCollectionC1("testKeyC1");
  testCollectionC1 = CxxUtils::make_unique<TestHitCollection_t>();

  std::vector<std::string> sgKeys{"testKeyA1", "testKeyB1", "testKeyC1"};
  std::vector<SG::ReadHandle<TestHitCollection_t>> readHandles{};

  ASSERT_TRUE( setupReadHandleVector(sgKeys, readHandles).isSuccess() );

  ASSERT_EQ( readHandles.size(), 3 );
  ASSERT_EQ( readHandles.at(0).key(), "testKeyA1" );
  ASSERT_TRUE( readHandles.at(0).isValid() );
  ASSERT_EQ( readHandles.at(1).key(), "testKeyB1" );
  ASSERT_TRUE( readHandles.at(1).isValid() );
  ASSERT_EQ( readHandles.at(2).key(), "testKeyC1" );
  ASSERT_TRUE( readHandles.at(2).isValid() );
}


TEST_F(CollectionMerger_test, setupReadHandleVector__oneEmptySGKeyString__expectFailure) {
  // populating test collections on StoreGate
  SG::WriteHandle<TestHitCollection_t> testCollectionA2("testKeyA2");
  testCollectionA2 = CxxUtils::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> testCollectionB2("testKeyB2");
  testCollectionB2 = CxxUtils::make_unique<TestHitCollection_t>();

  std::vector<std::string> sgKeys{"testKeyA2", std::string(), "testKeyB2"};
  std::vector<SG::ReadHandle<TestHitCollection_t>> readHandles{};

  // isFailure because of the empty string in the sgKeys vector
  ASSERT_TRUE( setupReadHandleVector(sgKeys, readHandles).isFailure() );
}


TEST_F(CollectionMerger_test, mergeCollections__emptySGInputKeysVector__expectSuccess) {
  std::vector<SG::ReadHandle<TestHitCollection_t>> inputKeys{};
  SG::WriteHandle<TestHitCollection_t> outputWriteHandle{"outputCollectionMergeTest1"};

  // should not fail, just does nothing
  mergeCollections(inputKeys, outputWriteHandle);
}


TEST_F(CollectionMerger_test, mergeCollections__filledInputCollections__expectMergedDataInOutputCollection) {
  // create dummy input collections containing dummy data
  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleA{"inputCollectionA"};
  inputTestDataHandleA = CxxUtils::make_unique<TestHitCollection_t>();
  inputTestDataHandleA->Emplace(1);
  inputTestDataHandleA->Emplace(20);
  inputTestDataHandleA->Emplace(5);

  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleB{"inputCollectionB"};
  inputTestDataHandleB = CxxUtils::make_unique<TestHitCollection_t>();
  inputTestDataHandleB->Emplace(50);
  inputTestDataHandleB->Emplace(1);

  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleC{"inputCollectionC"};
  inputTestDataHandleC = CxxUtils::make_unique<TestHitCollection_t>();
  inputTestDataHandleC->Emplace(20);
  inputTestDataHandleC->Emplace(5);
  inputTestDataHandleC->Emplace(1);

  // add inputCollections with test data to inputReadHandles for later merging
  // ordering A, C, B is on purpose to test for unintended alphabetic ordering
  std::vector<SG::ReadHandle<TestHitCollection_t>> inputReadHandles{};
  inputReadHandles.emplace_back( "inputCollectionA" );
  inputReadHandles.emplace_back( "inputCollectionC" );
  inputReadHandles.emplace_back( "inputCollectionB" );
  ASSERT_TRUE( inputReadHandles.at(0).isValid() );
  ASSERT_TRUE( inputReadHandles.at(1).isValid() );
  ASSERT_TRUE( inputReadHandles.at(2).isValid() );

  SG::WriteHandle<TestHitCollection_t> outputWriteHandle{"outputCollectionMergeTest2"};

  // access the inputTestData
  mergeCollections(inputReadHandles, outputWriteHandle);

  // test "outputCollectionMergeTest2" contents
  SG::ReadHandle<TestHitCollection_t> mergedCollectionHandle{"outputCollectionMergeTest2"};

  ASSERT_TRUE( mergedCollectionHandle.isValid() );
  ASSERT_EQ( mergedCollectionHandle->size(), 3+2+3 );

  const auto& mergedCollectionVector = mergedCollectionHandle->getVector();
  ASSERT_EQ( mergedCollectionVector.at(0).m_value, 1  ); // inputCollectionA
  ASSERT_EQ( mergedCollectionVector.at(1).m_value, 20 ); // inputCollectionA
  ASSERT_EQ( mergedCollectionVector.at(2).m_value, 5  ); // inputCollectionA
  ASSERT_EQ( mergedCollectionVector.at(3).m_value, 20 ); // inputCollectionC
  ASSERT_EQ( mergedCollectionVector.at(4).m_value, 5  ); // inputCollectionC
  ASSERT_EQ( mergedCollectionVector.at(5).m_value, 1  ); // inputCollectionC
  ASSERT_EQ( mergedCollectionVector.at(6).m_value, 50 ); // inputCollectionB
  ASSERT_EQ( mergedCollectionVector.at(7).m_value, 1  ); // inputCollectionB
}


TEST_F(CollectionMerger_test, initializeAndexecute__propertiesSetAndFilledCollections__expectMergedDataInOutputCollection) {
  // ordering A, C, B is on purpose to test for unintended alphabetic ordering
  std::string inputPropertyValue = "['inputPixelCollectionIntegrationTestA',"
                                    "'inputPixelCollectionIntegrationTestC',"
                                    "'inputPixelCollectionIntegrationTestB']";
  std::string outputPropertyValue = "'outputPixelCollectionIntegrationTest'";
  ASSERT_TRUE( m_alg->setProperty( "InputPixelHits",  inputPropertyValue).isSuccess()  );
  ASSERT_TRUE( m_alg->setProperty( "OutputPixelHits", outputPropertyValue).isSuccess() );

  // create dummy input collections containing dummy data
  HepGeom::Point3D<double> pos(0.,0.,0.);
  SG::WriteHandle<SiHitCollection> inputTestDataHandleA{"inputPixelCollectionIntegrationTestA"};
  inputTestDataHandleA = CxxUtils::make_unique<SiHitCollection>();
  inputTestDataHandleA->Emplace( pos, pos, 1., 1.,  1, 0 );
  inputTestDataHandleA->Emplace( pos, pos, 1., 1., 20, 0 );
  inputTestDataHandleA->Emplace( pos, pos, 1., 1.,  5, 0 );

  SG::WriteHandle<SiHitCollection> inputTestDataHandleB{"inputPixelCollectionIntegrationTestB"};
  inputTestDataHandleB = CxxUtils::make_unique<SiHitCollection>();
  inputTestDataHandleB->Emplace( pos, pos, 1., 1., 50, 0 );
  inputTestDataHandleB->Emplace( pos, pos, 1., 1.,  1, 0 );

  SG::WriteHandle<SiHitCollection> inputTestDataHandleC{"inputPixelCollectionIntegrationTestC"};
  inputTestDataHandleC = CxxUtils::make_unique<SiHitCollection>();
  inputTestDataHandleC->Emplace( pos, pos, 1., 1., 20, 0 );
  inputTestDataHandleC->Emplace( pos, pos, 1., 1.,  5, 0 );
  inputTestDataHandleC->Emplace( pos, pos, 1., 1.,  1, 0 );

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  ASSERT_TRUE( m_alg->execute().isSuccess() );

  // test "outputPixelCollectionIntegrationTest" contents
  SG::ReadHandle<SiHitCollection> mergedCollectionHandle{"outputPixelCollectionIntegrationTest"};

  ASSERT_TRUE( mergedCollectionHandle.isValid() );
  ASSERT_EQ( mergedCollectionHandle->size(), 3+2+3 );

  const auto& mergedCollectionVector = mergedCollectionHandle->getVector();
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
