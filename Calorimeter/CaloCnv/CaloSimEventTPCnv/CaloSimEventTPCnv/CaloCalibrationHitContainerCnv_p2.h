/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P2_H
#define CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P2_H

#define private public
#include "CaloSimEventTPCnv/CaloCalibrationHitContainer_p2.h"
#undef private 
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//typedef T_AthenaHitsVectorCnv< CaloCalibrationHitContainer, CaloCalibrationHitContainer_p1, CaloCalibrationHitCnv_p1 >  CaloCalibrationHitContainerCnv_p1;

class CaloCalibrationHitContainerCnv_p2 : public T_AthenaPoolTPCnvBase<CaloCalibrationHitContainer, CaloCalibrationHitContainer_p2>
{
 public:
  CaloCalibrationHitContainerCnv_p2(){};
  
  virtual void  persToTrans(const CaloCalibrationHitContainer_p2* persColl, CaloCalibrationHitContainer* transColl,  MsgStream &log);
  virtual void  transToPers(const CaloCalibrationHitContainer* transColl, CaloCalibrationHitContainer_p2* persColl,  MsgStream &log);
 private:
};

#endif // not CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINERCNV_P2_H
