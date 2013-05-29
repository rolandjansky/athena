/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACNV_P1_H
#define ALFA_RAWDATACNV_P1_H

#include "ALFA_RawData_p1.h"
#include "ALFA_RawEv/ALFA_RawData.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class ALFA_RawDataCnv_p1: public T_AthenaPoolTPCnvBase<ALFA_RawData, ALFA_RawData_p1> {

 public:
  
  ALFA_RawDataCnv_p1() {}

  virtual void persToTrans(const ALFA_RawData_p1* persObj, ALFA_RawData*    transObj, MsgStream &log);
  virtual void transToPers(const ALFA_RawData*   transObj, ALFA_RawData_p1* persObj , MsgStream &log);
};

#endif
