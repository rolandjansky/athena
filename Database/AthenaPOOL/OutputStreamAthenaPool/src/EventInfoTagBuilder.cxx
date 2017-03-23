/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoTagBuilder.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "POOLCore/Exception.h"

//#include "CLHEP/Vector/ThreeVector.h"
#include <vector>

EventInfoTagBuilder::EventInfoTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
    m_tool("EventInfoAttListTool/EventInfoAttListTool",this) {
  declareProperty("AttributeList", m_attributeListName);
}

EventInfoTagBuilder::~EventInfoTagBuilder() 
{}

StatusCode EventInfoTagBuilder::initialize() {
  ATH_MSG_DEBUG( "Initializing " << name() );

  ATH_CHECK( m_tool.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode EventInfoTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );

  /** retrieve event info */
  const DataHandle<xAOD::EventInfo> eventInfo;
  StatusCode sc = evtStore()->retrieve(eventInfo);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve event info from TDS.");
    return StatusCode::FAILURE;
  }

  /** create a EventInfo Tag and ask the tool to fill it */ 
  auto attribList = std::make_unique<AthenaAttributeList>(m_tool->getAttributeList(eventInfo));

  /** record attribute list to SG */
  SG::WriteHandle<AthenaAttributeList> wh(m_attributeListName);
  ATH_CHECK( wh.record(std::move(attribList)) );

  ATH_MSG_DEBUG( "Finished " << name() );

  return StatusCode::SUCCESS;
}


StatusCode EventInfoTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}
