/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "Identifier/Identifier.h"

#include "MuonCondInterface/IRpcDetectorStatusDbTool.h"


/////////////////////////////////////////////////////////
#include "MuonCondSvc/RpcStatusDbSvc.h"


RpcStatusDbSvc::RpcStatusDbSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  Service(name, pSvcLocator){


  m_dbToolType = "RpcDetectorStatusDbTool"; 
  m_dbToolName = "RpcDetectorStatusDbTool";  
  
  declareProperty("DBToolType", m_dbToolType);
  declareProperty("DBToolName", m_dbToolName);
  
  m_rpcStatusDataLocation="RpcKey"; 
  
                                 
}


  ///
StatusCode RpcStatusDbSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  if(InterfaceID_IRpcStatusDbSvc.versionMatch(riid) ) 
    { 
      *ppvIF = (RpcStatusDbSvc*)this; 
    } else { 
      return Service::queryInterface(riid, ppvIF); 
    }
  return StatusCode::SUCCESS;
} 

  ///
StatusCode RpcStatusDbSvc::initialize(){


  MsgStream log(messageService(), name());
  StatusCode sc ;

  log << MSG::DEBUG << "in initialize()" << endreq;


  // access the Tool for retreival of DB info
  IToolSvc* p_toolSvc;
  sc = service("ToolSvc", p_toolSvc);

  sc = p_toolSvc->retrieveTool(m_dbToolType, m_dbToolName, m_dbTool);
    if (sc.isFailure()) {
      log << MSG::FATAL << "Could not find tool " << m_dbToolName << ". Exiting."
	  << endreq;
      return sc;
    } else {
      log << MSG::INFO << "Database tool \"" << m_dbToolName << "\" retrieved."
	  << endreq;
    }


  return StatusCode::SUCCESS;

}


StatusCode RpcStatusDbSvc::finalize() {
   MsgStream log(messageService(), name());
   log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}




//}
