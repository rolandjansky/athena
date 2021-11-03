/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Tadej Novak
 * @brief Tests for ITk::PixelReadoutManager.
 */

#undef NDEBUG

// Otherwise we get warnings about mutable members in gmock.
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

// Tested service
#include "../src/ITkPixelReadoutManager.h"

#include "GaudiKernel/MsgStream.h"

// Framework includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

// Pixel includes
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"

// Google Test
#include "gtest/gtest.h"
// Google Mock
#include "gmock/gmock.h"


namespace InDetDD
{
namespace ITk
{

  class PixelReadoutManager_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      // Gaudi
      m_appMgr = Gaudi::createApplicationMgr();
      ASSERT_TRUE( m_appMgr!=nullptr );

      m_svcLoc = m_appMgr;
      ASSERT_TRUE( m_svcLoc.isValid() );

      m_svcMgr = m_appMgr;
      ASSERT_TRUE( m_svcMgr.isValid() );

      m_propMgr = m_appMgr;
      ASSERT_TRUE( m_propMgr.isValid() );
      ASSERT_TRUE( m_propMgr->setProperty( "EvtSel",         "NONE" ).isSuccess() );
      ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsType", "FILE" ).isSuccess() );
      ASSERT_TRUE( m_propMgr->setProperty( "JobOptionsPath", "ITkPixelReadoutManager_test.txt" ).isSuccess() );

      m_toolSvc = m_svcLoc->service("ToolSvc");
      ASSERT_TRUE( m_toolSvc.isValid() );

      ASSERT_TRUE( m_appMgr->configure().isSuccess() );
      ASSERT_TRUE( m_appMgr->initialize().isSuccess() );

      // the tested AthenaService
      const auto& svcTypeAndName = "InDetDD::ITk::PixelReadoutManager/ITkPixelReadoutManager";
      SmartIF<IService> svc = m_svcLoc->service(svcTypeAndName);
      m_svc = dynamic_cast<PixelReadoutManager *>(svc.get());
      ASSERT_NE(nullptr, m_svc);

      ASSERT_TRUE( m_svc->configure().isSuccess() );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_svcMgr->removeService(m_svc).isSuccess() );
      ASSERT_TRUE( m_svc->finalize().isSuccess() );
      ASSERT_TRUE( m_svc->terminate().isSuccess() );
      ReleaseSmartIFComponent(m_svc);

