/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

// Google Test
#include "gtest/gtest.h"
// Google Mock
// #include "gmock/gmock.h"

// Tested AthAlgorithm
#include "../src/CollectionMerger.h"



namespace ISFTesting {

// needed every time an AthAlgorithm, AthAlgTool or AthService is instantiated
ISvcLocator* g_svcLoc = nullptr;


// fake sensitive detector hits
class TestHit {
  public:
    TestHit(int value):m_value(value) { };
    int m_value;
};

// typedef for convenience
typedef AtlasHitsVector<ISFTesting::TestHit> TestHitCollection_t;
typedef AthenaHitsVector<ISFTesting::TestHit> TestPointerHitCollection_t;


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
      ASSERT_TRUE( g_svcLoc->service("StoreGateSvc", m_sg) );
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
    void mergeCollections(Args&&... args) const {
      ASSERT_TRUE( m_alg->mergeCollections(std::forward<Args>(args)...).isSuccess() );
    }

    ISF::CollectionMerger* m_alg;
    StoreGateSvc* m_sg{};
};  // CollectionMerger_test fixture


TEST_F(CollectionMerger_test, empty_alg_execute) {
  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
  ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
}

TEST_F(CollectionMerger_test, mergeCollections) {
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
  SG::ReadHandleKeyArray<TestHitCollection_t> inputKeys{};
  SG::WriteHandleKey<TestHitCollection_t>             outputKey{"outputCollectionMergeTest"};
  ASSERT_TRUE( outputKey.initialize().isSuccess() );

  // should not fail, just does nothing
  mergeCollections(inputKeys, outputKey, ctx);

  // create dummy input collections containing dummy data
  auto                                 inputTestDataA = std::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleA{"inputCollectionA"};
  ASSERT_TRUE( inputTestDataHandleA.record( std::move(inputTestDataA) ).isSuccess() );
  inputTestDataHandleA->Emplace(1);
  inputTestDataHandleA->Emplace(20);
  inputTestDataHandleA->Emplace(5);

  auto                                 inputTestDataB = std::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleB{"inputCollectionB"};
  ASSERT_TRUE( inputTestDataHandleB.record( std::move(inputTestDataB) ).isSuccess() );
  inputTestDataHandleB->Emplace(50);
  inputTestDataHandleB->Emplace(1);

  auto                                 inputTestDataC = std::make_unique<TestHitCollection_t>();
  SG::WriteHandle<TestHitCollection_t> inputTestDataHandleC{"inputCollectionC"};
  ASSERT_TRUE( inputTestDataHandleC.record( std::move(inputTestDataC) ).isSuccess() );
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
  mergeCollections(inputKeys, outputKey, ctx);

  // test "outputCollectionMergeTest" contents
  SG::ReadHandleKey<TestHitCollection_t>  mergedCollectionKey{"outputCollectionMergeTest"};
  ASSERT_TRUE( mergedCollectionKey.initialize().isSuccess() );
  SG::ReadHandle<TestHitCollection_t>     mergedCollectionHandle{mergedCollectionKey};

  ASSERT_TRUE( mergedCollectionHandle.isValid() );
  ASSERT_EQ( mergedCollectionHandle->size(), 3u+2u+3u );

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


TEST_F(CollectionMerger_test, integration_with_data) {
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
  // ordering A, C, B is on purpose to test for unintended alphabetic ordering
  std::string inputPropertyValue = "['inputPixelCollectionIntegrationTestA',"
                                    "'inputPixelCollectionIntegrationTestC',"
                                    "'inputPixelCollectionIntegrationTestB']";
  std::string outputPropertyValue = "'outputPixelCollectionIntegrationTest'";
  ASSERT_TRUE( m_alg->setProperty( "InputPixelHits",  inputPropertyValue).isSuccess()  );
  ASSERT_TRUE( m_alg->setProperty( "OutputPixelHits", outputPropertyValue).isSuccess() );

  // create dummy input collections containing dummy data
  HepGeom::Point3D<double> pos(0.,0.,0.);
  auto                                 inputTestDataA = std::make_unique<SiHitCollection>();
  SG::WriteHandleKey<SiHitCollection> inputTestDataHandleKeyA{"inputPixelCollectionIntegrationTestA"};
  ASSERT_TRUE( inputTestDataHandleKeyA.initialize().isSuccess() );
  SG::WriteHandle<SiHitCollection> inputTestDataHandleA{inputTestDataHandleKeyA, ctx};
  ASSERT_TRUE( inputTestDataHandleA.record( std::move(inputTestDataA) ).isSuccess() );
  inputTestDataHandleA->Emplace( pos, pos, 1., 1.,  1, 0 );
  inputTestDataHandleA->Emplace( pos, pos, 1., 1., 20, 0 );
  inputTestDataHandleA->Emplace( pos, pos, 1., 1.,  5, 0 );

  auto                                 inputTestDataB = std::make_unique<SiHitCollection>();
  SG::WriteHandleKey<SiHitCollection> inputTestDataHandleKeyB{"inputPixelCollectionIntegrationTestB"};
  ASSERT_TRUE( inputTestDataHandleKeyB.initialize().isSuccess() );
  SG::WriteHandle<SiHitCollection> inputTestDataHandleB{inputTestDataHandleKeyB, ctx};
  ASSERT_TRUE( inputTestDataHandleB.record( std::move(inputTestDataB) ).isSuccess() );
  inputTestDataHandleB->Emplace( pos, pos, 1., 1., 50, 0 );
  inputTestDataHandleB->Emplace( pos, pos, 1., 1.,  1, 0 );

  auto                                 inputTestDataC = std::make_unique<SiHitCollection>();
  SG::WriteHandleKey<SiHitCollection> inputTestDataHandleKeyC{"inputPixelCollectionIntegrationTestC"};
  ASSERT_TRUE( inputTestDataHandleKeyC.initialize().isSuccess() );
  SG::WriteHandle<SiHitCollection> inputTestDataHandleC{inputTestDataHandleKeyC, ctx};
  ASSERT_TRUE( inputTestDataHandleC.record( std::move(inputTestDataC) ).isSuccess() );
  inputTestDataHandleC->Emplace( pos, pos, 1., 1., 20, 0 );
  inputTestDataHandleC->Emplace( pos, pos, 1., 1.,  5, 0 );
  inputTestDataHandleC->Emplace( pos, pos, 1., 1.,  1, 0 );

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );

  // test "outputPixelCollectionIntegrationTest" contents
  SG::ReadHandleKey<SiHitCollection>  mergedCollectionKey{"outputPixelCollectionIntegrationTest"};
  ASSERT_TRUE( mergedCollectionKey.initialize().isSuccess() );
  SG::ReadHandle<SiHitCollection>     mergedCollectionHandle{mergedCollectionKey, ctx};

  ASSERT_TRUE( mergedCollectionHandle.isValid() );
  ASSERT_EQ( mergedCollectionHandle->size(), 3u+2u+3u );

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


TEST_F(CollectionMerger_test, one_empty_one_filled_input_collection___expect_filled_output_collection) {
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
  std::string inputPropertyValue = "['inputPixelCollectionIntegrationTestFilled',"
                                    "'inputPixelCollectionIntegrationTestEmpty']";
  std::string outputPropertyValue = "'outputPixelCollectionTestFilled'";
  ASSERT_TRUE( m_alg->setProperty( "InputPixelHits",  inputPropertyValue).isSuccess()  );
  ASSERT_TRUE( m_alg->setProperty( "OutputPixelHits", outputPropertyValue).isSuccess() );

  // create dummy input collections containing dummy data
  HepGeom::Point3D<double> pos(0.,0.,0.);
  SG::WriteHandleKey<SiHitCollection> inputTestDataHandleFilledKey{"inputPixelCollectionIntegrationTestFilled"};
  ASSERT_TRUE( inputTestDataHandleFilledKey.initialize().isSuccess() );
  SG::WriteHandle<SiHitCollection> inputTestDataHandleFilled{inputTestDataHandleFilledKey, ctx};
  ASSERT_TRUE(inputTestDataHandleFilled.record(std::make_unique<SiHitCollection>()).isSuccess());
  inputTestDataHandleFilled->Emplace( pos, pos, 1., 1.,  1, 0 );
  inputTestDataHandleFilled->Emplace( pos, pos, 1., 1., 20, 0 );
  inputTestDataHandleFilled->Emplace( pos, pos, 1., 1.,  5, 0 );

  SG::WriteHandleKey<SiHitCollection> inputTestDataHandleEmptyKey{"inputPixelCollectionIntegrationTestEmpty"};
  ASSERT_TRUE( inputTestDataHandleEmptyKey.initialize().isSuccess() );
  SG::WriteHandle<SiHitCollection> inputTestDataHandleEmpty{inputTestDataHandleEmptyKey, ctx};
  ASSERT_TRUE(inputTestDataHandleEmpty.record(std::make_unique<SiHitCollection>()).isSuccess());

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );

  // test "outputPixelCollectionIntegrationTest" contents
  SG::ReadHandleKey<SiHitCollection>  mergedCollectionKey{"outputPixelCollectionTestFilled"};
  ASSERT_TRUE( mergedCollectionKey.initialize().isSuccess() );
  SG::ReadHandle<SiHitCollection> mergedCollectionHandle{mergedCollectionKey, ctx};

  ASSERT_TRUE( mergedCollectionHandle.isValid() );
  ASSERT_EQ( 3u, mergedCollectionHandle->size() );

  const auto& mergedCollectionVector = mergedCollectionHandle->getVector();
  ASSERT_EQ( mergedCollectionVector.at(0).trackNumber(), 1  ); // inputPixelCollectionIntegrationTestA
  ASSERT_EQ( mergedCollectionVector.at(1).trackNumber(), 20 ); // inputPixelCollectionIntegrationTestA
  ASSERT_EQ( mergedCollectionVector.at(2).trackNumber(), 5  ); // inputPixelCollectionIntegrationTestA
}


