// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINERCNV_P2_H
#define INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINERCNV_P2_H

#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetByteStreamErrorsAthenaPool/InDetBSErrContainer_p2.h"

class InDetBSErrContainerCnv_p2 : public T_AthenaPoolTPCnvBase<InDetBSErrContainer, InDetBSErrContainer_p2>
{
 public:
  InDetBSErrContainerCnv_p2() {};

  virtual void persToTrans(const InDetBSErrContainer_p2* persCont,
                           InDetBSErrContainer* transCont,
                           MsgStream& log);
  virtual void transToPers(const InDetBSErrContainer* transCont,
                           InDetBSErrContainer_p2* persCont,
                           MsgStream& log);
  virtual InDetBSErrContainer* createTransient(const InDetBSErrContainer_p2* persObj,
                                               MsgStream& log);

};

#endif // INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINERCNV_P2_H
