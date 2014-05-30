/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1AlgsEventProd/VP1EventProd.h"

#include "VP1UtilsBase/VP1FileUtilities.h"

#include "StorageSvc/DbType.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventIncident.h"
#include "PathResolver/PathResolver.h"

#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <unistd.h>

VP1EventProd::VP1EventProd(const std::string& name, ISvcLocator* svcLocator):
  AthAlgorithm(name, svcLocator),
  m_fileUtil(0),
  m_runNumber(0),
  m_eventNumber(0),
  m_timeStamp(0),
  m_poolSvc("PoolSvc",name)
{
  declareProperty("InputPoolFile",m_inputPoolFile="");
  declareProperty("DestinationDirectory",m_destinationDir="."); // produce files in the run directory by default
  declareProperty("MaxNumberOfFiles",m_maxProducedFiles=5);     // keep 5 event files in the run directory
}

VP1EventProd::~VP1EventProd()
{
}

StatusCode VP1EventProd::initialize()
{
  msg(MSG::INFO) << " in initialize() " << endreq;

  StatusCode result = StatusCode::SUCCESS;

  // initialize file utility
  try
  {
    m_fileUtil = new VP1FileUtilities(m_destinationDir,m_maxProducedFiles);
  }
  catch(std::runtime_error& err)
  {
    msg(MSG::WARNING) << "Exception caught: " << err.what() << endreq;
    return result;
  }

  // use the incident service to register a handle
  IIncidentSvc* incsvc = 0;
  StatusCode status = service("IncidentSvc", incsvc, true);
 
  if(status.isFailure() || incsvc==0)
    msg(MSG::WARNING) << "Unable to get IncidentSvc!" << endreq;
  else
    incsvc->addListener(this, "EndEvent", 0);

  // get hold of the PoolSvc
  status = m_poolSvc.retrieve();
  if(status.isFailure())
    msg(MSG::WARNING) << "Unable to get PoolSvc" << endreq;

  return result;
}

StatusCode VP1EventProd::execute()
{
  msg(MSG::DEBUG) <<" in execute() " << endreq;

  // Do something only if m_fileUtil!=0
  if(m_fileUtil)
  {
    const EventInfo*  _evt = 0;
    StatusCode status = evtStore()->retrieve(_evt);
    if(status.isSuccess() && _evt!=0) 
    {
      m_eventNumber = _evt->event_ID()->event_number();
      m_runNumber = _evt->event_ID()->run_number();
      m_timeStamp = _evt->event_ID()->time_stamp();
      msg(MSG::DEBUG) << " Got run number = " << m_runNumber
	  << ", event number = " << m_eventNumber << endreq;
    }
    else
    {
      msg(MSG::WARNING) << " Unable to retrieve EventInfo from StoreGate" << endreq;
      m_eventNumber = 0;
      m_runNumber = 0;
      m_timeStamp = 0;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode VP1EventProd::finalize()
{
  msg(MSG::INFO) <<" in finalize() " << endreq;

  if(m_fileUtil) delete m_fileUtil;

  return StatusCode::SUCCESS;
}

void VP1EventProd::handle(const Incident& inc) 
{
  msg(MSG::INFO) << "Handling incident '" << inc.type() << "'" <<  endreq;

  // Do something only if m_fileUtil!=0
  if(m_fileUtil)
  {
    const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
    if(eventInc == 0) {
      msg(MSG::WARNING) << " Unable to cast incident type" << endreq;
      return;
    } else {
      msg(MSG::DEBUG) << " Event incident casting successful" << endreq;
    }

    //  **** Generate new pool file with IPoolSvc ****
    StatusCode sc(StatusCode::SUCCESS);
    if(m_poolSvc) {
      sc = m_poolSvc->commit(IPoolSvc::kOutputStream);
      if(sc.isFailure()) {
	msg(MSG::WARNING) << " Could not commit the output" << endreq;
	return;
      } else {
	msg(MSG::DEBUG) << " Commit output OK" << endreq;
      }

      sc = m_poolSvc->disconnect(IPoolSvc::kOutputStream);
      if(sc.isFailure()) {
	msg(MSG::WARNING) << " Could not disconnect from output" << endreq;
	return;
      } else {
	msg(MSG::DEBUG) << " Output disconnected OK" << endreq;
      }
    }
    
    //  **** Generate name for the new VP1 event file ****
    //
    // vp1_<run>_<event>_<time>.pool.root
    //
    
    //  **** Produce new VP1 event file ****
    try {
      if (m_fileUtil) delete m_fileUtil;
      m_fileUtil = new VP1FileUtilities(m_destinationDir,m_maxProducedFiles);
      m_fileUtil->produceNewFile(m_inputPoolFile,m_runNumber,m_eventNumber,m_timeStamp);
    }
    catch(std::runtime_error& err) {
      msg(MSG::WARNING) << "Exception caught: " << err.what() << endreq;
      msg(MSG::WARNING) << "Unable to produce new VP1 event file" << endreq;
    }

    sc = m_poolSvc->setAttribute("DEFAULT_SPLITLEVEL","0",pool::ROOTTREE_StorageType.type());
    if(sc.isFailure()) {
      msg(MSG::WARNING) << " Could not set attribute" << endreq;
      return;
    }
    else {
      msg(MSG::DEBUG) << " Set attribute OK" << endreq;
    }
  }
}

