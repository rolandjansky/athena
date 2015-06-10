/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCNV_P2_H
#define CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCNV_P2_H

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEventTPCnv/CaloCalibrationHit_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class CaloCalibrationHitCnv_p2 : public T_AthenaPoolTPCnvBase<CaloCalibrationHit, CaloCalibrationHit_p2>
{
public:

  CaloCalibrationHitCnv_p2() {}

  virtual void persToTrans(const CaloCalibrationHit_p2* persObj, CaloCalibrationHit* transObj, MsgStream &log);
  virtual void transToPers(const CaloCalibrationHit* transObj, CaloCalibrationHit_p2* persObj, MsgStream &log);
};

#endif
