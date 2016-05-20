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

// Standard C/C++
#include <cassert>

// ATLAS C++
#include "CxxUtils/make_unique.h"

// Tested AthAlgorithm
#include "../src/CollectionMerger.h"



namespace ISFTesting {

class TestHits {
  public:
    TestHits(int value):m_value(value) { };
    int m_value;
};

// typedef for convenience
typedef AtlasHitsVector<ISFTesting::TestHits> TestHitCollection_t;


class CollectionMerger_test {

  public:
    static void test_empty_alg(ISvcLocator* svcLoc) {
      ISF::CollectionMerger alg{"CollectionMergerEmptyAlgTest", svcLoc};
      assert( alg.setProperties().isSuccess() );
      assert( alg.initialize().isSuccess()    );
      assert( alg.execute().isSuccess()       );
      assert( alg.finalize().isSuccess()      );
    }


    static void test_initializeVarHandleKey(ISvcLocator* svcLoc) {
      ISF::CollectionMerger alg{"CollectionMergerInitializeVarHandleKeyTest", svcLoc};

      SG::VarHandleKey testKey{ 123456789 /* clid */,
                                std::string("testKey"),
                                Gaudi::DataHandle::Reader };

      assert( alg.initializeVarHandleKey(testKey).isSuccess() );
      assert( testKey.storeHandle().isSet() );

    }


    static void test_initializeVarHandleKey_emptyKey(ISvcLocator* svcLoc) {
      ISF::CollectionMerger alg("CollectionMergerInitializeVarHandleEmptyKeyTest", svcLoc);

      SG::VarHandleKey emptyTestKey{ 123456789 /* clid */,
                                     std::string(),
                                     Gaudi::DataHandle::Reader };

      assert( alg.initializeVarHandleKey(emptyTestKey).isSuccess() );
      assert( !emptyTestKey.storeHandle().isSet() );
    }


    static void test_setupReadHandleKeyVector(ISvcLocator* svcLoc) {
      ISF::CollectionMerger alg("CollectionMergerSetupReadHandleKeyVectorTest", svcLoc);

      std::vector<std::string>                            sgKeys{};
      std::vector<SG::ReadHandleKey<TestHitCollection_t>> readHandles{};

      assert( alg.setupReadHandleKeyVector(sgKeys, readHandles).isSuccess() );
      assert( readHandles.empty() );

      sgKeys.emplace_back( "testKeyA" );
      sgKeys.emplace_back( "testKeyB" );
      sgKeys.emplace_back( "testKeyC" );

      assert( alg.setupReadHandleKeyVector(sgKeys, readHandles).isSuccess() );
      assert( readHandles.size()==3 );
      assert( readHandles.at(0).key() == "testKeyA" );
      assert( readHandles.at(0).clid() );
      assert( readHandles.at(0).mode() == Gaudi::DataHandle::Reader );
      assert( readHandles.at(0).storeHandle().isSet() );
      assert( readHandles.at(1).key() == "testKeyB" );
      assert( readHandles.at(1).clid() );
      assert( readHandles.at(1).mode() == Gaudi::DataHandle::Reader );
      assert( readHandles.at(1).storeHandle().isSet() );
      assert( readHandles.at(2).key() == "testKeyC" );
      assert( readHandles.at(2).clid() );
      assert( readHandles.at(2).mode() == Gaudi::DataHandle::Reader );
      assert( readHandles.at(2).storeHandle().isSet() );
    }


    static void test_setupReadHandleKeyVector_emptyKey(ISvcLocator* svcLoc) {
      ISF::CollectionMerger alg("CollectionMergerSetupReadHandleKeyVectorEmptyKeyTest", svcLoc);

      std::vector<std::string>                            sgKeys{"testKeyA", std::string(), "testKeyC"};
      std::vector<SG::ReadHandleKey<TestHitCollection_t>> readHandles{};

      // isFailure because of the empty string in the sgKeys vector
      assert( alg.setupReadHandleKeyVector(sgKeys, readHandles).isFailure() );
    }


