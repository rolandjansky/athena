/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRawData/Pixel1RawData.h"

// Persistent class and converter header file
#include "InDetEventAthenaPool/InDetRawData_p1.h"
#include "Pixel1RawDataCnv_p1.h"

// Other stuff
#include "Identifier/Identifier.h"
#include "AthenaKernel/errorcheck.h"

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
    *transObj = Pixel1RawData (m_pixId->pixel_id(persObj->m_rdoId),
                               persObj->m_word);
}

void
Pixel1RawDataCnv_p1::transToPers(const Pixel1RawData* transObj, InDetRawData_p1* persObj, MsgStream &log) 
{
    MSG_VERBOSE(log,"Pixel1RawDataCnv_p1::transToPers called ");
    persObj->m_rdoId = transObj->identify().get_compact();
    persObj->m_word = transObj->getWord();
}

StatusCode Pixel1RawDataCnv_p1::initialize(MsgStream &/*log*/) {
    // Do not initialize again:
    m_isInitialized=true;

    // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();

    // get DetectorStore service
    StoreGateSvc *detStore = nullptr;
    CHECK(  svcLocator->service("DetectorStore", detStore) );
    CHECK( detStore->retrieve(m_pixId, "PixelID") );

    return StatusCode::SUCCESS;
}
