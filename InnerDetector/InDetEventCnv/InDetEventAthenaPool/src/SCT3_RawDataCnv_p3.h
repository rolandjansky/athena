/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT3_RAWDATACNV_P3_H
#define SCT3_RAWDATACNV_P3_H

/*
Transient/Persistent converter for SCT3_RawData class
*/

#include "InDetRawData/SCT3_RawData.h"
#include "InDetEventAthenaPool/InDetRawData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class SCT3_RawDataCnv_p3  : public T_AthenaPoolTPCnvBase<SCT3_RawData, SCT3_RawData_p3>
{
public:

  SCT3_RawDataCnv_p3() {}
  virtual void          persToTrans(const SCT3_RawData_p3* persObj, SCT3_RawData* transObj, MsgStream& log);
  virtual void          transToPers(const SCT3_RawData* transObj, SCT3_RawData_p3* persObj, MsgStream& log);
};


#endif


