/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACNV_CHARGE_P1_H
#define ALFA_RAWDATACNV_CHARGE_P1_H

#include "ALFA_RawData_charge_p1.h"
#include "ALFA_RawEv/ALFA_RawData_charge.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class ALFA_RawDataCnv_charge_p1: public T_AthenaPoolTPCnvBase<ALFA_RawData_charge, ALFA_RawData_charge_p1> {

 public:
  
  ALFA_RawDataCnv_charge_p1() {}

  virtual void persToTrans(const ALFA_RawData_charge_p1* persObj, ALFA_RawData_charge*    transObj, MsgStream &log);
  virtual void transToPers(const ALFA_RawData_charge*   transObj, ALFA_RawData_charge_p1* persObj , MsgStream &log);
};

#endif
