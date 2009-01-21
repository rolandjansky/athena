/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETBCM_EVENTATHENAPOOL_BCM_RDO_CONTAINERCNV_P0_H
#define INDETBCM_EVENTATHENAPOOL_BCM_RDO_CONTAINERCNV_P0_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "InDetBCM_RawData/BCM_RDO_Container.h"
#include "InDetBCM_EventAthenaPool/BCM_RDO_Container_p0.h"
#include "BCM_RDO_CollectionCnv_p0.h"

class MsgStream;

typedef T_AthenaPoolTPCnvVector< BCM_RDO_Container, BCM_RDO_Container_p0, BCM_RDO_CollectionCnv_p0 > BCM_RDO_ContainerCnv_p0_basetype;

class BCM_RDO_ContainerCnv_p0 : public BCM_RDO_ContainerCnv_p0_basetype {

 public:
  BCM_RDO_ContainerCnv_p0() {}

  virtual void persToTrans(const BCM_RDO_Container_p0* persCont, BCM_RDO_Container* transCont, MsgStream &log);
  virtual void transToPers(const BCM_RDO_Container* transCont, BCM_RDO_Container_p0* persCont, MsgStream &log);
};

#endif
