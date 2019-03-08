/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoTagBuilder.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

EventInfoTagBuilder::EventInfoTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
    m_tool("EventInfoAttListTool/EventInfoAttListTool",this) {
  declareProperty("EventInfoKey", m_evtKey = "EventInfo");
  declareProperty("AttributeList", m_attributeListName);
}

EventInfoTagBuilder::~EventInfoTagBuilder() 
{}

StatusCode EventInfoTagBuilder::initialize() {
  ATH_MSG_DEBUG( "Initializing " << name() );

  ATH_CHECK( m_tool.retrieve() );

  ATH_CHECK( m_evtKey.initialize() );
  ATH_CHECK( m_attributeListName.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode EventInfoTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );

  SG::ReadHandle<xAOD::EventInfo> h_evt(m_evtKey);

  /** create a EventInfo Tag and ask the tool to fill it */ 
  if (h_evt.isValid()) {
    auto attribList = std::make_unique<AthenaAttributeList>
      ( m_tool->getAttributeList( *h_evt ) );

    /** record attribute list to SG */
    SG::WriteHandle<AthenaAttributeList> wh(m_attributeListName);
    ATH_CHECK( wh.record(std::move(attribList)) );
  } else {
    ATH_MSG_WARNING("Did not find xAOD::EventInfo");
  }

  ATH_MSG_DEBUG( "Finished " << name() );

  return StatusCode::SUCCESS;
}


StatusCode EventInfoTagBuilder::finalize() 
{
  return StatusCode::SUCCESS;
}
