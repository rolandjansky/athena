/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @class BCMOverlay_test

  Tests for BCMOverlay
      @author  Jakob Novak <jakob.novak@cern.ch>

*/

#undef NDEBUG

// Framework
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"

// Tested AthAlgorithm
#include "../InDetOverlay/BCMOverlay.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // Use of global g_svcLoc is not thread safe.

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

  class BCMOverlay_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_alg = new BCMOverlay{"BCMOverlay", g_svcLoc};
      ASSERT_TRUE( m_alg->setProperties().isSuccess() );
      ASSERT_TRUE( g_svcLoc->service("StoreGateSvc", m_sg) );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
      ASSERT_TRUE( m_sg->clearStore().isSuccess() );
    }

    BCMOverlay* m_alg{};
    StoreGateSvc* m_sg{};
  };   // BCMOverlay_test fixture


  TEST_F(BCMOverlay_test, set_properties) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isFailure() ); //inputs don't exist
  }

  TEST_F(BCMOverlay_test, empty_containers_alg_execute) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG"};
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
  }

  TEST_F(BCMOverlay_test, containers_with_empty_collections) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG1"};
    const unsigned int sigChannel(0);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG1"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG1'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG1'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs1'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs1"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->empty() );
  }

  TEST_F(BCMOverlay_test, containers_with_empty_bkg_collection) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG2"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(5);
    const unsigned int sigPulse1Width(8);
    const unsigned int sigPulse2Position(0);
    const unsigned int sigPulse2Width(0);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG2"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG2'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG2'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs2'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isFailure() ); //signal and background collection size mismatch
  }

  TEST_F(BCMOverlay_test, containers_with_pulses_from_different_channels) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG3"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(0);
    const unsigned int sigPulse1Width(0);
    const unsigned int sigPulse2Position(0);
    const unsigned int sigPulse2Width(0);
    const unsigned int bkgChannel(1);
    const unsigned int bkgPulse1Position(0);
    const unsigned int bkgPulse1Width(0);
    const unsigned int bkgPulse2Position(0);
    const unsigned int bkgPulse2Width(0);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG3"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG3'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG3'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs3'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isFailure() ); //signal and background channel mismatch
  }

  TEST_F(BCMOverlay_test, containers_with_two_independent_pulses) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG4"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(5);
    const unsigned int sigPulse1Width(8);
    const unsigned int sigPulse2Position(0);
    const unsigned int sigPulse2Width(0);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(39);
    const unsigned int bkgPulse1Width(10);
    const unsigned int bkgPulse2Position(0);
    const unsigned int bkgPulse2Width(0);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG4"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG4'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG4'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs4'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs4"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==sigPulse1Position ); // First pulse is signal pulse
    ASSERT_TRUE( outputDigit->getPulse1Width()==sigPulse1Width );
    ASSERT_TRUE( outputDigit->getPulse2Position()==bkgPulse1Position ); // Second pulse is background pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==bkgPulse1Width );
  }

  TEST_F(BCMOverlay_test, containers_with_two_pulses_affecting_each_other) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG5"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(12);
    const unsigned int sigPulse1Width(9);
    const unsigned int sigPulse2Position(0);
    const unsigned int sigPulse2Width(0);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(5);
    const unsigned int bkgPulse1Width(3);
    const unsigned int bkgPulse2Position(0);
    const unsigned int bkgPulse2Width(0);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG5"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG5'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG5'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs5'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs5"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==bkgPulse1Position ); // First pulse is background pulse
    ASSERT_TRUE( outputDigit->getPulse1Width()==bkgPulse1Width );
    ASSERT_TRUE( outputDigit->getPulse2Position()==11 ); // Second pulse is broader signal pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==10 );
  }

  TEST_F(BCMOverlay_test, containers_with_two_merging_pulses) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG6"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(5);
    const unsigned int sigPulse1Width(3);
    const unsigned int sigPulse2Position(0);
    const unsigned int sigPulse2Width(0);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(11);
    const unsigned int bkgPulse1Width(3);
    const unsigned int bkgPulse2Position(0);
    const unsigned int bkgPulse2Width(0);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG6"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG6'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG6'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs6'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs6"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==sigPulse1Position ); // Merged pulse
    ASSERT_TRUE( outputDigit->getPulse1Width()==9 );
    ASSERT_TRUE( outputDigit->getPulse2Position()==0 ); // No second pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==0 );
  }

  TEST_F(BCMOverlay_test, containers_with_one_signal_and_two_independent_background_pulses) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG7"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(25);
    const unsigned int sigPulse1Width(9);
    const unsigned int sigPulse2Position(0);
    const unsigned int sigPulse2Width(0);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(5);
    const unsigned int bkgPulse1Width(9);
    const unsigned int bkgPulse2Position(45);
    const unsigned int bkgPulse2Width(9);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG7"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG7'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG7'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs7'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs7"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==bkgPulse1Position ); // First pulse is first background pulse
    ASSERT_TRUE( outputDigit->getPulse1Width()==bkgPulse1Width );
    ASSERT_TRUE( outputDigit->getPulse2Position()==sigPulse1Position ); // Second pulse is signal pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==sigPulse1Width );
  }

  TEST_F(BCMOverlay_test, containers_with_two_signal_and_one_independent_background_pulse) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG8"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(5);
    const unsigned int sigPulse1Width(9);
    const unsigned int sigPulse2Position(45);
    const unsigned int sigPulse2Width(9);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(25);
    const unsigned int bkgPulse1Width(9);
    const unsigned int bkgPulse2Position(0);
    const unsigned int bkgPulse2Width(0);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG8"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG8'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG8'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs8'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs8"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==sigPulse1Position ); // First pulse is first signal pulse
    ASSERT_TRUE( outputDigit->getPulse1Width()==sigPulse1Width );
    ASSERT_TRUE( outputDigit->getPulse2Position()==bkgPulse1Position ); // Second pulse is background pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==bkgPulse1Width );
  }

  TEST_F(BCMOverlay_test, containers_with_three_pulses_two_merging) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG9"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(5);
    const unsigned int sigPulse1Width(3);
    const unsigned int sigPulse2Position(0);
    const unsigned int sigPulse2Width(0);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(11);
    const unsigned int bkgPulse1Width(3);
    const unsigned int bkgPulse2Position(45);
    const unsigned int bkgPulse2Width(9);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG9"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG9'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG9'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs9'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs9"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==sigPulse1Position ); // First pulse is merged
    ASSERT_TRUE( outputDigit->getPulse1Width()==9 );
    ASSERT_TRUE( outputDigit->getPulse2Position()==bkgPulse2Position ); // Second pulse is second background pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==bkgPulse2Width );
  }

  TEST_F(BCMOverlay_test, containers_with_one_signal_two_bkg_merging_pulses) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG10"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(11);
    const unsigned int sigPulse1Width(3);
    const unsigned int sigPulse2Position(0);
    const unsigned int sigPulse2Width(0);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(5);
    const unsigned int bkgPulse1Width(3);
    const unsigned int bkgPulse2Position(17);
    const unsigned int bkgPulse2Width(3);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG10"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG10'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG10'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs10'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs10"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==bkgPulse1Position ); // Merged pulse
    ASSERT_TRUE( outputDigit->getPulse1Width()==15 );
    ASSERT_TRUE( outputDigit->getPulse2Position()==0 ); // No second pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==0 );
  }

  TEST_F(BCMOverlay_test, containers_with_one_bkg_two_signal_merging_pulses) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG11"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(5);
    const unsigned int sigPulse1Width(3);
    const unsigned int sigPulse2Position(17);
    const unsigned int sigPulse2Width(3);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(11);
    const unsigned int bkgPulse1Width(3);
    const unsigned int bkgPulse2Position(0);
    const unsigned int bkgPulse2Width(0);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG11"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG11'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG11'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs11'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs11"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==sigPulse1Position ); // Merged pulse
    ASSERT_TRUE( outputDigit->getPulse1Width()==15 );
    ASSERT_TRUE( outputDigit->getPulse2Position()==0 ); // No second pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==0 );
  }

  TEST_F(BCMOverlay_test, containers_with_four_pulses_two_merging) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<BCM_RDO_Container> inputSigDataHandle{"StoreGateSvc+BCM_RDOs_SIG12"};
    const unsigned int sigChannel(0);
    const unsigned int sigPulse1Position(5);
    const unsigned int sigPulse1Width(3);
    const unsigned int sigPulse2Position(50);
    const unsigned int sigPulse2Width(9);
    const unsigned int bkgChannel(0);
    const unsigned int bkgPulse1Position(11);
    const unsigned int bkgPulse1Width(3);
    const unsigned int bkgPulse2Position(30);
    const unsigned int bkgPulse2Width(9);
    inputSigDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> sigCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> sigDigit = std::make_unique<BCM_RawData>(sigChannel,sigPulse1Position,sigPulse1Width,sigPulse2Position,sigPulse2Width,0,0,0,0);
    sigCollection->push_back(sigDigit.release());
    inputSigDataHandle->push_back(sigCollection.get());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<BCM_RDO_Container> inputBkgDataHandle{"StoreGateSvc+BCM_RDOs_BKG12"};
    inputBkgDataHandle = std::make_unique<BCM_RDO_Container>();
    std::unique_ptr<BCM_RDO_Collection> bkgCollection = std::make_unique<BCM_RDO_Collection>();
    //Add a BCM_RawData object
    std::unique_ptr<BCM_RawData> bkgDigit = std::make_unique<BCM_RawData>(bkgChannel,bkgPulse1Position,bkgPulse1Width,bkgPulse2Position,bkgPulse2Width,0,0,0,0);
    bkgCollection->push_back(bkgDigit.release());
    inputBkgDataHandle->push_back(bkgCollection.get());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+BCM_RDOs_SIG12'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+BCM_RDOs_BKG12'";
    std::string    outputPropertyValue = "'StoreGateSvc+BCM_RDOs12'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<BCM_RDO_Container> outputDataHandle{"StoreGateSvc+BCM_RDOs12"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const BCM_RDO_Collection *outputCollection = outputDataHandle->at(sigChannel);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const BCM_RawData* outputDigit = outputCollection->at(0);
    ASSERT_TRUE( outputDigit!=nullptr );
    ASSERT_TRUE( outputDigit->getChannel()==sigChannel );
    ASSERT_TRUE( outputDigit->getPulse1Position()==sigPulse1Position ); // Merged pulse
    ASSERT_TRUE( outputDigit->getPulse1Width()==9 );
    ASSERT_TRUE( outputDigit->getPulse2Position()==bkgPulse2Position ); // Second pulse is second background pulse
    ASSERT_TRUE( outputDigit->getPulse2Width()==bkgPulse2Width );
  }

} // <-- namespace OverlayTesting


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new OverlayTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
