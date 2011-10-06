/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/mT2_332_CalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include "Mt2/Basic_Mt2_332_Calculator.h"
#include "Mt2/TemporaryZhenyuHanWrapper_Mt2_332_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<string>




// No longer required: class Mt2::Basic_Mt2_332_Calculator;


mT2_332_CalcTool::mT2_332_CalcTool(const std::string& type, const std::string& name, const IInterface* parent):
  AlgTool(type,name,parent),
  m_Basic_mt2(NULL),m_Bisect_mt2(NULL),m_calc(NULL),
  m_VecA(NULL),m_VecB(NULL),m_mpt(NULL),
  m_Chi(0.0)
{

  declareInterface<mT2_332_CalcTool>( this );


}


mT2_332_CalcTool::~mT2_332_CalcTool() {
  if( m_calc ) delete m_calc;
  if( m_VecA ) delete m_VecA;
  if( m_VecB ) delete m_VecB;
  if( m_mpt ) delete m_mpt;
}



StatusCode mT2_332_CalcTool::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  this->setType( Mt2::Bisect );

  return StatusCode::SUCCESS;
}

StatusCode mT2_332_CalcTool::initialize(Mt2::Atl4Vec const a,Mt2::Atl4Vec const b,Mt2::AtlMET mpt,double Chi){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;

  this->setType( Mt2::Bisect );
  setMom(a,b,mpt);
  setChi(Chi);

  return StatusCode::SUCCESS;

}


StatusCode mT2_332_CalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing " << name() << endreq;


  return StatusCode::SUCCESS;
}



StatusCode mT2_332_CalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing " << name() << endreq;
  
  if( m_calc ){
    delete m_calc;
    m_calc=NULL;
  }

  return StatusCode::SUCCESS;

}


