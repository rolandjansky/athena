/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IOVDbDataModel/IOVMetaDataContainer.h"
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
    transObj->setFolderName (persObj->m_folderName);
    transObj->setFolderDescription (persObj->m_folderDescription);

    // convert payload
    m_payloadCnv.persToTrans(&(persObj->m_payload), transObj->m_payload, log);
}

void
IOVMetaDataContainerCnv_p1::transToPers(const IOVMetaDataContainer* transObj, 
					IOVMetaDataContainer_p1* persObj,
					MsgStream &log)
{

    // copy foldername and description
    persObj->m_folderName        = transObj->folderName();
    persObj->m_folderDescription = transObj->folderDescription();

    // convert payload
    m_payloadCnv.transToPers(transObj->payloadContainer(),  &(persObj->m_payload), log);
}
