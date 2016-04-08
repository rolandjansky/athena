/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IOVMetaDataContainerPTCnv_p1.cxx
 *
 * @author Alexandre Vaniachine <vaniachine@anl.gov>
 *
 */

#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"
#include "IOVDbPTCnv/IOVMetaDataContainerPTCnv_p1.h"
#include "IOVDbTPCnv/IOVMetaDataContainer_p1.h"
#include "CoralBase/Date.h"
#include "CoralBase/TimeStamp.h"

void
IOVMetaDataContainerPTCnv_p1::persToTrans(const IOVMetaDataContainer_p1* persObj, 
					  IOVMetaDataContainer* transObj)
{

    // copy foldername and description
    transObj->m_folderName        = persObj->m_folderName;
    transObj->m_folderDescription = persObj->m_folderDescription;

    // convert payload
    m_payloadCnv.persToTrans(&(persObj->m_payload), transObj->m_payload);
}
