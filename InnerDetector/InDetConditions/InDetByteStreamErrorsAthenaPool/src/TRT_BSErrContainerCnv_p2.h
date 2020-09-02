/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSERRCONTAINERCNV_P2_H
#define INDETBYTESTREAMERRORSATHENAPOOL_TRT_BSERRCONTAINERCNV_P2_H

#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetByteStreamErrorsAthenaPool/TRT_BSErrContainer_p2.h"


class TRT_BSErrContainerCnv_p2 : public T_AthenaPoolTPCnvBase<TRT_BSErrContainer, TRT_BSErrContainer_p2>
{
 public:
  TRT_BSErrContainerCnv_p2(){};

  virtual void persToTrans(const TRT_BSErrContainer_p2* persCont,
                            TRT_BSErrContainer* transCont,
                            MsgStream &log) ;
  virtual void transToPers(const TRT_BSErrContainer* transCont,
                            TRT_BSErrContainer_p2* persCont,
                            MsgStream &log) ;
  virtual TRT_BSErrContainer* createTransient(const TRT_BSErrContainer_p2* persObj, MsgStream& log) ;

};

#endif
