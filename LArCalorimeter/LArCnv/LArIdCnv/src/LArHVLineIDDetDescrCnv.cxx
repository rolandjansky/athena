/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LAr DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/


#include "LArIdCnv/LArHVLineIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "IdDictDetDescr/IdDictManager.h"
#include "LArIdentifier/LArHVLineID.h"


long int
LArHVLineIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode 
LArHVLineIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "LArHVLineIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
LArHVLineIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "LArHVLineIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
LArHVLineIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "LArHVLineIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a LArHVLineID helper object in the detector store" << endmsg;

    // Create a new LArHVLineID

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
    }
    else {
	log << MSG::DEBUG << "Helper key is " << helperKey << endmsg;
    }
    

    // get DetectorStore service
    StoreGateSvc * detStore;
    StatusCode status = serviceLocator()->service("DetectorStore", detStore);
    if (status.isFailure()) {
	log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
	return StatusCode::FAILURE;
    } else {}
 
    // Get the dictionary manager from the detector store
    //const DataHandle<IdDictManager> idDictMgr;
    const IdDictManager* idDictMgr;
    status = detStore->retrieve(idDictMgr, "IdDict");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get IdDictManager !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the IdDictManager. " << endmsg;
    }


    // create the helper
    LArHVLineID* hvline_id = new LArHVLineID;
    // pass a pointer to IMessageSvc to the helper
    hvline_id->setMessageSvc(msgSvc());

    if (idDictMgr->initializeHelper(*hvline_id)) {
	log << MSG::ERROR << "Unable to initialize LArHVLineID" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Initialized LArHVLineID. " << endmsg;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(hvline_id);
    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long 
LArHVLineIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
LArHVLineIDDetDescrCnv::classID() { 
    return ClassID_traits<LArHVLineID>::ID(); 
}

//--------------------------------------------------------------------
LArHVLineIDDetDescrCnv::LArHVLineIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<LArHVLineID>::ID(), svcloc)
{}



