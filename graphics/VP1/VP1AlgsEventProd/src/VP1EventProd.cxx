/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1AlgsEventProd/VP1EventProd.h"

#include "VP1UtilsBase/VP1FileUtilities.h"

#include "StorageSvc/DbType.h"
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
  m_runNumber(0),
  m_eventNumber(0),
  m_timeStamp(0),
  m_humanTimestamp(""),
  m_nEvent(0)
{
  declareProperty("InputPoolFile", m_inputPoolFile="");
  declareProperty("DestinationDirectory", m_destinationDir="."); // produce files in the run directory by default
  declareProperty("ForceCreateDestinationDirectory", m_createDestinationDir=true); // force creation of new output folder
  declareProperty("MaxNumberOfFiles", m_maxProducedFiles=-1);     // keep N event files in the run directory. Default is '-1' that means ALL.
  declareProperty("OutputFileTypeLabel", m_outputFileType="");        // put a label at the end of the output file, stating its file type
  declareProperty("RemoveTempInputFiles", m_removeTempInputFiles=true);        // remove the temp input file, i.e. the single-event files produced by the Athena job (HITS, ESD, ...) and which are then copied to files called "vp1_...pool.root"

  // Note: you cannot initialize a VP1FileUtils instance here, because otherwise it would take the default properties only.

}

VP1EventProd::~VP1EventProd()
{
}

StatusCode VP1EventProd::initialize()
{
  msg(MSG::INFO) << " in initialize() " << endmsg;

  StatusCode result = StatusCode::SUCCESS;

  // use the incident service to register a handle
  IIncidentSvc* incsvc = 0;
  StatusCode status = service("IncidentSvc", incsvc, true);

  if(status.isFailure() || incsvc==0)
    msg(MSG::WARNING) << "Unable to get IncidentSvc!" << endmsg;
  else
    incsvc->addListener(this, "BeginEvent", 0);

  return result;
}

