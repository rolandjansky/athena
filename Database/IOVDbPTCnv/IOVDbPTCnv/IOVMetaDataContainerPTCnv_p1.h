/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBPTCNV_IOVMETADATACONTAINERPTCNV_P1_H
#define IOVDBPTCNV_IOVMETADATACONTAINERPTCNV_P1_H
/**
 * @file IOVMetaDataContainerPTCnv_p1.h
 *
 * @brief Header file for in-file metadata P/T converters (file peeking without Athena)
 *
 * @author  <vaniachine@anl.gov>
 */

//<<<<<< INCLUDES                                                       >>>>>>
#include "IOVDbPTCnv/IOVPayloadContainerPTCnv_p1.h"

class IOVMetaDataContainer_p1;
class IOVMetaDataContainer;

class IOVMetaDataContainerPTCnv_p1 {

public:
    IOVMetaDataContainerPTCnv_p1() {}
    virtual ~IOVMetaDataContainerPTCnv_p1() {}
    virtual void   persToTrans(const IOVMetaDataContainer_p1* persObj, IOVMetaDataContainer* transObj);

private:

    IOVPayloadContainerPTCnv_p1 m_payloadCnv;
};

#endif // IOVDBPTCNV_IOVMETADATACONTAINERPTCNV_P1_H
