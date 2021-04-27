/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TBXMLWriterToolBase.h"
#include "TBXMLWriter.h"


// event
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/ListItem.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <iomanip>

TBXMLWriter::TBXMLWriter(const std::string& name, 
			 ISvcLocator* pSvcLocator)
  : AthAlgorithm(name,pSvcLocator)
    , m_outputFrequency(1)
    , m_eventCounter(0)
    , m_beginRun(0)
    , m_runNumber((unsigned int)-1)
    , m_eventNumber(0)
{
  declareProperty("OutputFrequency",m_outputFrequency);
  declareProperty("WriteTools",m_writerToolNames);
  declareProperty("FileTopDirectory",m_topDirectory);
}

TBXMLWriter::~TBXMLWriter()
{ }

////////////////
// Initialize //
////////////////

StatusCode TBXMLWriter::initialize()
{
  //////////////
  // Services //
  //////////////

  // tool service
  IToolSvc* toolSvcPtr = 0;
  ATH_CHECK( service("ToolSvc",toolSvcPtr) );

  /////////////////////////
  // Current Job Options //
  /////////////////////////

  /////////////////////
  // Algorithm Tools //
  /////////////////////

  // loop all writers

  for (const std::string& toolName : m_writerToolNames)
    {
      IAlgTool* algToolPtr;
      ListItem writerAlgoTool(toolName);

      // pick up tool
      StatusCode checkOut = toolSvcPtr->retrieveTool(writerAlgoTool.type(),
                                                     writerAlgoTool.name(),
                                                     algToolPtr, 
                                                     this);
      // not found
      if ( checkOut.isFailure() )
	{
	  ATH_MSG_ERROR
            ( "failed to pick up tool of type \042"
	      << writerAlgoTool.type()
	      << "\042 with name <"
	      << writerAlgoTool.name()
	      << ">"
	      );
	  return StatusCode::FAILURE;
	}

      // store pointer to tool if right type 
      TBXMLWriterToolBase* thisTool = 
	dynamic_cast< TBXMLWriterToolBase* >(algToolPtr);
      if ( thisTool != 0 )
	{
	  m_writerTools.push_back(thisTool);
	}

    }

  // check tools
  if ( m_writerTools.size() == 0 )
    {
      ATH_MSG_ERROR ( "no tools found!" );
      return StatusCode::FAILURE;
    }

  for (TBXMLWriterToolBase* tool : m_writerTools)
    {
      // reset statistics
      m_toolInvoke[tool->name()] = 0;
      m_toolReject[tool->name()] = 0;
      m_toolAccept[tool->name()] = 0;
    }

  return StatusCode::SUCCESS;
}

/////////////
// Execute //
/////////////

StatusCode TBXMLWriter::execute()
{
  /////////////////
  // Check Tools //
  /////////////////

  //////////////////////////////
  // Output Frequency Control //
  //////////////////////////////

  m_eventCounter++;
  if ( m_eventCounter > 1 && m_eventCounter % m_outputFrequency != 0 )
    {
      return StatusCode::SUCCESS;
    }

  /////////////////////////
  // Access Event Header //
  /////////////////////////

  const EventInfo* eventHeader;
  ATH_CHECK( evtStore()->retrieve(eventHeader) );

  const EventID* theEvent = eventHeader->event_ID();

  ///////////////
  // XML Files //
  ///////////////

  m_beginRun = theEvent->run_number() != m_runNumber;
  if ( m_beginRun )
    {
      m_runNumber = theEvent->run_number();
    }
  m_eventNumber = theEvent->event_number();

  // run info
  //  if ( m_geomWriter > 0 && thisRun != m_oldRun )
  //    {
  //      m_oldRun = thisRun;
  //      this->writeRunInfo();
  //    }

  // construct directory and file name
  std::ostringstream thisFileName;
  thisFileName << m_topDirectory << "/evnt." 
	       << std::setw(6)   << std::setfill('0')
	       << m_runNumber    << "." << std::setfill('0')
	       << std::setw(6)
	       << m_eventNumber  << ".xml" << std::ends;

  std::ofstream thisFileStream((thisFileName.str()).c_str());

  ////////////////
  // Loop Tools //
  ////////////////

  for (TBXMLWriterToolBase* tool : m_writerTools)
    {
      StatusCode checkOut = tool->writeOut(thisFileStream);
      m_toolInvoke[tool->name()]++;
      if ( checkOut.isFailure() )
	{
	  m_toolReject[tool->name()]++;
	}
      else
	{
	  m_toolAccept[tool->name()]++;
	}
    }

  // finalize the stream
  ATH_CHECK((*(m_writerTools.begin()))->finalize(thisFileStream));
  thisFileStream.close();

  return StatusCode::SUCCESS;
}


StatusCode TBXMLWriter::finalize()
{
  return StatusCode::SUCCESS;
}

  
