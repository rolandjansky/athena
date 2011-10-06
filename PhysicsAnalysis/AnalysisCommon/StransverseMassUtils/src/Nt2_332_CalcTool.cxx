/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/Nt2_332_CalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include "Mt2/Basic_Nt2_332_Calculator.h"
#include<string>

Nt2_332_CalcTool::Nt2_332_CalcTool(const std::string& type, const std::string& name, const IInterface *parent) :
  AlgTool(type,name,parent),
  m_calc(NULL),
  m_VecA(NULL),m_VecB(NULL),m_mpt(NULL),
  m_P1(0.0),m_P2(0.0)
  {

  declareInterface<Nt2_332_CalcTool>( this );

  m_calc = new Mt2::Basic_Nt2_332_Calculator();

}


Nt2_332_CalcTool::~Nt2_332_CalcTool(){
  this->clear();
  if( m_calc ) delete m_calc;
}


StatusCode Nt2_332_CalcTool::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode Nt2_332_CalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode Nt2_332_CalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing" << endreq;

  this->clear();
 
  return StatusCode::SUCCESS;
}
