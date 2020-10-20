/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthLinks/ElementLink.h"
#include "TrigT1Result/RoIBResult.h"

#include "TrigHypoCommonTools/IL1InfoHypoTool.h"
#include "TrigHypoCommonTools/L1InfoHypo.h"


L1InfoHypo::L1InfoHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::HypoBase(name, pSvcLocator) {}


StatusCode L1InfoHypo::initialize() {
    ATH_CHECK( m_hypoTools.retrieve() );
    ATH_CHECK( m_RoIBResultKey.initialize() );

    return StatusCode::SUCCESS;
}


StatusCode L1InfoHypo::execute(const EventContext& context) const {

    // Retrieve previous (L1 decision) decision
    ATH_MSG_DEBUG("Retrieving L1 decision \"" << decisionInput().key() << "\"");

    SG::ReadHandle<ROIB::RoIBResult> roib ( m_RoIBResultKey, context );

    auto previousDecisionsHandle = SG::makeHandle( decisionInput(), context );
    ATH_CHECK( previousDecisionsHandle.isValid() );
    ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() <<" previous decisions");

    SG::WriteHandle<TrigCompositeUtils::DecisionContainer> outputHandle 
        = TrigCompositeUtils::createAndStore(decisionOutput(), context ); 

    TrigCompositeUtils::DecisionContainer* decisions = outputHandle.ptr();

    std::vector<IL1InfoHypoTool::L1Info> hypoToolInput;
    for (const TrigCompositeUtils::Decision* previousDecision: *previousDecisionsHandle) {
        
        TrigCompositeUtils::Decision* d = TrigCompositeUtils::newDecisionIn( decisions, previousDecision, "", context );

        TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
        TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
        
        // Set dummy feature link
        ElementLink<TrigCompositeUtils::DecisionContainer> selfLink = TrigCompositeUtils::decisionToElementLink(d, context);
        d->setObjectLink<TrigCompositeUtils::DecisionContainer>(TrigCompositeUtils::featureString(), selfLink);

        hypoToolInput.push_back( IL1InfoHypoTool::L1Info{ &*roib, previousDecisionIDs, d } );
    }

    for ( auto & tool: m_hypoTools ) {
        ATH_CHECK( tool->decide( hypoToolInput ) );    
    } 
  
    ATH_CHECK( hypoBaseOutputProcessing(outputHandle) );

    return StatusCode::SUCCESS;
}
