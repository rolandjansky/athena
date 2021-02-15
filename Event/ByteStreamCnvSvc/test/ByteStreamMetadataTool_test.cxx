/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */
/** Tests for ByteStreamMetadata management.
 *
 * The various operations that should be performed by the
 * ByteStreamMetadataTool when transferring objects from the input to the
 * metadata store for use by clients.
 *
 * @author Frank Berghaus <fberghaus@anl.gov>
 * @date Dec 4, 2020
 */
#include <ostream>
#include <memory>
#include <set>

#include "GoogleTestTools/InitGaudiGoogleTest.h"
#include "TestTools/initGaudi.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaKernel/IMetaDataTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/setupStoreGate.h"
#include "../src/ByteStreamMetadataTool.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"


namespace Athena_test {

  class ByteStreamMetadataToolTest : public InitGaudiGoogleTest {
   public:
    ByteStreamMetadataToolTest() : InitGaudiGoogleTest( MSG::INFO ) {}

    void SetUp() override {
      assert(m_inputStore.retrieve().isSuccess());
      assert(m_metaStore.retrieve().isSuccess());

      assert(m_tool.retrieve().isSuccess());
      assert(m_tool->initialize().isSuccess());
    }

    ToolHandle< IMetaDataTool > m_tool{"ByteStreamMetadataTool"};
    ServiceHandle< StoreGateSvc > m_inputStore{
      "StoreGateSvc/InputMetaDataStore", "ByteStreamMetadataToolTest"};
    ServiceHandle< StoreGateSvc > m_metaStore{
      "StoreGateSvc/MetaDataStore", "ByteStreamMetadataToolTest"};

    const ByteStreamMetadata testObject{
      1u,
      50u,
      50u,
      1u,
      1u,
      1ul<<31,
      1ul<<33,
      1u,
      6500u,
      "testObject", // GUID
      "stream",
      "project",
      1u,
      std::vector<std::string>()};

    const ByteStreamMetadata diffObject{
      2u,
      20u,
      20u,
      1u,
      1u,
      1ul<<30,
      1ul<<32,
      1u,
      6500u,
      "diffObject", // GUID
      "stream2",
      "project",
      2u,
      std::vector<std::string>()};
  };

  /**
   * An empty input store should not offend the tool and do nothing
   */
  // cppcheck-suppress syntaxError
  TEST_F(ByteStreamMetadataToolTest, emptyInput) {
    EXPECT_TRUE(m_tool->beginInputFile().isSuccess());
    EXPECT_TRUE(
        !m_metaStore->contains< ByteStreamMetadataContainer >(
            "ByteStreamMetadata"));
  }

  /**
   * Test tool with ByteStreamMetadata object in input
   *
   * Add a ByteStreamMetadata object to input. Tool should generate a
   * ByteStreamMetadataContainer that one entry in the output
   */
  TEST_F(ByteStreamMetadataToolTest, objectInput) {
    auto testBSM = std::make_unique< ByteStreamMetadata >(testObject);
    const ByteStreamMetadata * in = testBSM.get();
    assert(
        m_inputStore->record< ByteStreamMetadata >(
            std::move(testBSM), "Test").isSuccess());
    EXPECT_TRUE(m_tool->beginInputFile().isSuccess());
    EXPECT_TRUE(
        m_metaStore->contains< ByteStreamMetadataContainer >(
            "Test"));
    auto bsmc =
        m_metaStore->tryConstRetrieve< ByteStreamMetadataContainer >("Test");
    assert(bsmc != nullptr);
    size_t expectedSize = 1;
    EXPECT_EQ(bsmc->size(), expectedSize);
    const ByteStreamMetadata * out = bsmc->at(0);
    EXPECT_EQ(*in, *out);
    assert(m_inputStore->clearStore().isSuccess());
    assert(m_metaStore->clearStore().isSuccess());
  }

