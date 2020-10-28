/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHypoCommonTools/L1InfoHypoTool.h"

L1InfoHypoTool::L1InfoHypoTool(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent),
    m_decisionId(HLT::Identifier::fromToolName(name)) {}


StatusCode L1InfoHypoTool::initialize(){

    ATH_CHECK(m_ctpUnpackingTool.retrieve());

    return StatusCode::SUCCESS;
}


StatusCode L1InfoHypoTool::decide(const std::vector<IL1InfoHypoTool::L1Info>& input) const{
    ATH_MSG_DEBUG("Executing decide() of " << name());

    std::vector<IL1InfoHypoTool::L1Info* > positive( input.size() );

    for ( const IL1InfoHypoTool::L1Info& hypoInfo : input ) {
        if ( hypoInfo.previousDecisionsIDs.count( m_decisionId.numeric() ) > 0 ) {
            bool decisionPassed = false;
            ATH_CHECK(m_ctpUnpackingTool->passBeforePrescaleSelection(hypoInfo.roib, m_l1ItemNames, decisionPassed));
            if (decisionPassed) {
                TrigCompositeUtils::addDecisionID( m_decisionId.numeric(), hypoInfo.decision );
            }
        }
    }
 
    return StatusCode::SUCCESS;
}

const HLT::Identifier& L1InfoHypoTool::getId() const{
    return m_decisionId;
} 