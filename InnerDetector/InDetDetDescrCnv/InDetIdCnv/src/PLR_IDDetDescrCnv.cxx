/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDet DetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

#include "PLR_IDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "IdDictDetDescr/IdDictManager.h"
#include "InDetIdentifier/PLR_ID.h"

//--------------------------------------------------------------------

PLR_IDDetDescrCnv::PLR_IDDetDescrCnv(ISvcLocator* svcloc)
    : DetDescrConverter(ClassID_traits<PLR_ID>::ID(), svcloc)
{}

//--------------------------------------------------------------------

long int
PLR_IDDetDescrCnv::repSvcType() const
{
  return storageType();
}

//--------------------------------------------------------------------

StatusCode
PLR_IDDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "PLR_IDDetDescrCnv");
    log << MSG::DEBUG << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
        return sc;
    }

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------

StatusCode
PLR_IDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
    MsgStream log(msgSvc(), "PLR_IDDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a PLR_ID helper object in the detector store" << endmsg;

    // Create a new PLR_ID
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
    }

    // Get the dictionary manager from the detector store
    const IdDictManager* idDictMgr;
    status = detStore->retrieve(idDictMgr, "IdDict");
    if (status.isFailure()) {
        log << MSG::FATAL << "Could not get IdDictManager !" << endmsg;
        return StatusCode::FAILURE;
    } else {
        log << MSG::DEBUG << " Found the IdDictManager." << endmsg;
    }


    // Only initialize helper if it is the first pass or if there is a
    // change in the the file or tag
    bool initHelper{};

    const IdDictMgr* mgr = idDictMgr->manager();

    // Internal InDet id tag
    std::string inDetIDTag = mgr->tag();

    // DoChecks flag
    bool doChecks = mgr->do_checks();

    IdDictDictionary* dict = mgr->find_dictionary("InnerDetector");
    if (!dict) {
        log << MSG::ERROR << "unable to find idDict for InnerDetector" << endmsg;
        return StatusCode::FAILURE;
    }

    // File to be read for InDet ids
    std::string inDetIDFileName = dict->file_name();

    // Tag of RDB record for InDet ids
    std::string inDetIdDictTag  = dict->dict_tag();

    // PLR id helper already exists - second pass. Check for a change
    if (m_plrId) {
        if (inDetIDTag != m_inDetIDTag) {
            // Internal InDet id tag
            initHelper = true;
            log << MSG::DEBUG << " Changed internal InDet id tag: "
            << inDetIDTag << endmsg;
        }
        if (inDetIDFileName != m_inDetIDFileName) {
            // File to be read for InDet ids
            initHelper = true;
            log << MSG::DEBUG << " Changed InDetFileName: "
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
    } else {
        // create the helper
        m_plrId = new PLR_ID;
        initHelper = true;
        // add in message service for printout
        m_plrId->setMessageSvc(msgSvc());
    }

    if (initHelper) {
        if (idDictMgr->initializeHelper(*m_plrId)) {
            log << MSG::ERROR << "Unable to initialize PLR_ID" << endmsg;
            return StatusCode::FAILURE;
        }
        // Save state:
        m_inDetIDTag      = inDetIDTag;
        m_inDetIDFileName = inDetIDFileName;
        m_inDetIdDictTag  = inDetIdDictTag;
        m_doChecks        = doChecks;
    }

    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(m_plrId);

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------

long
PLR_IDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID&
PLR_IDDetDescrCnv::classID()
{
    return ClassID_traits<PLR_ID>::ID();
}
