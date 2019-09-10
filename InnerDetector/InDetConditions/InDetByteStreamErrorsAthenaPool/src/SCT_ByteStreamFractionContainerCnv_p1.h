// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBYTESTREAMERRORSATHENAPOOL_SCT_BYTESTREAMFRACTIONCONTAINERCNV_P1_H
#define INDETBYTESTREAMERRORSATHENAPOOL_SCT_BYTESTREAMFRACTIONCONTAINERCNV_P1_H

#include "InDetByteStreamErrors/SCT_ByteStreamFractionContainer.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetByteStreamErrorsAthenaPool/SCT_ByteStreamFractionContainer_p1.h"


class SCT_ByteStreamFractionContainerCnv_p1 : public T_AthenaPoolTPCnvBase<SCT_ByteStreamFractionContainer, SCT_ByteStreamFractionContainer_p1>
{
 public:
  SCT_ByteStreamFractionContainerCnv_p1(){};

  virtual void persToTrans(const SCT_ByteStreamFractionContainer_p1* persCont,
                           SCT_ByteStreamFractionContainer* transCont,
                           MsgStream &log) ;
  virtual void transToPers(const SCT_ByteStreamFractionContainer* transCont,
                           SCT_ByteStreamFractionContainer_p1* persCont,
                           MsgStream &log) ;
  virtual SCT_ByteStreamFractionContainer* createTransient(const SCT_ByteStreamFractionContainer_p1* persObj, MsgStream& log) ;

};

#endif // INDETBYTESTREAMERRORSATHENAPOOL_SCT_BYTESTREAMFRACTIONCONTAINERCNV_P1_H