    static void test_mergeCollections(ISvcLocator* svcLoc) {
      ISF::CollectionMerger alg("CollectionMergerMergeCollectionsTest", svcLoc);

      std::vector<SG::ReadHandleKey<TestHitCollection_t>> inputKeys{};
      SG::WriteHandleKey<TestHitCollection_t>             outputKey{"outputCollectionMergeTest"};
      assert( outputKey.initialize().isSuccess() );

      // should not fail, just does nothing
      alg.mergeCollections(inputKeys, outputKey);

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
      assert( inputKeys.at(0).initialize().isSuccess() );
      assert( inputKeys.at(1).initialize().isSuccess() );
      assert( inputKeys.at(2).initialize().isSuccess() );

      // access the inputTestData
      alg.mergeCollections(inputKeys, outputKey);

      // test "outputCollectionMergeTest" contents
      SG::ReadHandleKey<TestHitCollection_t>  mergedCollectionKey{"outputCollectionMergeTest"};
      assert( mergedCollectionKey.initialize().isSuccess() );
      SG::ReadHandle<TestHitCollection_t>     mergedCollectionHandle{mergedCollectionKey};

      assert( mergedCollectionHandle.isValid() );
      assert( mergedCollectionHandle->size() == 3+2+3 );

      auto& mergedCollectionVector = mergedCollectionHandle->getVector();
      assert( mergedCollectionVector.at(0).m_value == 1  ); // inputCollectionA
      assert( mergedCollectionVector.at(1).m_value == 20 ); // inputCollectionA
      assert( mergedCollectionVector.at(2).m_value == 5  ); // inputCollectionA
      assert( mergedCollectionVector.at(3).m_value == 20 ); // inputCollectionC
      assert( mergedCollectionVector.at(4).m_value == 5  ); // inputCollectionC
      assert( mergedCollectionVector.at(5).m_value == 1  ); // inputCollectionC
      assert( mergedCollectionVector.at(6).m_value == 50 ); // inputCollectionB
      assert( mergedCollectionVector.at(7).m_value == 1  ); // inputCollectionB
    }


    static void test_integration_empty(ISvcLocator* svcLoc) {
      ISF::CollectionMerger alg("CollectionMergerIntegrationTestEmpty", svcLoc);
      assert( alg.setProperties().isSuccess() );
      assert( alg.initialize().isSuccess()    );
      assert( alg.execute().isSuccess()       );
      assert( alg.execute().isSuccess()       );
      assert( alg.finalize().isSuccess()      );
    }


    static void test_integration_with_data(ISvcLocator* svcLoc) {
      ISF::CollectionMerger alg("CollectionMergerIntegrationTestWithData", svcLoc);
      assert( alg.setProperties().isSuccess() );

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

      assert( alg.initialize().isSuccess() );
      assert( alg.execute().isSuccess() );

      // test "outputPixelCollectionIntegrationTest" contents
      SG::ReadHandleKey<SiHitCollection>  mergedCollectionKey{"outputPixelCollectionIntegrationTest"};
      assert( mergedCollectionKey.initialize().isSuccess() );
      SG::ReadHandle<SiHitCollection>     mergedCollectionHandle{mergedCollectionKey};

      assert( mergedCollectionHandle.isValid() );
      assert( mergedCollectionHandle->size() == 3+2+3 );

      auto& mergedCollectionVector = mergedCollectionHandle->getVector();
      assert( mergedCollectionVector.at(0).trackNumber() == 1  ); // inputPixelCollectionIntegrationTestA
      assert( mergedCollectionVector.at(1).trackNumber() == 20 ); // inputPixelCollectionIntegrationTestA
      assert( mergedCollectionVector.at(2).trackNumber() == 5  ); // inputPixelCollectionIntegrationTestA
      assert( mergedCollectionVector.at(3).trackNumber() == 20 ); // inputPixelCollectionIntegrationTestC
      assert( mergedCollectionVector.at(4).trackNumber() == 5  ); // inputPixelCollectionIntegrationTestC
      assert( mergedCollectionVector.at(5).trackNumber() == 1  ); // inputPixelCollectionIntegrationTestC
      assert( mergedCollectionVector.at(6).trackNumber() == 50 ); // inputPixelCollectionIntegrationTestB
      assert( mergedCollectionVector.at(7).trackNumber() == 1  ); // inputPixelCollectionIntegrationTestB
    }

};  // class CollectionMerger_test

}

CLASS_DEF (ISFTesting::TestHitCollection_t, 1234567890, 1)


int main()
{
  ISvcLocator* svcLoc = nullptr;
  Athena_test::initGaudi("CollectionMerger_test_jobOptions.txt", svcLoc);

  ISFTesting::CollectionMerger_test::test_empty_alg( svcLoc );
  ISFTesting::CollectionMerger_test::test_initializeVarHandleKey( svcLoc );
  ISFTesting::CollectionMerger_test::test_initializeVarHandleKey_emptyKey( svcLoc );
  ISFTesting::CollectionMerger_test::test_setupReadHandleKeyVector( svcLoc );
  ISFTesting::CollectionMerger_test::test_setupReadHandleKeyVector_emptyKey( svcLoc );
  ISFTesting::CollectionMerger_test::test_mergeCollections( svcLoc );
  ISFTesting::CollectionMerger_test::test_integration_empty( svcLoc );
  ISFTesting::CollectionMerger_test::test_integration_with_data( svcLoc );

  return 0;
}
