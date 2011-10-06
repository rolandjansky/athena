/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/mTGen_332_CalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include "Mt2/Basic_MtGen_332_Calculator.h"
#include "Mt2/Mt2_332_Calculator.h"

#include<string>


mTGen_332_CalcTool::mTGen_332_CalcTool(const std::string& type, const std::string& name,  const IInterface* parent):
  AlgTool(type,name,parent),
  m_mpt(NULL),
  m_mTGen_Calc(NULL),m_mTGen(NULL),
  m_mChi(0.0)
{

  declareInterface<mTGen_332_CalcTool>( this );

  m_theEvent.reserve(20); 


}

mTGen_332_CalcTool::~mTGen_332_CalcTool() {
  if(m_mTGen)delete m_mTGen;
  if(m_mTGen_Calc)delete m_mTGen_Calc;
   }

StatusCode mTGen_332_CalcTool::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  StatusCode sc = this->setType(Mt2::Basic332);
  if( sc.isFailure() ){
    mLog << MSG::ERROR << "Could not properly setup mTGen mT2 Calculator" << endreq;
    return sc;
  }
  //m_mTGen = new Mt2::Basic_MtGen_332_Calculator(*m_mTGen_Calc);

  return StatusCode::SUCCESS;
}

StatusCode mTGen_332_CalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing " << name() << endreq;

  return StatusCode::SUCCESS;
}

StatusCode mTGen_332_CalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing " << name() << endreq;

  //delete m_mTGen_Calc;
  delete m_mTGen;
  m_mTGen=0;

  return StatusCode::SUCCESS;
} 
