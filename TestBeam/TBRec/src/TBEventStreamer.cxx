/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include "StoreGate/StoreGateSvc.h"

#include "TBRec/TBEventStreamerTool.h"
#include "TBRec/TBEventStreamer.h"

TBEventStreamer::TBEventStreamer(const std::string& name,
				 ISvcLocator* pService)
  : AthAlgorithm(name,pService)
{
  declareProperty("ToolNames",m_tools);
}

TBEventStreamer::~TBEventStreamer() 
{ }

StatusCode TBEventStreamer::initialize()
{
  ATH_CHECK( m_tools.retrieve() );


  // check tools
  if ( m_tools.size() == 0 )
    {
      ATH_MSG_ERROR ( "no tools found!" );
      return StatusCode::FAILURE;
    } else {
       for(unsigned int i = 0; i < m_tools.size(); ++i) {
           m_acceptCounter[m_tools[i]] = 0;
           m_rejectCounter[m_tools[i]] = 0;
           m_invokeCounter[m_tools[i]] = 0;
       }
    }

 
  // print out list of tools
  ATH_MSG_INFO ( " " );
  ATH_MSG_INFO ( "List of tools in execution sequence:" );
  ATH_MSG_INFO ( "------------------------------------" );
  ATH_MSG_INFO ( " " );
  unsigned int toolCtr = 0;
  ToolHandleArray<TBEventStreamerTool>::iterator fTool = m_tools.begin();
  ToolHandleArray<TBEventStreamerTool>::iterator lTool = m_tools.end();
  for ( ; fTool != lTool; fTool++ ) {
      toolCtr++;
      ATH_MSG_INFO (std::setw(2)
                    //<< std::setiosflags(std::ios_base::right)
                    << toolCtr << ".) "
                    //<< std::resetiosflags(std::ios_base::right)
                    //<< std::setw(36)
                    //<< std::setfill('.')
                    //<< std::setiosflags(std::ios_base::left)
                    << (*fTool)->type()
                    //<< std::setfill('.')
                    << (*fTool)->name() );
                    //<< std::setfill(' ')
    }

  return StatusCode::SUCCESS;

}

StatusCode TBEventStreamer::execute()
{
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
  // print summary
  ToolHandleArray<TBEventStreamerTool>::iterator fTool = m_tools.begin();
  ToolHandleArray<TBEventStreamerTool>::iterator lTool = m_tools.end();
  for ( ; fTool != lTool; fTool++ ) {
    ATH_MSG_INFO ( "Tool "
                   << (*fTool)->name()
                   << " (invoked/accept/reject) "
                   << "(" << m_invokeCounter[*fTool]
                   << "/" << m_acceptCounter[*fTool]
                   << "/" << m_rejectCounter[*fTool]
                   << ")" );
    }

  return StatusCode::SUCCESS;
}
