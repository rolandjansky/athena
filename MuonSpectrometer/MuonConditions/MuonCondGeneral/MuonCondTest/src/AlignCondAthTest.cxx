/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonCondTest/AlignCondAthTest.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
// #include "MuonCondSvc/MuonAlignmentDbSvc.h"


AlignCondAthTest::AlignCondAthTest(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator){
}
 
StatusCode AlignCondAthTest::initialize(){
//

  ATH_MSG_INFO( "in initialize()"  );
  if (StatusCode::SUCCESS != detStore()->retrieve(m_MuonDetMgrDS)) {
    ATH_MSG_FATAL("Couldn't load MuonDetectorManager");
    return StatusCode::FAILURE;
  }

  if (detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" ).isFailure()) {
    ATH_MSG_ERROR("Can't retrieve MdtIdHelper");
    return StatusCode::FAILURE;
  } else ATH_MSG_INFO("MdtIdHelper retrieved from the DetectorStore");

  ATH_CHECK(m_DetectorManagerKey.initialize());

  return StatusCode::SUCCESS;

}
 
StatusCode AlignCondAthTest::execute() {
//

  ATH_MSG_INFO( " AlignCondAthTest in execute()"  );

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr{*DetectorManagerHandle}; 
  if(MuonDetMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return StatusCode::FAILURE; 
  } 

  bool MdtPrint = true;
  bool RpcPrint = false;
  bool TgcPrint = false;
  bool CscPrint = false;

  if(MdtPrint) {
    std::cout << "************************ BEGIN: MdtReadoutElements from Detector Store **************************" << std::endl;
    std::cout << " cachingFlag() " << m_MuonDetMgrDS->cachingFlag() << " cacheFillingFlag() " << m_MuonDetMgrDS->cacheFillingFlag() << std::endl;
    if(checkMdtGeometry(m_MuonDetMgrDS).isFailure()) return StatusCode::FAILURE;
    std::cout << "************************ END: MdtReadoutElements from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: MdtReadoutElements from Conditions Store **************************" << std::endl;
    std::cout << " cachingFlag() " << MuonDetMgr->cachingFlag() << " cacheFillingFlag() " << MuonDetMgr->cacheFillingFlag() << std::endl;
    if(checkMdtGeometry(MuonDetMgr).isFailure()) return StatusCode::FAILURE;
    std::cout << "************************ END: MdtReadoutElements from Conditions Store **************************" << std::endl;
  }
  if(RpcPrint) {
    std::cout << "************************ BEGIN: RpcReadoutElements from Detector Store **************************" << std::endl;
    if(checkRpcGeometry(m_MuonDetMgrDS).isFailure()) return StatusCode::FAILURE;
    std::cout << "************************ END: RpcReadoutElements from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: RpcReadoutElements from Conditions Store **************************" << std::endl;
    if(checkRpcGeometry(MuonDetMgr).isFailure()) return StatusCode::FAILURE;
    std::cout << "************************ END: RpcReadoutElements from Conditions Store **************************" << std::endl;
  }
  if(TgcPrint) {
    std::cout << "************************ BEGIN: TgcReadoutElements from Detector Store **************************" << std::endl;
    if(checkTgcGeometry(m_MuonDetMgrDS).isFailure()) return StatusCode::FAILURE;
    std::cout << "************************ END: TgcReadoutElements from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: TgcReadoutElements from Conditions Store **************************" << std::endl;
    if(checkTgcGeometry(MuonDetMgr).isFailure()) return StatusCode::FAILURE;
    std::cout << "************************ END: TgcReadoutElements from Conditions Store **************************" << std::endl;
  }
  if(CscPrint) {
    std::cout << "************************ BEGIN: CscReadoutElements from Detector Store **************************" << std::endl;
    if(checkCscGeometry(m_MuonDetMgrDS).isFailure()) return StatusCode::FAILURE;
    std::cout << "************************ END: CscReadoutElements from Detector Store **************************" << std::endl;
    std::cout << "************************ BEGIN: CscReadoutElements from Conditions Store **************************" << std::endl;
    if(checkCscGeometry(MuonDetMgr).isFailure()) return StatusCode::FAILURE;
    std::cout << "************************ END: CscReadoutElements from Conditions Store **************************" << std::endl;
  }
  // if(checkCscGeometry().isFailure()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
//
}
 
StatusCode AlignCondAthTest::finalize() {
//

  ATH_MSG_INFO( "in finalize()"  );
//
  return StatusCode::SUCCESS;
//
}
  
