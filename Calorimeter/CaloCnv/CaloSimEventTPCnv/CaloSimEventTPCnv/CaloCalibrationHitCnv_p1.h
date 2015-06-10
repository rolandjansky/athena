/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCNV_P1_H
#define CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCNV_P1_H

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEventTPCnv/CaloCalibrationHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class CaloCalibrationHitCnv_p1 : public T_AthenaPoolTPCnvBase<CaloCalibrationHit, CaloCalibrationHit_p1>
{
public:

  CaloCalibrationHitCnv_p1() {}

  virtual void persToTrans(const CaloCalibrationHit_p1* persObj, CaloCalibrationHit* transObj, MsgStream &log);
  virtual void transToPers(const CaloCalibrationHit* transObj, CaloCalibrationHit_p1* persObj, MsgStream &log);
};

#endif
