/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "Identifier/Identifier.h"

#include "MuonCondInterface/IRpcDetectorStatusDbTool.h"


/////////////////////////////////////////////////////////
#include "MuonCondSvc/RpcStatusDbSvc.h"


RpcStatusDbSvc::RpcStatusDbSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_dbTool(nullptr),
  m_rpcStatusDataLocation("RpcKey")
{
  declareProperty("DBToolType", m_dbToolType = "RpcDetectorStatusDbTool");
  declareProperty("DBToolName", m_dbToolName = "RpcDetectorStatusDbTool");
}


  ///
StatusCode RpcStatusDbSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  if(InterfaceID_IRpcStatusDbSvc.versionMatch(riid) ) 
    { 
      *ppvIF = (RpcStatusDbSvc*)this; 
    } else { 
      return AthService::queryInterface(riid, ppvIF); 
    }
  return StatusCode::SUCCESS;
} 

  ///
StatusCode RpcStatusDbSvc::initialize(){


  MsgStream log(msgSvc(), name());
  StatusCode sc ;

  log << MSG::DEBUG << "in initialize()" << endmsg;


  // access the Tool for retreival of DB info
  IToolSvc* p_toolSvc;
  sc = service("ToolSvc", p_toolSvc);

  sc = p_toolSvc->retrieveTool(m_dbToolType, m_dbToolName, m_dbTool);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Could not find tool " << m_dbToolName << ". Exiting."
	  << endmsg;
      return sc;
    } else {
      log << MSG::INFO << "Database tool \"" << m_dbToolName << "\" retrieved."
	  << endmsg;
    }


  return StatusCode::SUCCESS;

}


StatusCode RpcStatusDbSvc::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}




//}
