/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetBCM_RawData/BCM_RDO_Collection.h"
#include "GaudiKernel/MsgStream.h"
#include "BCM_RDO_CollectionCnv_p0.h"

void BCM_RDO_CollectionCnv_p0::persToTrans(const BCM_RDO_Collection_p0* persColl, BCM_RDO_Collection* transColl, MsgStream &log) {

  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RDO_CollectionCnv_p0::persToTrans" << endreq;

  BCM_RDO_CollectionCnv_p0_basetype::persToTrans(persColl, transColl, log);
  transColl->setChannel (persColl->m_chanId);
}

void BCM_RDO_CollectionCnv_p0::transToPers(const BCM_RDO_Collection* transColl, BCM_RDO_Collection_p0* persColl, MsgStream &log) { 

  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RDO_CollectionCnv_p0::transToPers" << endreq;

  BCM_RDO_CollectionCnv_p0_basetype::transToPers(transColl, persColl, log);
  persColl->m_chanId = transColl->getChannel();
}

