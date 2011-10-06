/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/




#include "StransverseMassUtils/mTGen_330_CalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include "Mt2/Basic_MtGen_330_Calculator.h"
#include "Mt2/Mt2_330_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<string>

mTGen_330_CalcTool::mTGen_330_CalcTool(const std::string& type, const std::string& name, const IInterface* parent):
  AlgTool(type,name,parent),
  m_mTGen_Calc(NULL),m_mTGen(NULL),
  m_mChi(0.0)
{

  declareInterface<mTGen_330_CalcTool>( this );

  m_theEvent.reserve(20);

}


mTGen_330_CalcTool::~mTGen_330_CalcTool() { 
  if(m_mTGen)delete m_mTGen;
  if(m_mTGen_Calc)delete m_mTGen_Calc;
}


StatusCode mTGen_330_CalcTool::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  StatusCode sc = this->setType(Mt2::Analytic330);
  if( sc.isFailure() ){
    mLog << MSG::ERROR << "Could not properly setup mTGen 330 mT2 Calculator" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode mTGen_330_CalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing " << name() << endreq;

  return StatusCode::SUCCESS;
}


StatusCode mTGen_330_CalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing" << endreq;

  if(m_mTGen){
    delete m_mTGen;
    m_mTGen=NULL;
  }
  if(m_mTGen_Calc){
    delete m_mTGen_Calc;
    m_mTGen_Calc=NULL;
  }

  return StatusCode::SUCCESS;
}
