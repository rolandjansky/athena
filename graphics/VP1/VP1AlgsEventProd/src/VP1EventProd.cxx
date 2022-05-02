/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1AlgsEventProd/VP1EventProd.h"

#include "VP1UtilsBase/VP1FileUtilities.h"

#include "StorageSvc/DbType.h"
#include "PathResolver/PathResolver.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h" // ATH_MSG_* macros

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
  ATH_MSG_INFO(" in initialize() ");

  StatusCode result = StatusCode::SUCCESS;

  // use the incident service to register a handle
  IIncidentSvc* incsvc = 0;
  StatusCode status = service("IncidentSvc", incsvc, true);

  if(status.isFailure() || incsvc==0)
    ATH_MSG_WARNING("Unable to get IncidentSvc!");
  else
    incsvc->addListener(this, "BeginEvent", 0);

  return result;
}

StatusCode VP1EventProd::execute()
{
  ATH_MSG_DEBUG(" in execute(). Nothing to do here...");

  return StatusCode::SUCCESS;
}

StatusCode VP1EventProd::finalize()
{
  ATH_MSG_INFO("in finalize() ");

  // handle the output of the last event
  if(m_nEvent) {

    --m_nEvent; // since we don't use another call to handle() to process the last event, we need to revert the counter by one, otherwise the wrong file is looked for

    ATH_MSG_INFO("--> Input POOL file: " << m_inputPoolFile);

    std::ostringstream ostri;
    ostri << m_inputPoolFile << "._" << std::setw(4) << std::setfill('0') << m_nEvent;

    std::string inputFileName = ostri.str();
    ATH_MSG_DEBUG("copying the input file: '"<< inputFileName << "'...");
    
    try {

        /* clean the output directory if m_maxProducedFiles == 0
         * or keep up to 'm_maxProducedFiles' output files
         */
        VP1FileUtilities fileUtil(".", m_maxProducedFiles, m_destinationDir, m_createDestinationDir, m_removeTempInputFiles); // inputDir, fileLimit, outputDir, forceMakeOutputDir, removeInputFile

        if (m_outputFileType != "")
            fileUtil.produceNewFile(inputFileName, m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp+"."+m_outputFileType); // with UNIX and human-readable timestamp
        else
            fileUtil.produceNewFile(inputFileName, m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp); // with UNIX timestamp
    }
    catch(std::runtime_error& err) {
      ATH_MSG_WARNING("Exception caught: " << err.what());
      ATH_MSG_WARNING("In finalize() -- Unable to produce new VP1 event file");
    }
  }
  
  return StatusCode::SUCCESS;
}



void VP1EventProd::handle(const Incident& inc) 
{
  ATH_MSG_INFO("in handle()... ");
  ATH_MSG_INFO("Handling incident '" << inc.type() << "'");

  // Let VP1FileUtilities handle the output of the previous event.
  // Skip this if m_nEvent == 0,
  // because the processing of the event is not completed, yet;
  // and the input file is not created at this stage, yet.
  // Basically we run the code below while in event_2, to get the processed file for event_1
  if(m_nEvent) {

   unsigned  int nLastFile = m_nEvent - 1; // we copy the file produced while processing the previous event, so we need a file number of (current - 1)

    ATH_MSG_INFO("--> Input POOL file: " << m_inputPoolFile);

    std::ostringstream ostri;
    ostri << m_inputPoolFile << "._" << std::setw(4) << std::setfill('0') << nLastFile;

    std::string inputFileName = ostri.str();
    ATH_MSG_DEBUG("copying the input file: '"<< inputFileName << "'...");

    try {

        /* clean the output directory if m_maxProducedFiles == 0
         * or keep up to 'm_maxProducedFiles' output files
         */
        VP1FileUtilities fileUtil(".", m_maxProducedFiles, m_destinationDir, m_createDestinationDir, m_removeTempInputFiles); // inputDir, fileLimit, outputDir, forceMakeOutputDir, removeInputFile

        if (m_outputFileType != "")
            fileUtil.produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp+"."+m_outputFileType); // with UNIX and human-readable timestamp
        else
            fileUtil.produceNewFile(ostri.str(), m_runNumber, m_eventNumber, m_timeStamp, m_humanTimestamp); // with UNIX timestamp

    }
    catch(std::runtime_error& err) {
      ATH_MSG_WARNING("Exception caught: " << err.what());
      ATH_MSG_WARNING("In handle() -- Unable to produce new VP1 event file");
    }
  }


  /*
   * -------------------------------------------------------------------
   * === NOTE!!! ===
   *
   * AFTER having processed the file from the previous event,
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

  ATH_MSG_DEBUG(" Got run number = " << m_runNumber
          << ", event number = " << m_eventNumber
          << ", UNIX timestamp = " << m_timeStamp);

  time_t t_timestamp = m_timeStamp;
  tm *ltm = localtime(&t_timestamp);

  // print various components of tm structure.
  ATH_MSG_DEBUG("Year: "<< 1900 + ltm->tm_year
          << " - " << "Month: "<< 1 + ltm->tm_mon<< " - "  // tm_mon is in the range [0, 11], so 1 must be added to get real months
          << "Day: "<<  ltm->tm_mday
          << " - " "Time: "<< ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "CEST"
         );

  std::ostringstream ostri;
  ostri  << 1900 + ltm->tm_year
          << "-" << 1 + ltm->tm_mon  // tm_mon is in the range [0, 11], so 1 must be added to get real months
          << "-" << ltm->tm_mday
          << "T" << ltm->tm_hour << "-" << ltm->tm_min << "-" << ltm->tm_sec << "CEST";

  m_humanTimestamp = ostri.str();
  ATH_MSG_DEBUG("'human readable' timestamp: " << m_humanTimestamp);

}

