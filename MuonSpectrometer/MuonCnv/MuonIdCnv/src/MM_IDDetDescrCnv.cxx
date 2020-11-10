/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Muon DetDescrCnv package
 -----------------------------------------
***************************************************************************/

#include "MM_IDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 
#include "AthenaKernel/StorableConversions.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "MuonIdHelpers/MmIdHelper.h"



//--------------------------------------------------------------------
long int MM_IDDetDescrCnv::repSvcType() const {
  return (storageType());
}

//--------------------------------------------------------------------
StatusCode MM_IDDetDescrCnv::initialize() {
  // First call parent init
  StatusCode sc = DetDescrConverter::initialize();
  MsgStream log(msgSvc(), "MM_IDDetDescrCnv");
  log << MSG::DEBUG << "in initialize" << endmsg;

  if (sc.isFailure()) {
    log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
    return sc;
  }

  // diehl: Previously commented out code from TGC version.  Maybe useful so leave here
  // The following is an attempt to "bootstrap" the loading of a
  // proxy for TgcIdHelper into the detector store. However,
  // MM_IDDetDescrCnv::initialize is NOT called by the conversion
  // service.  So for the moment, this cannot be use. Instead the
  // DetDescrCnvSvc must do the bootstrap from a parameter list.

  //      // Add InDet_DetDescrManager proxy as entry point to the detector store
  //      // - this is ONLY needed for the manager of each system
  //      sc = addToDetStore(classID(), "PidelID");
  //      if (sc.isFailure()) {
  //  	log << MSG::FATAL << "Unable to add proxy for MmIdHelper to the Detector Store!" << endmsg;
  //  	return StatusCode::FAILURE;
  //      } else {}

  return StatusCode::SUCCESS; 
}  //end MM_IDDetDescrCnv::initialize

//--------------------------------------------------------------------
StatusCode MM_IDDetDescrCnv::finalize() {
  MsgStream log(msgSvc(), "MM_IDDetDescrCnv");
  log << MSG::DEBUG << "in finalize" << endmsg;

  return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------
StatusCode MM_IDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  //StatusCode sc = StatusCode::SUCCESS;
  MsgStream log(msgSvc(), "MM_IDDetDescrCnv");
  log << MSG::INFO << "in createObj: creating a MmIdHelper object in the detector store" << endmsg;

  // Create a new MmIdHelper
  DetDescrAddress* ddAddr;
  ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
  if(!ddAddr) {
    log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the StoreGate key of this container.
  std::string helperKey  = *( ddAddr->par() );
  if ("" == helperKey) {
    log << MSG::DEBUG << "No Helper key " << endmsg;
  } else {
    log << MSG::DEBUG << "Helper key is " << helperKey << endmsg;
  }
    
  // get DetectorStore service
  StoreGateSvc * detStore;
  StatusCode status = serviceLocator()->service("DetectorStore", detStore);
  if (status.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  } // else {}  //pointless else from TGC code - Diehl
 
  // Get the dictionary manager from the detector store
  const DataHandle<IdDictManager> idDictMgr;
  status = detStore->retrieve(idDictMgr, "IdDict");
  if (status.isFailure()) {
    log << MSG::FATAL << "Could not get IdDictManager !" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << " Found the IdDictManager. " << endmsg;
  }

  // create the helper
  MmIdHelper* mm_id = new MmIdHelper;
  if (idDictMgr->initializeHelper(*mm_id)) {
    log << MSG::ERROR << "Unable to initialize MmIdHelper" << endmsg;
    return StatusCode::FAILURE;
  } 

  // Pass a pointer to the container to the Persistency service by reference.
  pObj = SG::asStorable(mm_id);

  return StatusCode::SUCCESS; 
}   //end MM_IDDetDescrCnv::createObj

//--------------------------------------------------------------------
long MM_IDDetDescrCnv::storageType() {
  return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& MM_IDDetDescrCnv::classID() { 
  return ClassID_traits<MmIdHelper>::ID(); 
}

//--------------------------------------------------------------------
MM_IDDetDescrCnv::MM_IDDetDescrCnv(ISvcLocator* svcloc) : 
  DetDescrConverter(ClassID_traits<MmIdHelper>::ID(), svcloc)
{}
