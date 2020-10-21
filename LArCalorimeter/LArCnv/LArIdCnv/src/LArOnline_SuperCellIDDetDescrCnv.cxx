/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 LAr DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/


#include "LArIdCnv/LArOnline_SuperCellIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "IdDictDetDescr/IdDictManager.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"


long int
LArOnline_SuperCellIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode 
LArOnline_SuperCellIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "LArOnline_SuperCellIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
LArOnline_SuperCellIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "LArOnline_SuperCellIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
LArOnline_SuperCellIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "LArOnline_SuperCellIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a LArOnline_SuperCellID helper object in the detector store" << endmsg;

    // Create a new LArOnline_SuperCellID

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
    LArOnline_SuperCellID* online_id = new LArOnline_SuperCellID;
    // pass a pointer to IMessageSvc to the helper
    online_id->setMessageSvc(msgSvc());

    if (idDictMgr->initializeHelper(*online_id)) {
	log << MSG::ERROR << "Unable to initialize LArOnline_SuperCellID" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Initialized LArOnline_SuperCellID. " << endmsg;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(online_id);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long 
LArOnline_SuperCellIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
LArOnline_SuperCellIDDetDescrCnv::classID() { 
    return ClassID_traits<LArOnline_SuperCellID>::ID(); 
}

//--------------------------------------------------------------------
LArOnline_SuperCellIDDetDescrCnv::LArOnline_SuperCellIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<LArOnline_SuperCellID>::ID(), svcloc)
{}



