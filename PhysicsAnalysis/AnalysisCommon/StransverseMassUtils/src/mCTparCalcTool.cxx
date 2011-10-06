/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/mCTparCalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include<string>

mCTparCalcTool::mCTparCalcTool(const std::string& type, const std::string& name, const IInterface* parent):
  AlgTool(type,name,parent),
  m_VecA(NULL),m_VecB(NULL),m_utm(NULL)
{

  declareInterface<mCTparCalcTool>( this );

}

mCTparCalcTool::~mCTparCalcTool(){
  if( m_VecA ) delete m_VecA;
  if( m_VecB ) delete m_VecB;
  if( m_utm ) delete m_utm;
}

StatusCode mCTparCalcTool::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode mCTparCalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode mCTparCalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing" << endreq;

  this->clear();

  return StatusCode::SUCCESS;
}
