/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDet DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: SiliconIDDetDescrCnv.cxx,v 1.4 2007-01-16 17:06:15 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "SiliconIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGate.h" 

#include "IdDictDetDescr/IdDictManager.h"
//#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SiliconID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

//--------------------------------------------------------------------

long int   
SiliconIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
SiliconIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "SiliconIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }

    // The following is an attempt to "bootstrap" the loading of a
    // proxy for SiliconID into the detector store. However,
    // SiliconIDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add InDet_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "PidelID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for SiliconID to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
SiliconIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "SiliconIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
SiliconIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "SiliconIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a SiliconID helper object in the detector store" << endmsg;

    // Create a new SiliconID

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
    const IdDictManager* idDictMgr;
    status = detStore->retrieve(idDictMgr, "IdDict");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get IdDictManager !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the IdDictManager. " << endmsg;
    }

    // Get both Pixel and SCT id helpers
    const PixelID* pixelID;
    status = detStore->retrieve(pixelID, "PixelID");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get PixelID helper !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the PixelID. " << endmsg;
    }

    const SCT_ID* sctID;
    status = detStore->retrieve(sctID, "SCT_ID");
    if (status.isFailure()) {
	log << MSG::FATAL << "Could not get SCT_ID helper !" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Found the SCT_ID. " << endmsg;
    }

    if (!m_siliconId) {
	// create the helper only once
	log << MSG::DEBUG << " Create SiliconID. " << endmsg;
	m_siliconId = new SiliconID(pixelID, sctID);
        m_siliconId->setMessageSvc(msgSvc());
    }
    
    if (idDictMgr->initializeHelper(*m_siliconId)) {
	log << MSG::ERROR << "Unable to initialize SiliconID" << endmsg;
	return StatusCode::FAILURE;
    } 
    else {
	log << MSG::DEBUG << " Initialized SiliconID. " << endmsg;
    }
    
    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(m_siliconId);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
SiliconIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
SiliconIDDetDescrCnv::classID() { 
    return ClassID_traits<SiliconID>::ID(); 
}

//--------------------------------------------------------------------
SiliconIDDetDescrCnv::SiliconIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<SiliconID>::ID(), svcloc),
    m_siliconId(0)

{}