      ASSERT_TRUE( m_svcMgr->finalize().isSuccess() );
      ASSERT_TRUE( m_appMgr->finalize().isSuccess() );
      ASSERT_TRUE( m_appMgr->terminate().isSuccess() );
      m_svcLoc->release();
      m_svcMgr->release();
      Gaudi::setInstance( static_cast<IAppMgrUI*>(nullptr) );
    }

    void ReleaseSmartIFComponent(IInterface* comp) {
      size_t finalRefCount = 1; // keep one reference for the SmartIF destructor
      for (size_t refCount = comp->refCount(); refCount>finalRefCount; refCount--) {
        comp->release();
      }
    }

    // the tested service
    PixelReadoutManager* m_svc{};

    // Core Gaudi components
    IAppMgrUI*               m_appMgr{};
    SmartIF<ISvcLocator>     m_svcLoc;
    SmartIF<ISvcManager>     m_svcMgr;
    SmartIF<IToolSvc>        m_toolSvc;
    SmartIF<IProperty>       m_propMgr;
  };   // PixelReadoutManager_test fixture


  // TEST_F(PixelReadoutManager_test, initialize_empty) {
  //   ASSERT_TRUE( m_svc->initialize().isSuccess() );
  // }

  TEST_F(PixelReadoutManager_test, barrel) {
    ASSERT_TRUE( m_svc->initialize().isSuccess() );

    ServiceHandle<StoreGateSvc> detectorStore("DetectorStore", "ITkPixelReadoutManager_test");

    const InDetDD::PixelDetectorManager *detManager{};
    ASSERT_TRUE( detectorStore->retrieve(detManager, "ITkPixel").isSuccess() );
    const PixelID *pixelID{};
    ASSERT_TRUE( detectorStore->retrieve(pixelID, "PixelID").isSuccess() );

    // barrel, layer 2
    Identifier moduleId = pixelID->wafer_id(0, 2, 0, 1);

    const InDetDD::SiDetectorElement *element = detManager->getDetectorElement(moduleId);
    const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
    std::cerr << p_design->debugStringRepr() << std::endl;

    ASSERT_EQ( m_svc->getModuleType(moduleId), PixelModuleType::PIX_BARREL );

    std::cerr << p_design->rows() << " " << p_design->columns() << " "
              << p_design->rowsPerCircuit() << " " << p_design->columnsPerCircuit() << " "
              << p_design->numberOfCircuits() << std::endl;

    Identifier edge1 = element->identifierFromCellId(InDetDD::SiCellId(0, 0));
    Identifier edge2 = element->identifierFromCellId(InDetDD::SiCellId(0, p_design->columns() - 1));
    Identifier edge3 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rows() - 1, 0));
    Identifier edge4 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rows() - 1, p_design->columns() - 1));
    Identifier center1 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit() - 1));
    Identifier center2 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rowsPerCircuit(), p_design->columnsPerCircuit() - 1));
    Identifier center3 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit()));
    Identifier center4 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rowsPerCircuit(), p_design->columnsPerCircuit()));
    Identifier close1 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rowsPerCircuit() - 3, p_design->columnsPerCircuit() - 1));
    Identifier close2 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit() - 3));
    Identifier close3 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rowsPerCircuit(), p_design->columnsPerCircuit() + 2));
    Identifier close4 = element->identifierFromCellId(InDetDD::SiCellId(p_design->rowsPerCircuit() + 2, p_design->columnsPerCircuit()));

    ASSERT_EQ( m_svc->getRow(edge1, moduleId), 0u );
    ASSERT_EQ( m_svc->getRow(edge2, moduleId), 0u );
    ASSERT_EQ( m_svc->getRow(edge3, moduleId), 0u );
    ASSERT_EQ( m_svc->getRow(edge4, moduleId), 0u );
    ASSERT_EQ( m_svc->getRow(center1, moduleId), static_cast<uint32_t>(p_design->rowsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getRow(center2, moduleId), static_cast<uint32_t>(p_design->rowsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getRow(center3, moduleId), static_cast<uint32_t>(p_design->rowsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getRow(center4, moduleId), static_cast<uint32_t>(p_design->rowsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getRow(close1, moduleId), static_cast<uint32_t>(p_design->rowsPerCircuit() - 3) );
    ASSERT_EQ( m_svc->getRow(close2, moduleId), static_cast<uint32_t>(p_design->rowsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getRow(close3, moduleId), static_cast<uint32_t>(p_design->rowsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getRow(close4, moduleId), static_cast<uint32_t>(p_design->rowsPerCircuit() - 3) );

    ASSERT_EQ( m_svc->getColumn(edge1, moduleId), 0u );
    ASSERT_EQ( m_svc->getColumn(edge2, moduleId), 0u );
    ASSERT_EQ( m_svc->getColumn(edge3, moduleId), 0u );
    ASSERT_EQ( m_svc->getColumn(edge4, moduleId), 0u );
    ASSERT_EQ( m_svc->getColumn(center1, moduleId), static_cast<uint32_t>(p_design->columnsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getColumn(center2, moduleId), static_cast<uint32_t>(p_design->columnsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getColumn(center3, moduleId), static_cast<uint32_t>(p_design->columnsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getColumn(center4, moduleId), static_cast<uint32_t>(p_design->columnsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getColumn(close1, moduleId), static_cast<uint32_t>(p_design->columnsPerCircuit() - 1) );
    ASSERT_EQ( m_svc->getColumn(close2, moduleId), static_cast<uint32_t>(p_design->columnsPerCircuit() - 3) );
    ASSERT_EQ( m_svc->getColumn(close3, moduleId), static_cast<uint32_t>(p_design->columnsPerCircuit() - 3) );
    ASSERT_EQ( m_svc->getColumn(close4, moduleId), static_cast<uint32_t>(p_design->columnsPerCircuit() - 1) );

    ASSERT_EQ( m_svc->getDiodeType(edge1), PixelDiodeType::NORMAL );
    ASSERT_EQ( m_svc->getDiodeType(edge2), PixelDiodeType::NORMAL );
    ASSERT_EQ( m_svc->getDiodeType(edge3), PixelDiodeType::NORMAL );
    ASSERT_EQ( m_svc->getDiodeType(edge4), PixelDiodeType::NORMAL );
    ASSERT_EQ( m_svc->getDiodeType(center1), PixelDiodeType::LARGE );
    ASSERT_EQ( m_svc->getDiodeType(center2), PixelDiodeType::LARGE );
    ASSERT_EQ( m_svc->getDiodeType(center3), PixelDiodeType::LARGE );
    ASSERT_EQ( m_svc->getDiodeType(center4), PixelDiodeType::LARGE );
    ASSERT_EQ( m_svc->getDiodeType(close1), PixelDiodeType::LONG );
    ASSERT_EQ( m_svc->getDiodeType(close2), PixelDiodeType::LONG );
    ASSERT_EQ( m_svc->getDiodeType(close3), PixelDiodeType::LONG );
    ASSERT_EQ( m_svc->getDiodeType(close4), PixelDiodeType::LONG );
    
    ASSERT_EQ( m_svc->getFE(edge1, moduleId), 0u );
    ASSERT_EQ( m_svc->getFE(edge2, moduleId), 1u );
    ASSERT_EQ( m_svc->getFE(edge3, moduleId), 2u );
    ASSERT_EQ( m_svc->getFE(edge4, moduleId), 3u );
    ASSERT_EQ( m_svc->getFE(center1, moduleId), 0u );
    ASSERT_EQ( m_svc->getFE(center2, moduleId), 2u );
    ASSERT_EQ( m_svc->getFE(center3, moduleId), 1u );
    ASSERT_EQ( m_svc->getFE(center4, moduleId), 3u );
    ASSERT_EQ( m_svc->getFE(close1, moduleId), 0u );
    ASSERT_EQ( m_svc->getFE(close2, moduleId), 0u );
    ASSERT_EQ( m_svc->getFE(close3, moduleId), 3u );
    ASSERT_EQ( m_svc->getFE(close4, moduleId), 3u );

    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, 0, 0), edge1 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 1, 0, 0), edge2 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 2, 0, 0), edge3 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 3, 0, 0), edge4 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit() - 1), center1 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 2, p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit() - 1), center2 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 1, p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit() - 1), center3 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 3, p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit() - 1), center4 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, p_design->rowsPerCircuit() - 3, p_design->columnsPerCircuit() - 1), close1 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit() - 3), close2 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 3, p_design->rowsPerCircuit() - 1, p_design->columnsPerCircuit() - 3), close3 );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 3, p_design->rowsPerCircuit() - 3, p_design->columnsPerCircuit() - 1), close4 );

    // barrel, layer 0
    moduleId = pixelID->wafer_id(0, 0, 0, 1);

    const InDetDD::SiDetectorElement *elementS = detManager->getDetectorElement(moduleId);
    const InDetDD::PixelModuleDesign *p_designS = static_cast<const InDetDD::PixelModuleDesign*>(&elementS->design());
    std::cerr << p_designS->debugStringRepr() << std::endl;

    ASSERT_EQ( m_svc->getModuleType(moduleId), PixelModuleType::PIX_BARREL );

    std::cerr << p_designS->rows() << " " << p_designS->columns() << " "
              << p_designS->rowsPerCircuit() << " " << p_designS->columnsPerCircuit() << " "
              << p_designS->numberOfCircuits() << std::endl;

    Identifier edge1s = elementS->identifierFromCellId(InDetDD::SiCellId(0, 0));
    Identifier edge2s = elementS->identifierFromCellId(InDetDD::SiCellId(0, p_designS->columns() - 1));
    Identifier edge3s = elementS->identifierFromCellId(InDetDD::SiCellId(p_designS->rows() - 1, 0));
    Identifier edge4s = elementS->identifierFromCellId(InDetDD::SiCellId(p_designS->rows() - 1, p_designS->columns() - 1));
    Identifier centers = elementS->identifierFromCellId(InDetDD::SiCellId(p_designS->rows() / 2, p_designS->columns() / 2));

    ASSERT_EQ( m_svc->getRow(edge1s, moduleId), 0u );
    ASSERT_EQ( m_svc->getRow(edge2s, moduleId), 0u );
    ASSERT_EQ( m_svc->getRow(edge3s, moduleId), static_cast<uint32_t>(p_designS->rows() - 1) );
    ASSERT_EQ( m_svc->getRow(edge4s, moduleId), static_cast<uint32_t>(p_designS->rows() - 1) );
    ASSERT_EQ( m_svc->getRow(centers, moduleId), static_cast<uint32_t>(p_designS->rows() / 2) );

    ASSERT_EQ( m_svc->getColumn(edge1s, moduleId), 0u );
    ASSERT_EQ( m_svc->getColumn(edge2s, moduleId), static_cast<uint32_t>(p_designS->columns() - 1) );
    ASSERT_EQ( m_svc->getColumn(edge3s, moduleId), 0u );
    ASSERT_EQ( m_svc->getColumn(edge4s, moduleId), static_cast<uint32_t>(p_designS->columns() - 1) );
    ASSERT_EQ( m_svc->getColumn(centers, moduleId), static_cast<uint32_t>(p_designS->columns() / 2) );

    ASSERT_EQ( m_svc->getDiodeType(edge1s), PixelDiodeType::NORMAL );
    ASSERT_EQ( m_svc->getDiodeType(edge2s), PixelDiodeType::NORMAL );
    ASSERT_EQ( m_svc->getDiodeType(edge3s), PixelDiodeType::NORMAL );
    ASSERT_EQ( m_svc->getDiodeType(edge4s), PixelDiodeType::NORMAL );
    ASSERT_EQ( m_svc->getDiodeType(centers), PixelDiodeType::NORMAL );

    ASSERT_EQ( m_svc->getFE(edge1s, moduleId), 0u );
    ASSERT_EQ( m_svc->getFE(edge2s, moduleId), 0u );
    ASSERT_EQ( m_svc->getFE(edge3s, moduleId), 0u );
    ASSERT_EQ( m_svc->getFE(edge4s, moduleId), 0u );
    ASSERT_EQ( m_svc->getFE(centers, moduleId), 0u );

    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, 0, 0), edge1s );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, 0, p_designS->columns() - 1), edge2s );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, p_designS->rows() - 1, 0), edge3s );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, p_designS->rows() - 1, p_designS->columns() - 1), edge4s );
    ASSERT_EQ( m_svc->getPixelId(moduleId, 0, p_designS->rows() / 2, p_designS->columns() / 2), centers );
  }

} // namespace ITk
} // namespace InDetDD


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );

  //return RUN_ALL_TESTS();
  // if the above gets stuck forever while trying to finalize Boost stuff
  // inside SGTools, try to use the following:
  // skips proper finalization:
  std::quick_exit( RUN_ALL_TESTS() );
}
