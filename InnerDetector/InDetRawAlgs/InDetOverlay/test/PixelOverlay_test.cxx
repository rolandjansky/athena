/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author John Chapman
 * @brief Tests for PixelOverlay.
 */

#undef NDEBUG

// Framework
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"

// Tested AthAlgorithm
#include "../InDetOverlay/PixelOverlay.h"

namespace OverlayTesting {

  // needed every time an AthAlgorithm, AthAlgTool or AthService is instantiated
  ISvcLocator* g_svcLoc = nullptr;

  // global test environment takes care of setting up Gaudi
  class GaudiEnvironment : public ::testing::Environment {
  protected:
    virtual void SetUp() override {
      Athena_test::initGaudi(OverlayTesting::g_svcLoc);
    }
  };

  class PixelOverlay_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_alg = new PixelOverlay{"PixelOverlay", g_svcLoc};
      ASSERT_TRUE( m_alg->setProperties().isSuccess() );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
    }

    PixelOverlay* m_alg;
  };   // PixelOverlay_test fixture


  TEST_F(PixelOverlay_test, set_properties) {
    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isFailure() ); //inputs don't exist
  }

  TEST_F(PixelOverlay_test, empty_containers_alg_execute) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG"};
    const unsigned int containerSize(1188);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
  }

  TEST_F(PixelOverlay_test, containers_with_matching_empty_collections) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG1"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(1);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG1"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG1'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG1'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs1'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs1"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->empty() );
  }

  TEST_F(PixelOverlay_test, containers_with_different_empty_collections) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG2"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(2);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG2"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG2'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG2'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs2'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs2"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->empty() );
    const PixelRDO_Collection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->empty() );
  }

  TEST_F(PixelOverlay_test, containers_with_matching_collections_one_with_an_RDO) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG3"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const unsigned int sigToT(10);
    const unsigned int sigBCID(2);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> sigDigit = std::make_unique<Pixel1RawData>(Identifier(12),sigToT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG3"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG3'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG3'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs3'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs3"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const PixelRDORawData* outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getToT()==sigToT );
    ASSERT_TRUE( outputDigit1->getBCID()==sigBCID-1 );
  }

  TEST_F(PixelOverlay_test, containers_with_different_collections_one_RDO_each) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG4"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(2);
    const unsigned int sigToT(10);
    const unsigned int bkgToT(10);
    const unsigned int sigBCID(2);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> sigDigit = std::make_unique<Pixel1RawData>(Identifier(12),sigToT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG4"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> bkgDigit = std::make_unique<Pixel1RawData>(Identifier(12),bkgToT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG4'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG4'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs4'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs4"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const PixelRDORawData* outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getToT()==sigToT );
    ASSERT_TRUE( outputDigit1->getBCID()==sigBCID-1 );
    const PixelRDO_Collection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->size()==1 );
    const PixelRDORawData* outputDigit2 = outputCollection2->at(0);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->getToT()==bkgToT );
    ASSERT_TRUE( outputDigit2->getBCID()==bkgBCID-1 );
  }

  TEST_F(PixelOverlay_test, containers_with_matching_collections_one_different_RDO_each) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG5"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const unsigned int sigToT(11);
    const unsigned int bkgToT(10);
    const unsigned int sigBCID(1);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> sigDigit = std::make_unique<Pixel1RawData>(Identifier(12),sigToT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG5"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> bkgDigit = std::make_unique<Pixel1RawData>(Identifier(13),bkgToT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG5'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG5'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs5'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs5"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 );
    const PixelRDORawData* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getToT()==sigToT ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit1->getBCID()==sigBCID-1 ); // RDOs in same collection sorted according to Identifier
    const PixelRDORawData* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->getToT()==bkgToT ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit2->getBCID()==bkgBCID-1 ); // RDOs in same collection sorted according to Identifier
  }

  TEST_F(PixelOverlay_test, containers_with_matching_collections_one_matching_RDO_each) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG6"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const unsigned int sigToT(10);
    const unsigned int bkgToT(10);
    const unsigned int sigBCID(2);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> sigDigit = std::make_unique<Pixel1RawData>(Identifier(12),sigToT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG6"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> bkgDigit = std::make_unique<Pixel1RawData>(Identifier(12),bkgToT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG6'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG6'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs6'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );

    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs6"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const PixelRDORawData* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getToT()==bkgToT ); // Bkg RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getBCID()==bkgBCID-1 ); // Bkg RDO taken in case of matching Identifiers
  }

  TEST_F(PixelOverlay_test, two_RDOs_with_matching_id_signal_first) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG7"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const unsigned int sigToT(11);
    const unsigned int bkgToT(10);
    const unsigned int sigBCID(1);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> sigDigit = std::make_unique<Pixel1RawData>(Identifier(12),sigToT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG7"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> bkgDigit = std::make_unique<Pixel1RawData>(Identifier(12),bkgToT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG7'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG7'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs7'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs7"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const PixelRDORawData* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getToT()==bkgToT ); // Bkg RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getBCID()==bkgBCID-1 ); // Bkg RDO taken in case of matching Identifiers
  }

  TEST_F(PixelOverlay_test, two_RDOs_with_matching_id_bkg_first) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG8"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const unsigned int sigToT(11);
    const unsigned int bkgToT(10);
    const unsigned int sigBCID(2);
    const unsigned int bkgBCID(1);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> sigDigit = std::make_unique<Pixel1RawData>(Identifier(12),sigToT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG8"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> bkgDigit = std::make_unique<Pixel1RawData>(Identifier(12),bkgToT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG8'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG8'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs8'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs8"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const PixelRDORawData* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getToT()==bkgToT ); // Bkg RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getBCID()==bkgBCID-1 ); // Bkg RDO taken in case of matching Identifiers
  }

  TEST_F(PixelOverlay_test, containers_with_matching_collections_one_different_RDO_each_v2) {
    SG::WriteHandle<PixelRDO_Container> inputSigDataHandle{"StoreGateSvc+PixelRDOs_SIG9"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const unsigned int sigToT(11);
    const unsigned int bkgToT(10);
    const unsigned int sigBCID(1);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> sigCollection = std::make_unique<PixelRDO_Collection>(sigElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> sigDigit = std::make_unique<Pixel1RawData>(Identifier(13),sigToT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<PixelRDO_Container> inputBkgDataHandle{"StoreGateSvc+PixelRDOs_BKG9"};
    inputBkgDataHandle = std::make_unique<PixelRDO_Container>(containerSize);
    std::unique_ptr<PixelRDO_Collection> bkgCollection = std::make_unique<PixelRDO_Collection>(bkgElementHash);
    //Add a Pixel1RawData object
    std::unique_ptr<Pixel1RawData> bkgDigit = std::make_unique<Pixel1RawData>(Identifier(12),bkgToT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+PixelRDOs_SIG9'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+PixelRDOs_BKG9'";
    std::string    outputPropertyValue = "'StoreGateSvc+PixelRDOs9'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<PixelRDO_Container> outputDataHandle{"StoreGateSvc+PixelRDOs9"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const PixelRDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 );
    const PixelRDORawData* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getToT()==bkgToT ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit1->getBCID()==bkgBCID-1 ); // RDOs in same collection sorted according to Identifier
    const PixelRDORawData* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->getToT()==sigToT ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit2->getBCID()==sigBCID-1 ); // RDOs in same collection sorted according to Identifier
  }

} // <-- namespace OverlayTesting


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new OverlayTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
