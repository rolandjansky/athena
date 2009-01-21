/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Pool converter for BCM_RDO_Container
// Author: Dominique Tardif <Dominique.Tardif@cern.ch>

#include "BCM_RDO_ContainerCnv.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

BCM_RDO_Container_p0* BCM_RDO_ContainerCnv::createPersistent(BCM_RDO_Container* transCont) {
  MsgStream log(msgSvc(), "BCM_RDO_ContainerCnv");
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RDO_ContainerCnv::createPersistent" << endreq;
  BCM_RDO_Container_p0* persCont = m_Converter.createPersistent(transCont,log);
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Wrote persistent BCM RDO Container with " << persCont->size() << " entries" << endreq;
  return persCont;
}

BCM_RDO_Container* BCM_RDO_ContainerCnv::createTransient() {
  MsgStream log(msgSvc(), "BCM_RDO_ContainerCnv");
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In BCM_RDO_ContainerCnv::createTransient" << endreq;
  std::auto_ptr<BCM_RDO_Container_p0> persCont(poolReadObject<BCM_RDO_Container_p0>());
  BCM_RDO_Container* transCont = m_Converter.createTransient(persCont.get(),log);
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Made transient BCM RDO Container with " << transCont->size() << " entries" << endreq;
  return transCont;
}
