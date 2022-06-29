/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RAWDATACNV_P1_H
#define LUCID_RAWDATACNV_P1_H

#include "LUCID_RawData_p1.h"
#include "LUCID_RawEvent/LUCID_RawData.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_RawDataCnv_p1: public T_AthenaPoolTPCnvConstBase<LUCID_RawData, LUCID_RawData_p1> {

 public:
  using base_class::persToTrans;
  using base_class::transToPers;

  LUCID_RawDataCnv_p1() {}

  virtual void persToTrans(const LUCID_RawData_p1* persObj, LUCID_RawData*    transObj, MsgStream &log) const override;
  virtual void transToPers(const LUCID_RawData*   transObj, LUCID_RawData_p1* persObj , MsgStream &log) const override;
};

#endif
