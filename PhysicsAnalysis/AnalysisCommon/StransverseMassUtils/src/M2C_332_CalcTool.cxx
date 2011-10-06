/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/M2C_332_CalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include<string>

#include "Mt2/Basic_M2C_332_Calculator.h"

M2C_332_CalcTool::M2C_332_CalcTool(const std::string& type,const std::string& name, const IInterface *parent) :
  AlgTool(type,name,parent),
  m_VecA(NULL),m_VecB(NULL),m_mpt(NULL),
  m_DM(0.0)
{

  declareInterface<M2C_332_CalcTool>( this );

}

M2C_332_CalcTool::~M2C_332_CalcTool(){
  this->clear();
}


StatusCode M2C_332_CalcTool::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode M2C_332_CalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode M2C_332_CalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing" << endreq;

  this->clear();

  return StatusCode::SUCCESS;
}
