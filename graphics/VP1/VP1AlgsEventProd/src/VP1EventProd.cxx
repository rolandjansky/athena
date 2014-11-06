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
#include <ctime> /* time_t, time, ctime */


VP1EventProd::VP1EventProd(const std::string& name, ISvcLocator* svcLocator):
  AthAlgorithm(name, svcLocator),
  m_fileUtil(0),
  m_runNumber(0),
  m_eventNumber(0),
  m_timeStamp(0),
  m_humanTimestamp(""),
  m_nEvent(0),
  m_poolSvc("PoolSvc", name)
{
  declareProperty("InputPoolFile", m_inputPoolFile="");
  declareProperty("DestinationDirectory", m_destinationDir="."); // produce files in the run directory by default
  declareProperty("MaxNumberOfFiles", m_maxProducedFiles=5);     // keep 5 event files in the run directory
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
    m_fileUtil = new VP1FileUtilities(m_destinationDir, m_maxProducedFiles);
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
    incsvc->addListener(this, "BeginEvent", 0);

  // get hold of the PoolSvc
  status = m_poolSvc.retrieve();
  if(status.isFailure())
    msg(MSG::WARNING) << "Unable to get PoolSvc" << endreq;

  return result;
}

StatusCode VP1EventProd::execute()
{
  msg(MSG::DEBUG) <<" in execute() " << endreq;

  return StatusCode::SUCCESS;
}

StatusCode VP1EventProd::finalize()
{
  msg(MSG::INFO) <<"in finalize() " << endreq;

  // Let m_fileUtil handle the output of the last event
  if(m_nEvent) {
    
    msg(MSG::INFO) << "--> Input POOL file: " << m_inputPoolFile << endreq;

    std::ostringstream ostri;
    ostri << m_inputPoolFile << "._" << std::setw(4) << std::setfill('0') << m_nEvent;
    
    try {
      if (m_fileUtil) delete m_fileUtil;
      m_fileUtil = new VP1FileUtilities(m_destinationDir,m_maxProducedFiles);
      m_fileUtil->produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp);
      delete m_fileUtil;
    }
    catch(std::runtime_error& err) {
      msg(MSG::WARNING) << "Exception caught: " << err.what() << endreq;
      msg(MSG::WARNING) << "Unable to produce new VP1 event file" << endreq;
      return StatusCode::FAILURE;
    }
  }

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

    // Let m_fileUtil handle the output of the previous event
    if(m_nEvent) {

      msg(MSG::INFO) << "--> Input POOL file: " << m_inputPoolFile << endreq;

      std::ostringstream ostri;
      ostri << m_inputPoolFile << "._" << std::setw(4) << std::setfill('0') << m_nEvent;

      try {
	    if (m_fileUtil) delete m_fileUtil;
	        m_fileUtil = new VP1FileUtilities(m_destinationDir,m_maxProducedFiles);
	        //m_fileUtil->produceNewFile(ostri.str(),m_runNumber,m_eventNumber,m_timeStamp); // with UNIX timestamp
            m_fileUtil->produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp); // with UNIX and human-readable timestamp
      }
      catch(std::runtime_error& err) {
	    msg(MSG::WARNING) << "Exception caught: " << err.what() << endreq;
	    msg(MSG::WARNING) << "Unable to produce new VP1 event file" << endreq;
	    return;
      }
    }
    m_nEvent++;

    // Update run_number/event_number/time_stamp
    
    const EventInfo*  _evt = 0; // pointer for the event
    StatusCode status = evtStore()->retrieve(_evt); // retrieve the pointer to the event
    
    if(status.isSuccess() && _evt!=0) {
      
      m_eventNumber = _evt->event_ID()->event_number(); // from EventInfo/EventID.h
      m_runNumber = _evt->event_ID()->run_number();
      m_timeStamp = _evt->event_ID()->time_stamp();
      
      msg(MSG::DEBUG) << " Got run number = " << m_runNumber
		              << ", event number = " << m_eventNumber 
                      << ", UNIX timestamp = " << m_timeStamp << endreq;

      time_t t_timestamp = m_timeStamp;
      tm *ltm = localtime(&t_timestamp);

      // print various components of tm structure.
      msg(MSG::DEBUG) << "Year: "<< 1900 + ltm->tm_year 
                      << " - " << "Month: "<< 1 + ltm->tm_mon<< " - "  // tm_mon is in the range [0, 11], so 1 must be added to get real months
                      << "Day: "<<  ltm->tm_mday 
                      << " - " "Time: "<< ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "CEST" 
                      << endreq;

      std::ostringstream ostri;
      ostri  << 1900 + ltm->tm_year 
             << "-" << 1 + ltm->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
             << "-" << ltm->tm_mday 
             << "T" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec << "CEST";

      m_humanTimestamp = ostri.str();
      msg(MSG::DEBUG) << "'human readable' timestamp: " << m_humanTimestamp << endreq;

    }
    else {
      msg(MSG::ERROR) << " Unable to retrieve EventInfo from StoreGate" << endreq;
      m_eventNumber = 0;
      m_runNumber = 0;
      m_timeStamp = 0;
    }
  }
}

