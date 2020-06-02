/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_IDCINDETBSERRCONTAINERCNV_H
#define INDETBYTESTREAMERRORSATHENAPOOL_IDCINDETBSERRCONTAINERCNV_H

#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetByteStreamErrorsAthenaPool/InDetBSErrContainer64_p1.h"


class IDCInDetBSErrContainerCnv_p1 final : public T_AthenaPoolTPCnvBase<IDCInDetBSErrContainer, InDetBSErrContainer64_p1>
{
 public:
  IDCInDetBSErrContainerCnv_p1() {};

  virtual void persToTrans(const InDetBSErrContainer64_p1* persCont,
                           IDCInDetBSErrContainer* transCont,
                           MsgStream& log);
  virtual void transToPers(const IDCInDetBSErrContainer* transCont,
                           InDetBSErrContainer64_p1* persCont,
                           MsgStream& log);
  virtual IDCInDetBSErrContainer* createTransient(const InDetBSErrContainer64_p1* persObj,
                                               MsgStream& log);

};

#endif