/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RDO_CONTAINERCNV_P1_H
#define TRT_RDO_CONTAINERCNV_P1_H

#include "InDetRawData/TRT_RDO_Container.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "StoreGate/StoreGateSvc.h"

class TRT_ID;

typedef  TRT_RDO_colvector  TRT_RDO_Container_p1;

class MsgStream;

class TRT_RDO_ContainerCnv_p1  : public T_AthenaPoolTPCnvBase<TRT_RDO_Container, TRT_RDO_Container_p1> {
  const TRT_ID *m_trtId;
  StoreGateSvc *m_storeGate;
public:
  virtual void   persToTrans(const TRT_RDO_Container_p1* persObj, TRT_RDO_Container* transObj, MsgStream &log) ;
  virtual void   transToPers(const TRT_RDO_Container* transObj, TRT_RDO_Container_p1* persObj, MsgStream &log) ;
  virtual TRT_RDO_Container* createTransient(const TRT_RDO_Container_p1* persObj, MsgStream& log) ;
  virtual TRT_RDO_Container_p1* createPersistent(const TRT_RDO_Container* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  void initialize(const TRT_ID *idhelper, StoreGateSvc* sg) { m_trtId = idhelper; m_storeGate = sg; }
};


#endif/*TRT_RDO_CONTAINERCNV_P1_H*/
