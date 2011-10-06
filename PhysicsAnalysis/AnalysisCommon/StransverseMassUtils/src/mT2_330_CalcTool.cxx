/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "StransverseMassUtils/mT2_330_CalcTool.h"

#include "GaudiKernel/MsgStream.h"

#include "Mt2/Analytic_Mt2_330_Calculator.h"
#include "Mt2/Mt2Vectors.h"

#include<string>

mT2_330_CalcTool::mT2_330_CalcTool(const std::string& type, const std::string& name, const IInterface* parent):
  AlgTool(type,name,parent)
  {

  declareInterface<mT2_330_CalcTool>( this );

}


mT2_330_CalcTool::~mT2_330_CalcTool() { }



StatusCode mT2_330_CalcTool::initialize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Initializing " << name() << endreq;
 
  return StatusCode::SUCCESS;
}



StatusCode mT2_330_CalcTool::execute(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Executing " << name() << endreq;

  return StatusCode::SUCCESS;
}



StatusCode mT2_330_CalcTool::finalize(){
  MsgStream mLog(msgSvc(),name());
  mLog << MSG::DEBUG << "Finalizing " << name() << endreq;

  return StatusCode::SUCCESS;
}
