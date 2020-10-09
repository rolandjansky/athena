/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 ForwardIdCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id:  $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "ZdcIDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "IdDictDetDescr/IdDictManager.h"
#include "ZdcIdentifier/ZdcID.h"

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
ZdcIDDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
ZdcIDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "ZdcIDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
	return sc;
    }

    // The following is an attempt to "bootstrap" the loading of a
    // proxy for ZdcID into the detector store. However,
    // ZdcIDDetDescrCnv::initialize is NOT called by the conversion
    // service.  So for the moment, this cannot be use. Instead the
    // DetDescrCnvSvc must do the bootstrap from a parameter list.


//      // Add InDet_DetDescrManager proxy as entry point to the detector store
//      // - this is ONLY needed for the manager of each system
//      sc = addToDetStore(classID(), "PidelID");
//      if (sc.isFailure()) {
//  	log << MSG::FATAL << "Unable to add proxy for ZdcID to the Detector Store!" << endmsg;
//  	return StatusCode::FAILURE;
//      } else {}

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
ZdcIDDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "ZdcIDDetDescrCnv");
    log << MSG::DEBUG << "in finalize" << endmsg;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
ZdcIDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), "ZdcIDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a ZdcID helper object in the detector store" << endmsg;

    // Create a new ZdcID

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
    const IdDictManager* idDictMgr = nullptr;
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


    if (m_zdcId) {

	// Zdc id helper already exists - second pass. Check for a
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
	m_zdcId = new ZdcID;
	initHelper = true;
        // add in message service for printout
        m_zdcId->setMessageSvc(msgSvc());
    }
    
    if (initHelper) {
	if (idDictMgr->initializeHelper(*m_zdcId)) {
	    log << MSG::ERROR << "Unable to initialize ZdcID" << endmsg;
	    return StatusCode::FAILURE;
	} 
	// Save state:
	m_inDetIDTag      = inDetIDTag;
	m_inDetIDFileName = inDetIDFileName;
	m_inDetIdDictTag  = inDetIdDictTag;
	m_doChecks        = doChecks;
    }
    
    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(m_zdcId);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

long
ZdcIDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
ZdcIDDetDescrCnv::classID() { 
    return ClassID_traits<ZdcID>::ID(); 
}

//--------------------------------------------------------------------
ZdcIDDetDescrCnv::ZdcIDDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<ZdcID>::ID(), svcloc),
    m_zdcId(0),
    m_doChecks(false)

{}



