/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#undef private
#undef protected


#include "IOVDbDataModel/IOVPayloadContainer.h"
#include "IOVDbTPCnv/IOVMetaDataContainerCnv_p1.h"
#include "IOVDbTPCnv/IOVMetaDataContainer_p1.h"
#include "CoralBase/Date.h"
#include "CoralBase/TimeStamp.h"
//#include "CoralBase/AttributeListSpecification.h"

void
IOVMetaDataContainerCnv_p1::persToTrans(const IOVMetaDataContainer_p1* persObj, 
                                        IOVMetaDataContainer* transObj, 
					MsgStream &log)
{

    log << MSG::DEBUG << "IOVMetaDataContainerCnv_p1::persToTrans - entering "
        << endreq;

    // copy foldername and description
    transObj->m_folderName        = persObj->m_folderName;
    transObj->m_folderDescription = persObj->m_folderDescription;

    // convert payload
    m_payloadCnv.persToTrans(&(persObj->m_payload), transObj->m_payload, log);
}

void
IOVMetaDataContainerCnv_p1::transToPers(const IOVMetaDataContainer* transObj, 
					IOVMetaDataContainer_p1* persObj,
					MsgStream &log)
{

    // copy foldername and description
    persObj->m_folderName        = transObj->m_folderName;
    persObj->m_folderDescription = transObj->m_folderDescription;

    // convert payload
    m_payloadCnv.transToPers(transObj->m_payload,  &(persObj->m_payload), log);
}
