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
#include "MuonCondInterface/IMuonAlignmentDbTool.h"
#include "MuonCondSvc/MuonAlignmentDbSvc.h"

namespace MuonCalib 
{  
MuonAlignmentDbSvc::MuonAlignmentDbSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_dbTool(0)
{


     m_dbToolType = "MuonCalib::MuonAlignmentDbTool";
     m_dbToolName = "MuonCalib_MuonAlignmentDbTool";

                   
  
  declareProperty("DBToolType", m_dbToolType);
  declareProperty("DBToolName", m_dbToolName);
  
  m_alineDataLocation="ALineKey"; 
  m_blineDataLocation="BLineKey"; 
                                 
}


  ///
StatusCode MuonAlignmentDbSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  if(InterfaceID_IMuonAlignmentDbSvc.versionMatch(riid) ) 
    { 
      *ppvIF = (MuonAlignmentDbSvc*)this; 
    } else { 
      return AthService::queryInterface(riid, ppvIF); 
    }
  return StatusCode::SUCCESS;
} 

  ///
StatusCode MuonAlignmentDbSvc::initialize(){

  MsgStream log(msgSvc(), name());
  StatusCode sc ;

  log << MSG::INFO << "in initialize()" << endmsg;


  IToolSvc* p_toolSvc;
  sc = service("ToolSvc", p_toolSvc);

  log << MSG::INFO<<"Trying to locate the Tool"<<endmsg;
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


StatusCode MuonAlignmentDbSvc::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}




}
