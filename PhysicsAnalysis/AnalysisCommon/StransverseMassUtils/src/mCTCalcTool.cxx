/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/mCTCalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include<string>


mCTCalcTool::mCTCalcTool(const std::string& type, const std::string& name, const IInterface* parent):
  AlgTool(type,name,parent),
  m_VecA(NULL),m_VecB(NULL)
 
 {
  
  declareInterface<mCTCalcTool>( this );

}

mCTCalcTool::~mCTCalcTool() { 
  if(m_VecA)delete m_VecA;
  if(m_VecB)delete m_VecB;
}

StatusCode mCTCalcTool::initialize() {
  MsgStream mLog( msgSvc(), name() );
  mLog << MSG::DEBUG << "initialize() has been called" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode mCTCalcTool::execute() {
  MsgStream mLog ( msgSvc(), name() );
  mLog << MSG::DEBUG << "execute() has been called" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode mCTCalcTool::finalize(){
  MsgStream mLog( msgSvc(), name() );
  mLog << MSG::DEBUG << "finalize() has been called" << endreq;

  if(m_VecA){
    delete m_VecA;
    m_VecA=NULL;
  }
  if(m_VecB){
    delete m_VecB;
    m_VecB=NULL;
  }

  return StatusCode::SUCCESS;
}
