/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LAr DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/


#include "LArIdCnv/LArOnlineIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "IdDictDetDescr/IdDictManager.h"
#include "LArIdentifier/LArOnlineID.h"


long int
LArOnlineIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode 
LArOnlineIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "LArOnlineIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
LArOnlineIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "LArOnlineIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
LArOnlineIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "LArOnlineIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a LArOnlineID helper object in the detector store" << endmsg;

    // Create a new LArOnlineID

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
    LArOnlineID* online_id = new LArOnlineID;
    // pass a pointer to IMessageSvc to the helper
    online_id->setMessageSvc(msgSvc());

    if (idDictMgr->initializeHelper(*online_id)) {
	log << MSG::ERROR << "Unable to initialize LArOnlineID" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Initialized LArOnlineID. " << endmsg;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(online_id);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long 
LArOnlineIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
LArOnlineIDDetDescrCnv::classID() { 
    return ClassID_traits<LArOnlineID>::ID(); 
}

//--------------------------------------------------------------------
LArOnlineIDDetDescrCnv::LArOnlineIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<LArOnlineID>::ID(), svcloc)
{}



