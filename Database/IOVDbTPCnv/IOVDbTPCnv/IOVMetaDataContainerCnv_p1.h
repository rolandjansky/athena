/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBDATAMODEL_IOVMETADATACONTAINERCNV_P1_H
#define IOVDBDATAMODEL_IOVMETADATACONTAINERCNV_P1_H


//<<<<<< INCLUDES                                                       >>>>>>
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "IOVDbTPCnv/IOVMetaDataContainer_p1.h"
#include "IOVDbTPCnv/IOVPayloadContainerCnv_p1.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"

class MsgStream;

class IOVMetaDataContainerCnv_p1  : public T_AthenaPoolTPCnvBase<IOVMetaDataContainer, IOVMetaDataContainer_p1>  {
public:
    IOVMetaDataContainerCnv_p1() {}
    virtual void   persToTrans(const IOVMetaDataContainer_p1* persObj, 
                               IOVMetaDataContainer* transObj, MsgStream &log);
    virtual void   transToPers(const IOVMetaDataContainer* transObj, 
                               IOVMetaDataContainer_p1* persObj, MsgStream &log);
private:

    IOVPayloadContainerCnv_p1 m_payloadCnv;
};


#endif
