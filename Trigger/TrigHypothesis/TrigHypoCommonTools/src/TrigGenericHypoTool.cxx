/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigGenericHypoTool.h"
#include "StoreGate/ReadDecorHandle.h"

TrigGenericHypoTool::TrigGenericHypoTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent),
    m_decisionId(HLT::Identifier::fromToolName(name)) {}


StatusCode TrigGenericHypoTool::initialize(){

  ATH_CHECK(m_passKey.initialize());
  return StatusCode::SUCCESS;
}


StatusCode TrigGenericHypoTool::decide(const std::vector<TrigGenericHypoTool::HypoToolInfo>& input) const{
    ATH_MSG_DEBUG("Executing decide() of " << name());

    std::vector<TrigGenericHypoTool::HypoToolInfo* > positive( input.size() );
    SG::ReadDecorHandle<xAOD::TrigCompositeContainer, bool> trigCompositePassed(m_passKey);

    for ( const TrigGenericHypoTool::HypoToolInfo& hypoInfo : input ) {
      if ( TrigCompositeUtils::passed( m_decisionId.numeric(), hypoInfo.previousDecisionIDs ) ) {
	  bool decisionPassed = trigCompositePassed(*hypoInfo.trigComp);
	  if (decisionPassed) {
	    TrigCompositeUtils::addDecisionID( m_decisionId.numeric(), hypoInfo.decision );
	  }
        }
    }
 
    return StatusCode::SUCCESS;
}
