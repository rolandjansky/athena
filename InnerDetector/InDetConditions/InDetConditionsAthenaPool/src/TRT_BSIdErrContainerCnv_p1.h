/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_BSIDERRCONTAINERCNV_P1_H
#define TRT_BSIDERRCONTAINERCNV_P1_H

#include "InDetByteStreamErrors/TRT_BSIdErrContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetConditionsAthenaPool/TRT_BSIdErrContainer_p1.h"


class TRT_BSIdErrContainerCnv_p1 : public T_AthenaPoolTPCnvBase<TRT_BSIdErrContainer, TRT_BSIdErrContainer_p1>
{
 public:
  TRT_BSIdErrContainerCnv_p1(){};
  
  virtual void	persToTrans(const TRT_BSIdErrContainer_p1* persCont,
			    TRT_BSIdErrContainer* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const TRT_BSIdErrContainer* transCont,
			    TRT_BSIdErrContainer_p1* persCont,
			    MsgStream &log) ;
  virtual TRT_BSIdErrContainer* createTransient(const TRT_BSIdErrContainer_p1* persObj, MsgStream& log) ;

};

#endif
