/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     AthenaMon.cxx
//
// AUTHOR:   Rob McPherson & Manuel Diaz
//	     Manuel Diaz 01.04.2004  Changed interface access from 
//				     IAlgTool to IMonitorToolBase	
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "AthenaMonitoring/AthenaMon.h"

class ISvcLocator;
// class AlgFactory;

/*---------------------------------------------------------------*/
AthenaMon::AthenaMon(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{   
  MsgStream log(msgSvc(), "AthenaMon");
  
  declareProperty("AthenaMonTools",m_monToolNames);
  declareProperty("CheckEveryNoEvents",m_everyNevents=-1);
  declareProperty("BookHistsinInitialize",m_bookHistsinInitialize=true);
  m_THistSvc_streamnameMapping.clear();
  declareProperty("THistSvc_OutPutStreamMapping",m_THistSvc_streamnameMapping);
}

/*---------------------------------------------------------------*/
AthenaMon::~AthenaMon()
{
}

/*---------------------------------------------------------------*/
StatusCode AthenaMon::initialize()
/*---------------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize AthenaMon algorithm" << endreq;

  std::vector<std::string>::iterator it = m_monToolNames.begin();
  IToolSvc* p_toolSvc;
  StatusCode sc = service("ToolSvc",p_toolSvc);

  if (sc.isFailure()) {
    log << MSG::FATAL << " Tool Service not found " << endreq;
    return StatusCode::FAILURE;
  }

  for (; it < m_monToolNames.end(); it++) {
 
    std::string toolname(*it);
    IMonitorToolBase* p_tool;
    
    ListItem mytool(toolname);

    sc = p_toolSvc->retrieveTool(mytool.type(), mytool.name(), p_tool);
    if(sc.isFailure()) {
      log << MSG::FATAL << "Unable to create " << toolname
	  << " AlgTool" << endreq;
      return StatusCode::FAILURE;
    } else {
      
      log << MSG::INFO
	  << "Tool Name = " << toolname
	  << endreq;
      
      sc = p_tool->setupOutputStreams(m_THistSvc_streamnameMapping);
      if(sc.isFailure()) {
	log << MSG::WARNING << "Unable to setup the OutPutStreams in "
	    << toolname << endreq;
      }
      
      // shall I book histograms now ?
      if(m_bookHistsinInitialize)
	{
	  // Try to book the histograms now.  If the tool uses
	  // dynamic booking, it should define bookHists as empty.
	  sc = p_tool->bookHists();
	  if(sc.isFailure()) {
	    log << MSG::WARNING << "Unable to book in " << toolname << endreq;
	  }
	}
      m_monTools.push_back(p_tool);
    }
  } 
  m_eventCounter=m_everyNevents;
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------------*/
StatusCode AthenaMon::execute()
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "executing AthenaMon algorithm" << endreq;
  
  //Invoke all declared alg monitoring tools to fill their histograms
  std::vector<IMonitorToolBase*>::iterator it = m_monTools.begin();
  
  for (; it < m_monTools.end(); it++)  {
    if((*it)->preSelector())
      if((*it)->fillHists().isFailure())  {
	log << MSG::WARNING << "Error Filling Histograms" << endreq;
	// return StatusCode::FAILURE;
      }
  }
  if(m_eventCounter==0) {
    //Invoke periodically all declared alg monitoring tools to check their histograms
    it = m_monTools.begin();
    
    for (; it < m_monTools.end(); it++)  {
      log << MSG::INFO << "calling checkHists of tool " << endreq;
      StatusCode sc = (*it)->checkHists(false);
      if(sc.isFailure())  {
	log << MSG::WARNING << "Can\'t call checkHists of tool." << endreq;
	// return StatusCode::FAILURE;
      }
    }
    m_eventCounter=m_everyNevents;
  }
  if(m_eventCounter>0) m_eventCounter--;
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------------*/
StatusCode AthenaMon::finalize()
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "finalizing AthenaMon algorithm" << endreq;
  
  //Invoke all declared alg monitoring tools to finalize their histograms
  std::vector<IMonitorToolBase*>::iterator it = m_monTools.begin();
  
  for (; it < m_monTools.end(); it++)  {
    log << MSG::INFO << "finalizing tool " << endreq;
    StatusCode sc = (*it)->finalHists();
    if(sc.isFailure())  {
      log << MSG::WARNING << "Can\'t finalize a tool." << endreq;
      // return StatusCode::FAILURE;
    }
  }
  //Invoke all declared alg monitoring tools to check their histograms
  it = m_monTools.begin();
  
  for (; it < m_monTools.end(); it++)  {
    log << MSG::INFO << "calling checkHists of tool " << endreq;
    StatusCode sc = (*it)->checkHists(true);
    if(sc.isFailure())  {
      log << MSG::WARNING << "Can\'t call checkHists of tool." << endreq;
      // return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------------*/
StatusCode AthenaMon::beginRun()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "beginRun()" << endreq;
  
  // histograms already booked
  if(m_bookHistsinInitialize)
    return StatusCode::SUCCESS;
  
  std::vector<IMonitorToolBase*>::iterator it = m_monTools.begin();
  for (; it < m_monTools.end(); it++)
    if((*it)->bookHists().isFailure())
      log << MSG::WARNING << "Error Filling Histograms" << endreq;
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------------*/
StatusCode AthenaMon::endRun()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "endRun()" << endreq;
  
  std::vector<IMonitorToolBase*>::iterator it = m_monTools.begin();
  for (; it < m_monTools.end(); it++)
    if((*it)->runStat().isFailure())
      log << MSG::WARNING << "Error calling runStat" << endreq;
  
  return StatusCode::SUCCESS;
}
