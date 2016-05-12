/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARHITFLOATCONTAINERCNV_P1_h
#define LARTPCNV_LARHITFLOATCONTAINERCNV_P1_h

#include "LArSimEventTPCnv/LArHitContainer_p1.h"
#include "LArSimEvent/LArHitFloatContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LArSimEventTPCnv/LArHitFloatCnv_p1.h"


class LArHitFloatContainerCnv_p1 : public T_AthenaPoolTPCnvBase<LArHitFloatContainer, LArHitContainer_p1>
{
 public:
  LArHitFloatContainerCnv_p1() {};
  
  virtual void  persToTrans(const LArHitContainer_p1* persColl, LArHitFloatContainer* transColl,  MsgStream &log) ;
  virtual void  transToPers(const LArHitFloatContainer* transColl, LArHitContainer_p1* persColl,  MsgStream &log) ;
 private:
  LArHitFloatCnv_p1 m_LArHitFloatCnv_p1;
};

#endif // not LARTPCNV_LARHITFLOATCONTAINERCNV_P1_h
