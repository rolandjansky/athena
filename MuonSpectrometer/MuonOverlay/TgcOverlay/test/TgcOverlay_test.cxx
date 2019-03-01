/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author John Chapman
 * @brief Tests for TgcOverlay.
 */

#undef NDEBUG

// Framework
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"

// Tested AthAlgorithm
#include "../TgcOverlay/TgcOverlay.h"

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

  class TgcOverlay_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_alg = new TgcOverlay{"TgcOverlay", g_svcLoc};
      ASSERT_TRUE( m_alg->setProperties().isSuccess() );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
    }

    TgcOverlay* m_alg;
  };   // TgcOverlay_test fixture


  TEST_F(TgcOverlay_test, set_properties) {
    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isFailure() ); //inputs don't exist
  }

  TEST_F(TgcOverlay_test, empty_containers_alg_execute) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG"};
    const unsigned int containerSize(1188);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
  }

  TEST_F(TgcOverlay_test, containers_with_matching_empty_collections) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG1"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(1);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> sigCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG1"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> bkgCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG1'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG1'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS1'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<TgcDigitContainer> outputDataHandle{"StoreGateSvc+TGC_DIGITS1"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const TgcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->empty() );
  }

  TEST_F(TgcOverlay_test, containers_with_different_empty_collections) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG2"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(2);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> sigCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG2"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> bkgCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG2'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG2'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS2'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<TgcDigitContainer> outputDataHandle{"StoreGateSvc+TGC_DIGITS2"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const TgcDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->empty() );
    const TgcDigitCollection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->empty() );
  }

  TEST_F(TgcOverlay_test, containers_with_matching_collections_one_with_a_digit) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG3"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigBCTAG(12);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> sigCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> sigDigit = std::make_unique<TgcDigit>(Identifier(12),sigBCTAG);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG3"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> bkgCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG3'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG3'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS3'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<TgcDigitContainer> outputDataHandle{"StoreGateSvc+TGC_DIGITS3"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const TgcDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const TgcDigit* outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->bcTag()==sigBCTAG );
  }

  TEST_F(TgcOverlay_test, containers_with_different_collections_one_digit_each) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG4"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(2);
    const int sigBCTAG(12);
    const int bkgBCTAG(13);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> sigCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> sigDigit = std::make_unique<TgcDigit>(Identifier(12),sigBCTAG);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG4"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> bkgCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> bkgDigit = std::make_unique<TgcDigit>(Identifier(12),bkgBCTAG);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG4'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG4'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS4'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<TgcDigitContainer> outputDataHandle{"StoreGateSvc+TGC_DIGITS4"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const TgcDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const TgcDigit* outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->bcTag()==sigBCTAG );
    const TgcDigitCollection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->size()==1 );
    const TgcDigit* outputDigit2 = outputCollection2->at(0);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->bcTag()==bkgBCTAG );
  }

  TEST_F(TgcOverlay_test, containers_with_matching_collections_one_different_digit_each) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG5"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigBCTAG(12);
    const int bkgBCTAG(12);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> sigCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> sigDigit = std::make_unique<TgcDigit>(Identifier(12),sigBCTAG);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG5"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> bkgCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> bkgDigit = std::make_unique<TgcDigit>(Identifier(13),bkgBCTAG);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG5'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG5'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS5'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<TgcDigitContainer> outputDataHandle{"StoreGateSvc+TGC_DIGITS5"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const TgcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 );
    const TgcDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->bcTag()==bkgBCTAG ); // bkg digit first in collection
    const TgcDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->bcTag()==sigBCTAG ); // sig digits added to end of collection
  }

  TEST_F(TgcOverlay_test, containers_with_matching_collections_one_matching_digit_each) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG6"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigBCTAG(12);
    const int bkgBCTAG(12);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> sigCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> sigDigit = std::make_unique<TgcDigit>(Identifier(12),sigBCTAG);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG6"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> bkgCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> bkgDigit = std::make_unique<TgcDigit>(Identifier(12),bkgBCTAG);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG6'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG6'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS6'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<TgcDigitContainer> outputDataHandle{"StoreGateSvc+TGC_DIGITS6"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const TgcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 ); // FIXME Is this the correct behaviour?
    const TgcDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->bcTag()==bkgBCTAG ); // bkg digit first in collection
    const TgcDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->bcTag()==sigBCTAG ); // sig digits added to end of collection
  }

  TEST_F(TgcOverlay_test, two_digits_with_matching_id_signal_first) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG7"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigBCTAG(11);
    const int bkgBCTAG(12);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> sigCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> sigDigit = std::make_unique<TgcDigit>(Identifier(12),sigBCTAG);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG7"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> bkgCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> bkgDigit = std::make_unique<TgcDigit>(Identifier(12),bkgBCTAG);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG7'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG7'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS7'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<TgcDigitContainer> outputDataHandle{"StoreGateSvc+TGC_DIGITS7"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const TgcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 ); // FIXME Is this the correct behaviour?
    const TgcDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->bcTag()==bkgBCTAG ); // bkg digit first in collection
    const TgcDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->bcTag()==sigBCTAG ); // sig digits added to end of collection
  }

  TEST_F(TgcOverlay_test, two_digits_with_matching_id_bkg_first) {
    SG::WriteHandle<TgcDigitContainer> inputSigDataHandle{"StoreGateSvc+TGC_DIGITS_SIG8"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigBCTAG(12);
    const int bkgBCTAG(11);
    inputSigDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> sigCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> sigDigit = std::make_unique<TgcDigit>(Identifier(12),sigBCTAG);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<TgcDigitContainer> inputBkgDataHandle{"StoreGateSvc+TGC_DIGITS_BKG8"};
    inputBkgDataHandle = std::make_unique<TgcDigitContainer>(containerSize);
    std::unique_ptr<TgcDigitCollection> bkgCollection = std::make_unique<TgcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an TgcDigit
    std::unique_ptr<TgcDigit> bkgDigit = std::make_unique<TgcDigit>(Identifier(12),bkgBCTAG);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+TGC_DIGITS_SIG8'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+TGC_DIGITS_BKG8'";
    std::string    outputPropertyValue = "'StoreGateSvc+TGC_DIGITS8'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<TgcDigitContainer> outputDataHandle{"StoreGateSvc+TGC_DIGITS8"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const TgcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 ); // FIXME Is this the correct behaviour?
    const TgcDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->bcTag()==bkgBCTAG ); // bkg digit first in collection
    const TgcDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->bcTag()==sigBCTAG ); // sig digits added to end of collection
  }

} // <-- namespace OverlayTesting


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new OverlayTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
