/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_RDO_CONTAINERCNV_P0_H
#define TRT_RDO_CONTAINERCNV_P0_H

#include "AthContainers/DataVector.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class TRT_ID;

typedef  DataVector<InDetRawDataCollection< TRT_LoLumRawData > >  TRT_RDO_Container_p0;

class MsgStream;
class TRT_RDO_ContainerCnv_p0  : public T_AthenaPoolTPCnvBase<TRT_RDO_Container, TRT_RDO_Container_p0> {
   const TRT_ID*  m_trtId;
public:
  virtual void   persToTrans(const TRT_RDO_Container_p0*, TRT_RDO_Container*, MsgStream&) {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const TRT_RDO_Container*, TRT_RDO_Container_p0*, MsgStream&){
    throw std::runtime_error("Writing TRT RDOs in the old format is not supported");
  }

  virtual TRT_RDO_Container* createTransient(const TRT_RDO_Container_p0* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  void initialize(const TRT_ID *idhelper) { m_trtId = idhelper; }
};

#endif/*TRT_RDO_CONTAINERCNV_P0_H*/
