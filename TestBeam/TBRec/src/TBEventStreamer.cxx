/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TBEventStreamerTool.h"
#include "TBEventStreamer.h"

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
  for (ToolHandle<TBEventStreamerTool>& tool : m_tools) {
      toolCtr++;
      ATH_MSG_INFO (std::setw(2)
                    //<< std::setiosflags(std::ios_base::right)
                    << toolCtr << ".) "
                    //<< std::resetiosflags(std::ios_base::right)
                    //<< std::setw(36)
                    //<< std::setfill('.')
                    //<< std::setiosflags(std::ios_base::left)
                    << tool->type()
                    //<< std::setfill('.')
                    << tool->name() );
                    //<< std::setfill(' ')
    }

  return StatusCode::SUCCESS;

}

StatusCode TBEventStreamer::execute()
{
  bool successFlag = true;
  for (ToolHandle<TBEventStreamerTool>& tool : m_tools)
    {
      m_invokeCounter[tool]++;
      successFlag = tool->accept() == StatusCode::SUCCESS;
      if ( successFlag )
	{
	  m_acceptCounter[tool]++;
	}
      else
	{
	  m_rejectCounter[tool]++;
          break;
	}
    }

   setFilterPassed(successFlag); 
   return StatusCode::SUCCESS;
}

StatusCode TBEventStreamer::finalize()
{
  // print summary
  for (ToolHandle<TBEventStreamerTool>& tool : m_tools) {
    ATH_MSG_INFO ( "Tool "
                   << tool->name()
                   << " (invoked/accept/reject) "
                   << "(" << m_invokeCounter[tool]
                   << "/" << m_acceptCounter[tool]
                   << "/" << m_rejectCounter[tool]
                   << ")" );
    }

  return StatusCode::SUCCESS;
}
