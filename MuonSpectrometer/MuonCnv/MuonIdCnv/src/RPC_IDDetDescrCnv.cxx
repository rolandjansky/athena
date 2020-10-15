/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Muon DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/


#include "RPC_IDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 
#include "AthenaKernel/StorableConversions.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "MuonIdHelpers/RpcIdHelper.h"



//--------------------------------------------------------------------

long int   
RPC_IDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

StatusCode 
RPC_IDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "RPC_IDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }
    

    // The following is an attempt to "bootstrap" the loading of a
    // proxy for RpcIdHelper into the detector store. However,
    // RPC_IDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add InDet_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "PidelID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for RpcIdHelper to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
RPC_IDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "RPC_IDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
RPC_IDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "RPC_IDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a RpcIdHelper object in the detector store" << endmsg;

    // Create a new RpcIdHelper

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
    const DataHandle<IdDictManager> idDictMgr;
    status = detStore->retrieve(idDictMgr, "IdDict");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get IdDictManager !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the IdDictManager. " << endmsg;
    }


    // create the helper
    RpcIdHelper* rpc_id = new RpcIdHelper;
    if (idDictMgr->initializeHelper(*rpc_id)) {
	log << MSG::ERROR << "Unable to initialize RpcIdHelper from dictionary" << endmsg;
	return StatusCode::FAILURE;
    } 

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = SG::asStorable(rpc_id);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
RPC_IDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
RPC_IDDetDescrCnv::classID() { 
    return ClassID_traits<RpcIdHelper>::ID(); 
}

//--------------------------------------------------------------------
RPC_IDDetDescrCnv::RPC_IDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<RpcIdHelper>::ID(), svcloc)
{}



