/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINERCNV_P1_H
#define INDETBYTESTREAMERRORSATHENAPOOL_INDETBSERRCONTAINERCNV_P1_H

#include "InDetByteStreamErrors/InDetBSErrContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetByteStreamErrorsAthenaPool/InDetBSErrContainer_p1.h"


class InDetBSErrContainerCnv_p1 : public T_AthenaPoolTPCnvBase<InDetBSErrContainer, InDetBSErrContainer_p1>
{
 public:
  InDetBSErrContainerCnv_p1(){};

  virtual void persToTrans(const InDetBSErrContainer_p1* persCont,
                            InDetBSErrContainer* transCont,
                            MsgStream &log) ;
  virtual void transToPers(const InDetBSErrContainer* transCont,
                            InDetBSErrContainer_p1* persCont,
                            MsgStream &log) ;
  virtual InDetBSErrContainer* createTransient(const InDetBSErrContainer_p1* persObj, MsgStream& log) ;

};

#endif
