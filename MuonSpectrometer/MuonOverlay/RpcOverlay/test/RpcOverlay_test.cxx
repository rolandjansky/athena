/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author John Chapman
 * @brief Tests for RpcOverlay.
 */

#undef NDEBUG

// Framework
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"

// Tested AthAlgorithm
#include "../RpcOverlay/RpcOverlay.h"

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

  class RpcOverlay_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_alg = new RpcOverlay{"RpcOverlay", g_svcLoc};
      ASSERT_TRUE( m_alg->setProperties().isSuccess() );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
    }

    RpcOverlay* m_alg;
  };   // RpcOverlay_test fixture


  TEST_F(RpcOverlay_test, set_properties) {
    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isFailure() ); //inputs don't exist
  }

  TEST_F(RpcOverlay_test, empty_containers_alg_execute) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG"};
    const unsigned int containerSize(1188);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
  }

  TEST_F(RpcOverlay_test, containers_with_matching_empty_collections) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG1"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(1);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> sigCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG1"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> bkgCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG1'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG1'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS1'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<RpcDigitContainer> outputDataHandle{"StoreGateSvc+RPC_DIGITS1"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const RpcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->empty() );
  }

  TEST_F(RpcOverlay_test, containers_with_different_empty_collections) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG2"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(2);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> sigCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG2"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> bkgCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG2'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG2'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS2'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<RpcDigitContainer> outputDataHandle{"StoreGateSvc+RPC_DIGITS2"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const RpcDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->empty() );
    const RpcDigitCollection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->empty() );
  }

  TEST_F(RpcOverlay_test, containers_with_matching_collections_one_with_a_digit) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG3"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTime(12);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> sigCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> sigDigit = std::make_unique<RpcDigit>(Identifier(12),sigTime);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG3"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> bkgCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG3'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG3'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS3'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<RpcDigitContainer> outputDataHandle{"StoreGateSvc+RPC_DIGITS3"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const RpcDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const RpcDigit* outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->time()==sigTime );
  }

  TEST_F(RpcOverlay_test, containers_with_different_collections_one_digit_each) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG4"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(2);
    const int sigTime(12);
    const int bkgTime(13);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> sigCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> sigDigit = std::make_unique<RpcDigit>(Identifier(12),sigTime);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG4"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> bkgCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> bkgDigit = std::make_unique<RpcDigit>(Identifier(12),bkgTime);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG4'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG4'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS4'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<RpcDigitContainer> outputDataHandle{"StoreGateSvc+RPC_DIGITS4"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const RpcDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const RpcDigit* outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->time()==sigTime );
    const RpcDigitCollection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->size()==1 );
    const RpcDigit* outputDigit2 = outputCollection2->at(0);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->time()==bkgTime );
  }

  TEST_F(RpcOverlay_test, containers_with_matching_collections_one_different_digit_each) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG5"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTime(12);
    const int bkgTime(12);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> sigCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> sigDigit = std::make_unique<RpcDigit>(Identifier(12),sigTime);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG5"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> bkgCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> bkgDigit = std::make_unique<RpcDigit>(Identifier(13),bkgTime);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG5'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG5'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS5'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<RpcDigitContainer> outputDataHandle{"StoreGateSvc+RPC_DIGITS5"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const RpcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 );
    const RpcDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->time()==sigTime );
    const RpcDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->time()==sigTime );
  }

  TEST_F(RpcOverlay_test, containers_with_matching_collections_one_matching_digit_each) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG6"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTime(12);
    const int bkgTime(12);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> sigCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> sigDigit = std::make_unique<RpcDigit>(Identifier(12),sigTime);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG6"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> bkgCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> bkgDigit = std::make_unique<RpcDigit>(Identifier(12),bkgTime);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG6'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG6'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS6'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<RpcDigitContainer> outputDataHandle{"StoreGateSvc+RPC_DIGITS6"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const RpcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 ); //FIXME Is this the correct behaviour?
    const RpcDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->time()==sigTime ); // Time values were the same for both digits
    const RpcDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->time()==bkgTime ); // Time values were the same for both digits
  }

  TEST_F(RpcOverlay_test, two_digits_with_matching_id_signal_first) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG7"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTime(11);
    const int bkgTime(12);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> sigCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> sigDigit = std::make_unique<RpcDigit>(Identifier(12),sigTime);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG7"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> bkgCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> bkgDigit = std::make_unique<RpcDigit>(Identifier(12),bkgTime);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG7'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG7'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS7'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<RpcDigitContainer> outputDataHandle{"StoreGateSvc+RPC_DIGITS7"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const RpcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 ); //FIXME Is this the correct behaviour?
    const RpcDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->time()==bkgTime );
    const RpcDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->time()==sigTime ); //FIXME?? Digits are not time ordered?
  }

  TEST_F(RpcOverlay_test, two_digits_with_matching_id_bkg_first) {
    SG::WriteHandle<RpcDigitContainer> inputSigDataHandle{"StoreGateSvc+RPC_DIGITS_SIG8"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTime(12);
    const int bkgTime(11);
    inputSigDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> sigCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),sigElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> sigDigit = std::make_unique<RpcDigit>(Identifier(12),sigTime);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<RpcDigitContainer> inputBkgDataHandle{"StoreGateSvc+RPC_DIGITS_BKG8"};
    inputBkgDataHandle = std::make_unique<RpcDigitContainer>(containerSize);
    std::unique_ptr<RpcDigitCollection> bkgCollection = std::make_unique<RpcDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an RpcDigit
    std::unique_ptr<RpcDigit> bkgDigit = std::make_unique<RpcDigit>(Identifier(12),bkgTime);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+RPC_DIGITS_SIG8'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+RPC_DIGITS_BKG8'";
    std::string    outputPropertyValue = "'StoreGateSvc+RPC_DIGITS8'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<RpcDigitContainer> outputDataHandle{"StoreGateSvc+RPC_DIGITS8"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const RpcDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 ); //FIXME Is this the correct behaviour?
    const RpcDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->time()==bkgTime );
    const RpcDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->time()==sigTime );
  }

} // <-- namespace OverlayTesting


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new OverlayTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
