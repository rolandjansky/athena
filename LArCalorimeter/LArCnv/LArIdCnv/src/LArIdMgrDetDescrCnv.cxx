/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LAr Identifier converter package
 -----------------------------------------
 ***************************************************************************/


#include "LArIdCnv/LArIdMgrDetDescrCnv.h"
#include "LArIdentifier/LArIdManager.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloCell_ID.h"

// infrastructure includes
#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/StoreGateSvc.h" 

// Identifier includes
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArIdentifier/LArElectrodeID.h"


long int
LArIdMgrDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode 
LArIdMgrDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "LArIdMgrDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }
    
    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
LArIdMgrDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "LArIdMgrDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode LArIdMgrDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    MsgStream log(msgSvc(), "LArIdMgrDetDescrCnv");
    log << MSG::INFO 
	<< "in createObj: creating a LArDescrManager object in the detector store" 
	<< endmsg;

    // Create a new LArIdManager

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
	log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
	return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string mgrKey  = *( ddAddr->par() );
    if ("" == mgrKey) {
	log << MSG::DEBUG << "No Manager key " << endmsg;
    }
    else {
	log << MSG::DEBUG << "Manager key is " << mgrKey << endmsg;
    }
    
    // Create the manager
    LArIdManager* larIdMgr = new LArIdManager(); 

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(larIdMgr);

    // get DetectorStore service
    StoreGateSvc * detStore;
    StatusCode status = serviceLocator()->service("DetectorStore", detStore);
    if (status.isFailure()) {
	log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
	return StatusCode::FAILURE;
    } else {}
 
    // Get calo online idhelper from detector store and add to mgr
    const LArOnlineID* onl_id;
    status = detStore->retrieve(onl_id, "LArOnlineID");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get LArOnlineID helper !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArOnlineID helper. " << endmsg;
    }
    larIdMgr->set_helper(onl_id);
    log << MSG::INFO << "Set LArOnlineID helper in LArIdMgr " 
	<< endmsg;

     // Get calo HVLine idhelper from detector store and add to mgr
    const LArHVLineID* hv_id;
    status = detStore->retrieve(hv_id, "LArHVLineID");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get LArHVLineID helper !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArHVLineID helper. " << endmsg;
    }
    larIdMgr->set_helper(hv_id);
    log << MSG::INFO << "Set LArHVLineID helper in LArIdMgr " 
	<< endmsg;

     // Get Electrode idhelper from detector store and add to mgr
    const LArElectrodeID* elec_id;
    status = detStore->retrieve(elec_id, "LArElectrodeID");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get LArElectrodeID helper !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the LArElectrodeID helper. " << endmsg;
    }
    larIdMgr->set_helper(elec_id);
    log << MSG::INFO << "Set LArElectrodeID helper in LArIdMgr " 
	<< endmsg;
 
    // Get calo offline idhelper from detector store and add to mgr
    const CaloCell_ID* cell_id;
    status = detStore->retrieve(cell_id, "CaloCell_ID");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get CaloCell_ID helper !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the CaloCell_ID helper. " << endmsg;
    }
    larIdMgr->set_helper(cell_id);
    log << MSG::INFO << "Set CaloCell_ID helper in LArIdMgr " 
	<< endmsg;

    // Initialize the lar mgr
    //  We protect here in case this has been initialized elsewhere
    if (!larIdMgr->isInitialized()) {

      log << MSG::INFO << "Initializing LArIdMgr from values in LArIdMgrDetDescrCnv " 
	  << endmsg;
      
      // Initialize the manager ... 

      // in this one, only strictly lar work is done
      larIdMgr->initialize();

    }


    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long 
LArIdMgrDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
LArIdMgrDetDescrCnv::classID() { 
    return ClassID_traits<LArIdManager>::ID(); 
}

//--------------------------------------------------------------------
LArIdMgrDetDescrCnv::LArIdMgrDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<LArIdManager>::ID(), svcloc)
{}
