/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P3_H
#define CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P3_H

#define private public
#include "CaloSimEventTPCnv/CaloCalibrationHitContainer_p3.h"
#undef private 
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//typedef T_AthenaHitsVectorCnv< CaloCalibrationHitContainer, CaloCalibrationHitContainer_p1, CaloCalibrationHitCnv_p1 >  CaloCalibrationHitContainerCnv_p1;

class CaloCalibrationHitContainerCnv_p3 : public T_AthenaPoolTPCnvBase<CaloCalibrationHitContainer, CaloCalibrationHitContainer_p3>
{
 public:
  CaloCalibrationHitContainerCnv_p3(){};
  
  virtual void  persToTrans(const CaloCalibrationHitContainer_p3* persColl, CaloCalibrationHitContainer* transColl,  MsgStream &log);
  virtual void  transToPers(const CaloCalibrationHitContainer* transColl, CaloCalibrationHitContainer_p3* persColl,  MsgStream &log);
 private:
};

#endif // not CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P1_H
