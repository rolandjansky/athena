/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_RDO_CONTAINERCNV_P0_H
#define SCT_RDO_CONTAINERCNV_P0_H

#include "AthContainers/DataVector.h"

#include "InDetRawData/SCT_RDO_Container.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetRawData/SCT1_RawData.h"

class SCT_ID;

typedef  DataVector<InDetRawDataCollection< SCT1_RawData > >  SCT_RDO_Container_p0;

class MsgStream;
class SCT_RDO_ContainerCnv_p0  : public T_AthenaPoolTPCnvBase<SCT_RDO_Container, SCT_RDO_Container_p0> {
   const SCT_ID*  m_sctId{nullptr};
public:
  virtual void   persToTrans(const SCT_RDO_Container_p0*, SCT_RDO_Container*, MsgStream&) {
    // everything is done in createTransient()
  }

  virtual void   transToPers(const SCT_RDO_Container*, SCT_RDO_Container_p0*, MsgStream&)  {
    throw std::runtime_error("Writing SCT RDOs in the old format is not supported");
  }

  virtual SCT_RDO_Container* createTransient(const SCT_RDO_Container_p0* persObj, MsgStream& log);

  // ID helper can't be used in the constructor, need initialize()
  void initialize(const SCT_ID* idhelper) { m_sctId = idhelper; }
};

#endif/*SCT_RDO_CONTAINERCNV_P0_H*/
