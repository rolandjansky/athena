/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author John Chapman
 * @brief Tests for MdtOverlay.
 */

#undef NDEBUG

// Framework
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"

// Tested AthAlgorithm
#include "../MdtOverlay/MdtOverlay.h"

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

  class MdtOverlay_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_alg = new MdtOverlay{"MdtOverlay", g_svcLoc};
      ASSERT_TRUE( m_alg->setProperties().isSuccess() );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
    }

    MdtOverlay* m_alg;
  };   // MdtOverlay_test fixture


  TEST_F(MdtOverlay_test, set_properties) {
    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isFailure() ); //inputs don't exist
  }

  TEST_F(MdtOverlay_test, empty_containers_alg_execute) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
    const unsigned int containerSize(1188);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
  }

  TEST_F(MdtOverlay_test, containers_with_matching_empty_collections) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG1"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(1);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG1"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG1'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG1'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS1'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS1"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->empty() );
  }

  TEST_F(MdtOverlay_test, containers_with_different_empty_collections) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG2"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(2);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG2"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG2'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG2'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS2'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS2"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->empty() );
    const MdtDigitCollection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->empty() );
  }

  TEST_F(MdtOverlay_test, containers_with_matching_collections_one_with_a_digit) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG3"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTDC(12);
    const int sigADC(13);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> sigDigit = std::make_unique<MdtDigit>(Identifier(12),sigTDC,sigADC,false);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG3"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG3'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG3'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS3'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS3"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const MdtDigit* outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->tdc()==sigTDC );
    ASSERT_TRUE( outputDigit1->adc()==sigADC );
    ASSERT_TRUE( outputDigit1->is_masked()==false );
  }

  TEST_F(MdtOverlay_test, containers_with_different_collections_one_digit_each) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG4"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(2);
    const int sigTDC(12);
    const int bkgTDC(13);
    const int sigADC(13);
    const int bkgADC(14);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> sigDigit = std::make_unique<MdtDigit>(Identifier(12),sigTDC,sigADC,false);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG4"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(12),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG4'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG4'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS4'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS4"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const MdtDigit* outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->tdc()==sigTDC );
    ASSERT_TRUE( outputDigit1->adc()==sigADC );
    ASSERT_TRUE( outputDigit1->is_masked()==false );
    const MdtDigitCollection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->size()==1 );
    const MdtDigit* outputDigit2 = outputCollection2->at(0);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->tdc()==bkgTDC );
    ASSERT_TRUE( outputDigit2->adc()==bkgADC );
    ASSERT_TRUE( outputDigit2->is_masked()==false );
  }

  TEST_F(MdtOverlay_test, containers_with_matching_collections_one_different_digit_each) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG5"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTDC(12);
    const int bkgTDC(12);
    const int sigADC(13);
    const int bkgADC(13);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> sigDigit = std::make_unique<MdtDigit>(Identifier(12),sigTDC,sigADC,false);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG5"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(13),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG5'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG5'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS5'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS5"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 );
    const MdtDigit* outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->tdc()==sigTDC );
    ASSERT_TRUE( outputDigit1->adc()==sigADC );
    ASSERT_TRUE( outputDigit1->is_masked()==false );
    const MdtDigit* outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->tdc()==sigTDC );
    ASSERT_TRUE( outputDigit2->adc()==sigADC );
    ASSERT_TRUE( outputDigit2->is_masked()==false );
  }

  TEST_F(MdtOverlay_test, containers_with_matching_collections_one_matching_digit_each) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG6"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTDC(12);
    const int bkgTDC(12);
    const int sigADC(13);
    const int bkgADC(13);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> sigDigit = std::make_unique<MdtDigit>(Identifier(12),sigTDC,sigADC,false);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG6"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(12),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG6'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG6'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS6'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS6"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const MdtDigit* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->tdc()==sigTDC ); // TDC values were the same for both digits
    ASSERT_TRUE( outputDigit->adc()==sigADC+bkgADC ); // Adds up the ADC values on matching digits
    ASSERT_TRUE( outputDigit->is_masked()==false );
  }

  TEST_F(MdtOverlay_test, two_digits_with_matching_id_signal_first) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG7"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTDC(11);
    const int bkgTDC(12);
    const int sigADC(13);
    const int bkgADC(13);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> sigDigit = std::make_unique<MdtDigit>(Identifier(12),sigTDC,sigADC,false);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG7"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(12),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG7'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG7'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS7'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS7"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const MdtDigit* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->tdc()==sigTDC ); // Use lowest TDC value
    ASSERT_TRUE( outputDigit->adc()==sigADC+bkgADC ); // Adds up the ADC values on matching digits
    ASSERT_TRUE( outputDigit->is_masked()==false );
  }

  TEST_F(MdtOverlay_test, two_digits_with_matching_id_bkg_first) {
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG8"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const int sigTDC(12);
    const int bkgTDC(11);
    const int sigADC(13);
    const int bkgADC(13);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> sigDigit = std::make_unique<MdtDigit>(Identifier(12),sigTDC,sigADC,false);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG8"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(12),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG8'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG8'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS8'";
    ASSERT_TRUE( m_alg->setProperty( "MainInputDigitKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OverlayInputDigitKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputDigitKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->overlayInitialize().isSuccess() );
    ASSERT_TRUE( m_alg->overlayExecute().isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS8"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const MdtDigit* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->tdc()==bkgTDC ); // Use lowest TDC value
    ASSERT_TRUE( outputDigit->adc()==sigADC+bkgADC ); // Adds up the ADC values on matching digits
    ASSERT_TRUE( outputDigit->is_masked()==false );
  }

} // <-- namespace OverlayTesting


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new OverlayTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
