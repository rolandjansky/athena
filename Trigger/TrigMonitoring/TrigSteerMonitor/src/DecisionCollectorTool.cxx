/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionCollectorTool.h"

DecisionCollectorTool::DecisionCollectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) {}

DecisionCollectorTool::~DecisionCollectorTool() {}

StatusCode DecisionCollectorTool::initialize() {
  ATH_CHECK( m_decisionsKey.initialize() );
  renounceArray( m_decisionsKey ); 
  return StatusCode::SUCCESS;
}

StatusCode DecisionCollectorTool::finalize() {
  return StatusCode::SUCCESS;
}



StatusCode DecisionCollectorTool::getDecisions( TrigCompositeUtils::DecisionIDContainer& output ) const {
  for (auto decisionKey: m_decisionsKey ) {
    auto handle = SG::makeHandle( decisionKey );
    if ( handle.isValid() ) {
      for ( const TrigCompositeUtils::Decision* d : *handle.cptr() )  {
	TrigCompositeUtils::decisionIDs( d, output );
      }
    }
  }
  return StatusCode::SUCCESS;
}
