/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetRawData/Pixel1RawData.h"
#undef private
#undef protected

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "Pixel1RawDataCnv_p1.h"

// Other stuff
#include "Identifier/Identifier.h"

#include "MsgUtil.h"

void
Pixel1RawDataCnv_p1::persToTrans(const InDetRawData_p1* persObj, Pixel1RawData* transObj, MsgStream &log)
{
    MSG_VERBOSE(log,"Pixel1RawDataCnv_p1::persToTrans called ");

    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize PixelClusterOnTrackCnv_p1 " << endreq;
        }
    }
    // Must ask pixelID helper to contruct channel id. Special
    // treatment is needed when reading in unsigned int and going to
    // 64-bit id.
    transObj->m_rdoId = m_pixId->pixel_id(persObj->m_rdoId);
    transObj->m_word =  persObj->m_word;
}

void
Pixel1RawDataCnv_p1::transToPers(const Pixel1RawData* transObj, InDetRawData_p1* persObj, MsgStream &log) 
{
    MSG_VERBOSE(log,"Pixel1RawDataCnv_p1::transToPers called ");
    persObj->m_rdoId = transObj->m_rdoId.get_compact();
    persObj->m_word = transObj->m_word;
}

StatusCode Pixel1RawDataCnv_p1::initialize(MsgStream &log) {
    // Do not initialize again:
    m_isInitialized=true;

    // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();

    // get DetectorStore service
    StoreGateSvc *detStore;
    StatusCode sc = svcLocator->service("DetectorStore", detStore);
    if (sc.isFailure()) {
        log << MSG::FATAL << "DetectorStore service not found !" << endreq;
        return StatusCode::FAILURE;
    } 

    // Get the sct helper from the detector store
    sc = detStore->retrieve(m_pixId, "PixelID");
    if (sc.isFailure()) {
        log << MSG::FATAL << "Could not get PixelID helper !" << endreq;
        return StatusCode::FAILURE;
    } 

    return StatusCode::SUCCESS;
}
