/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RAWDATACNV_P1_H
#define LUCID_RAWDATACNV_P1_H

#include "LUCID_RawData_p1.h"
#include "LUCID_RawEvent/LUCID_RawData.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_RawDataCnv_p1: public T_AthenaPoolTPCnvBase<LUCID_RawData, LUCID_RawData_p1> {

 public:
  
  LUCID_RawDataCnv_p1() {}

  virtual void persToTrans(const LUCID_RawData_p1* persObj, LUCID_RawData*    transObj, MsgStream &log);
  virtual void transToPers(const LUCID_RawData*   transObj, LUCID_RawData_p1* persObj , MsgStream &log);
};

#endif
