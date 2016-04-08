/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARHITCONTAINERCNV_P2_H
#define LARTPCNV_LARHITCONTAINERCNV_P2_H

#include "LArSimEventTPCnv/LArHitContainer_p2.h"
#include "LArSimEvent/LArHitContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class LArHitContainerCnv_p2 : public T_AthenaPoolTPCnvBase<LArHitContainer, LArHitContainer_p2>
{
 public:
  LArHitContainerCnv_p2() {};
  
  virtual void  persToTrans(const LArHitContainer_p2* persColl, LArHitContainer* transColl,  MsgStream &log) ;
  virtual void  transToPers(const LArHitContainer* transColl, LArHitContainer_p2* persColl,  MsgStream &log) ;
 private:
};

#endif // not LARTPCNV_LARHITCONTAINERCNV_P2_H
