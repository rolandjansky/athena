/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

// StoreGate services
#include "StoreGate/StoreGateSvc.h"

// event
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

// TBRec includes
#include "TBRec/TBXMLWriterToolBase.h"
#include "TBRec/TBXMLWriter.h"

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
  std::vector<std::string>::iterator firstTool = m_writerToolNames.begin();
  std::vector<std::string>::iterator lastTool  = m_writerToolNames.end(); 

  for ( ; firstTool != lastTool; firstTool++ )
    {
      IAlgTool* algToolPtr;
      ListItem writerAlgoTool(*firstTool);

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

  // print out list of tools
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( "List of tools in execution sequence:" );
  ATH_MSG_INFO ( "------------------------------------" );
  ATH_MSG_INFO ( " " );
  unsigned int toolCtr = 0;
  std::vector<TBXMLWriterToolBase*>::iterator fTool = m_writerTools.begin();
  std::vector<TBXMLWriterToolBase*>::iterator lTool = m_writerTools.end();
  for ( ; fTool != lTool; fTool++ )
    {
      toolCtr++;
//       log << MSG::INFO 
// 	  << std::setw(2)     << std::setiosflags(std::ios_base::right)
// 	  << toolCtr << ".) " << std::resetiosflags(std::ios_base::right)
// 	  << std::setw(36) << std::setfill('.') 
// 	  << std::setiosflags(std::ios_base::left) << (*fTool)->type()
// 	  << std::setfill('.')
// 	  << (*fTool)->name()
// 	  << std::setfill(' ')
// 	  << endreq;
      // reset statistics
      m_toolInvoke[(*fTool)->name()] = 0;
      m_toolReject[(*fTool)->name()] = 0;
      m_toolAccept[(*fTool)->name()] = 0;
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

  std::vector<TBXMLWriterToolBase*>::const_iterator firstTool = 
    m_writerTools.begin();
  std::vector<TBXMLWriterToolBase*>::const_iterator lastTool =
    m_writerTools.end();
  for ( ; firstTool != lastTool; firstTool++ )
    {
      StatusCode checkOut = (*firstTool)->writeOut(thisFileStream);
      m_toolInvoke[(*firstTool)->name()]++;
      if ( checkOut.isFailure() )
	{
	  m_toolReject[(*firstTool)->name()]++;
	}
      else
	{
	  m_toolAccept[(*firstTool)->name()]++;
	}
    }

  // finalize the stream
  (*(m_writerTools.begin()))->finalize(thisFileStream);
  thisFileStream.close();

  return StatusCode::SUCCESS;
}


StatusCode TBXMLWriter::finalize()
{
  //////////////////////////
  // Re-allocate Services //
  //////////////////////////

  ////////////////
  // Tool Stats //
  ////////////////

  ATH_MSG_INFO
    ( "\n"
      << "Summary of Tool invocation: (invoked/success/failure)"
      << "\n"
      << "---------------------------"
      );

  std::map<std::string,unsigned int>::const_iterator 
    firstName = m_toolInvoke.begin();
  std::map<std::string,unsigned int>::const_iterator 
    lastName  = m_toolInvoke.end();
  unsigned int toolCtr = 0;
  for ( ; firstName != lastName; firstName++ )
    {
      toolCtr++;
//       log << MSG::INFO
// 	  << std::setw(2)     << std::setiosflags(std::ios_base::right)
// 	  << toolCtr << ".) " << std::resetiosflags(std::ios_base::right)
// 	  << std::setw(36) << std::setfill('.') 
// 	  << std::setiosflags(std::ios_base::left)
// 	  << (*firstName).first << std::resetiosflags(std::ios_base::left)
// 	  << std::setfill(' ')
// 	  << " ("
// 	  << std::setw(6) << std::setiosflags(std::ios_base::right)
// 	  << (*firstName).second
// 	  << "/"
// 	  << std::setw(6) << std::setiosflags(std::ios_base::right)
// 	  << m_toolAccept[(*firstName).first]
// 	  << "/"
// 	  << std::setw(6) << std::setiosflags(std::ios_base::right)
// 	  << m_toolReject[(*firstName).first]
// 	  << ")"
// 	  << endreq;
    }
  return StatusCode::SUCCESS;
}

  
