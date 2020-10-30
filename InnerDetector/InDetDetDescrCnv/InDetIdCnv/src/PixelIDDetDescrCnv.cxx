/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDet DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: PixelIDDetDescrCnv.cxx,v 1.4 2007-01-16 17:06:15 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "PixelIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h" 

#include "IdDictDetDescr/IdDictManager.h"
//#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"

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
PixelIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
PixelIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "PixelIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }

    // The following is an attempt to "bootstrap" the loading of a
    // proxy for PixelID into the detector store. However,
    // PixelIDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add InDet_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "PidelID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for PixelID to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
PixelIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "PixelIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
PixelIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "PixelIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a PixelID helper object in the detector store" << endmsg;

    // Create a new PixelID

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


    // Only initialize helper if it is the first pass or if there is a
    // change in the the file or tag
    bool initHelper              = false;

    const IdDictMgr* mgr          = idDictMgr->manager();

    // Internal InDet id tag
    std::string   inDetIDTag      = mgr->tag();

    // DoChecks flag
    bool doChecks                 = mgr->do_checks();

    IdDictDictionary* dict = mgr->find_dictionary("InnerDetector");  
    if (!dict) {
	log << MSG::ERROR 
	    << "unable to find idDict for InnerDetector" 
	    << endmsg;
	return StatusCode::FAILURE;
    }

    // File to be read for InDet ids
    std::string   inDetIDFileName = dict->file_name();

    // Tag of RDB record for InDet ids
    std::string   inDetIdDictTag  = dict->dict_tag();


    if (m_pixelId) {

	// Pixel id helper already exists - second pass. Check for a
	// change 
	if (inDetIDTag != m_inDetIDTag) { 
	    // Internal InDet id tag
	    initHelper = true;
	    log << MSG::DEBUG << " Changed internal InDet id tag: " 
		<< inDetIDTag << endmsg;
	}
	if (inDetIDFileName != m_inDetIDFileName) {
	    // File to be read for InDet ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed InDetFileName:" 
		<< inDetIDFileName << endmsg;
	}
	if (inDetIdDictTag != m_inDetIdDictTag) {
	    // Tag of RDB record for InDet ids
	    initHelper = true;
	    log << MSG::DEBUG << " Changed InDetIdDictTag: "
		<< inDetIdDictTag 
		<< endmsg;
	}
	if (doChecks != m_doChecks) {
	    // DoChecks flag
	    initHelper = true;
	    log << MSG::DEBUG << " Changed doChecks flag: "
		<< doChecks
		<< endmsg;
        }
    }
    else {
	// create the helper
	m_pixelId = new PixelID;
	initHelper = true;
        // add in message service for printout
        m_pixelId->setMessageSvc(msgSvc());
    }

    if (initHelper) {
	if (idDictMgr->initializeHelper(*m_pixelId)) {
	    log << MSG::ERROR << "Unable to initialize PixelID" << endmsg;
	    return StatusCode::FAILURE;
	} 
	// Save state:
	m_inDetIDTag      = inDetIDTag;
	m_inDetIDFileName = inDetIDFileName;
	m_inDetIdDictTag  = inDetIdDictTag;
	m_doChecks        = doChecks;
    }
    
    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(m_pixelId);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
PixelIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
PixelIDDetDescrCnv::classID() { 
    return ClassID_traits<PixelID>::ID(); 
}

//--------------------------------------------------------------------
PixelIDDetDescrCnv::PixelIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<PixelID>::ID(), svcloc),
    m_pixelId(0),
    m_doChecks(false)

{}