TEST_F(CollectionMerger_test, preexisting_output_collection___expect_execute_isFailure) {
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
  std::string inputPropertyValue = "['inputPixelCollectionTestX']";
  std::string outputPropertyValue = "'outputPixelCollectionTestPreexisting'";
  ASSERT_TRUE( m_alg->setProperty( "InputPixelHits",  inputPropertyValue).isSuccess()  );
  ASSERT_TRUE( m_alg->setProperty( "OutputPixelHits", outputPropertyValue).isSuccess() );

  // create dummy input collections containing dummy data
  SG::WriteHandleKey<SiHitCollection> inputTestDataHandleKeyA{"inputPixelCollectionTestX"};
  ASSERT_TRUE( inputTestDataHandleKeyA.initialize().isSuccess() );
  SG::WriteHandle<SiHitCollection> inputTestDataHandleA{inputTestDataHandleKeyA, ctx};
  ASSERT_TRUE( inputTestDataHandleA.record(std::make_unique<SiHitCollection>()).isSuccess() );

  // create pre-existing output collection
  SG::WriteHandleKey<SiHitCollection> outputDataHandleKey{"outputPixelCollectionTestPreexisting"};
  ASSERT_TRUE( outputDataHandleKey.initialize().isSuccess() );
  SG::WriteHandle<SiHitCollection> outputDataHandle{outputDataHandleKey, ctx};
  ASSERT_TRUE( outputDataHandle.record(std::make_unique<SiHitCollection>()).isSuccess() );

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  ASSERT_TRUE( m_alg->execute(ctx).isFailure() );
}


