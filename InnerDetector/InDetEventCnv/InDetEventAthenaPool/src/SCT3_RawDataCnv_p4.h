/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT3_RAWDATACNV_P4_H
#define SCT3_RAWDATACNV_P4_H

/*
  Transient/Persistent converter for SCT3_RawData class
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "Identifier/Identifier.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetEventAthenaPool/SCT3_RawData_p4.h"

class MsgStream;
class SCT_ID;

class SCT3_RawDataCnv_p4 : public T_AthenaPoolTPCnvBase<SCT3_RawData, SCT3_RawData_p4>
{
 public:
  SCT3_RawDataCnv_p4(const SCT_ID* sctId) { m_sctId = sctId; }
  virtual void persToTrans(const SCT3_RawData_p4* persObj, SCT3_RawData* transObj, MsgStream& log);
  virtual void transToPers(const SCT3_RawData* transObj, SCT3_RawData_p4* persObj, MsgStream& log);
  void setWaferId(const Identifier waferId) { m_waferId = waferId; }
 private:
  const SCT_ID* m_sctId;
  Identifier m_waferId;
};

#endif // SCT3_RAWDATACNV_P4_H
