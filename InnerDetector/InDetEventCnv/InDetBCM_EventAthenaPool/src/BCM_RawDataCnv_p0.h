/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBCM_EVENTATHENAPOOL_BCM_RAWDATACNV_P0_H
#define INDETBCM_EVENTATHENAPOOL_BCM_RAWDATACNV_P0_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetBCM_RawData/BCM_RawData.h"
#include "InDetBCM_EventAthenaPool/BCM_RawData_p0.h"

class MsgStream;

class BCM_RawDataCnv_p0 : public T_AthenaPoolTPCnvBase<BCM_RawData, BCM_RawData_p0> {

 public:
  BCM_RawDataCnv_p0() {}

  virtual void persToTrans(const BCM_RawData_p0* persObj, BCM_RawData* transObj, MsgStream &log);
  virtual void transToPers(const BCM_RawData* transObj, BCM_RawData_p0* persObj, MsgStream &log);
};

#endif