TEST_F(CollectionMerger_test, nonexisting_input_collection___expect_SG_exception) {
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
  std::string inputPropertyValue = "['inputPixelCollectionDoesntExist']";
  std::string outputPropertyValue = "'outputPixelCollectionTest123'";
  ASSERT_TRUE( m_alg->setProperty( "InputPixelHits",  inputPropertyValue).isSuccess()  );
  ASSERT_TRUE( m_alg->setProperty( "OutputPixelHits", outputPropertyValue).isSuccess() );

  ASSERT_TRUE( m_alg->initialize().isSuccess() );
  ASSERT_THROW( m_alg->execute(ctx).ignore(), SG::ExcNullReadHandle );
}


TEST_F(CollectionMerger_test, mergeCollections_with_pointer_types___expect_merged_pointers_not_equal_to_input) {
  EventContext ctx(0,0);
  ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
  SG::ReadHandleKeyArray<TestPointerHitCollection_t> inputKeys{};
  SG::WriteHandleKey<TestPointerHitCollection_t> outputKey{"outputPointerCollectionMergeTest"};
  ASSERT_TRUE( outputKey.initialize().isSuccess() );

  // create dummy input collections containing dummy data
  auto inputTestDataA = std::make_unique<TestPointerHitCollection_t>();
  SG::WriteHandleKey<TestPointerHitCollection_t> inputTestDataHandleKeyA{"inputPointerCollectionA"};
  ASSERT_TRUE( inputTestDataHandleKeyA.initialize().isSuccess() );
  SG::WriteHandle<TestPointerHitCollection_t> inputTestDataHandleA{inputTestDataHandleKeyA, ctx};
  ASSERT_TRUE( inputTestDataHandleA.record( std::move(inputTestDataA) ).isSuccess() );
  auto* inputHitA1 = new ISFTesting::TestHit(1);
  inputTestDataHandleA->push_back(inputHitA1);
  auto* inputHitA2 = new ISFTesting::TestHit(20);
  inputTestDataHandleA->push_back(inputHitA2);
  auto* inputHitA3 = new ISFTesting::TestHit(5);
  inputTestDataHandleA->push_back(inputHitA3);

  auto inputTestDataB = std::make_unique<TestPointerHitCollection_t>();
  SG::WriteHandleKey<TestPointerHitCollection_t> inputTestDataHandleKeyB{"inputPointerCollectionB"};
  ASSERT_TRUE( inputTestDataHandleKeyB.initialize().isSuccess() );
  SG::WriteHandle<TestPointerHitCollection_t> inputTestDataHandleB{inputTestDataHandleKeyB, ctx};
  ASSERT_TRUE( inputTestDataHandleB.record( std::move(inputTestDataB) ).isSuccess() );
  auto* inputHitB1 = new ISFTesting::TestHit(50);
  inputTestDataHandleB->push_back(inputHitB1);
  auto* inputHitB2 = new ISFTesting::TestHit(1);
  inputTestDataHandleB->push_back(inputHitB2);

  // add inputCollections with test data to inputKeys for later merging
  // ordering A, C, B is on purpose to test for unintended alphabetic ordering
  inputKeys.emplace_back( "inputPointerCollectionA" );
  inputKeys.emplace_back( "inputPointerCollectionB" );
  ASSERT_TRUE( inputKeys.at(0).initialize().isSuccess() );
  ASSERT_TRUE( inputKeys.at(1).initialize().isSuccess() );

  // merge pointer collections
  mergeCollections(inputKeys, outputKey, ctx);

  // test "outputCollectionMergeTest" contents
  SG::ReadHandleKey<TestPointerHitCollection_t> mergedCollectionKey{"outputPointerCollectionMergeTest"};
  ASSERT_TRUE( mergedCollectionKey.initialize().isSuccess() );
  SG::ReadHandle<TestPointerHitCollection_t> mergedCollectionHandle{mergedCollectionKey, ctx};

  ASSERT_TRUE( mergedCollectionHandle.isValid() );
  ASSERT_EQ( 3u+2u, mergedCollectionHandle->size() );

  const auto* outputHit1 = (*mergedCollectionHandle)[0];  // inputPointerCollectionA
  const auto* outputHit2 = (*mergedCollectionHandle)[1];  // inputPointerCollectionA
  const auto* outputHit3 = (*mergedCollectionHandle)[2];  // inputPointerCollectionA
  const auto* outputHit4 = (*mergedCollectionHandle)[3];  // inputPointerCollectionB
  const auto* outputHit5 = (*mergedCollectionHandle)[4];  // inputPointerCollectionB

  // check merged hit pointers are different from input (i.e. expect copies, not identities of input in output)
  ASSERT_NE( inputHitA1, outputHit1 );  // inputPointerCollectionA
  ASSERT_NE( inputHitA2, outputHit2 );  // inputPointerCollectionA
  ASSERT_NE( inputHitA3, outputHit3 );  // inputPointerCollectionA
  ASSERT_NE( inputHitB1, outputHit4 );  // inputPointerCollectionB
  ASSERT_NE( inputHitB2, outputHit5 );  // inputPointerCollectionB

  // check values of hits are copied
  ASSERT_EQ( outputHit1->m_value, 1  );  // inputPointerCollectionA
  ASSERT_EQ( outputHit2->m_value, 20 );  // inputPointerCollectionA
  ASSERT_EQ( outputHit3->m_value, 5  );  // inputPointerCollectionA
  ASSERT_EQ( outputHit4->m_value, 50 );  // inputPointerCollectionB
  ASSERT_EQ( outputHit5->m_value, 1  );  // inputPointerCollectionB
}


} // <-- namespace ISFTesting


CLASS_DEF (ISFTesting::TestHitCollection_t, 1234567890, 1)
CLASS_DEF (ISFTesting::TestPointerHitCollection_t, 1234567891, 1)


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new ISFTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
