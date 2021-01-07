/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
      ASSERT_TRUE( g_svcLoc->service("StoreGateSvc", m_sg) );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
      ASSERT_TRUE( m_sg->clearStore().isSuccess() );
    }

    MdtOverlay* m_alg{};
    StoreGateSvc* m_sg{};
  };   // MdtOverlay_test fixture


  TEST_F(MdtOverlay_test, set_properties) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isFailure() ); //inputs don't exist
  }

  TEST_F(MdtOverlay_test, empty_containers_alg_execute) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
    const unsigned int containerSize(1188);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
  }

  TEST_F(MdtOverlay_test, containers_with_matching_empty_collections) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(1);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->empty() );
  }

  TEST_F(MdtOverlay_test, containers_with_different_empty_collections) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(2);
    inputSigDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> sigCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const MdtDigitCollection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->empty() );
    const MdtDigitCollection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->empty() );
  }

  TEST_F(MdtOverlay_test, containers_with_matching_collections_one_with_a_digit) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
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
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS"};
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
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
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
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(12),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS"};
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
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
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
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(13),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS"};
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
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
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
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(12),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS"};
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
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
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
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(12),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS"};
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
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<MdtDigitContainer> inputSigDataHandle{"StoreGateSvc+MDT_DIGITS_SIG"};
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
    SG::WriteHandle<MdtDigitContainer> inputBkgDataHandle{"StoreGateSvc+MDT_DIGITS_BKG"};
    inputBkgDataHandle = std::make_unique<MdtDigitContainer>(containerSize);
    std::unique_ptr<MdtDigitCollection> bkgCollection = std::make_unique<MdtDigitCollection>(Identifier(1234),bkgElementHash);
    //Add an MdtDigit
    std::unique_ptr<MdtDigit> bkgDigit = std::make_unique<MdtDigit>(Identifier(12),bkgTDC,bkgADC,false);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+MDT_DIGITS_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+MDT_DIGITS_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+MDT_DIGITS'";
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",    inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey", inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey",      outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<MdtDigitContainer> outputDataHandle{"StoreGateSvc+MDT_DIGITS"};
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
