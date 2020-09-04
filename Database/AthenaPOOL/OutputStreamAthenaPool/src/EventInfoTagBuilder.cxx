/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoTagBuilder.h"

#include <StoreGate/ReadHandle.h>
#include <StoreGate/WriteHandle.h>

EventInfoTagBuilder::EventInfoTagBuilder( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm(name, pSvcLocator)
{
}


StatusCode EventInfoTagBuilder::initialize()
{
  ATH_MSG_DEBUG( "Initializing " << name() );

  if (m_attributeListName.empty()) {
    ATH_MSG_ERROR("Output attribute list name should not be empty.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_tool.retrieve() );

  ATH_CHECK( m_evtKey.initialize() );
  ATH_CHECK( m_inputAttList.initialize() );
  ATH_CHECK( m_attributeListName.initialize() );

  return StatusCode::SUCCESS;
}


StatusCode EventInfoTagBuilder::execute()
{
  ATH_MSG_DEBUG( "Executing " << name() );

  SG::ReadHandle<xAOD::EventInfo> h_evt(m_evtKey);
  if (!h_evt.isValid()) {
    ATH_MSG_ERROR("Did not find xAOD::EventInfo");
    return StatusCode::FAILURE;
  }

  /** create a EventInfo Tag and ask the tool to fill it */
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
    } else { // valid input
      ATH_MSG_INFO("No input attribute list");
    }
  } // propagate

  /** record attribute list to SG */
  SG::WriteHandle<AthenaAttributeList> wh(m_attributeListName);
  ATH_CHECK( wh.record(std::move(attribList)) );

  ATH_MSG_DEBUG( "Finished " << name() );

  return StatusCode::SUCCESS;
}
