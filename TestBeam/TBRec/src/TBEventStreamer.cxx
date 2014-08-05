/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBRec/TBEventStreamerTool.h"
#include "TBRec/TBEventStreamer.h"

TBEventStreamer::TBEventStreamer(const std::string& name,
				 ISvcLocator* pService)
  : Algorithm(name,pService),
    m_storeGate(0)
{
  declareProperty("ToolNames",m_tools);
}

TBEventStreamer::~TBEventStreamer() 
{ }

StatusCode TBEventStreamer::initialize()
{
  // message service
  MsgStream report(messageService(),name());

  // allocate StoreGate
  StatusCode checkOut = service("StoreGateSvc",m_storeGate);
  if ( checkOut.isFailure() ) {
      report << MSG::ERROR
	     << "Cannot allocate StoreGate service"
	     << endreq;
      return checkOut;
    }

   checkOut = m_tools.retrieve();
      // not found
   if ( checkOut.isFailure() ) {
          report << MSG::ERROR << "failed to retrieve tools " << endreq;
          return checkOut;
   }


  // check tools
  if ( m_tools.size() == 0 )
    {
      report << MSG::ERROR
	     << "no tools found!"
	     <<endreq;
      return StatusCode::FAILURE;
    } else {
       for(unsigned int i = 0; i < m_tools.size(); ++i) {
           m_acceptCounter[m_tools[i]] = 0;
           m_rejectCounter[m_tools[i]] = 0;
           m_invokeCounter[m_tools[i]] = 0;
       }
    }

 
  // print out list of tools
  report << MSG::INFO << " " << endreq;
  report << MSG::INFO << "List of tools in execution sequence:" << endreq;
  report << MSG::INFO << "------------------------------------" << endreq;
  report << MSG::INFO << " " << endreq;
  unsigned int toolCtr = 0;
  ToolHandleArray<TBEventStreamerTool>::iterator fTool = m_tools.begin();
  ToolHandleArray<TBEventStreamerTool>::iterator lTool = m_tools.end();
  for ( ; fTool != lTool; fTool++ ) {
      toolCtr++;
      report << MSG::INFO
             << std::setw(2)
        //<< std::setiosflags(std::ios_base::right)
	     << toolCtr << ".) "
        //<< std::resetiosflags(std::ios_base::right)
        //<< std::setw(36)
        //<< std::setfill('.')
        //<< std::setiosflags(std::ios_base::left)
             << (*fTool)->type()
        //<< std::setfill('.')
	     << (*fTool)->name()
        //<< std::setfill(' ')
	     << endreq;
    }

  return StatusCode::SUCCESS;

}

StatusCode TBEventStreamer::execute()
{
  // message service
  MsgStream report(messageService(),name());

  // invoke tools
  ToolHandleArray<TBEventStreamerTool>::iterator fTool = m_tools.begin();
  ToolHandleArray<TBEventStreamerTool>::iterator lTool = m_tools.end();

  bool successFlag = true;
  while ( successFlag && fTool != lTool )
    {
      m_invokeCounter[*fTool]++;
      successFlag = (*fTool)->accept() == StatusCode::SUCCESS;
      if ( successFlag )
	{
	  m_acceptCounter[*fTool]++;
	}
      else
	{
	  m_rejectCounter[*fTool]++;
	}
      fTool++;
    }

   setFilterPassed(successFlag); 
   return StatusCode::SUCCESS;
}

StatusCode TBEventStreamer::finalize()
{
  // message service
  MsgStream report(messageService(),name());

  // print summary
  ToolHandleArray<TBEventStreamerTool>::iterator fTool = m_tools.begin();
  ToolHandleArray<TBEventStreamerTool>::iterator lTool = m_tools.end();
  for ( ; fTool != lTool; fTool++ ) {
      report << MSG::INFO
	     << "Tool "
	     << (*fTool)->name()
	     << " (invoked/accept/reject) "
	     << "(" << m_invokeCounter[*fTool]
	     << "/" << m_acceptCounter[*fTool]
	     << "/" << m_rejectCounter[*fTool]
	     << ")"
	     << endreq;
    }

  return StatusCode::SUCCESS;
}
