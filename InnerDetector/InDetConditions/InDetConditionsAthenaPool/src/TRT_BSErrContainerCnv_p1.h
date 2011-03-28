/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_BSERRCONTAINERCNV_P1_H
#define TRT_BSERRCONTAINERCNV_P1_H

#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetConditionsAthenaPool/TRT_BSErrContainer_p1.h"


class TRT_BSErrContainerCnv_p1 : public T_AthenaPoolTPCnvBase<TRT_BSErrContainer, TRT_BSErrContainer_p1>
{
 public:
  TRT_BSErrContainerCnv_p1(){};
  
  virtual void	persToTrans(const TRT_BSErrContainer_p1* persCont,
			    TRT_BSErrContainer* transCont,
			    MsgStream &log) ;
  virtual void	transToPers(const TRT_BSErrContainer* transCont,
			    TRT_BSErrContainer_p1* persCont,
			    MsgStream &log) ;
  virtual TRT_BSErrContainer* createTransient(const TRT_BSErrContainer_p1* persObj, MsgStream& log) ;

};

#endif
