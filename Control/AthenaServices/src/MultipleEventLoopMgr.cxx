/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartIF.h"
#include "AthenaKernel/INextPassFilter.h"
#include "MultipleEventLoopMgr.h"

using std::string;
using std::vector;

MultipleEventLoopMgr::MultipleEventLoopMgr(const std::string& nam, 
					   ISvcLocator* svcLoc) :
  AthenaEventLoopMgr(nam, svcLoc),
  m_pToolSvc( "ToolSvc", nam ), 
  m_pAlgMgr(0), m_passDone(0) 
{
  declareProperty("NextPassFilter", m_nextPassFilterName);
  declareProperty("ToBeReinitialized", m_toBeReInitializedNames);
}

MultipleEventLoopMgr::~MultipleEventLoopMgr() {
  if (m_pAlgMgr) m_pAlgMgr->release();
}

IAlgManager* 
MultipleEventLoopMgr::algMgr() {
  if ( 0 == m_pAlgMgr ) {
    SmartIF<IAlgManager> algMan(serviceLocator());
    if( algMan.isValid() ) m_pAlgMgr=&*algMan;
    else throw GaudiException("IAlgManager not found", name(), StatusCode::FAILURE);
    m_pAlgMgr->addRef();
  }
  return m_pAlgMgr;
}

INextPassFilter* 
MultipleEventLoopMgr::nextPassFilter() {
  INextPassFilter* pFilter(0); 
  const string& filterName(m_nextPassFilterName.value());
  if (!(filterName.empty())) {
    ListItem theFilter(filterName);
    IAlgTool* pHoldTool(0);
    if ( (m_pToolSvc->retrieveTool(theFilter.type(), theFilter.name(), 
				   pHoldTool)).isSuccess() ) { 
      pFilter=dynamic_cast<INextPassFilter*>(pHoldTool);
    }
    IAlgorithm* pHoldAlg(0);
    if (0 == pFilter &&
	(algMgr()->getAlgorithm(theFilter.name(), pHoldAlg)).isSuccess() ) {
      pFilter=dynamic_cast<INextPassFilter*>(pHoldAlg);
    }
  }
  if (0 == pFilter) {
    ListItem theFilter(filterName);
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "Could not locate filter " 
	<< theFilter.type() << '/' << theFilter.name() << endmsg;
  }
  return pFilter;
}
bool
MultipleEventLoopMgr::doNextPass() {
  INextPassFilter* pFilter(nextPassFilter());
  //if no tool found or tool not an INextPassFilter we return false
  //and terminate the multiple pass iteration
  return ( 0 != pFilter && pFilter->doNextPass() );
}
StatusCode
MultipleEventLoopMgr::reInitList() {
  StatusCode sc(StatusCode::SUCCESS);
  const vector<string>& theNames(m_toBeReInitializedNames.value());
  vector<string>::const_iterator iN(theNames.begin());
  vector<string>::const_iterator iEnd(theNames.end());
  while ( sc.isSuccess() && (iN != iEnd) ) {
    ListItem theSvc(*iN++); //not really needed but safer...
    IService* pSvc(0);
    sc = serviceLocator()->getService(theSvc.name(), pSvc);
    if (sc.isSuccess()) sc = pSvc->reinitialize();
  }
  return sc;
}

StatusCode 
MultipleEventLoopMgr::nextEvent(int maxevt) {
  StatusCode sc;
  do {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "nextEvent: starting pass #" << m_passDone << endreq;
    // Reset run number to assure BeginRun each rewind
    m_currentRun = 0;
    sc = AthenaEventLoopMgr::nextEvent(maxevt);
    log << MSG::INFO << "nextEvent: finished pass #" << m_passDone << endreq;
    m_passDone++;
  } while ( sc.isSuccess() &&                    //pass ok
	    doNextPass() &&                      //another one?
	    (sc = reInitList()).isSuccess() &&   //then reinit svcs
	    (sc = seek(0)).isSuccess() );        //and rewind selector
  return sc;
}
    
