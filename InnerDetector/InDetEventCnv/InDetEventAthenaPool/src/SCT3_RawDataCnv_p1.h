/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT3_RAWDATACNV_P1_H
#define SCT3_RAWDATACNV_P1_H

/*
Transient/Persistent converter for SCT3_RawData class
Author: Davide Costanzo
*/

#include "InDetRawData/SCT3_RawData.h"
#include "InDetEventAthenaPool/InDetRawData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class SCT3_RawDataCnv_p1  : public T_AthenaPoolTPCnvBase<SCT3_RawData, SCT3_RawData_p1>
{
public:

  SCT3_RawDataCnv_p1() {}
  virtual void          persToTrans(const SCT3_RawData_p1* persObj, SCT3_RawData* transObj, MsgStream& log);
  virtual void          transToPers(const SCT3_RawData* transObj, SCT3_RawData_p1* persObj, MsgStream& log);
};


#endif


