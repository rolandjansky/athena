/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Tests for HGTD_Overlay.
 */

#undef NDEBUG

// Framework
#include "TestTools/initGaudi.h"

// Google Test
#include "gtest/gtest.h"

// Tested AthAlgorithm
#include "../src/HGTD_Overlay.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY; // Use of global g_svcLoc is not thread safe.

namespace OverlayTesting
{

  // needed every time an AthAlgorithm, AthAlgTool or AthService is instantiated
  ISvcLocator *g_svcLoc = nullptr;

  // global test environment takes care of setting up Gaudi
  class GaudiEnvironment : public ::testing::Environment {
  protected:
    virtual void SetUp() override {
      Athena_test::initGaudi(OverlayTesting::g_svcLoc);
    }
  };

  class HGTD_Overlay_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_alg = new HGTD_Overlay{"HGTD_Overlay", g_svcLoc};
      ASSERT_TRUE( g_svcLoc->service("StoreGateSvc", m_sg) );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
      ASSERT_TRUE( m_sg->clearStore().isSuccess() );
    }

    HGTD_Overlay *m_alg{};
    StoreGateSvc *m_sg{};
  }; // HGTD_Overlay_test fixture


  TEST_F(HGTD_Overlay_test, set_properties) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isFailure() ); //inputs don't exist
  }

  TEST_F(HGTD_Overlay_test, empty_containers_alg_execute) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG"};
    const unsigned int containerSize(1188);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
  }

  TEST_F(HGTD_Overlay_test, containers_with_matching_empty_collections) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG1"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(1);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG1"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG1'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG1'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs1'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs1"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->empty() );
  }

  TEST_F(HGTD_Overlay_test, containers_with_different_empty_collections) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG2"};
    const unsigned int containerSize(1188);
    IdentifierHash sigElementHash(1);
    IdentifierHash bkgElementHash(2);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG2"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG2'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG2'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs2'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs2"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->empty() );
    const HGTD_RDO_Collection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->empty() );
  }

  TEST_F(HGTD_Overlay_test, containers_with_matching_collections_one_with_an_RDO) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG3"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const float sigTOA(1.23);
    const unsigned int sigTOT(10);
    const unsigned int sigBCID(2);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> sigDigit = std::make_unique<HGTD_RDO>(Identifier(12),sigTOA,sigTOT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG3"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG3'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG3'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs3'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs3"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const HGTD_RDO *outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getTOA()==sigTOA );
    ASSERT_TRUE( outputDigit1->getTOT()==sigTOT );
    ASSERT_TRUE( outputDigit1->getBCID()==sigBCID-1 );
  }

  TEST_F(HGTD_Overlay_test, containers_with_different_collections_one_RDO_each) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG4"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(2);
    const float sigTOA(1.23);
    const float bkgTOA(1.23);
    const unsigned int sigTOT(10);
    const unsigned int bkgTOT(10);
    const unsigned int sigBCID(2);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> sigDigit = std::make_unique<HGTD_RDO>(Identifier(12),sigTOA,sigTOT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG4"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> bkgDigit = std::make_unique<HGTD_RDO>(Identifier(12),bkgTOA,bkgTOT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG4'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG4'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs4'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs4"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection1 = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection1!=nullptr );
    ASSERT_TRUE( outputCollection1->size()==1 );
    const HGTD_RDO *outputDigit1 = outputCollection1->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getTOA()==sigTOA );
    ASSERT_TRUE( outputDigit1->getTOT()==sigTOT );
    ASSERT_TRUE( outputDigit1->getBCID()==sigBCID-1 );
    const HGTD_RDO_Collection *outputCollection2 = outputDataHandle->indexFindPtr(bkgElementHash);
    ASSERT_TRUE( outputCollection2!=nullptr );
    ASSERT_TRUE( outputCollection2->size()==1 );
    const HGTD_RDO *outputDigit2 = outputCollection2->at(0);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->getTOA()==bkgTOA );
    ASSERT_TRUE( outputDigit2->getTOT()==bkgTOT );
    ASSERT_TRUE( outputDigit2->getBCID()==bkgBCID-1 );
  }

  TEST_F(HGTD_Overlay_test, containers_with_matching_collections_one_different_RDO_each) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG5"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const float sigTOA(1.23);
    const float bkgTOA(3.21);
    const unsigned int sigTOT(11);
    const unsigned int bkgTOT(10);
    const unsigned int sigBCID(1);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> sigDigit = std::make_unique<HGTD_RDO>(Identifier(12),sigTOA,sigTOT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG5"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> bkgDigit = std::make_unique<HGTD_RDO>(Identifier(13),bkgTOA,bkgTOT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG5'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG5'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs5'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs5"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 );
    const HGTD_RDO *outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getTOA()==sigTOA ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit1->getTOT()==sigTOT ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit1->getBCID()==sigBCID-1 ); // RDOs in same collection sorted according to Identifier
    const HGTD_RDO *outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->getTOA()==bkgTOA ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit2->getTOT()==bkgTOT ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit2->getBCID()==bkgBCID-1 ); // RDOs in same collection sorted according to Identifier
  }

  TEST_F(HGTD_Overlay_test, containers_with_matching_collections_one_matching_RDO_each) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG6"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const float sigTOA(1.23);
    const float bkgTOA(1.23);
    const unsigned int sigTOT(10);
    const unsigned int bkgTOT(10);
    const unsigned int sigBCID(2);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> sigDigit = std::make_unique<HGTD_RDO>(Identifier(12),sigTOA,sigTOT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG6"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> bkgDigit = std::make_unique<HGTD_RDO>(Identifier(12),bkgTOA,bkgTOT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG6'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG6'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs6'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );

    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs6"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const HGTD_RDO *outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getTOA()==bkgTOA ); // Bkg RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getTOT()==bkgTOT ); // Bkg RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getBCID()==bkgBCID-1 ); // Bkg RDO taken in case of matching Identifiers
  }

  TEST_F(HGTD_Overlay_test, two_RDOs_with_matching_id_signal_first) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG7"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const float sigTOA(1.23);
    const float bkgTOA(3.21);
    const unsigned int sigTOT(11);
    const unsigned int bkgTOT(10);
    const unsigned int sigBCID(1);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> sigDigit = std::make_unique<HGTD_RDO>(Identifier(12),sigTOA,sigTOT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG7"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> bkgDigit = std::make_unique<HGTD_RDO>(Identifier(12),bkgTOA,bkgTOT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG7'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG7'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs7'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs7"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const HGTD_RDO *outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getTOA()==sigTOA ); // Signal RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getTOT()==sigTOT ); // Signal RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getBCID()==sigBCID-1 ); // Signal RDO taken in case of matching Identifiers
  }

  TEST_F(HGTD_Overlay_test, two_RDOs_with_matching_id_bkg_first) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG8"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const float sigTOA(1.23);
    const float bkgTOA(3.21);
    const unsigned int sigTOT(11);
    const unsigned int bkgTOT(10);
    const unsigned int sigBCID(2);
    const unsigned int bkgBCID(1);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> sigDigit = std::make_unique<HGTD_RDO>(Identifier(12),sigTOA,sigTOT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG8"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> bkgDigit = std::make_unique<HGTD_RDO>(Identifier(12),bkgTOA,bkgTOT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG8'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG8'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs8'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs8"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==1 );
    const HGTD_RDO *outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getTOA()==sigTOA ); // Signal RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getTOT()==sigTOT ); // Signal RDO taken in case of matching Identifiers
    ASSERT_TRUE( outputDigit1->getBCID()==sigBCID-1 ); // Signal RDO taken in case of matching Identifiers
  }

  TEST_F(HGTD_Overlay_test, containers_with_matching_collections_one_different_RDO_each_v2) {
    EventContext ctx(0,0);
    ctx.setExtension( Atlas::ExtendedEventContext( m_sg, 0 ) );
    SG::WriteHandle<HGTD_RDO_Container> inputSigDataHandle{"StoreGateSvc+HGTD_RDOs_SIG9"};
    const unsigned int containerSize(1188);
    const IdentifierHash sigElementHash(1);
    const IdentifierHash bkgElementHash(1);
    const float sigTOA(1.23);
    const float bkgTOA(3.21);
    const unsigned int sigTOT(11);
    const unsigned int bkgTOT(10);
    const unsigned int sigBCID(1);
    const unsigned int bkgBCID(2);
    inputSigDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> sigCollection = std::make_unique<HGTD_RDO_Collection>(sigElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> sigDigit = std::make_unique<HGTD_RDO>(Identifier(13),sigTOA,sigTOT,sigBCID-1,0,sigBCID-1);
    sigCollection->push_back(sigDigit.release());
    ASSERT_TRUE(inputSigDataHandle->addCollection(sigCollection.get(),sigElementHash).isSuccess());
    sigCollection.release(); // Now owned by inputSigDataHandle
    SG::WriteHandle<HGTD_RDO_Container> inputBkgDataHandle{"StoreGateSvc+HGTD_RDOs_BKG9"};
    inputBkgDataHandle = std::make_unique<HGTD_RDO_Container>(containerSize);
    std::unique_ptr<HGTD_RDO_Collection> bkgCollection = std::make_unique<HGTD_RDO_Collection>(bkgElementHash);
    //Add a HGTD_RDO object
    std::unique_ptr<HGTD_RDO> bkgDigit = std::make_unique<HGTD_RDO>(Identifier(12),bkgTOA,bkgTOT,bkgBCID-1,0,bkgBCID-1);
    bkgCollection->push_back(bkgDigit.release());
    ASSERT_TRUE(inputBkgDataHandle->addCollection(bkgCollection.get(),bkgElementHash).isSuccess());
    bkgCollection.release(); // Now owned by inputBkgDataHandle

    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputSigPropertyValue = "'StoreGateSvc+HGTD_RDOs_SIG9'";
    std::string  inputBkgPropertyValue = "'StoreGateSvc+HGTD_RDOs_BKG9'";
    std::string    outputPropertyValue = "'StoreGateSvc+HGTD_RDOs9'";
    ASSERT_TRUE( m_alg->setProperty( "SignalInputKey",   inputSigPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "BkgInputKey",   inputBkgPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputKey", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->initialize().isSuccess() );
    ASSERT_TRUE( m_alg->execute(ctx).isSuccess() );
    // check output makes sense
    SG::ReadHandle<HGTD_RDO_Container> outputDataHandle{"StoreGateSvc+HGTD_RDOs9"};
    ASSERT_TRUE( outputDataHandle.isValid() );
    const HGTD_RDO_Collection *outputCollection = outputDataHandle->indexFindPtr(sigElementHash);
    ASSERT_TRUE( outputCollection!=nullptr );
    ASSERT_TRUE( outputCollection->size()==2 );
    const HGTD_RDO *outputDigit1 = outputCollection->at(0);
    ASSERT_TRUE( outputDigit1!=nullptr );
    ASSERT_TRUE( outputDigit1->getTOA()==bkgTOA ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit1->getTOT()==bkgTOT ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit1->getBCID()==bkgBCID-1 ); // RDOs in same collection sorted according to Identifier
    const HGTD_RDO *outputDigit2 = outputCollection->at(1);
    ASSERT_TRUE( outputDigit2!=nullptr );
    ASSERT_TRUE( outputDigit2->getTOA()==sigTOA ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit2->getTOT()==sigTOT ); // RDOs in same collection sorted according to Identifier
    ASSERT_TRUE( outputDigit2->getBCID()==sigBCID-1 ); // RDOs in same collection sorted according to Identifier
  }

} // <-- namespace OverlayTesting


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new OverlayTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
