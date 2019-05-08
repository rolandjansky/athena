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



StatusCode DecisionCollectorTool::getDecisions( std::vector<TrigCompositeUtils::DecisionID>& output ) const {
  for (auto decisionKey: m_decisionsKey ) {
    auto handle = SG::makeHandle( decisionKey );
    if ( handle.isValid() ) {
      for ( const TrigCompositeUtils::Decision* d : *handle.cptr() )  {	
	output.insert( output.end(), 
		       TrigCompositeUtils::decisionIDs( d ).begin(),
		       TrigCompositeUtils::decisionIDs( d ).end() );
      }
      ATH_MSG_DEBUG("Collected from decision container " << decisionKey.key() <<  " objects " <<handle.cptr()->size()  << " accumulated decisions " << output.size() );
    }
  }
  return StatusCode::SUCCESS;
}
