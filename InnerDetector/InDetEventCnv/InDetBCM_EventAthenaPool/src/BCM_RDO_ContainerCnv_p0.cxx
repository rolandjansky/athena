/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "BCM_RDO_ContainerCnv_p0.h"

void BCM_RDO_ContainerCnv_p0::persToTrans(const BCM_RDO_Container_p0* persCont, BCM_RDO_Container* transCont, MsgStream &log) {

  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RDO_ContainerCnv_p0::persToTrans" << endreq;

  BCM_RDO_ContainerCnv_p0_basetype::persToTrans(persCont, transCont, log);
}

void BCM_RDO_ContainerCnv_p0::transToPers(const BCM_RDO_Container* transCont, BCM_RDO_Container_p0* persCont, MsgStream &log) { 

  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RDO_ContainerCnv_p0::transToPers" << endreq;

  BCM_RDO_ContainerCnv_p0_basetype::transToPers(transCont, persCont, log);
}
