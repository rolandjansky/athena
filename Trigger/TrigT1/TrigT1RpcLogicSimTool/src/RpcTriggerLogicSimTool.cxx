/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RpcLogicSimTool/RpcTriggerLogicSimTool.h"
#include "TrigT1RpcLogicSimTool/InefficientTrigger.h"
#include "TrigT1RpcLogicSimTool/LowPtTrigger.h"

RpcTriggerLogicSimTool::RpcTriggerLogicSimTool(const std::string& t, const std::string& n,const IInterface* p)  :
  AlgTool(t,n,p)
{
  declareInterface<IRpcTriggerLogicSimTool>(this);
}

RpcTriggerLogicSimTool::~RpcTriggerLogicSimTool() {
}

StatusCode RpcTriggerLogicSimTool::initialize() {

  m_log = new MsgStream(msgSvc(),name());
  StatusCode sc;
  *m_log << MSG::DEBUG << "RpcTriggerLogicSimTool initialize() called" 
	 << endreq;

  // Inefficient trigger should take care of free-ing resource.
  // Memory leak, at the moment: will change pointer with object, maybe
  m_trigger = new InefficientTrigger(new LowPtTrigger(1000), 1.0, 31); 
  // global ineff., seed

  return StatusCode::SUCCESS;
}

StatusCode RpcTriggerLogicSimTool::finalize() {
  *m_log << MSG::DEBUG << "RpcTriggerLogicSimTool finalize() called" 
	 << endreq;
  delete m_log;
  delete m_trigger;
  return StatusCode::SUCCESS;
}

bool RpcTriggerLogicSimTool::doTrigger(vector<unsigned int> hits,
				       int mode) {
  if (mode==0)
    return m_trigger->doesTrigger(hits);
  else
    return m_trigger->shouldTrigger(hits);
    
}
