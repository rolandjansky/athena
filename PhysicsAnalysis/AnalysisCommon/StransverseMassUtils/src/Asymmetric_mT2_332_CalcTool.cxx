/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/Asymmetric_mT2_332_CalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include "Mt2/Basic_Mt2_Asymmetric332_Calculator.h"

#include<string>

Asymmetric_mT2_332_CalcTool::Asymmetric_mT2_332_CalcTool(const std::string& type, const std::string &name,const IInterface *parent):
  AlgTool(type,name,parent),
  m_calc(NULL),
  m_VecA(NULL),m_VecB(NULL),m_mpt(NULL),
  m_Chi(0.0),m_theta(1.0)
{

  declareInterface<Asymmetric_mT2_332_CalcTool>( this );

  m_calc = new Mt2::Basic_Mt2_Asymmetric332_Calculator();

}

Asymmetric_mT2_332_CalcTool::~Asymmetric_mT2_332_CalcTool(){
  if( m_VecA )delete m_VecA;
  if( m_VecB )delete m_VecB;
  if( m_mpt )delete m_mpt;

  if( m_calc )delete m_calc;
}

StatusCode Asymmetric_mT2_332_CalcTool::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode Asymmetric_mT2_332_CalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode Asymmetric_mT2_332_CalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing " << endreq;
  
  this->clear();

  return StatusCode::SUCCESS;
}