  /**
   * Test tool with ByteStreamMetadataContainer in input
   *
   * Add a ByteStreamMetadataContainer object to input. Tool should generate a
   * ByteStreamMetadataContainer with equal entries
   */
  TEST_F(ByteStreamMetadataToolTest, containerInput) {
    auto testBSMC = std::make_unique< ByteStreamMetadataContainer >();
    testBSMC->push_back(std::make_unique< ByteStreamMetadata >(testObject));
    const ByteStreamMetadataContainer * in = testBSMC.get();
    assert(
        m_inputStore->record< ByteStreamMetadataContainer >(
            std::move(testBSMC), "Test").isSuccess());
    EXPECT_TRUE(m_tool->beginInputFile().isSuccess());
    EXPECT_TRUE(
        m_metaStore->contains< ByteStreamMetadataContainer >(
            "Test"));
    auto out =
        m_metaStore->tryConstRetrieve< ByteStreamMetadataContainer >("Test");
    assert(out != nullptr);
    EXPECT_EQ(in->size(), out->size());
    EXPECT_EQ(*in->at(0), *out->at(0));
    assert(m_inputStore->clearStore().isSuccess());
    assert(m_metaStore->clearStore().isSuccess());
  }

  /**
   * Test tool with equal object in input and output
   *
   * An input object equal to one that already exists in MetaDataStore should
   * result in no new addition
   */
  TEST_F(ByteStreamMetadataToolTest, inputSame) {
    auto inputBSMC = std::make_unique< ByteStreamMetadataContainer >();
    inputBSMC->push_back(std::make_unique< ByteStreamMetadata >(testObject));
    assert(
        m_inputStore->record< ByteStreamMetadataContainer >(
            std::move(inputBSMC), "Test").isSuccess());

    auto copyBSMC = std::make_unique< ByteStreamMetadataContainer >();
    copyBSMC->push_back(std::make_unique< ByteStreamMetadata >(testObject));
    size_t expectedSize = copyBSMC->size();
    assert(
        m_metaStore->record< ByteStreamMetadataContainer >(
            std::move(copyBSMC), "Test").isSuccess());

    EXPECT_TRUE(m_tool->beginInputFile().isSuccess());
    EXPECT_TRUE(
        m_metaStore->contains< ByteStreamMetadataContainer >(
            "Test"));
    auto out =
        m_metaStore->tryConstRetrieve< ByteStreamMetadataContainer >("Test");
    assert(out != nullptr);
    EXPECT_EQ(out->size(), expectedSize);

    assert(m_inputStore->clearStore().isSuccess());
    assert(m_metaStore->clearStore().isSuccess());
  }

  /**
   * Test tool adding new objects from input to exisint output
   *
   * An input object not equal to one that already exists should result in a
   * and additional entry in the MetaDataStore container
   */
  TEST_F(ByteStreamMetadataToolTest, inputNew) {
    auto inputBSMC = std::make_unique< ByteStreamMetadataContainer >();
    inputBSMC->push_back(std::make_unique< ByteStreamMetadata >(testObject));
    assert(
        m_inputStore->record< ByteStreamMetadataContainer >(
            std::move(inputBSMC), "Test").isSuccess());

    auto oldBSMC = std::make_unique< ByteStreamMetadataContainer >();
    oldBSMC->push_back(std::make_unique< ByteStreamMetadata >(diffObject));
    size_t expectedSize = 1 + oldBSMC->size();
    assert(
        m_metaStore->record< ByteStreamMetadataContainer >(
            std::move(oldBSMC), "Test").isSuccess());

    EXPECT_TRUE(m_tool->beginInputFile().isSuccess());
    EXPECT_TRUE(
        m_metaStore->contains< ByteStreamMetadataContainer >(
            "Test"));
    auto out =
        m_metaStore->tryConstRetrieve< ByteStreamMetadataContainer >("Test");
    assert(out != nullptr);
    EXPECT_EQ(out->size(), expectedSize);

    assert(m_inputStore->clearStore().isSuccess());
    assert(m_metaStore->clearStore().isSuccess());
  }

}  // namespace Athena_test

int main(int argc, char ** argv) {
  ISvcLocator * svcLocator = nullptr;
  if (!Athena_test::initGaudi(svcLocator)) {
    std::cerr << "This test can not be run" << std::endl;
    return 1;
  }
  assert(svcLocator);

  Athena_test::setupStoreGate(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