StatusCode VP1EventProd::execute()
{
  msg(MSG::DEBUG) <<" in execute() " << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode VP1EventProd::finalize()
{
  msg(MSG::INFO) <<"in finalize() " << endmsg;

  // handle the output of the last event
  if(m_nEvent) {

    msg(MSG::INFO) << "--> Input POOL file: " << m_inputPoolFile << endmsg;

    std::ostringstream ostri;
    ostri << m_inputPoolFile << "._" << std::setw(4) << std::setfill('0') << m_nEvent;

    try {

    	/* clean the output directory if m_maxProducedFiles == 0
    	 * or keep up to 'm_maxProducedFiles' output files
    	 */
    	VP1FileUtilities fileUtil(".", m_maxProducedFiles, m_destinationDir, m_createDestinationDir, m_removeTempInputFiles); // inputDir, fileLimit, outputDir, forceMakeOutputDir, removeInputFile

    	if (m_outputFileType != "")
    		fileUtil.produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp+"."+m_outputFileType);
    	else
    		fileUtil.produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp);
    }
    catch(std::runtime_error& err) {
      msg(MSG::WARNING) << "Exception caught: " << err.what() << endmsg;
      msg(MSG::WARNING) << "Unable to produce new VP1 event file" << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

void VP1EventProd::handle(const Incident& inc) 
{
  msg(MSG::INFO) << "Handling incident '" << inc.type() << "'" <<  endmsg;

  const EventIncident* eventInc  = dynamic_cast<const EventIncident*>(&inc);
  if(eventInc == 0) {
    msg(MSG::WARNING) << " Unable to cast incident type" << endmsg;
    return;
  } else {
    msg(MSG::DEBUG) << " Event incident casting successful" << endmsg;
  }

  // Let VP1FileUtilities handle the output of the previous event.
  // Skip this if m_nEvent == 0,
  // because the processing of the event is not done yet
  // and the input file is not made at this stage.
  // Basically we run the code below while in event_2, to get the processed file for event_1
  if(m_nEvent) {

    msg(MSG::INFO) << "--> Input POOL file: " << m_inputPoolFile << endmsg;

    std::ostringstream ostri;
    ostri << m_inputPoolFile << "._" << std::setw(4) << std::setfill('0') << m_nEvent;

    try {

    	/* clean the output directory if m_maxProducedFiles == 0
    	 * or keep up to 'm_maxProducedFiles' output files
    	 */
    	VP1FileUtilities fileUtil(".", m_maxProducedFiles, m_destinationDir, m_createDestinationDir, m_removeTempInputFiles); // inputDir, fileLimit, outputDir, forceMakeOutputDir, removeInputFile

    	if (m_outputFileType != "")
    		fileUtil.produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp+"."+m_outputFileType); // with UNIX and human-readable timestamp
    	else
    		fileUtil.produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp); // with UNIX and human-readable timestamp

    }
    catch(std::runtime_error& err) {
      msg(MSG::WARNING) << "Exception caught: " << err.what() << endmsg;
      msg(MSG::WARNING) << "Unable to produce new VP1 event file" << endmsg;
    }
  }


  /*
   * -------------------------------------------------------------------
   * === NOTE!!! ===
   *
   * AFTER having processed the file from previous event,
   * we now update the run number and event number for the current event
   * --------------------------------------------------------------------
   */

  // increment event count
  m_nEvent++;

  // Update run_number/event_number/time_stamp
  const EventContext& context = getContext();
  m_eventNumber = context.eventID().event_number();
  m_runNumber = context.eventID().run_number();
  m_timeStamp = context.eventID().time_stamp();

  msg(MSG::DEBUG) << " Got run number = " << m_runNumber
		  << ", event number = " << m_eventNumber
		  << ", UNIX timestamp = " << m_timeStamp << endmsg;

  time_t t_timestamp = m_timeStamp;
  tm *ltm = localtime(&t_timestamp);

  // print various components of tm structure.
  msg(MSG::DEBUG) << "Year: "<< 1900 + ltm->tm_year
		  << " - " << "Month: "<< 1 + ltm->tm_mon<< " - "  // tm_mon is in the range [0, 11], so 1 must be added to get real months
		  << "Day: "<<  ltm->tm_mday
		  << " - " "Time: "<< ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "CEST"
		  << endmsg;

  std::ostringstream ostri;
  ostri  << 1900 + ltm->tm_year
		  << "-" << 1 + ltm->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
		  << "-" << ltm->tm_mday
		  << "T" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec << "CEST";

  m_humanTimestamp = ostri.str();
  msg(MSG::DEBUG) << "'human readable' timestamp: " << m_humanTimestamp << endmsg;



// OLD CODE
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
//		    << ", UNIX timestamp = " << m_timeStamp << endmsg;
//
//    time_t t_timestamp = m_timeStamp;
//    tm *ltm = localtime(&t_timestamp);
//
//    // print various components of tm structure.
//    msg(MSG::DEBUG) << "Year: "<< 1900 + ltm->tm_year
//		    << " - " << "Month: "<< 1 + ltm->tm_mon<< " - "  // tm_mon is in the range [0, 11], so 1 must be added to get real months
//		    << "Day: "<<  ltm->tm_mday
//		    << " - " "Time: "<< ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "CEST"
//		    << endmsg;
//
//    std::ostringstream ostri;
//    ostri  << 1900 + ltm->tm_year
//	   << "-" << 1 + ltm->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
//	   << "-" << ltm->tm_mday
//	   << "T" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec << "CEST";
//
//    m_humanTimestamp = ostri.str();
//    msg(MSG::DEBUG) << "'human readable' timestamp: " << m_humanTimestamp << endmsg;
//
//  }
//  else {
//    msg(MSG::ERROR) << " Unable to retrieve EventInfo from StoreGate" << endmsg;
//    m_eventNumber = 0;
//    m_runNumber = 0;
//    m_timeStamp = 0;
//  }

}

