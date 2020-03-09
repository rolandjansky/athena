/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_IDCINDETBSERRCONTAINERCNV_H
#define INDETBYTESTREAMERRORSATHENAPOOL_IDCINDETBSERRCONTAINERCNV_H

#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetByteStreamErrorsAthenaPool/InDetBSErrContainer_p2.h"


class IDCInDetBSErrContainerCnv : public T_AthenaPoolTPCnvBase<IDCInDetBSErrContainer, InDetBSErrContainer_p2>
{
 public:
  IDCInDetBSErrContainerCnv() {};

  virtual void persToTrans(const InDetBSErrContainer_p2* persCont,
                           IDCInDetBSErrContainer* transCont,
                           MsgStream& log);
  virtual void transToPers(const IDCInDetBSErrContainer* transCont,
                           InDetBSErrContainer_p2* persCont,
                           MsgStream& log);
  virtual IDCInDetBSErrContainer* createTransient(const InDetBSErrContainer_p2* persObj,
                                               MsgStream& log);

};

#endif