/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//<<<<<< INCLUDES                                                       >>>>>>

#include "HGTD_IDDetDescrCnv.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaBaseComps/AthCheckMacros.h"

#include "IdDictDetDescr/IdDictManager.h"

//--------------------------------------------------------------------

long int
HGTD_IDDetDescrCnv::repSvcType() const
{
    return (storageType());
}

//--------------------------------------------------------------------

StatusCode
HGTD_IDDetDescrCnv::initialize()
{
    // First call parent init
    ATH_CHECK(DetDescrConverter::initialize());

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------

StatusCode
HGTD_IDDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
    ATH_MSG_INFO( "in createObj: creating a HGTD_ID helper object in the detector store" );

    // Create a new HGTD_ID

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
        ATH_MSG_FATAL( "Could not cast to DetDescrAddress." );
        return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string helperKey  = *( ddAddr->par() );
    if ("" == helperKey) {
        ATH_MSG_DEBUG( "No Helper key " );
    }
    else {
        ATH_MSG_DEBUG( "Helper key is " << helperKey );
    }

    // get DetectorStore service
    StoreGateSvc * detStore;
    StatusCode status = serviceLocator()->service("DetectorStore", detStore);
    if (status.isFailure()) {
        ATH_MSG_FATAL( "DetectorStore service not found !" );
        return StatusCode::FAILURE;
    } else {}

    // Get the dictionary manager from the detector store
    const IdDictManager* idDictMgr;
    status = detStore->retrieve(idDictMgr, "IdDict");
    if (status.isFailure()) {
        ATH_MSG_FATAL( "Could not get IdDictManager !" );
        return StatusCode::FAILURE;
    } 
    else {
        ATH_MSG_DEBUG( " Found the IdDictManager. " );
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
        ATH_MSG_ERROR( "unable to find idDict for InnerDetector" );
        return StatusCode::FAILURE;
    }

    // File to be read for InDet ids
    std::string   inDetIDFileName = dict->file_name();

    // Tag of RDB record for InDet ids
    std::string   inDetIdDictTag  = dict->dict_tag();


    if (m_hgtdId) {

        // HGTD id helper already exists - second pass. Check for a
        // change
        if (inDetIDTag != m_inDetIDTag) {
            // Internal InDet id tag
            initHelper = true;
            ATH_MSG_DEBUG( " Changed internal InDet id tag: " << inDetIDTag );
        }
        if (inDetIDFileName != m_inDetIDFileName) {
            // File to be read for InDet ids
            initHelper = true;
            ATH_MSG_DEBUG( " Changed InDetFileName:" << inDetIDFileName );
        }
        if (inDetIdDictTag != m_inDetIdDictTag) {
            // Tag of RDB record for InDet ids
            initHelper = true;
            ATH_MSG_DEBUG( " Changed InDetIdDictTag: " << inDetIdDictTag );
        }
        if (doChecks != m_doChecks) {
            // DoChecks flag
            initHelper = true;
            ATH_MSG_DEBUG( " Changed doChecks flag: " << doChecks );
        }
    }
    else {
        // create the helper
        m_hgtdId = std::make_unique<HGTD_ID>();
        initHelper = true;
        // add in message service for printout
        m_hgtdId->setMessageSvc(msgSvc());
    }

    if (initHelper) {
        if (idDictMgr->initializeHelper(*m_hgtdId)) {
            ATH_MSG_ERROR( "Unable to initialize HGTD_ID" );
            return StatusCode::FAILURE;
        }
        // Save state:
        m_inDetIDTag      = inDetIDTag;
        m_inDetIDFileName = inDetIDFileName;
        m_inDetIdDictTag  = inDetIdDictTag;
        m_doChecks        = doChecks;
    }
    
    // Pass a pointer to the container to the Persistency service by reference.
    pObj = StoreGateSvc::asStorable(m_hgtdId.release());

    return StatusCode::SUCCESS;

}

//--------------------------------------------------------------------

long
HGTD_IDDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID&
HGTD_IDDetDescrCnv::classID() {
    return ClassID_traits<HGTD_ID>::ID();
}

//--------------------------------------------------------------------
HGTD_IDDetDescrCnv::HGTD_IDDetDescrCnv(ISvcLocator* svcloc)
    :
    DetDescrConverter(ClassID_traits<HGTD_ID>::ID(), svcloc),
    AthMessaging(svcloc->service< IMessageSvc >( "MessageSvc" ),"HGTD_IDDetDescrCnv"),
    m_hgtdId(),
    m_doChecks(false)

{}
