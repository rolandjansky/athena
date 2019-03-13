/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARHITFLOATCONTAINERCNV_p2_H
#define LARTPCNV_LARHITFLOATCONTAINERCNV_p2_H

#include "LArSimEventTPCnv/LArHitContainer_p2.h"
#include "LArSimEvent/LArHitFloatContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class CaloCell_ID;


class LArHitFloatContainerCnv_p2 : public T_AthenaPoolTPCnvBase<LArHitFloatContainer, LArHitContainer_p2>
{
public:
  LArHitFloatContainerCnv_p2();
  
  virtual void  persToTrans(const LArHitContainer_p2* persColl, LArHitFloatContainer* transColl,  MsgStream &log) override;
  virtual void  transToPers(const LArHitFloatContainer* transColl, LArHitContainer_p2* persColl,  MsgStream &log) override;

private:
  const CaloCell_ID* m_cellIdHelper;
};


#endif // not LARTPCNV_LARHITFLOATCONTAINERCNV_p2_H
