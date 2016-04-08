/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "AthenaMonitoring/MonitorToolBase.h"
#include "TrigTileMuId/TrigTileMonAlg.h"

TrigTileMonAlg::TrigTileMonAlg(std::string name, ISvcLocator* pSvcLocator)
    : HLT::FexAlgo(name, pSvcLocator),
      m_tool(NULL)
{
    
}

TrigTileMonAlg::~TrigTileMonAlg() 
{
}

HLT::ErrorCode TrigTileMonAlg::hltInitialize()
{ 
    ATH_MSG_INFO("initialize ");

    IToolSvc* p_toolSvc;
    if( service("ToolSvc",p_toolSvc).isFailure() ) {
        ATH_MSG_FATAL(" Tool Service not found ");
	return HLT::TOOL_FAILURE;
    }

    IAlgTool* tool;
    if(p_toolSvc->retrieveTool( "TrigTileMonTool", "TrigTileMonTool",
				tool).isFailure()) {
        ATH_MSG_FATAL("Unable to create " << "TrigTileMonTool"
	    << " AlgTool");
	return HLT::TOOL_FAILURE;
    }
    m_tool = dynamic_cast<MonitorToolBase*>(tool);
    
    if(m_tool == NULL) {
      ATH_MSG_FATAL("dynamic cast to MonitorToolBase failed for TrigTileMonTool");
      return HLT::TOOL_FAILURE;
    }
    else {
      m_tool->bookHists(); 
    }
    
    return HLT::OK;
}

HLT::ErrorCode TrigTileMonAlg::hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* /*outputTE*/)
{
    m_tool->fillHists();

    return HLT::OK;
}

HLT::ErrorCode TrigTileMonAlg::hltFinalize()
{
    m_tool->finalHists();

    return HLT::OK;
}