StatusCode AlignCondAthTest::checkMdtGeometry(const MuonGM::MuonDetectorManager* manager) 
{
  for (unsigned int hash = 0; hash < manager->MdtRElMaxHash; ++hash) {
    const MuonGM::MdtReadoutElement* roEl = manager->getMdtReadoutElement(IdentifierHash(hash));
    if (!roEl) {
      ATH_MSG_WARNING("initialize() failed to retrieve detEL byebye");
      continue;
    }
    Identifier id = roEl->identify();
    int stationName = m_mdtIdHelper->stationName(id);
    int stationEta  = m_mdtIdHelper->stationEta(id);
    int stationPhi  = m_mdtIdHelper->stationPhi(id);
    int multilayer  = roEl->getMultilayer();
    
    std::vector<Identifier> MDTIDs;
    m_mdtIdHelper->idChannels (id, MDTIDs);
    for (std::vector<Identifier>::iterator tubeId = MDTIDs.begin(); tubeId != MDTIDs.end(); tubeId++) {
      
      if (m_mdtIdHelper->validElement(*tubeId)) {
        int ml   = m_mdtIdHelper->multilayer(*tubeId);
        if( ml == multilayer) {
          const Amg::Vector3D tubePos = roEl->tubePos(*tubeId);
          const Amg::Vector3D localTubePos = roEl->localTubePos(*tubeId);
          int itube = m_mdtIdHelper->tube(*tubeId);
          
          std::cout << "Station, eta,phi Tube id, X, Y, Z: " << m_mdtIdHelper->stationNameString(stationName) << "," 
		    << stationEta << "," << stationPhi 
                    << " " << itube << ", " << tubePos.x() << ", " << tubePos.y() << ", " 
                    << tubePos.z() << " Local position: " << localTubePos.x() << ", " 
                    << localTubePos.y() << ", " << localTubePos.z() << ", surface center " 
		    << roEl->surface( id ).center().x() << ", " << roEl->surface( id ).center().y() 
		    << ", " << roEl->surface( id ).center().z() << std::endl;
        }
      }
    } 
  } 
  return StatusCode::SUCCESS;
}

StatusCode AlignCondAthTest::checkRpcGeometry(const MuonGM::MuonDetectorManager* manager) 
{
  for (unsigned int hash = 0; hash < manager->RpcRElMaxHash; ++hash) {
    const MuonGM::RpcReadoutElement* roEl = manager->getRpcReadoutElement(IdentifierHash(hash));
    if (!roEl) {
      ATH_MSG_WARNING("initialize() failed to retrieve detEL byebye");
      continue;
    }
  
    Identifier id = roEl->identify();
    const Amg::Vector3D stripPos = roEl->stripPos(id);
    std::cout << "Strip position: " << stripPos.x() << ", " 
	      << stripPos.y() << ", " << stripPos.z() 
	      << " Surface " << roEl->surface( id ).center().x() << ", " << roEl->surface( id ).center().y() << ", " << roEl->surface( id ).center().z() << std::endl;

  } 

  return StatusCode::SUCCESS;
}

StatusCode AlignCondAthTest::checkTgcGeometry(const MuonGM::MuonDetectorManager* manager) 
{
  for (unsigned int hash = 0; hash < manager->TgcRElMaxHash; ++hash) {
    const MuonGM::TgcReadoutElement* roEl = manager->getTgcReadoutElement(IdentifierHash(hash));
    if (!roEl) {
      ATH_MSG_WARNING("initialize() failed to retrieve detEL byebye");
      continue;
    }
  
    Identifier id = roEl->identify();
    const Amg::Vector3D stripPos = roEl->stripPos(id);
    std::cout << "Strip position: " << stripPos.x() << ", " 
	      << stripPos.y() << ", " << stripPos.z() 
	      << " Surface " << roEl->surface( id ).center().x() << ", " << roEl->surface( id ).center().y() << ", " << roEl->surface( id ).center().z() << std::endl;

  } 

  return StatusCode::SUCCESS;
}

StatusCode AlignCondAthTest::checkCscGeometry(const MuonGM::MuonDetectorManager* manager) 
{
  for (unsigned int hash = 0; hash < manager->CscRElMaxHash; ++hash) {
    const MuonGM::CscReadoutElement* roEl = manager->getCscReadoutElement(IdentifierHash(hash));
    if (!roEl) {
      ATH_MSG_WARNING("initialize() failed to retrieve detEL byebye");
      continue;
    }
  
    Identifier id = roEl->identify();
    const Amg::Vector3D stripPos = roEl->stripPos(id);
    std::cout << "Strip position: " << stripPos.x() << ", " 
	      << stripPos.y() << ", " << stripPos.z() 
	      << " Surface " << roEl->surface( id ).center().x() << ", " << roEl->surface( id ).center().y() << ", " << roEl->surface( id ).center().z() << std::endl;

  } 

  return StatusCode::SUCCESS;
}

