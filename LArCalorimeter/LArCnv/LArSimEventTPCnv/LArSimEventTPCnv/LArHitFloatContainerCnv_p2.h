/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARHITFLOATCONTAINERCNV_p2_H
#define LARTPCNV_LARHITFLOATCONTAINERCNV_p2_H

#include "LArSimEventTPCnv/LArHitContainer_p2.h"
#include "LArSimEvent/LArHitFloatContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class LArHitFloatContainerCnv_p2 : public T_AthenaPoolTPCnvBase<LArHitFloatContainer, LArHitContainer_p2>
{
 public:
  LArHitFloatContainerCnv_p2() {};
  
  virtual void  persToTrans(const LArHitContainer_p2* persColl, LArHitFloatContainer* transColl,  MsgStream &log) ;
  virtual void  transToPers(const LArHitFloatContainer* transColl, LArHitContainer_p2* persColl,  MsgStream &log) ;
 private:
};


#endif // not LARTPCNV_LARHITFLOATCONTAINERCNV_p2_H
