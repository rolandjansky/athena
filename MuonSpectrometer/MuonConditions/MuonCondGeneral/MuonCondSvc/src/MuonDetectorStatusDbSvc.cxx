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


#include "MuonCondInterface/IMuonDetectorStatusDbTool.h"


/////////////////////////////////////////////////////////
#include "MuonCondSvc/MuonDetectorStatusDbSvc.h"
//namespace MuonCalib 
//{  
MuonDetectorStatusDbSvc::MuonDetectorStatusDbSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_dbTool(0)
{


  m_dbToolType = "MuonDetectorStatusDbTool"; 
  m_dbToolName = "MuonDetectorStatusDbTool";  
  
  declareProperty("DBToolType", m_dbToolType);
  declareProperty("DBToolName", m_dbToolName);
  
  m_tubeStatusDataLocation="TubeStatusKey"; 
  
                                 
}


  ///
StatusCode MuonDetectorStatusDbSvc::queryInterface(const InterfaceID& riid, void** ppvIF) 
{ 
  if(InterfaceID_IMuonDetectorStatusDbSvc.versionMatch(riid) ) 
    { 
      *ppvIF = (MuonDetectorStatusDbSvc*)this; 
    } else { 
      return AthService::queryInterface(riid, ppvIF); 
    }
  return StatusCode::SUCCESS;
} 

  ///
StatusCode MuonDetectorStatusDbSvc::initialize(){

  MsgStream log(msgSvc(), name());
  StatusCode sc ;

  log << MSG::DEBUG << "in initialize()" << endmsg;


  // initialize the pointer to the detector store
  /*  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure())   {
    log << MSG::ERROR << "Can't locate the DetectorStore" << endmsg; 
    return sc;
  }

  // retrieve the Mdt Id Helper
  sc = m_detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure())   {
    log << MSG::ERROR << "Can't retrieve MdtIdHelper from the detector store" << endmsg; 
    return sc;
  }

  */
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

  /*
StatusCode MuonDetectorStatusDbSvc::loadParameterStatus(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  StatusCode sc=StatusCode::SUCCESS;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "LoadParameters has been triggered for the following keys " << endmsg;                                    
  std::list<std::string>::const_iterator itr;
  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    log << MSG::INFO << *itr << " I="<<I<<" ";
  }
  log << MSG::INFO << endmsg;

  for (itr=keys.begin(); itr!=keys.end(); ++itr) {
    if(*itr==m_tubeStatusDataLocation) {
      sc=loadTubeStatus(I,keys);
      if(sc!=StatusCode::SUCCESS) return sc;
    }
  }

  return sc;
}


StatusCode MuonDetectorStatusDbSvc::loadTubeStatus(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{ 
  MsgStream log(msgSvc(), name());
  StatusCode sc=StatusCode::SUCCESS;
  log << MSG::DEBUG << "In loadParameters " <<endmsg;  
  sc=m_detStore->retrieve(m_tubeStatusData, m_tubeStatusDataLocation);

  return sc;
}

*/
StatusCode MuonDetectorStatusDbSvc::finalize() {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}




//}
