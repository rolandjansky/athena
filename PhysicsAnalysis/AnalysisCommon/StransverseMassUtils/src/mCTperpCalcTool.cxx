/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/mCTperpCalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include<string>

mCTperpCalcTool::mCTperpCalcTool(const std::string& type, const std::string& name, const IInterface* parent):
  AlgTool(type,name,parent),
  m_VecA(NULL),m_VecB(NULL),m_utm(NULL)
  {

  declareInterface<mCTperpCalcTool>( this );

}

mCTperpCalcTool::~mCTperpCalcTool() {
  if(m_VecA)delete m_VecA;
  if(m_VecB)delete m_VecB;
  if(m_utm)delete m_utm;
 }


StatusCode mCTperpCalcTool::initialize(){
 MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  return StatusCode::SUCCESS;
}

StatusCode mCTperpCalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "executing " << name() << endreq;

  return StatusCode::SUCCESS;
}

StatusCode mCTperpCalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing " << name() << endreq;

  this->clear();

  return StatusCode::SUCCESS;
} 
