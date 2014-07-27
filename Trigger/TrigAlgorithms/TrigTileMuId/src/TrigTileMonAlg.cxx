/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaMonitoring/MonitorToolBase.h"
#include "TrigTileMuId/TrigTileMonAlg.h"

TrigTileMonAlg::TrigTileMonAlg(std::string name, ISvcLocator* pSvcLocator)
    : HLT::FexAlgo(name, pSvcLocator)       
{
    
}

TrigTileMonAlg::~TrigTileMonAlg() 
{
}

HLT::ErrorCode TrigTileMonAlg::hltInitialize()
{ 
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "initialize " << endreq;

    IToolSvc* p_toolSvc;
    if( service("ToolSvc",p_toolSvc).isFailure() ) {
	log << MSG::FATAL << " Tool Service not found " << endreq;
	return HLT::TOOL_FAILURE;
    }

    IAlgTool* tool;
    if(p_toolSvc->retrieveTool( "TrigTileMonTool", "TrigTileMonTool",
				tool).isFailure()) {
	log << MSG::FATAL << "Unable to create " << "TrigTileMonTool"
	    << " AlgTool" << endreq;
	return HLT::TOOL_FAILURE;
    }
    m_tool = dynamic_cast<MonitorToolBase*>(tool);
    m_tool->bookHists(); 
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


