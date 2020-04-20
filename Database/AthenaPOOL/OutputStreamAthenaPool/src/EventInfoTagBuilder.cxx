/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoTagBuilder.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

EventInfoTagBuilder::EventInfoTagBuilder( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAlgorithm(name, pSvcLocator),
    m_tool("EventInfoAttListTool/EventInfoAttListTool",this) {
  declareProperty("EventInfoKey", m_evtKey = "EventInfo");
  declareProperty("InputList", m_inputAttList = "Input");
  declareProperty("AttributeList", m_attributeListName);
  declareProperty("PropagateInput", m_propInput = true);
  declareProperty("FilterString", m_filter = "");
}

EventInfoTagBuilder::~EventInfoTagBuilder() 
{}

StatusCode EventInfoTagBuilder::initialize() {
  ATH_MSG_DEBUG( "Initializing " << name() );

  ATH_CHECK( m_tool.retrieve() );

  ATH_CHECK( m_evtKey.initialize() );
  ATH_CHECK( m_attributeListName.initialize() );
  ATH_CHECK( m_inputAttList.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode EventInfoTagBuilder::execute() {
  ATH_MSG_DEBUG( "Executing " << name() );


  SG::ReadHandle<xAOD::EventInfo> h_evt(m_evtKey);

  /** create a EventInfo Tag and ask the tool to fill it */ 
  if (h_evt.isValid()) {
    std::unique_ptr<AthenaAttributeList> attribList =
      m_tool->getAttributeListPtr( *h_evt );

    // Check whether to propagate
    if (m_propInput) {
      SG::ReadHandle<AthenaAttributeList> h_att(m_inputAttList);
      // Check if there is an input to propagate
      if (h_att.isValid()) {
        for (auto it = h_att->specification().begin();
                  it!= h_att->specification().end(); ++it) {
          // Only propagate bool properties
          if (it->typeName()=="bool"&&!attribList->exists(it->name())) {
            // Check if there is filtering on the name
            if (m_filter != "") {
              if (it->name().find(m_filter)!=std::string::npos) {
                // Add those bools to the output attribute list
                (*attribList).extend(it->name(),it->type());
                (*attribList)[it->name()].data<bool>() = (*h_att)[it->name()].data<bool>();
              }
            }
          } // take only bools
        }   // loop
      } else {     // valid input
        ATH_MSG_INFO("No input attribute list");
      }
    }       // propagate

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
