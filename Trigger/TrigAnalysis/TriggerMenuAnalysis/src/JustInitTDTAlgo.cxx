/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  JustInitTDTAlgo.cxx
*/
#include "TriggerMenuAnalysis/JustInitTDTAlgo.hxx"

JustInitTDTAlgo::JustInitTDTAlgo(const std::string& name, ISvcLocator* svcloc) : 
  Algorithm(name, svcloc), 
  mTrigAccessTool("TrigAccessTool/TrigAccessTool", this) {
  declareProperty("TrigAccessTool", mTrigAccessTool, "TrigAccessTool");
}

JustInitTDTAlgo::~JustInitTDTAlgo() {
}

StatusCode JustInitTDTAlgo::initialize() {
  if (mTrigAccessTool.retrieve().isFailure()) {
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "Cannot retrieve TrigAccessTool" << endreq;
  }
  return StatusCode::SUCCESS;
}

StatusCode JustInitTDTAlgo::execute() {
  return StatusCode::SUCCESS;
}

StatusCode JustInitTDTAlgo::finalize() {
  return StatusCode::SUCCESS;
}
