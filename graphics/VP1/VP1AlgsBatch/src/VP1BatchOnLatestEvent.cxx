/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1AlgsBatch/VP1BatchOnLatestEvent.h"

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


VP1BatchOnLatestEvent::VP1BatchOnLatestEvent(const std::string& name, ISvcLocator* svcLocator):
  AthAlgorithm(name, svcLocator),
//  m_runNumber(0),
//  m_eventNumber(0),
//  m_timeStamp(0),
//  m_humanTimestamp(""),
  m_nEvent(0),
  m_poolSvc("PoolSvc", name)
{
  declareProperty("VP1ConfigFile", m_inputVP1CfgFile="");
  declareProperty("DestinationDirectory", m_destinationDir="."); // produce files in the run directory by default
//  declareProperty("MaxNumberOfFiles", m_maxProducedFiles=5);     // keep 5 event files in the run directory
}

VP1BatchOnLatestEvent::~VP1BatchOnLatestEvent()
{
}

StatusCode VP1BatchOnLatestEvent::initialize()
{
  msg(MSG::INFO) << " in initialize() " << endreq;

  StatusCode result = StatusCode::SUCCESS;

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

StatusCode VP1BatchOnLatestEvent::execute()
{
  msg(MSG::DEBUG) <<" in execute() " << endreq;

  return StatusCode::SUCCESS;
}

StatusCode VP1BatchOnLatestEvent::finalize()
{
  msg(MSG::INFO) <<"in finalize() " << endreq;

  // Let VP1FileUtilities handle the output of the last event
  if(m_nEvent) {
    
    msg(MSG::INFO) << "--> Input VP1 Configuration file: " << m_inputVP1CfgFile << endreq;

//    std::ostringstream ostri;
//    ostri << m_inputPoolFile << "._" << std::setw(4) << std::setfill('0') << m_nEvent;
    
    /*
    // PRODUCE THE SINGLE-EVENT ESD FILE FOR THE LAST EVENT
    try {
      VP1FileUtilities fileUtil(m_destinationDir,m_maxProducedFiles);
      fileUtil.produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp);
    }
    catch(std::runtime_error& err) {
      msg(MSG::WARNING) << "Exception caught: " << err.what() << endreq;
      msg(MSG::WARNING) << "Unable to produce new VP1 event file" << endreq;
    }
    */


    // LAUNCH VP1-Batch on the latest-produced ESD file
//    QString commandStr = "vp1 -batch `cat latest_vp1event` " + m_inputVP1CfgFile;
    std::string commandStr = "vp1 -batch `cat latest_vp1event` " + m_inputVP1CfgFile;
    msg(MSG::INFO) << " ===> launching VP1-Batch on last event: " << commandStr << endreq;
    try {
    	//system("vp1 -batch `cat latest_vp1event` vp1_conf_truth_event_wTRTGeo.vp1");
    	system(commandStr.c_str());
    } catch (std::runtime_error& err) {
    	msg(MSG::WARNING) << "Exception caught: " << err.what() << endreq;
    	msg(MSG::WARNING) << "Unable to launch VP1-Batch on the latest-produced event file" << endreq;
    }

  }

  return StatusCode::SUCCESS;
}

void VP1BatchOnLatestEvent::handle(const Incident& inc)
{
  msg(MSG::INFO) << "Handling incident '" << inc.type() << "'" <<  endreq;

  const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
  if(eventInc == 0) {
    msg(MSG::WARNING) << " Unable to cast incident type" << endreq;
    return;
  } else {
    msg(MSG::DEBUG) << " Event incident casting successful" << endreq;
  }

  // Let VP1FileUtilities handle the output of the previous event
  if(m_nEvent) {

	msg(MSG::INFO) << "--> Input VP1 Configuration file: " << m_inputVP1CfgFile << endreq;
    
//    std::ostringstream ostri;
//    ostri << m_inputPoolFile << "._" << std::setw(4) << std::setfill('0') << m_nEvent;

    /*
    // PRODUCE THE SINGLE-EVENT ESD FILE
    try {
      VP1FileUtilities fileUtil(m_destinationDir,m_maxProducedFiles);
      //fileUtil.produceNewFile(ostri.str(),m_runNumber,m_eventNumber,m_timeStamp); // with UNIX timestamp
      fileUtil.produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp); // with UNIX and human-readable timestamp
    }
    catch(std::runtime_error& err) {
      msg(MSG::WARNING) << "Exception caught: " << err.what() << endreq;
      msg(MSG::WARNING) << "Unable to produce new VP1 event file" << endreq;
    }
    */

    // LAUNCH VP1-Batch on the latest-produced ESD file
    std::string commandStr = "vp1 -batch `cat latest_vp1event` " + m_inputVP1CfgFile;
    msg(MSG::INFO) << " ===> launching VP1-Batch: " << commandStr << endreq;
    try {
    	//system("vp1 -batch `cat latest_vp1event` vp1_conf_truth_event_wTRTGeo.vp1");
    	system(commandStr.c_str());
    } catch (std::runtime_error& err) {
        msg(MSG::WARNING) << "Exception caught: " << err.what() << endreq;
        msg(MSG::WARNING) << "Unable to launch VP1-Batch on the latest-produced event file" << endreq;
      }
  }
  m_nEvent++;
  
  // Update run_number/event_number/time_stamp
//
//  const EventInfo*  _evt = 0; // pointer for the event
//  StatusCode status = evtStore()->retrieve(_evt); // retrieve the pointer to the event
//
//  if(status.isSuccess() && _evt!=0) {
//
//    m_eventNumber = _evt->event_ID()->event_number(); // from EventInfo/EventID.h
//    m_runNumber = _evt->event_ID()->run_number();
//    m_timeStamp = _evt->event_ID()->time_stamp();
//
//    msg(MSG::DEBUG) << " Got run number = " << m_runNumber
//		    << ", event number = " << m_eventNumber
//		    << ", UNIX timestamp = " << m_timeStamp << endreq;
//
//    time_t t_timestamp = m_timeStamp;
//    tm *ltm = localtime(&t_timestamp);
    
//    // print various components of tm structure.
//    msg(MSG::DEBUG) << "Year: "<< 1900 + ltm->tm_year
//		    << " - " << "Month: "<< 1 + ltm->tm_mon<< " - "  // tm_mon is in the range [0, 11], so 1 must be added to get real months
//		    << "Day: "<<  ltm->tm_mday
//		    << " - " "Time: "<< ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "CEST"
//		    << endreq;
//
//    std::ostringstream ostri;
//    ostri  << 1900 + ltm->tm_year
//	   << "-" << 1 + ltm->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
//	   << "-" << ltm->tm_mday
//	   << "T" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec << "CEST";
//
//    m_humanTimestamp = ostri.str();
//    msg(MSG::DEBUG) << "'human readable' timestamp: " << m_humanTimestamp << endreq;
//
//  }
//  else {
//    msg(MSG::ERROR) << " Unable to retrieve EventInfo from StoreGate" << endreq;
////    m_eventNumber = 0;
////    m_runNumber = 0;
////    m_timeStamp = 0;
//  }


}


