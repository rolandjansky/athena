/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StransverseMassUtils/mT2Rec.h"

#include "GaudiKernel/MsgStream.h"

mT2Rec::mT2Rec(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name,pSvcLocator)
{ }

mT2Rec::~mT2Rec() { }

StatusCode mT2Rec::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  return StatusCode::SUCCESS;
}

StatusCode mT2Rec::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing " << name() << endreq;

  return StatusCode::SUCCESS;
}

StatusCode mT2Rec::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing " << name() << endreq;

  return StatusCode::SUCCESS;
}
