/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT1_RAWDATACNV_P2_H
#define SCT1_RAWDATACNV_P2_H

/*
Transient/Persistent converter for SCT1_RawData class
Author: Davide Costanzo
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "InDetRawData/SCT1_RawData.h"
#include "InDetEventAthenaPool/InDetRawData_p2.h"

class MsgStream;


class SCT1_RawDataCnv_p2  : public T_AthenaPoolTPCnvBase<SCT1_RawData, InDetRawData_p2>
{
public:

  SCT1_RawDataCnv_p2() {}
  virtual void          persToTrans(const InDetRawData_p2* persObj, SCT1_RawData* transObj, MsgStream& log);
  virtual void          transToPers(const SCT1_RawData* transObj, InDetRawData_p2* persObj, MsgStream& log);
};


#endif


